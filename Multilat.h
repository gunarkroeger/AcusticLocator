#ifndef MULTILAT_H
#define MULTILAT_H

#include <vector>
#include "Definitions.h"
 
class Multilat {

public:
    Multilat();
    virtual ~Multilat();
	
	Pos GetPosition(Signal t);

protected:
	const float p[DIMENSIONS][ADC_LENGTH] = {
			{0,		-75,		75, 		-86.6f,		86.6f},
			{0,		0,			0,			67,			67},
			{0,		-181,		-181,		-56,		-56}
		};
};

#endif