#ifndef GPIO_H
#define GPIO_H

#include "Common.h"

// [define]
// GP番号

//#define GP_8  8     // bme280_pico.hでSDAとして定義
//#define GP_9  9     // bme280_pico.hでSCLとして定義

#define GP_10 10    // GPIO         
#define GP_11 11    // GPIO
#define GP_12 12    // GPIO
#define GP_13 13    // GPIO
#define GP_14 14    // GPIO
#define GP_15 15    // GPIO

#define GP_25 25    // ONBOARD LED

#define GP_26 26    // ADC 
#define GP_27 27    // ADC
#define GP_28 28    // ADC

// ピン(GP番号)の機能選択
#define ONBOARD_LED GP_25

#pragma pack(1)

// [構造体]
// GPIOのGP番号と方向
typedef struct _ST_GPIO_PIN{
    ULONG gp;  // GP番号
    bool  dir; // true:出力 false:入力
} ST_GPIO_PIN;

// GPIO設定
typedef struct _ST_GPIO_CONFIG {
   ULONG pullDownBits;   // プルダウンかプルアップか 
   ULONG initialValBits; // 電源ON時出力値
} ST_GPIO_CONFIG;

#pragma pack()

// [関数プロトタイプ宣言]
ULONG GPIO_GetInDirBits();
ULONG GPIO_GetOutDirBits();
void GPIO_SetDefault(ST_GPIO_CONFIG *pstConfig);
void GPIO_Init(ST_GPIO_CONFIG *pstConfig);

#endif