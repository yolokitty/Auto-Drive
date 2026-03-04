

#include "act.h"

extern volatile uint8_t Flag1;
extern volatile uint8_t Flag2;
extern volatile uint8_t Flag3;
extern volatile uint8_t Flag4;

extern TIM_HandleTypeDef htim3;

void Act()
{
    if (Flag1)
    {
    	Flag1 = 0;
        // 100 중에서 80만큼의 힘으로만 회전 (80% 속도)
    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    	TIM3->CCR1 = 800;
    	TIM3->CCR2 = 800;
    }
    if (Flag2)
    {
    	Flag2 = 0;
        // 정지
    	TIM3->CCR1 = 0;
    	TIM3->CCR2 = 0;
    }
    if (Flag3)
    {
    	Flag3 = 0;
    	TIM3->CCR1 = 800;
    	TIM3->CCR2 = 0;
    }
    if (Flag4)
    {
    	Flag4 = 0;
        // 정지
    	TIM3->CCR1 = 0;
    	TIM3->CCR2 = 800;
    }
}
