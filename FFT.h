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
       
   	void CalculateFFT(SignalBuf &buf, unsigned c);
    //void CalculateFFT ();
   
   OLED *oled;
};

#endif