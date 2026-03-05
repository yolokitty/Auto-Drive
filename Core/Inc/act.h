
#ifndef INC_ACT_H_
#define INC_ACT_H_

#include "main.h"
#include "UL.h"

extern volatile uint8_t Flag1;
extern volatile uint8_t Flag2;
extern volatile uint8_t Flag3;
extern volatile uint8_t Flag4;
extern TIM_HandleTypeDef htim3;

void Act();

#endif /* INC_ACT_H_ */
