#ifndef OLED_H
#define OLED_H

#include "Adafruit_SSD1306.h"
#include "QEI.h"
#include "mbed.h"

class OLED{
public:
    //criando o PreInit
    class I2CPreInit : public I2C
    {
    public:
        I2CPreInit(PinName sda, PinName scl) : I2C(sda, scl)
        {
            frequency(400000);
            start();
        };
    };
    
    OLED(PinName sda, PinName scl, PinName channelA, PinName channelB, PinName index, PinName channelA2, PinName channelB2, PinName index2);
    
    bool getAlteracao(void);
    void setAlteracao(void);
    
//    Pos setPos(Pos X);
    void setX(float X[3]);
    void setFreqPot(float freq, float pot);
    void setC(int c);
    void setFFT(float fftOut[16], float maxValue);
    void setThreshold(int &threshold);
    
    void rodaTela(void);
	float getFilter(unsigned i);
    
private:
//------------------------------FUNCOES-----------------------------------------
    void rodaTelaMenu(void);
    void rodaTelaLocalizador(void);
    void rodaTelaThresholdEdit(void);
    void rodaTelaBussola(void);
    void rodaTelaFreqPot(void);
    void rodaTelaFFT(void);
    void rodaTelaExit(void);
    void rodaTelaDebug(void);
    void rodaTelaCreditos(void);

//------------------------------VARIAVEIS---------------------------------------
//----------------------------DO LOCALIZADOR------------------------------------
//    typedef struct Pos {
//        float& operator[](unsigned i) { return axis[i]; }
//        float axis[DIMENSIONS];
//    } Pos;
    float X[3];
    int c;
    float freq, pot;
    float fftOut[16], maxValue;
    float filtro[16];
    int *threshold;
    
//------------------------------VARIAVEIS---------------------------------------
//------------------------------DO DISPLAY--------------------------------------
    float dist, bussx, bussy, raio, preto, branco;
    float posx[6], posy[6];
    float x0, x1, x2;
    float bussyDebug, raioDebug;
    
//-------------------------------OBJETOS----------------------------------------
    I2CPreInit gI2C;
    QEI roda1;
    QEI roda2;
    Adafruit_SSD1306_I2c gOled2;
    
//------------------------------UTILIDADES--------------------------------------
    enum estado{menu, localizador, thresholdEdit, bussola, FreqPot, FFT, exitTela, creditos, debug}estadoDaTela, estadoAnterior, menuSelect;
    bool alteracao;
    int optionSelect, optionSelectLocal, optionSelectThreshold, optionSelectFFT;
    int sentido1;
    int sentido2;
    bool click;
    
};

#endif