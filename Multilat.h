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
			{0,		-0.75f,		0.75f, 		-0.75f,		0.75f},
			{0,		-0.75f,		-0.75f,		-0.75f,		-0.75f},
			{0,		0.75f,		0.75f,		-0.75f,		-0.75f}
		};
};

#endif