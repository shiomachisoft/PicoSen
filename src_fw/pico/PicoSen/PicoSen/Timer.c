#include "Common.h"

// [define] 
#define TIMER_CALLBACK_PERIOD   1    // 定期タイマコールバックの周期(ms)
#define TIMER_WDT_TIMEOUT       5000 // WDTタイマクリアのタイムアウト時間(ms) ※FLASHの消去・書き込みが間に合う時間にすること
#define TIMER_RECV_TIMEOUT      500  // 要求フレームのヘッダを受信後、TIMER_RECV_TIMEOUT[ms]経過しても要求フレームの末尾まで受信してない場合はタイムアウトとする
#define TIMER_USB_SEND_TIMEOUT  1000 // USB送信タイムアウト(ms)
#define TIMER_SENSOR_UPDATE_PERIOD 5000 // センサデータの更新周期(ms)

// [ファイルスコープ変数の宣言]
static repeating_timer_t f_stTimer = {0};       // 定期タイマコールバック登録時に渡すパラメータ
static ULONG f_timerCnt_wdt = 0;                // WDTタイマのタイマカウント
static ULONG f_timerCnt_stabilizationWait = 0;  // 起動してからの安定待ち時間のタイマカウント
static ULONG f_aTimerCnt_recvTimeout[E_FRM_LINE_NUM] = {0}; // 右記のタイマカウント:要求フレームのヘッダを受信後、TIMER_RECV_TIMEOUT[ms]経過しても要求フレームの末尾まで受信してない場合はタイムアウトとする
static ULONG f_timerCnt_usbSendTimeout = 0;     // USB送信タイムアウトのタイマカウント 
static ULONG f_timerCnt_led = 0;                // LED点滅のタイマカウント
static ULONG f_timerCnt_sensor = 0;             // センサデータの更新周期のタイマカウント

// [関数プロトタイプ宣言]
static bool Timer_PeriodicCallback(repeating_timer_t *rt);

// 定期タイマコールバック
static bool Timer_PeriodicCallback(repeating_timer_t *pstTimer) 
{
    ULONG i;

    // [WDTタイマのタイマカウント]
    if (f_timerCnt_wdt <  TIMER_WDT_TIMEOUT) { // タイムアウトしてない場合
        f_timerCnt_wdt++;
    }
    else { // タイムアウトした場合
        // watchdog_enable()を使用して即WDTタイムアウトで再起動する
        CMN_WdtEnableReboot();
    }

    // [起動してからの安定待ち時間のタイマカウント]
    if (f_timerCnt_stabilizationWait < TIMER_STABILIZATION_WAIT_TIME) {
        f_timerCnt_stabilizationWait++;
    }

    // 右記のタイマカウント:要求フレームのヘッダを受信後、TIMER_RECV_TIMEOUT[ms]経過しても要求フレームの末尾まで受信してない場合はタイムアウトとする
    for (i = 0; i < E_FRM_LINE_NUM; i++) {
        if (f_aTimerCnt_recvTimeout[i] < TIMER_RECV_TIMEOUT) {
            f_aTimerCnt_recvTimeout[i]++;
        }
    }

    // [USB送信タイムアウトのタイマカウント]
    if (f_timerCnt_usbSendTimeout < TIMER_USB_SEND_TIMEOUT) {
        f_timerCnt_usbSendTimeout++;
    }

    // [LED点滅のタイマカウント]
    if (f_timerCnt_led < TIMER_LED_PERIOD) {
        f_timerCnt_led++; 
    }
 
    // [センサデータの更新周期のタイマカウント]
    if (f_timerCnt_sensor < TIMER_SENSOR_UPDATE_PERIOD) {
        f_timerCnt_sensor++;
    }

    return true; // keep repeating
}

// WDTタイマのタイマカウントをクリア
void TIMER_WdtClear()
{
    f_timerCnt_wdt = 0;
}

// 起動してからの安定待ち時間が経過したかどうかを取得
bool TIMER_IsStabilizationWaitTimePassed()
{
    return (f_timerCnt_stabilizationWait >=TIMER_STABILIZATION_WAIT_TIME) ? true : false;
}

// 右記のタイマカウントをクリア:要求フレームのヘッダを受信後、TIMER_RECV_TIMEOUT[ms]経過しても要求フレームの末尾まで受信してない場合はタイムアウトとする
void TIMER_ClearRecvTimeout(ULONG line)
{
    f_aTimerCnt_recvTimeout[line] = 0;
}

// 右記のタイムアウトか否かを取得:要求フレームのヘッダを受信後、TIMER_RECV_TIMEOUT[ms]経過しても要求フレームの末尾まで受信してない場合はタイムアウトとする
bool TIMER_IsRecvTimeout(ULONG line)
{
    return (f_aTimerCnt_recvTimeout[line] >= TIMER_RECV_TIMEOUT) ? true : false;
}

// USB送信タイムアウトのタイマカウントをクリア
void TIMER_ClearUsbSendTimeout()
{
    f_timerCnt_usbSendTimeout = 0;
}

// USB送信タイムアウトか否かを取得
// ※本関数は、pico-sdk\src\rp2_common\pico_stdio_usb\stdio_usb.c のstdio_usb_out_chars()の内部で使用している
bool TIMER_IsUsbSendTimeout()
{
    return (f_timerCnt_usbSendTimeout >= TIMER_USB_SEND_TIMEOUT) ? true : false;
}

// LEDのON/OFFを変更するタイミングか否かを取得
bool TIMER_IsLedChangeTiming()
{
    return (f_timerCnt_led >= TIMER_LED_PERIOD) ? true : false;
}

// LED点滅のタイマカウントをクリア
void TIMER_ClearLedTimer()
{
    f_timerCnt_led = 0;
}

// センサデータ更新タイミングか否かを取得
bool TIMER_IsSensorUpdateTiming()
{
    return (f_timerCnt_sensor >= TIMER_SENSOR_UPDATE_PERIOD) ? true : false;
}

// センサデータの更新周期のタイマカウントをクリア
void TIMER_ClearSensorTimer()
{
    f_timerCnt_sensor = 0;
}

// タイマーを初期化
void TIMER_Init()
{
    // 定期タイマコールバックの登録
    add_repeating_timer_ms(TIMER_CALLBACK_PERIOD, Timer_PeriodicCallback, NULL, &f_stTimer);
}