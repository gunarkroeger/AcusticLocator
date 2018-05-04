#include "mbed.h"
#include "main.h"
#include "AnalogInDma.h"
#include "Crosscorrel.h"
#include "Definitions.h"
#include <vector>
#include <queue>
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
//------------------------------------------------------------------------------
Timer timer;

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
	
    while (true)
	{
		a++;
		while(!captureReady); //wait for full capture
		
		NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		
		//printf("Signals:\n");
		for(unsigned t = 0; t < processQueue.size(); t++) //process captured signal
		{
			processFlag = 1;
			
			//serial debug sample
			pc.printf("%i,%i\n", int(processQueue[t][0]), int(processQueue[t][1]));
		}
		
		float p[3][5] = {
			{0,	-75,	75, 	-75,	75},
			{0,	-75,	-75,	-75,	-75},
			{0,	75,		75,		-75,	-75}
		};
		
		float t[5] = {0.0819f,0.0808f,0.1105f,0,0.0306f};
		float A[3][3];
		float b[3];
		
		unsigned i = 0;
		unsigned c = 3;
		float v = 1484;
		unsigned line = 0;
		for(unsigned j = 1; j < 5; j++)
		{
			if(j == c) continue;
			
			for(unsigned k = 0; k < 3; k++)
			{
				A[line][k] = 2*(v*(t[j]-t[c])*(p[k][i]-p[k][c]) - 
							 v*(t[i]-t[c])*(p[k][j]-p[k][c]));
			
			}
			b[line]	= v*(t[i]-t[c])*(v*v*(t[j]-t[c])*(t[j]-t[c])-
									 	(p[0][j]*p[0][j]+
									  	p[1][j]*p[1][j]+
										p[2][j]*p[2][j])) +
						(v*(t[i]-t[c])-v*(t[j]-t[c]))*
										(p[0][c]*p[0][c]+
									  	p[1][c]*p[1][c]+
										p[2][c]*p[2][c]) +
						v*(t[j]-t[c])*
							
										((p[0][i]*p[0][i]+
									  	p[1][i]*p[1][i]+
										p[2][i]*p[2][i]) -
						v*v*(t[i]-t[c])*(t[i]-t[c]));
											
											  
			line++;
		}
		
		for(unsigned i = 0; i < 3; i++)
			for(unsigned j = 0; j < 3; j++)
				pc.printf("A[%d][%d] = %d\n", i, j, int(A[i][j]));
		
			for(unsigned j = 0; j < 3; j++)
				pc.printf("b[%d] = %d\n", j, int(b[j]));
			
		printf("Crosscorrel[0,1]: %i\n", crosscorrel.GetDelay(processQueue, 0, 1));
		printf("Crosscorrel[0,2]: %i\n", crosscorrel.GetDelay(processQueue, 0, 2));
		printf("Crosscorrel[0,3]: %i\n", crosscorrel.GetDelay(processQueue, 0, 3));
		printf("Crosscorrel[1,2]: %i\n", crosscorrel.GetDelay(processQueue, 1, 2));
		printf("Crosscorrel[1,3]: %i\n", crosscorrel.GetDelay(processQueue, 1, 3));
		printf("Crosscorrel[2,3]: %i\n", crosscorrel.GetDelay(processQueue, 2, 3));
			   
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

