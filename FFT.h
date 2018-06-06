#ifndef FFT_H
#define FFT_H

#include <vector>
#include "Definitions.h"
#include "arm_math.h"
#include "OLED.h"
 
class FFT {

public:
    FFT();
    virtual ~FFT();
    
   float32_t input[2*CAPTURE_LENGTH];
   float32_t output[CAPTURE_LENGTH];
	float filter[16];
   
   void CalculateFFT(SignalBuf &buf, unsigned ch);
    //void CalculateFFT ();
   
   OLED *oled;
};

#endif