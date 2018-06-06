#include "FFT.h"
#include "arm_math.h"
#include "arm_const_structs.h"

FFT::FFT()
{
}

void FFT::CalculateFFT(SignalBuf &buf, unsigned ch)
{      
    arm_status status;
    arm_cfft_radix4_instance_f32 S;
	
    float32_t maxValue;
    uint32_t maxIndex;
	
    uint32_t fftSize = CAPTURE_LENGTH/2;

    status = ARM_MATH_SUCCESS;
    
    for(int i = 0; i < fftSize; i++) 
    {            
        input[2*i] = buf[i][ch]; 	//real part
        input[2*i + 1] = 0;			//imaginary part
    }
  
    /* Initialize*/
    status = arm_cfft_radix4_init_f32(&S, fftSize, 0, 1);

     /* Process the data through the CFFT/CIFFT module */
    arm_cfft_radix4_f32(&S, input);

     /* Process the data through the Complex Magnitude Module for
    calculating the magnitude at each bin */
    arm_cmplx_mag_f32(input, output, fftSize);

    /* Calculates maxValue and returns corresponding BIN value */
    arm_max_f32(output, fftSize, &maxValue, &maxIndex);
    
	
	printf("FFT: \n"); 
    for (int i = 0; i < fftSize/2; i++) //print only one half
    {
        printf("%i\n",(int)output[i]);        
    }
	
	float fftOut[16] = {0};
	for(unsigned i = 0; i < 16; i++)
	{
		for(unsigned j = fftLogIndex[i]; j < fftLogIndex[i+1]; j++)
			fftOut[i] += output[j];
		filter[i] = 1;
	}
	
	for(unsigned i = 0; i < 16; i++)
		if(fftOut[i] > FFT_MAX_DISPLAY)
			fftOut[i] = FFT_MAX_DISPLAY;
	
    oled->setFFT(fftOut, FFT_MAX_DISPLAY);
	printf("end FFT\n");
                         
}

FFT::~FFT()
{

}