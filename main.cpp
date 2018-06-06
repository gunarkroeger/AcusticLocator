#include "mbed.h"
#include "main.h"
#include "AnalogInDma.h"
#include "Crosscorrel.h"
#include "Multilat.h"
#include "Definitions.h"
#include <vector>
#include <queue>
#include "Adafruit_SSD1306.h"
#include "OLED.h"
#include "FFT.h"

//------------------------------------------------------------------------------
enum { ADC_ACQ_LENGHT = 2 * ADC_LENGTH * 1};
enum { ADC_MEAN_SIZE = 1024 };
enum { THRESHOLD = 100 };
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
DigitalOut led1(LED1);


OLED oled(D14, D15, D8, D7, D9, D5, D4, D3);
FFT fft;

Serial pc(USBTX, USBRX);

AnalogIn a0(A0);
AnalogIn a1(A1);
AnalogIn a2(A2);
AnalogIn a3(A3);
AnalogIn a4(A4);

int threshold = THRESHOLD;
//------------------------------------------------------------------------------
Timer timer;
//------------------------------------------------------------------------------

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

void refreshDisp()
{
	oled.rodaTela();
}
void ProcessAdc(ADC_HandleTypeDef* AdcHandle, unsigned offset, unsigned length)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(AdcHandle);
	//adcTime = float(timer.read_us()) * 2 * ADC_LENGTH / ADC_ACQ_LENGHT;
	//timer.reset();
	if(processQueue.size() < CAPTURE_LENGTH)
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
				if(adcUnbiasedValue[ch] > threshold)
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
	fft.oled = &oled;
   	//.setX();
   	//oled.setFreqPot(float freq, float pot);
   	//oled.setC(int c);
	
   	oled.setThreshold(threshold);
	
    while (true)
	{
		a++;
		while(!captureReady)
		{
			//wait for full capture
			refreshDisp();
			wait(0.02f);
		}
		
		NVIC_DisableIRQ(DMA1_Channel1_IRQn);
				
		printf("Signals:\n");
		for(unsigned t = 0; t < processQueue.size(); t++) //process captured signal
		{
			
			//serial debug sample
			for(unsigned i = 0; i < ADC_LENGTH; i++)
				pc.printf("%i,", int(processQueue[t][i]));
			pc.printf("\n");
		}
		fft.CalculateFFT(processQueue, 0);
    	//setFFT(float fftOut[16], float maxValue, float &filtro);
	
		Signal t = {23 * SAMPLE_TIME,36 * SAMPLE_TIME,12 * SAMPLE_TIME,28 * SAMPLE_TIME,0 * SAMPLE_TIME};
		
		//multilat.GetPosition(t);
		
					
		t[0] = 0; //get delays in relation to channel 0
		t[1] = crosscorrel.GetDelay(processQueue, 0, 1);
		t[2] = crosscorrel.GetDelay(processQueue, 0, 2);
		t[3] = crosscorrel.GetDelay(processQueue, 0, 3);
		t[4] = crosscorrel.GetDelay(processQueue, 0, 4);
		
		int c = 0;
		float min = 10000;
		for(unsigned i = 0; i < ADC_LENGTH; i++) //find most negative delay
			if(t[i] < min)
			{
				min = t[i];
				c = i;
			}	
		
   		oled.setC(c);
		
		for(unsigned i = 0; i < ADC_LENGTH; i++)
			printf("crosscorrel[0,%d]: %i\n", i, int(t[i]));
		
		Pos X = multilat.GetPosition(t);
		
		float Y[3] = {X[0]*1000-8, X[1]*1000+39, X[2]*1000 -85};
		oled.setX(Y);
		pc.printf("Threshold = %i\n", threshold);
		pc.printf("\n\n");
		
		processQueue.clear();
		captureReady = false;
		NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		refreshDisp();
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

