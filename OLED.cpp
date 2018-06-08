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
    optionSelectFFT = 0;
    sentido1 = 0;
    sentido2 = 0;
    click = false;
	for(unsigned i = 0; i < 16; i++)
		filtro[i] = 1;
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

//float fftOutPos[128], 
void OLED::setFFT(float fftOut[16], float maxValue){
    for(int i = 0; i < 16; i++){
        this->fftOut[i] = fftOut[i];
    }
    this->maxValue = maxValue;
    alteracao = true;
}


float OLED::getFilter(unsigned i)
{
	return filtro[i];
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
//            case creditos:
//            //telaCreditos
//                rodaTelaCreditos();
//                break;
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
        else if(roda1.getRevolutions() != 0/* || roda2.getRevolutions() != 0*/){
            click = !click;
        }
        else
            alteracao = false;
        roda1.reset();
        roda2.reset();
        //debounce
        //wait(0.05);
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
    
    //opcao Bussola
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
    
    //opcao FFT
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
    
    //opcao Debug
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
    
//    opcao Creditos
//    if(optionSelect == 2){
//        gOled2.setTextColor(0, 1);
//        gOled2.drawFastHLine(18, 51, 60, 1);
//        if(click)
//            estadoDaTela = creditos;
//    }
//    else
//        gOled2.setTextColor(1);
//    gOled2.setTextCursor(18, 52);
//    gOled2.printf(" Creditos ");
    
    //resetar variaveis
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
    
    //opcao Bussola
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
    
    //opcao Threshold
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
    
    //opcao Exit
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
    
    //resetar variaveis
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
    //dist = distancia eixo x2x0
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
    //desenha circulo externo da bussola
    gOled2.fillCircle(bussx, bussy, raio, 1);
    gOled2.fillCircle(bussx, bussy, raio - 3, 0);
    //desenha a seta
    gOled2.fillTriangle(posx[0], posy[0], posx[1], posy[1], posx[2], posy[2], 0);
    gOled2.fillTriangle(posx[3], posy[3], posx[4], posy[4], posx[5], posy[5], 1);
    //imprime distancia da bussola
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
    //resetar variaveis
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
                *threshold = *threshold + 5;
            else if(sentido1 < 0 && *threshold > 30)
                *threshold = *threshold - 5;
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
    //resetar variaveis
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
        gOled2.printf("%d", int(freq));
        gOled2.setTextSize(1);
        gOled2.printf("Hz");
    }
    else{
        gOled2.setTextSize(2);
        gOled2.printf("%d.%02d", int(freq/1000), (int(freq) % 1000) / 10);
        
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
    gOled2.printf("dBx");
    
    gOled2.drawFastHLine(0, 55, 128, 1);
    gOled2.drawFastVLine(126, 56, 8, 1);
    gOled2.drawFastVLine(127, 56, 8, 1);
    gOled2.setTextCursor(0,56);
    gOled2.setTextColor(0,1);
    gOled2.printf("<-BUSSOLA            ");
    //resetar variaveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//----------------------------RODA TELA FFT-------------------------------------
void OLED::rodaTelaFFT(void){
    //lidar com o rotary
    //lidar com o click

    float valorHeight[16], filtroHeight[16];
    int view;
    
    if(not(click)){
        view = 64;
        if(sentido1 < 0){
            alteracao = true;
            estadoAnterior = FFT;
            estadoDaTela = exitTela;
        }
        else
            alteracao = false;
    }
    
    if(click){
        view = 56;
        alteracao = true;
        if(sentido1 > 0 && optionSelectFFT < 15)
            optionSelectFFT++;
        else if(sentido1 < 0 && optionSelectFFT > 0)
            optionSelectFFT--;
        if(sentido2 > 0 && *(filtro+optionSelectFFT) < 1)
            *(filtro + optionSelectFFT) += 0.1;
        else if(sentido2 < 0 && *(filtro+optionSelectFFT) > 0.05)
            *(filtro + optionSelectFFT) -= 0.1;
        else
            alteracao = false;
    }
    
    
    gOled2.drawFastHLine(0, 0, 128, 1);
    gOled2.drawFastVLine(0, 1, 8, 1);
    gOled2.drawFastVLine(127, 1, 8, 1);
    gOled2.setTextCursor(1, 1);
    gOled2.setTextColor(0, 1);
    if(click){
        switch(optionSelectFFT){
            case 0: gOled2.printf("119Hz       EDIT MODE"); break;
            case 1: gOled2.printf("238Hz       EDIT MODE"); break;
            case 2: gOled2.printf("357Hz       EDIT MODE"); break;
            case 3: gOled2.printf("476Hz       EDIT MODE"); break;
            case 4: gOled2.printf("715Hz       EDIT MODE"); break;
            case 5: gOled2.printf("953Hz       EDIT MODE"); break;
            case 6: gOled2.printf("1.3kHz      EDIT MODE"); break;
            case 7: gOled2.printf("1.7kHz      EDIT MODE"); break;
            case 8: gOled2.printf("2.3kHz      EDIT MODE"); break;
            case 9: gOled2.printf("3.6kHz      EDIT MODE"); break;
            case 10: gOled2.printf("5.0kHz      EDIT MODE"); break;
            case 11: gOled2.printf("7.5kHz      EDIT MODE"); break;
            case 12: gOled2.printf("10.0kHz     EDIT MODE"); break;
            case 13: gOled2.printf("13.1kHz     EDIT MODE"); break;
            case 14: gOled2.printf("18.1kHz     EDIT MODE"); break;
            case 15: gOled2.printf("25.0kHz     EDIT MODE"); break;
        }
        for(int i = 0; i < 16; i++){
            filtroHeight[i] = 64 * 0.70 * (*(filtro + i));
            valorHeight[i] = 64 * 0.70 * fftOut[i]/* * (*(filtro + i)) *// maxValue;
            gOled2.drawFastVLine(i*8+3, view + 1, 8, 1);
            if(int(valorHeight[i]) == 0)
                valorHeight[i] = 1;
            if(int(filtroHeight[i]) == 0)
                filtroHeight[i] = 1;
        }
        for(int i = 0; i < 4; i++)
            gOled2.drawFastHLine(0, 57+2*i, 128, 0);
        gOled2.fillRect(optionSelectFFT*8+1, view+1, 4, 8, 1);
    }
    else{
        gOled2.printf("<-EXIT      VIEW MODE");
        for(int i = 0; i < 16; i++){
            filtroHeight[i] = 64 * 0.80 * (*(filtro + i));
            valorHeight[i] = 64 * 0.80 * fftOut[i]/* * (*(filtro + i)) *// maxValue;
            if(int(valorHeight[i]) == 0)
                valorHeight[i] = 1;
            if(int(filtroHeight[i]) == 0)
                filtroHeight[i] = 1;
        }
    }

    for(int i = 0; i < 16; i++){
        //desenhar o valor
        gOled2.fillRect(i*8, view-valorHeight[i], 6, valorHeight[i], 1);
        //desenhar barra de filtro
        gOled2.drawFastHLine(i*8, view-filtroHeight[i], 4, 1);
        gOled2.drawFastHLine(i*8, view+1-filtroHeight[i], 6, 0);
        gOled2.drawFastVLine(i*8, view-filtroHeight[i], filtroHeight[i], 1);
    }
    
    //resetar variaveis
    sentido1 = 0;
    sentido2 = 0;
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
    //resetar variaveis
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
    //dist = distancia eixo x2x0
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
    //desenha circulo externo da bussola
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
    //facilidade de ver qual microfone eh o primeiro
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
    //resetar variaveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}

//------------------------------------------------------------------------------
//---------------------------RODA TELA CREDITOS---------------------------------

void OLED::rodaTelaCreditos(void){
    //lidar com o rotary
    if(sentido1 != 0 || click){
        alteracao = true;
        estadoDaTela = menuSelect;
    }
    else{
        alteracao = false;
    }
    gOled2.setTextColor(1);
    gOled2.setTextSize(1);
    gOled2.setTextCursor(25, 0);
    gOled2.printf("Carolina Dias\n");
    gOled2.setTextCursor(25, 14);
    gOled2.printf("Gunar Kroeger\n");
    gOled2.setTextCursor(25, 28);
    gOled2.printf("Oliver Penner\n");
    gOled2.setTextCursor(25, 42);
    gOled2.printf("Bruno S Chang\n");
    gOled2.setTextCursor(25, 56);
    gOled2.printf("Hermes Monego\n");
    
    //resetar variaveis
    sentido1 = 0;
    sentido2 = 0;
    click = false;
}