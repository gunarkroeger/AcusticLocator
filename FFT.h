#ifndef FFT_H
#define FFT_H

#include <vector>
#include "Definitions.h"
#include "arm_math.h"
 
class FFT {

public:
    FFT();
    virtual ~FFT();
    
   void CalculateFFT(SignalBuf &buf, unsigned chA, unsigned chB);
    //void CalculateFFT ();
};

#endif