#ifndef DEFINITIONS_H
#define DEFINITIONS_H

enum { ADC_LENGTH = 5 };
enum { DIMENSIONS = 3 };
#define SPEED_OF_SOUND 340
#define SAMPLE_TIME 0.0000163397//0.000013f
#define	MEAN_SIZE	6

enum { CAPTURE_LENGTH = 512}; //6.64ms

typedef struct Signal {
	float& operator[](unsigned i) { return channel[i]; }
	float channel[ADC_LENGTH];
} Signal;

typedef std::vector<Signal> SignalBuf;

typedef struct Pos {
	float& operator[](unsigned i) { return axis[i]; }
	float axis[DIMENSIONS];
} Pos;

//#define DEBUG_PRINT
#define FFT_MAX_DISPLAY 10000
const unsigned fftLogIndex[] = {
	1  ,
	2  ,
	3  ,
	4  ,
	6  ,
	8  ,
	10 ,
	13 ,
	16 ,
	26 ,
	37 ,
	51 ,
	72 ,
	98 ,
	131,
	168,
	256,
};

const float aproxAngle2d[]
{
0,
-135,
135,
-60,
60,
};
#endif