#include "Crosscorrel.h"

Crosscorrel::Crosscorrel()
{
	this->tau = 50;
}

int Crosscorrel::GetDelay(SignalBuf &buf, unsigned chA, unsigned chB)
{
	float max = 0;
	int delay = 0;
	for(int j = -tau; j <= tau; j++)
	{
		float correl = 0;
		
		for(unsigned t = 0; t < buf.size(); t++)
		{
			correl += buf[t][chA] * GetValue(buf, t+j, chB);
		}
		//printf("%i\n", int(correl));
		if(correl > max)
		{
			max = correl;
			delay = j;
		}
	}
	return delay;
}

Crosscorrel::~Crosscorrel()
{

}

float Crosscorrel::GetValue(SignalBuf &buf, int index, unsigned ch)
{
	if(index < 0 || index >= buf.size())
		return 0;
	
	return buf[index][ch];
}