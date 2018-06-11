#ifndef MULTILAT_H
#define MULTILAT_H

#include <vector>
#include "Definitions.h"
#include "Dht11.h"

class Multilat {

public:
    Multilat();
    virtual ~Multilat();
	
	Pos GetPosition(Signal t);
	float calculateSpeedOfSound(void);

protected:
	const float p[DIMENSIONS][ADC_LENGTH] = {
			{0,		-0.0746f,		0.0746f, 		-0.080729f,		0.080729f	},
			{0,		0,				0,				0.0727f,			0.0727f	},
			{0,		-0.1801f,		-0.1801f,		-0.058891f,		-0.058891f	}
		};
};

#endif