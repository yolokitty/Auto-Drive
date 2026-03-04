#include "drive.h"
#include "UL.h"

extern volatile uint8_t Flag1; // 직진
extern volatile uint8_t Flag2; // 정지
extern volatile uint8_t Flag3; // 우회전
extern volatile uint8_t Flag4; // 좌회전

void Drive_Control(void) {
    float L = HCSR04_GetDistance(0);
    float C = HCSR04_GetDistance(1);
    float R = HCSR04_GetDistance(2);

    // 1. 최우선순위: 전방 장애물 (정말 들이박을 것 같을 때만 정지)
    if (C > 0 && C < STOP_DIST) {
        Flag2 = 1; // 정지
    }
    // 2. 전방 회피 시작 (여유 있게 꺾기)
    else if (C > 0 && C < AVOID_DIST) {
        if (L >= R) Flag4 = 1; // 좌회전
        else Flag3 = 1;        // 우회전
    }
    // 3. 전방이 뚫려 있을 때 (C >= AVOID_DIST 이거나 0일 때)
    else {
        // 측면 장애물이 감지되어도 '정지'시키지 않고 '조향'만 합니다.
        if (L > 0 && L < SIDE_MARGIN) {
            Flag3 = 1; // 왼쪽이 가까우면 우회전 살짝 (멈추지 않음)
        }
        else if (R > 0 && R < SIDE_MARGIN) {
            Flag4 = 1; // 오른쪽이 가까우면 좌회전 살짝 (멈추지 않음)
        }
        else {
            Flag1 = 1; // 아무것도 없으면 시원하게 직진
        }
    }
}
