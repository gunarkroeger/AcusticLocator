#include "FFT.h"
#include "arm_math.h"
#include "arm_const_structs.h"

static float32_t input[1024];
static float32_t output[512];
static float32_t filter[512];

FFT::FFT()
{
}

void FFT::CalculateFFT(SignalBuf &buf, unsigned c)
{   
	filter[0] = 0; //remove dc component
	
	for(unsigned i = 0; i < 16; i++) //get filter
	{
		for(unsigned j = fftLogIndex[i]; j < fftLogIndex[i+1]; j++)
			filter[j] = oled->getFilter(i);
	}
	for(unsigned i = 0; i < CAPTURE_LENGTH/2; i++) //mirror filter
	{
		filter[CAPTURE_LENGTH-1 - i] = filter[i];
	}
	
	for(unsigned ch = 0; ch < ADC_LENGTH; ch++)
	{
		for(unsigned i = 0; i < buf.size(); i++) 
		{            
			input[2*i] = buf[i][ch]; 	//real part
			input[2*i + 1] = 0;			//imaginary part
		}
		
		const static arm_cfft_instance_f32 *S;
		
		S = &arm_cfft_sR_f32_len512;;
		
		/* Calculate FFT*/
		arm_cfft_f32(S, input, 0, 1);
		
		//apply filter
		arm_cmplx_mult_real_f32(input, filter, input, CAPTURE_LENGTH);
		
		if(ch == c) //show on oled
		{
			arm_cmplx_mag_f32(input, output, CAPTURE_LENGTH);
			
#ifdef DEBUG_PRINT
			printf("FFT[%d]: \n", ch); 
			for (int i = 0; i < CAPTURE_LENGTH/2; i++) //print only one half
			{
				printf("%i\n",(int)output[i]);        
			}
			printf("end FFT\n");
#endif
			float maxValue = 0;
			unsigned maxIndex = 0;
			arm_max_f32(output, CAPTURE_LENGTH, &maxValue, &maxIndex);
			
			float fftOut[16] = {0};				//reduce to 16 bins
			float power = 0;
			for(unsigned i = 0; i < 16; i++)
			{
				for(unsigned j = fftLogIndex[i]; j < fftLogIndex[i+1]; j++)
					fftOut[i] += output[j];
				power += fftOut[i];
			}
			
			for(unsigned i = 0; i < 16; i++) //limit output
				if(fftOut[i] > FFT_MAX_DISPLAY)
					fftOut[i] = FFT_MAX_DISPLAY;
			oled->setFreqPot(float(maxIndex)/(SAMPLE_TIME*CAPTURE_LENGTH), power);
			oled->setFFT(fftOut, FFT_MAX_DISPLAY);
		}
				
		arm_cfft_f32(S, input, 1, 1); //IFFT
		
		for (int i = 0; i < CAPTURE_LENGTH; i++) //return filtered data
		{
			buf[i][ch] = input[2*i];    
		}
	}
}

FFT::~FFT()
{

}