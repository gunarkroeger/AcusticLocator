#ifndef DEFINITIONS_H
#define DEFINITIONS_H

enum { ADC_LENGTH = 4 };

typedef struct Signal {
	float& operator[](unsigned i) { return channel[i]; }
	float channel[ADC_LENGTH];
} Signal;

typedef std::vector<Signal> SignalBuf;

#endif