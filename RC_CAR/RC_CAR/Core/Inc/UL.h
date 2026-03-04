///* UL.h */
//#ifndef __UL_H
//#define __UL_H
//
//#include "main.h"
//#include "tim.h"
//
//// 설정 값
//#define TRIG_PORT GPIOA
//#define TRIG_PIN  GPIO_PIN_5
//#define UL_TIMER  &htim4
//#define UL_CHANNEL TIM_CHANNEL_1
//
//// 함수 선언
//void HCSR04_Trigger(void);
//float HCSR04_GetDistance(void);
//void HCSR04_Process(uint32_t interval_ms); // main의 while문에서 호출
//
//#endif /* __UL_H */


#ifndef __UL_H
#define __UL_H

#include "main.h"

// 센서 데이터를 통합 관리하는 구조체
typedef struct {
    GPIO_TypeDef* TrigPort;
    uint16_t      TrigPin;
    uint32_t      Channel;     // TIM_CHANNEL_1, 2, 4
    float         Distance;
    uint8_t       CaptureFlag;
    uint32_t      IC_Val1;
    uint32_t      IC_Val2;
} HCSR04_t;

// 함수 선언
void HCSR04_Trigger(uint8_t idx);
void HCSR04_Process(uint32_t interval_ms);
float HCSR04_GetDistance(uint8_t idx);

#endif
