
#include "UL.h"
#include "tim.h"
#include <stdio.h>

// main.c에 정의된 타이머 핸들러 호출
extern TIM_HandleTypeDef htim4;

// 3개의 센서 설정: Trig(PB3,4,5) / Echo(CH 1,2,4 -> PB6,7,9)
HCSR04_t sensors[3] = {
    {GPIOD, GPIO_PIN_2, TIM_CHANNEL_2, 0, 0, 0, 0}, // [0] 왼쪽: PB4(T) / PB7(E)
    {GPIOA, GPIO_PIN_5, TIM_CHANNEL_1, 0, 0, 0, 0}, // [1] 중앙: PA5(T) / PB6(E)
    {GPIOB, GPIO_PIN_5, TIM_CHANNEL_4, 0, 0, 0, 0}  // [2] 오른쪽: PB5(T) / PB9(E)
};

void HCSR04_Trigger(uint8_t idx) {
    if (idx >= 3) return;

    // 1. 모든 채널 인터럽트 강제 종료 (CPU 부하 감소 및 간섭 차단)
    __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC4);

    HCSR04_t* s = &sensors[idx];
    s->CaptureFlag = 0;

    // 2. 타이머 상태 청소
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC4);
    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4, s->Channel, TIM_INPUTCHANNELPOLARITY_RISING);

    // 3. 트리거 펄스 (for문 숫자를 줄여 CPU 점유율 낮춤)
    HAL_GPIO_WritePin(s->TrigPort, s->TrigPin, GPIO_PIN_SET);
    for(volatile int i=0; i<500; i++); // 10us만 확보되면 충분합니다.
    HAL_GPIO_WritePin(s->TrigPort, s->TrigPin, GPIO_PIN_RESET);

    // 4. 현재 채널만 다시 활성화
    uint32_t it_ch = (idx == 0) ? TIM_IT_CC2 : (idx == 1) ? TIM_IT_CC1 : TIM_IT_CC4;
    __HAL_TIM_ENABLE_IT(&htim4, it_ch);
}

void HCSR04_Process(uint32_t interval_ms) {
    static uint32_t lastTick = 0;
    static uint8_t step = 0;
    // 시퀀스: 중앙(1) -> 왼쪽(0) -> 중앙(1) -> 오른쪽(2)
    const uint8_t seq[] = {1, 0, 1, 2};

    if (HAL_GetTick() - lastTick >= interval_ms) {
        lastTick = HAL_GetTick();

        HCSR04_Trigger(seq[step]);

        // 모든 방향을 한 번씩 훑었을 때(4단계 완료 시) 출력
        if (step == 3) {
            printf("L:%3d C:%3d R:%3d\r\n",
                   (int)sensors[0].Distance, (int)sensors[1].Distance, (int)sensors[2].Distance);
        }

        step = (step + 1) % 4;
    }
}

// 타이머 캡처 콜백 함수
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        uint8_t idx;
        uint32_t channel;
        uint32_t active_ch = htim->Channel;

        // 1. 채널 판별 (정확한 매칭)
        if (active_ch == HAL_TIM_ACTIVE_CHANNEL_2)      { idx = 0; channel = TIM_CHANNEL_2; }
        else if (active_ch == HAL_TIM_ACTIVE_CHANNEL_1) { idx = 1; channel = TIM_CHANNEL_1; }
        else if (active_ch == HAL_TIM_ACTIVE_CHANNEL_4) { idx = 2; channel = TIM_CHANNEL_4; }
        else return;

        HCSR04_t* s = &sensors[idx];

        // 2. 캡처 로직
        if (s->CaptureFlag == 0) { // Rising Edge
            s->IC_Val1 = HAL_TIM_ReadCapturedValue(htim, channel);
            s->CaptureFlag = 1;
            // 하강 엣지로 변경
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (s->CaptureFlag == 1) { // Falling Edge
            s->IC_Val2 = HAL_TIM_ReadCapturedValue(htim, channel);

            uint32_t diff;
            if (s->IC_Val2 >= s->IC_Val1) diff = s->IC_Val2 - s->IC_Val1;
            else diff = (0xFFFF - s->IC_Val1) + s->IC_Val2;

            float dist = (float)diff * 0.017f; // (diff / 58.0f)와 동일

            // 유효 범위 필터링 (비정상적인 큰 값 무시)
            if (dist > 2.0f && dist < 400.0f) {
                s->Distance = dist;
            }

            s->CaptureFlag = 0;
            // 다시 상승 엣지로 변경
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_RISING);
            // 측정 완료 후 인터럽트 끄기 (트리거에서 다시 킴)
            uint32_t it_ch = (idx == 0) ? TIM_IT_CC2 : (idx == 1) ? TIM_IT_CC1 : TIM_IT_CC4;
            __HAL_TIM_DISABLE_IT(htim, it_ch);
        }
    }
}

float HCSR04_GetDistance(uint8_t idx) {
    return (idx < 3) ? sensors[idx].Distance : 0;
}

// 센서 구조체에 배열 추가
float filter_buf[3][3]; // [센서idx][최근3개값]

float Get_Filtered_Distance(uint8_t idx, float new_dist) {
    static uint8_t buf_idx[3] = {0, 0, 0};

    // 새로운 값 저장
    filter_buf[idx][buf_idx[idx]] = new_dist;
    buf_idx[idx] = (buf_idx[idx] + 1) % 3;

    // 평균 내기
    return (filter_buf[idx][0] + filter_buf[idx][1] + filter_buf[idx][2]) / 3.0f;
}
