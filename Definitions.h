#ifndef DEFINITIONS_H
#define DEFINITIONS_H

enum { ADC_LENGTH = 5 };
enum { DIMENSIONS = 3 };
#define SPEED_OF_SOUND 340
#define SAMPLE_TIME 0.000013f

enum { CAPTURE_LENGTH = 512}; //6.64ms/2

typedef struct Signal {
	float& operator[](unsigned i) { return channel[i]; }
	float channel[ADC_LENGTH];
} Signal;

typedef std::vector<Signal> SignalBuf;

typedef struct Pos {
	float& operator[](unsigned i) { return axis[i]; }
	float axis[DIMENSIONS];
} Pos;

#define FFT_MAX_DISPLAY 8000
const unsigned fftLogIndex[] = {
0  ,
1  ,
2  ,
3  ,
5  ,
7  ,
9  ,
13 ,
17 ,
21 ,
27 ,
35 ,
47 ,
63 ,
87 ,
120,
128,
};
#endif