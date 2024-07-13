#ifndef TIMER_H
#define TIMER_H

#include "Common.h"

// [define]
#define TIMER_STABILIZATION_WAIT_TIME 50 // 起動してからの安定待ち時間(ms) ※値は適当
#define TIMER_LED_PERIOD 500             // ノーマルモード時のLED点滅の周期(ms)

// [関数プロトタイプ宣言]
void TIMER_WdtClear();
bool TIMER_IsStabilizationWaitTimePassed();
void TIMER_ClearRecvTimeout(ULONG line);
bool TIMER_IsRecvTimeout(ULONG line);
void TIMER_ClearUsbSendTimeout();
bool TIMER_IsUsbSendTimeout();
bool TIMER_IsLedChangeTiming();
void TIMER_ClearLedTimer();
bool TIMER_IsSensorUpdateTiming();
void TIMER_ClearSensorTimer();
void TIMER_Init();

#endif