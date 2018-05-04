#ifndef CROSSCORREL_H
#define CROSSCORREL_H

#include <vector>
#include "Definitions.h"
 
class Crosscorrel {

public:
    Crosscorrel();
    virtual ~Crosscorrel();
	
	int GetDelay(SignalBuf &buf, unsigned chA, unsigned chB);


protected:
	float GetValue(SignalBuf &a, int index, unsigned ch);
	float tau;
};

#endif

