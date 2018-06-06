#include "FFT.h"
#include "arm_math.h"
#include "arm_const_structs.h"

static float32_t input[1024];
static float32_t output[512];

FFT::FFT()
{
}

void FFT::CalculateFFT(SignalBuf &buf, unsigned ch)
{      
    for(int i = 0; i < buf.size(); i++) 
    {            
        input[2*i] = buf[i][ch]; 	//real part
        input[2*i + 1] = 0;			//imaginary part
    }
  
    const static arm_cfft_instance_f32 *S;
	
	S = &arm_cfft_sR_f32_len512;;
	
	/* Calculate*/
	arm_cfft_f32(S, input, 0, 1);
		
  	arm_cmplx_mag_f32(input, output, 512);
//    /* Initialize*/
//    status = arm_cfft_radix4_init_f32(&S, fftSize, 0, 1);
//
//     /* Process the data through the CFFT/CIFFT module */
//    arm_cfft_radix4_f32(&S, input);
//
//     /* Process the data through the Complex Magnitude Module for
//    calculating the magnitude at each bin */
//    arm_cmplx_mag_f32(input, output, fftSize);
//
//    /* Calculates maxValue and returns corresponding BIN value */
//    arm_max_f32(output, fftSize, &maxValue, &maxIndex);
    
	
	printf("FFT: \n"); 
    for (int i = 0; i < CAPTURE_LENGTH/2; i++) //print only one half
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