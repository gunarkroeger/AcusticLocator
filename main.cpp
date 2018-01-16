#include "mbed.h"
#include "main.h"
#include "AnalogInDma.h"
#include <vector>

enum { ADC_ACQ_LENGHT = 1024 };

DigitalOut led1(LED1);
DigitalOut adcFlag(D2);

Timer timer;

DMA_HandleTypeDef hdma_adc1;

AnalogIn a0(A0);
AnalogIn a1(A1);
AnalogIn a2(A2);
AnalogIn a3(A3);
AnalogInDma adc;

uint32_t adcBuffer[ADC_ACQ_LENGHT];
int a = 0, b = 0;
unsigned adcTime = 0;
unsigned ad;
std::vector<int> test;

void ProcessAdc(ADC_HandleTypeDef* AdcHandle, unsigned offset, unsigned length)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(AdcHandle);
	adcTime = timer.read_us();
	timer.reset();
	adcFlag = !adcFlag;
		
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
  	HAL_Init();
	
	timer.start();
	
	if(!adc.init())
		_Error_Handler(__FILE__, __LINE__);
	if(!adc.start(adcBuffer, ADC_ACQ_LENGHT))
		_Error_Handler(__FILE__, __LINE__);
		
    while (true) {
        led1 = !led1;
		if(a % 2 == 0)
			test.push_back(a);
		else
		  test.pop_back();
        wait(0.01*a);
		a++;
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

