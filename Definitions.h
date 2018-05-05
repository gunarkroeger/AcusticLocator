#ifndef DEFINITIONS_H
#define DEFINITIONS_H

enum { ADC_LENGTH = 5 };
enum { DIMENSIONS = 3 };
#define SPEED_OF_SOUND 340
#define SAMPLE_TIME 0.000013f

typedef struct Signal {
	float& operator[](unsigned i) { return channel[i]; }
	float channel[ADC_LENGTH];
} Signal;

typedef std::vector<Signal> SignalBuf;

typedef struct Pos {
	float& operator[](unsigned i) { return axis[i]; }
	float axis[DIMENSIONS];
} Pos;

#endif