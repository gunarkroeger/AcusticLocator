#include "Crosscorrel.h"

Crosscorrel::Crosscorrel()
{
	this->tau = 50;
}

float Crosscorrel::GetMax(SignalBuf &buf, unsigned chA, unsigned chB)
{
	float max = 0;
	for(int j = -tau; j <= tau; j++)
	{
		float correl = 0;
		
		for(unsigned t = 0; t < buf.size(); t++)
		{
			correl += buf[t][chA] * GetValue(buf, t+j, chB);
		}
		printf("%i\n", int(correl));
		max = correl > max ? correl : max;
	}
	return max;
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