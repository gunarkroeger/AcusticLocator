#include "mbed.h"
#include "main.h"
#include "AnalogInDma.h"
#include <vector>
#include <queue>
//------------------------------------------------------------------------------
enum { ADC_LENGTH = 4 };
enum { ADC_ACQ_LENGHT = 2 * ADC_LENGTH };
enum { ADC_MEAN_SIZE = 1024 };
//------------------------------------------------------------------------------
enum { SERIAL_BUF_LENGTH = 1024 };

DigitalOut led1(LED1);
DigitalOut adcFlag(D2);
Serial pc(USBTX, USBRX);

Timer timer;

DMA_HandleTypeDef hdma_adc1;

AnalogIn a0(A0);
AnalogIn a1(A1);
AnalogIn a2(A2);
AnalogIn a3(A3);
AnalogInDma adc;

uint32_t adcBuffer[ADC_ACQ_LENGHT];
queue<char> serialQueue;

int a = 0, b = 0;
unsigned adcTime = 0;
float adcValue[ADC_LENGTH];
float adcMeanValue[ADC_LENGTH];
float adcUnbiasedValue[ADC_LENGTH];
std::vector<int> test;

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
	adcTime = timer.read_us();
	timer.reset();
	led1 = adcFlag;
	for(unsigned i = 0; i < ADC_LENGTH; i++)
	{
		adcValue[i] = adcBuffer[offset+i];
		adcMeanValue[i] += (adcValue[i]-adcMeanValue[i]) / ADC_MEAN_SIZE;
		adcUnbiasedValue[i] = adcValue[i] - adcMeanValue[i];
	}
	adcFlag = 1;
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
	
	
	wait(2);
	char line[20];
    while (true) {
    	if(adcFlag)
		{
			a++;
			if(adcUnbiasedValue[0] > 100 || adcUnbiasedValue[1] > 100)
				a = 0;
			if(a<100)
			{
				snprintf(line, sizeof(line),"%i,%i\r\n", int(adcUnbiasedValue[0]), int(adcUnbiasedValue[1]));
				for(unsigned i = 0; i < sizeof(line) && line[i] != '\0'; i++)
					serialQueue.push(line[i]);
			}
			if(a == 100)
			{
    			NVIC_DisableIRQ(DMA1_Channel1_IRQn);
				
				while (serialQueue.size()) {
					if(pc.writable())
					{
						pc.putc(serialQueue.front());
						serialQueue.pop();
					}
				}
				
				adcFlag = 0;
    			NVIC_EnableIRQ(DMA1_Channel1_IRQn);
			}
			
			adcFlag = 0;
		}
		/*ad = a0.read_u16();
		ad = a1.read_u16();
		ad = a2.read_u16();
		ad = a3.read_u16();*/
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

