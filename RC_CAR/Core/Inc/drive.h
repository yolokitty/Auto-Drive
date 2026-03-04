#ifndef __DRIVE_H
#define __DRIVE_H

#include "main.h"

#define STOP_DIST    6   // 정지 거리
#define AVOID_DIST   48   // 회피 시작 거리
#define SIDE_BLOCK   7   // 측면이 "막혔다"고 판단하는 기준
#define SIDE_MARGIN  8   // 단순 측면 보정 마진

void Drive_Control(void);

#endif /* INC_DRIVE_H_ */
