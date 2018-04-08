#include "mbed.h"
#include "main.h"
#include "AnalogInDma.h"
#include <vector>
#include <queue>
//------------------------------------------------------------------------------
enum { ADC_LENGTH = 4 };
enum { ADC_ACQ_LENGHT = 2 * ADC_LENGTH * 1};
enum { CAPUTURE_LENGTH = 512}; //6.64ms
enum { ADC_MEAN_SIZE = 1024 };
enum { THREASHOLD = 100 };
//------------------------------------------------------------------------------
enum { SERIAL_BUF_LENGTH = 1024 };

//------------------------------------------------------------------------------
typedef struct Signal {
	float& operator[](unsigned i) { return channel[i]; }
	float channel[ADC_LENGTH];
} Signal;
//------------------------------------------------------------------------------
DigitalOut led1(LED1);

DigitalOut adcFlag(D2);
DigitalOut processFlag(D3);

Serial pc(USBTX, USBRX);

AnalogIn a0(A0);
AnalogIn a1(A1);
AnalogIn a2(A2);
AnalogIn a3(A3);
//------------------------------------------------------------------------------
Timer timer;

//------------------------------------------------------------------------------
DMA_HandleTypeDef hdma_adc1;

AnalogInDma adc;

uint32_t adcBuffer[ADC_ACQ_LENGHT];
queue<char> serialQueue;
queue<Signal> processQueue;

int a = 0, b = 0;

float adcTime = 0;
unsigned stayActive = 0;

volatile bool captureReady = false;

Signal adcValue;
Signal adcMeanValue;
Signal adcUnbiasedValue;

void serialInterruptTx()
{
	while ((pc.writeable()) && serialQueue.size()) {
		pc.putc(serialQueue.front());
		serialQueue.pop();
	}
}

void ProcessAdc(ADC_HandleTypeDef* AdcHandle, unsigned offset, unsigned length)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(AdcHandle);
	adcFlag = 1;
	adcTime = float(timer.read_us()) * 2 * ADC_LENGTH / ADC_ACQ_LENGHT;
	timer.reset();
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
				processQueue.push(adcUnbiasedValue);
			}
		}
	}
	else {
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
	
	char line[20];
    while (true)
	{
		a++;
		while(!captureReady); //wait for full capture
		
		NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		
		while(processQueue.size()) //process captured signal
		{
			processFlag = 1;
			
			//get sample
			Signal signal = processQueue.front();
			processQueue.pop();
			//analyse data
			
			//serial debug sample
			snprintf(line, sizeof(line),"%i,%i\r\n", int(signal[0]), int(signal[1]));
			for(unsigned i = 0; i < sizeof(line) && line[i] != '\0'; i++)
				serialQueue.push(line[i]);
			
			while (serialQueue.size()) {
				if(pc.writable())
				{
					pc.putc(serialQueue.front());
					serialQueue.pop();
				}
			}
			processFlag = 0;
		}
		adcFlag = 0;
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

