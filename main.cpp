#include "mbed.h"
#include "main.h"
#include "AnalogInDma.h"
#include <vector>

enum { ADC_ACQ_LENGHT = 64 };
DigitalOut led1(LED1);

DMA_HandleTypeDef hdma_adc1;

AnalogIn a0(A0);
AnalogInDma adc;

uint32_t adcBuffer[ADC_ACQ_LENGHT];
int a = 0, b = 0;

std::vector<int> test;



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hadc);
	b++;
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	UNUSED(hadc);
	b--;
}


// main() runs in its own thread in the OS
int main()
{
  	HAL_Init();
	
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

