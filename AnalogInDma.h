#ifndef ANALOGINDMA_H
#define ANALOGINDMA_H

#include "platform/platform.h"


#include "hal/analogin_api.h"
#include "platform/SingletonPtr.h"
#include "platform/PlatformMutex.h"

class AnalogInDma {

public:


    AnalogInDma();
	bool init();
	bool start(uint32_t* pData, uint32_t Length);

    virtual ~AnalogInDma();

protected:
	bool MX_ADC1_Init();
	void MX_DMA_Init();
	
	ADC_HandleTypeDef hadc1;
};

#endif

