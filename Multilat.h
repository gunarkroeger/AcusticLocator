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
			{0,		-0.075f,		0.075f, 		-0.0866f,		0.0866f	},
			{0,		0,				0,				0.067f,			0.067f	},
			{0,		-0.181f,		-0.181f,		-0.056f,		-0.056f	}
		};
};

#endif