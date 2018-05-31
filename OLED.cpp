#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "QEI.h"
#include "OLED.h"

//------------------------------------------------------------------------------
//-----------------------------CONSTRUTOR---------------------------------------

OLED::OLED( PinName sda, PinName scl, 
            PinName channelA, PinName channelB, PinName index, 
            PinName channelA2, PinName channelB2, PinName index2) 
            :gI2C(sda, scl), 
            roda1(channelA, channelB, index, 20), 
            roda2(channelA2, channelB2, index2, 20),
            gOled2(gI2C, NC){
                
    bussx = 32;
    bussy = 26;
    raio = 25;
    bussyDebug = 32;
    raioDebug = 31;
    preto = 3.5;
    branco = 1.5;            
    
    alteracao = true;
    menuSelect = menu;
    estadoDaTela = menu;
    optionSelect = 0;
    optionSelectThreshold = 0;
    optionSelectLocal = 0;
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//-----------------------------GETS E SETS--------------------------------------

bool OLED::getAlteracao(void){
    return alteracao;
}

void OLED::setAlteracao(void){
    alteracao = true;
}

void OLED::setX(float X[3]){
    for(int i = 0; i < 3; i++){
        this->X[i] = X[i];    
    }
    alteracao = true;
}

void OLED::setFreqPot(float freq, float pot){
    this->freq = freq;
    this->pot = pot;
    alteracao = true;
}

void OLED::setFFT(float fftOut[128], float maxValue){
    for(int i = 0; i < 128; i++){
        this->fftOut[i] = fftOut[i];    
    }    
    this->maxValue = maxValue;
    alteracao = true;
}

void OLED::setC(int c){
    this->c = c;
    alteracao = true;
}

void OLED::setThreshold(int &threshold){
    this->threshold = &threshold;
    alteracao = true;
    }
//void OLED::setPos(Pos X){
//    this.X = X;
//}
//------------------------------------------------------------------------------
//-----------------------------RODA TELA----------------------------------------

void OLED::rodaTela(){
    if(alteracao){
        gOled2.clearDisplay();
        switch(estadoDaTela){
            case menu:
            //menu
                rodaTelaMenu();
                break;
            case localizador:
            //TelaLocalizador
                rodaTelaLocalizador();
                break;
            case bussola:
            //TelaBussola
                rodaTelaBussola();
                break;
            case thresholdEdit:
            //TelaThresholdEdit
                rodaTelaThresholdEdit();
                break;
            case FreqPot:
            //TelaFreqPot
                rodaTelaFreqPot();
                break;
            case FFT:
            //TelaFFT
                rodaTelaFFT();
                break;
            case exitTela:
            //telaExit
                rodaTelaExit();
                break;
            case debug:
            //TelaDebug
                rodaTelaDebug();
                break;
        }
        gOled2.display();
    }
    else{
        alteracao = true;
        if(roda1.getPulses() > 0){
            sentido1--;
        }
        else if(roda1.getPulses() < 0){
            sentido1++;
        }
        else if(roda2.getPulses() > 0){
            sentido2--;
        }
        else if(roda2.getPulses() < 0){
            sentido2++;
        }
        else if(roda1.getRevolutions() != 0){
            click = !click;
        }
        else
            alteracao = false;
        roda1.reset();
        roda2.reset();
        //debounce
        wait(0.05);
    }
}

//------------------------------------------------------------------------------
//--------------------------RODA TELA MENU--------------------------------------

void OLED::rodaTelaMenu(void){
    //escrever na cor certa e no lugar certo e no tamanho certo
    menuSelect = menu;
    gOled2.setTextSize(1);
    gOled2.setTextCursor(0, 4);
    gOled2.setTextColor(1);
    gOled2.printf("MENU:");
    
    //lidar com o rotary
    if(sentido1 != 0){
        if(not(optionSelect + sentido1 > 2) && not(optionSelect + sentido1 < 0))
            optionSelect = optionSelect + sentido1;
        alteracao = false;
    }

    //lidar com o click
    if(not(click))
        alteracao = false;
    
    //opção Bussola
    if(optionSelect == 0){
        gOled2.setTextColor(0, 1);
        gOled2.drawFastHLine(18, 19, 78, 1);
        if(click)
            estadoDaTela = localizador;
    }
    else
        gOled2.setTextColor(1);
    gOled2.setTextCursor(18, 20);
    gOled2.printf(" Localizador ");
    
    //opção FFT
    if(optionSelect == 1){
        gOled2.setTextColor(0, 1);
        gOled2.drawFastHLine(18, 35, 30, 1);
        if(click)
            estadoDaTela = FFT;
    }
    else
        gOled2.setTextColor(1);
    gOled2.setTextCursor(18, 36);
    gOled2.printf(" FFT ");
    
    //opção Debug
    if(optionSelect == 2){
        gOled2.setTextColor(0, 1);
        gOled2.drawFastHLine(18, 51, 42, 1);
        if(click)
            estadoDaTela = debug;
    }
    else
        gOled2.setTextColor(1);
    gOled2.setTextCursor(18, 52);
    gOled2.printf(" Debug ");
    
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//--------------------------RODA TELA LOCALIZADOR-------------------------------

void OLED::rodaTelaLocalizador(void){
    //escrever na cor certa e no lugar certo e no tamanho certo
    menuSelect = localizador;
    gOled2.setTextSize(1);
    gOled2.setTextCursor(0, 4);
    gOled2.setTextColor(1);
    gOled2.printf("LOCALIZADOR:");
    
    //lidar com o rotary
    if(sentido1 != 0){
        if(not(optionSelectLocal + sentido1 > 2) && not(optionSelectLocal + sentido1 < 0))
            optionSelectLocal = optionSelectLocal + sentido1;
        alteracao = false;
    }

    //lidar com o click
    if(not(click))
        alteracao = false;
    
    //opção Bussola
    if(optionSelectLocal == 0){
        gOled2.setTextColor(0, 1);
        gOled2.drawFastHLine(18, 19, 54, 1);
        if(click)
            estadoDaTela = bussola;
    }
    else
        gOled2.setTextColor(1);
    gOled2.setTextCursor(18, 20);
    gOled2.printf(" Bussola ");
    
    //opção Threshold
    if(optionSelectLocal == 1){
        gOled2.setTextColor(0, 1);
        gOled2.drawFastHLine(18, 35, 66, 1);
        if(click)
            estadoDaTela = thresholdEdit;
    }
    else
        gOled2.setTextColor(1);
    gOled2.setTextCursor(18, 36);
    gOled2.printf(" Threshold ");
    
    //opção Exit
    if(optionSelectLocal == 2){
        gOled2.setTextColor(0, 1);
        gOled2.drawFastHLine(18, 51, 36, 1);
        if(click){
            estadoDaTela = menu;
            optionSelectLocal = 0;
        }
    }
    else
        gOled2.setTextColor(1);
    gOled2.setTextCursor(18, 52);
    gOled2.printf(" Exit ");
    
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//-------------------------RODA TELA BUSSOLA------------------------------------

void OLED::rodaTelaBussola(void){
    //lidar com o rotary
    if(sentido1 != 0){
        alteracao = true;
        estadoAnterior = bussola;
        if(sentido1 < 0)
            estadoDaTela = exitTela;
        else
            estadoDaTela = FreqPot;
    }
    else
        alteracao = false;
    x0 = X[0];  //x0 --> direita
    x1 = X[1];  //x1 --> cima
    x2 = X[2];  //x2 --> frente 
    //dist = distância eixo x2x0
    dist = x2*x2 + x0*x0;
    dist = sqrt(dist);
    //colocando eixo x2x0 na norma
    x2 = x2/dist;
    x0 = x0/dist;
    //definindo o triangulo do ponteiro
    posx[0] = bussx;
    posy[0] = bussy;
    posx[1] = bussx + x0*raio + x2*preto;
    posy[1] = bussy - x2*raio + x0*preto;
    posx[2] = bussx + x0*raio - x2*preto;
    posy[2] = bussy - x2*raio - x0*preto;
    posx[3] = bussx;
    posy[3] = bussy;
    posx[4] = bussx + x0*raio + x2*branco;
    posy[4] = bussy - x2*raio + x0*branco;
    posx[5] = bussx + x0*raio - x2*branco;
    posy[5] = bussy - x2*raio - x0*branco;
    float * pontx[6], * ponty[6];
    for(int i = 0; i < 6; i++)
        pontx[i] = &posx[i];
    for(int i = 0; i < 6; i++)
        ponty[i] = &posy[i];
    //colocando valores de y decrescentes
    if(posy[0] < posy[1]){
        swap(*ponty[0], *ponty[1]);
        swap(*pontx[0], *pontx[1]);
        swap(*ponty[3], *ponty[4]);
        swap(*pontx[3], *pontx[4]);
    }
    if(posy[1] < posy[2]){
        swap(*ponty[1], *ponty[2]);
        swap(*pontx[1], *pontx[2]);
        swap(*ponty[4], *ponty[5]);
        swap(*pontx[4], *pontx[5]);
    }
    if(posy[0] < posy[1]){
        swap(*ponty[0], *ponty[1]);
        swap(*pontx[0], *pontx[1]);
        swap(*ponty[3], *ponty[4]);
        swap(*pontx[3], *pontx[4]);
    }
    //desenha círculo externo da bussola
    gOled2.fillCircle(bussx, bussy, raio, 1);
    gOled2.fillCircle(bussx, bussy, raio - 3, 0);
    //desenha a seta
    gOled2.fillTriangle(posx[0], posy[0], posx[1], posy[1], posx[2], posy[2], 0);
    gOled2.fillTriangle(posx[3], posy[3], posx[4], posy[4], posx[5], posy[5], 1);
    //imprime distância da bússola
    gOled2.setTextColor(1);
    gOled2.setTextSize(1);
    gOled2.setTextCursor(68, 0);
    gOled2.printf("DIST:");
    gOled2.setTextSize(2);
    gOled2.setTextCursor(68, 8);
    if(dist<1000)
        gOled2.printf("%dcm", int (dist/10));
    else
        gOled2.printf("%d.%dm", int(dist/1000), (int(dist) % 1000) / 100);
    //imprime o eixo x1
    gOled2.setTextSize(1);
    gOled2.setTextCursor(68, 28);
    if(x1>0)
        gOled2.printf("UP:");
    else if(x1<0)
        gOled2.printf("DOWN:");
    gOled2.setTextCursor(68, 36);
    gOled2.setTextSize(2);
    if(abs(x1)<1000)
        gOled2.printf("%dcm", abs(int (x1/10)));
    else
		gOled2.printf("%d.%dm", int(abs(x1/1000)), abs(int(x1) % 1000) / 100);

    gOled2.drawFastHLine(0, 55, 128, 1);
    gOled2.drawFastVLine(126, 56, 8, 1);
    gOled2.drawFastVLine(127, 56, 8, 1);
    gOled2.setTextColor(0, 1);
    gOled2.setTextSize(1);
    gOled2.setTextCursor(0,56);
    gOled2.printf("<-EXIT   FREQ E POT->");
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//---------------------------RODA TELA THRESHOLD--------------------------------

void OLED::rodaTelaThresholdEdit(void){
    //lidar com o rotary
    if(not(click)){
        if(sentido1 != 0){
            if(not(optionSelectThreshold + sentido1 > 1) && not(optionSelectThreshold + sentido1 < 0))
                optionSelectThreshold = optionSelectThreshold + sentido1;
        }
        alteracao = false;
    }
    
    gOled2.drawFastHLine(0, 0, 128, 1);
    gOled2.drawFastVLine(0, 1, 8, 1);
    gOled2.drawFastVLine(127, 1, 8, 1);
    gOled2.setTextCursor(1, 1);
    gOled2.setTextColor(0, 1);
    if(click)
        gOled2.printf("EDIT MODE: ON        ");
    else
        gOled2.printf("EDIT MODE: OFF       ");
    //write Threshold
    gOled2.setTextSize(1);
    gOled2.setTextCursor(0, 16);
    gOled2.setTextColor(1);
    if(optionSelectThreshold == 0){
        if(click){
            alteracao = false;
            gOled2.setTextColor(1);
            gOled2.printf(" Threshold: ");
            if(sentido1 > 0)
                *threshold = *threshold + 1;
            else if(sentido1 < 0)
                *threshold = *threshold - 1;
            gOled2.setTextColor(0, 1);
            gOled2.setTextSize(2);
            gOled2.printf("%d", *threshold);
        }
        else{
            gOled2.setTextSize(1);
            gOled2.setTextColor(0,1);
            gOled2.drawFastHLine(0, 15, 66, 1);
            gOled2.printf(" Threshold:");
            gOled2.setTextColor(1);
            gOled2.printf(" %d", *threshold);
        }
    }
    if(optionSelectThreshold != 0)
        gOled2.printf(" Threshold: %d", *threshold);
    gOled2.setTextCursor(0, 32);
    gOled2.setTextSize(1);
    gOled2.setTextColor(1);
    if(optionSelectThreshold == 1){
        gOled2.setTextColor(0, 1);
        gOled2.drawFastHLine(0, 31, 36, 1);
        if(click){
            click = false;
            estadoDaTela = localizador;
            optionSelectThreshold = 0;
        }
    }
    gOled2.printf(" Exit ");
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
}

//------------------------------------------------------------------------------
//---------------------------RODA TELA FREQPOT----------------------------------

void OLED::rodaTelaFreqPot(void){
    //lidar com o rotary
    if(sentido1 < 0){
        alteracao = true;
        estadoDaTela = estadoAnterior;
    }
    else{
        alteracao = false;
    }
    //write Freq
    gOled2.setTextColor(1);
    gOled2.setTextSize(1);
    gOled2.setTextCursor(0, 8);
    gOled2.printf("Freq: ");
    if(freq < 1000){
        gOled2.setTextSize(2);
    	gOled2.printf("%d", int(freq/1000));
        gOled2.setTextSize(1);
        gOled2.printf("Hz");
    }
    else{
        gOled2.setTextSize(2);
    	gOled2.printf("%d.%d", int(freq/1000), (int(freq) % 1000) / 10);
		
        gOled2.setTextSize(1);
        gOled2.printf("kHz");
    }
    //write Pot
    gOled2.setTextCursor(0, 32);
    gOled2.setTextSize(1);
    gOled2.printf("Pot: ");
    gOled2.setTextSize(2);
    gOled2.printf("%d.%d", int(pot/1000), (int(pot) % 1000) / 100);
    gOled2.setTextSize(1);
    gOled2.printf("dB");
    
    gOled2.drawFastHLine(0, 55, 128, 1);
    gOled2.drawFastVLine(126, 56, 8, 1);
    gOled2.drawFastVLine(127, 56, 8, 1);
    gOled2.setTextCursor(0,56);
    gOled2.setTextColor(0,1);
    gOled2.printf("<-BUSSOLA            ");
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//----------------------------RODA TELA FFT-------------------------------------
void OLED::rodaTelaFFT(void){
    //lidar com o rotary
    //lidar com o click
    //resetar variáveis
    if(sentido1 != 0){
        alteracao = true;
        estadoAnterior = FFT;
        if(sentido1 < 0)
            estadoDaTela = exitTela;
    }
    else
        alteracao = false;
    
    float heightMult = (64) / maxValue;
    for(int i = 0; i < 128; i++){
        gOled2.drawFastVLine(i, 64, -fftOut[i]*heightMult, 1);
    }
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;    
}

//------------------------------------------------------------------------------
//----------------------------RODA TELA EXIT------------------------------------

void OLED::rodaTelaExit(void){
    //escrever na cor certa e no lugar certo e no tamanho certo
    gOled2.setTextCursor(0,16);
    gOled2.setTextSize(4);
    gOled2.setTextColor(1);
    gOled2.printf("Exit?");
    
    //lidar com o rotary
    if(sentido1 > 0){
        alteracao = true;
        estadoDaTela = estadoAnterior;
    }
    else
        alteracao = false;
    //lidar com o click
    if(click){
        estadoDaTela = menuSelect;
        alteracao = true;
    }
    gOled2.drawFastHLine(0, 55, 128, 1);
    gOled2.drawFastVLine(126, 56, 8, 1);
    gOled2.drawFastVLine(127, 56, 8, 1);
    gOled2.setTextSize(1);
    gOled2.setTextColor(0,1);
    gOled2.setTextCursor(0,56);
    gOled2.printf("               BACK->");
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//---------------------------RODA TELA DEBUG------------------------------------

void OLED::rodaTelaDebug(void){
    //escrever na cor certa e no lugar certo e no tamanho certo
    //lidar com o rotary
    //lidar com o click
    if(sentido1 != 0){
        alteracao = true;
        estadoAnterior = debug;
        if(sentido1 < 0)
            estadoDaTela = exitTela;
        else
            estadoDaTela = FreqPot;
    }
    else
        alteracao = false;
    x0 = X[0];  //x0 --> direita
    x1 = X[1];  //x1 --> cima
    x2 = X[2];  //x2 --> frente 
    //dist = distância eixo x2x0
    dist = x2*x2 + x0*x0;
    dist = sqrt(dist);
    //colocando eixo x2x0 na norma
    x2 = x2/dist;
    x0 = x0/dist;
    //definindo o triangulo do ponteiro
    posx[0] = bussx;
    posy[0] = bussyDebug;
    posx[1] = bussx + x0*raioDebug + x2*preto;
    posy[1] = bussyDebug - x2*raioDebug + x0*preto;
    posx[2] = bussx + x0*raioDebug - x2*preto;
    posy[2] = bussyDebug - x2*raioDebug - x0*preto;
    posx[3] = bussx;
    posy[3] = bussyDebug;
    posx[4] = bussx + x0*raioDebug + x2*branco;
    posy[4] = bussyDebug - x2*raioDebug + x0*branco;
    posx[5] = bussx + x0*raioDebug - x2*branco;
    posy[5] = bussyDebug - x2*raioDebug - x0*branco;
    //colocando valores de y decrescentes
    float * pontx[6], * ponty[6];
    for(int i = 0; i < 6; i++)
        pontx[i] = &posx[i];
    for(int i = 0; i < 6; i++)
        ponty[i] = &posy[i];
    //colocando valores de y decrescentes
    if(posy[0] < posy[1]){
        swap(*ponty[0], *ponty[1]);
        swap(*pontx[0], *pontx[1]);
        swap(*ponty[3], *ponty[4]);
        swap(*pontx[3], *pontx[4]);
    }
    if(posy[1] < posy[2]){
        swap(*ponty[1], *ponty[2]);
        swap(*pontx[1], *pontx[2]);
        swap(*ponty[4], *ponty[5]);
        swap(*pontx[4], *pontx[5]);
    }
    if(posy[0] < posy[1]){
        swap(*ponty[0], *ponty[1]);
        swap(*pontx[0], *pontx[1]);
        swap(*ponty[3], *ponty[4]);
        swap(*pontx[3], *pontx[4]);
    }
    //desenha círculo externo da bussola
    gOled2.fillCircle(bussx, bussyDebug, raioDebug, 1);
    gOled2.fillCircle(bussx, bussyDebug, raioDebug - 3, 0);
    //desenha a seta
    gOled2.fillTriangle(posx[0], posy[0], posx[1], posy[1], posx[2], posy[2], 0);
    gOled2.fillTriangle(posx[3], posy[3], posx[4], posy[4], posx[5], posy[5], 1);
    //imprime x0, x1, x2, c
    gOled2.setTextSize(1);
    gOled2.setTextColor(1);
    gOled2.setTextCursor(75, 8);
    gOled2.printf("X:%d", int(X[0]));
    gOled2.setTextCursor(75, 24);
    gOled2.printf("Y:%d", int(X[2]));
    gOled2.setTextCursor(75, 40);
    gOled2.printf("Z:%d", int(X[1]));
    gOled2.setTextCursor(75, 54);
    gOled2.printf("c:%d", c);
    //facilidade de ver qual microfone é o primeiro
    switch(c){
        case 0:
            gOled2.fillCircle(64, 0, 5, 1);
            break;
        case 1:
            gOled2.fillCircle(0, 64, 5, 1);
            break;
        case 2:
            gOled2.fillCircle(128, 64, 5, 1);
            break;
        case 3:
            gOled2.fillCircle(0, 0, 5, 1);
            break;
        case 4:
            gOled2.fillCircle(128, 0, 5, 1);
            break;
    }
    //resetar variáveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}