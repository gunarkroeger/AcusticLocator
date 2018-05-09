#include "FFT.h"
#include "arm_math.h"
#include "arm_const_structs.h"


FFT::FFT()
{
}

void FFT::CalculateFFT(SignalBuf &buf, unsigned chA, unsigned chB)
//void FFT::CalculateFFT()
{
   /*float32_t testInput[512];
   float32_t testOutput[256];
   uint32_t fftSize = 256; */
   float32_t InputA[512];
   float32_t InputB[512];
    
   float32_t OutputA[512/2];
   float32_t OutputB[512/2];
      
    arm_status status;
    arm_cfft_radix4_instance_f32 S;
    float32_t maxValue;
    uint32_t maxIndex;
    uint32_t fftSize = buf.size()/2;

    status = ARM_MATH_SUCCESS;
    
    for (int i = 0;i<buf.size(); i+= 2) 
    {            
        
        InputA[(uint16_t)i] = (float32_t)chA;
        
        InputA[(uint16_t)(i + 1)] = 0;
        
        
        InputB[(uint16_t)i] = (float32_t)chB;
        
        InputB[(uint16_t)(i + 1)] = 0;
        
    }
  
    /* Initialize*/
    status = arm_cfft_radix4_init_f32(&S, fftSize, 0, 1);

     /* Process the data through the CFFT/CIFFT module */
    arm_cfft_radix4_f32(&S, InputA);
    arm_cfft_radix4_f32(&S, InputB);
    //arm_cfft_radix4_f32(&S, testInput);

     /* Process the data through the Complex Magnitude Module for
    calculating the magnitude at each bin */
    arm_cmplx_mag_f32(InputA, OutputA, fftSize);
    arm_cmplx_mag_f32(InputB, OutputB, fftSize);
    //arm_cmplx_mag_f32(testInput, testOutput, fftSize);

    /* Calculates maxValue and returns corresponding BIN value */
    arm_max_f32(OutputA, fftSize, &maxValue, &maxIndex);
    arm_max_f32(OutputB, fftSize, &maxValue, &maxIndex);
    //arm_max_f32(testOutput, fftSize, &maxValue, &maxIndex);
    
    /*for (int i = 0; i < fftSize; i++) 
    {
        printf("%f/n",(float32_t)OutputA[(uint16_t)i]);
       printf("%f/n",(float32_t)OutputB[(uint16_t)i]);
       //printf("%i/n",int(testOutput[i]));
        
    };*/

    while(1);                             
}

FFT::~FFT()
{

}