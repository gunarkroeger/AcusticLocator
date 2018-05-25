#include "mbed.h"
#include "main.h"
#include "AnalogInDma.h"
#include "Crosscorrel.h"
#include "Multilat.h"
#include "Definitions.h"
#include <vector>
#include <queue>
#include "Adafruit_SSD1306.h"
//------------------------------------------------------------------------------
enum { ADC_ACQ_LENGHT = 2 * ADC_LENGTH * 1};
enum { CAPUTURE_LENGTH = 512}; //6.64ms
enum { ADC_MEAN_SIZE = 1024 };
enum { THREASHOLD = 100 };
enum { TAU = 100 };
//------------------------------------------------------------------------------
enum { SERIAL_BUF_LENGTH = 1024 };

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
DigitalOut led1(LED1);


DigitalOut adcFlag(D2);
DigitalOut processFlag(D3);

Serial pc(USBTX, USBRX);

AnalogIn a0(A0);
AnalogIn a1(A1);
AnalogIn a2(A2);
AnalogIn a3(A3);
AnalogIn a4(A4);
//------------------------------------------------------------------------------
Timer timer;

//------------------------------------------------------------------------------
class I2CPreInit : public I2C
{
public:
    I2CPreInit(PinName sda, PinName scl) : I2C(sda, scl)
    {
        frequency(400000);
        start();
    };
};
 
I2CPreInit gI2C(PB_14,PB_13);
Adafruit_SSD1306_I2c gOled2(gI2C, NC);
//------------------------------------------------------------------------------
DMA_HandleTypeDef hdma_adc1;

AnalogInDma adc;

uint32_t adcBuffer[ADC_ACQ_LENGHT];
vector<Signal> processQueue;

int a = 0, b = 0;

unsigned adcTime = 0;
unsigned stayActive = 0;

volatile bool captureReady = false;

Signal adcValue;
Signal adcMeanValue;
Signal adcUnbiasedValue;
 
void ProcessAdc(ADC_HandleTypeDef* AdcHandle, unsigned offset, unsigned length)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(AdcHandle);
	adcFlag = 1;
	//adcTime = float(timer.read_us()) * 2 * ADC_LENGTH / ADC_ACQ_LENGHT;
	//timer.reset();
	if(processQueue.size() < CAPUTURE_LENGTH)
	{
		led1 = 0;
		for(unsigned i = 0; i < length; i+= ADC_LENGTH)
		{
			for(unsigned ch = 0; ch < ADC_LENGTH; ch++)
			{
				adcValue[ch] = adcBuffer[offset+ch+i];
				adcMeanValue[ch] += (adcValue[ch]-adcMeanValue[ch]) / ADC_MEAN_SIZE;
				adcUnbiasedValue[ch] = adcValue[ch] - adcMeanValue[ch];		
			}
			
			bool greaterThanThr = false;
			for(unsigned ch = 0; ch < ADC_LENGTH; ch++)
				if(adcUnbiasedValue[ch] > THREASHOLD)
					greaterThanThr = true;
			
			if(greaterThanThr)
				stayActive = true;
			
			if(stayActive)
			{
				if(processQueue.size() == 0)
					timer.reset();
				processQueue.push_back(adcUnbiasedValue);
			}
		}
	}
	else {
		adcTime = timer.read_us();
		captureReady = true;
		stayActive = false;
	}
	adcFlag = 0;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hadc);
    ProcessAdc(hadc, ADC_ACQ_LENGHT / 2, ADC_ACQ_LENGHT / 2);
	b++;
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	UNUSED(hadc);
    ProcessAdc(hadc, 0, ADC_ACQ_LENGHT / 2);
	b--;
}


void rodaTela(float x, float y, float z){
		gOled2.clearDisplay();
		gOled2.setTextCursor(64, 10);
		gOled2.printf("x:%d", int (x*1000));
		gOled2.setTextCursor(64, 30);
		gOled2.printf("y:%d", int (y*1000));
		gOled2.setTextCursor(64, 50);
		gOled2.printf("z:%d", int (z*1000));
        gOled2.fillCircle(32, 32, 30, 1);
        gOled2.fillCircle(32, 32, 24, 0);
        float div = x*x + y*y;
        div = sqrt(div);
        x = x/div;
        y = y/div;
        gOled2.drawLine(32, 32, 32 + y*30, 32 - x*30, 1);
		
//        gOled2.drawChar(20, 25, (uint16_t)div/10, 1, 0, 2);
//        gOled2.drawChar(40, 25, (uint16_t)div-div/10, 1, 0, 2);
//        if(z>0){
//            gOled2.drawChar(85, 20, 'U', 1, 0, 4);
//        }
//        else if(z<0){
//            gOled2.drawChar(85, 20, 'D', 1, 0, 4);
//        }
        gOled2.display();
}

// main() runs in its own thread in the OS
int main()
{
	pc.baud(1000000);
	//pc.attach(&serialInterruptTx, Serial::TxIrq);
	
	timer.start();
	
	if(!adc.init())
		_Error_Handler(__FILE__, __LINE__);
	if(!adc.start(adcBuffer, ADC_ACQ_LENGHT))
		_Error_Handler(__FILE__, __LINE__);
	
	Crosscorrel crosscorrel;
	Multilat multilat;
	
    while (true)
	{
		a++;
		while(!captureReady); //wait for full capture
		
		NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		
		processFlag = 1;
		//printf("Signals:\n");
		for(unsigned t = 0; t < processQueue.size(); t++) //process captured signal
		{
			
			//serial debug sample
			for(unsigned i = 0; i < ADC_LENGTH; i++)
				pc.printf("%i,", int(processQueue[t][i]));
			pc.printf("\n");
		}
				
		Signal t = {23 * SAMPLE_TIME,36 * SAMPLE_TIME,12 * SAMPLE_TIME,28 * SAMPLE_TIME,0 * SAMPLE_TIME};
		
		//multilat.GetPosition(t);
		
					
		t[0] = 0; //get delays in relation to channel 0
		t[1] = crosscorrel.GetDelay(processQueue, 0, 1);
		t[2] = crosscorrel.GetDelay(processQueue, 0, 2);
		t[3] = crosscorrel.GetDelay(processQueue, 0, 3);
		t[4] = crosscorrel.GetDelay(processQueue, 0, 4);
				
		for(unsigned i = 0; i < ADC_LENGTH; i++)
			printf("crosscorrel[0,%d]: %i\n", i, int(t[i]));
		
		Pos X = multilat.GetPosition(t);
		
		rodaTela(X[2], X[0], X[1]);
		
		pc.printf("\n\n");
		processFlag = 0;
		
		adcFlag = 0;
		processQueue.clear();
		captureReady = false;
		NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    }
}

void _Error_Handler(const char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

