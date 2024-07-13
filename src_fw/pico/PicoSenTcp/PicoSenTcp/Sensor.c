#include "common.h"

// [define]
#define SEN_ADC_CH_NUM_WITHOUT_TEMP 3 // ADCのチャンネル数(温度センサ含まない)
#define SEN_ADC_CH_NUM 4              // ADCのチャンネル数(温度センサ含む)
#define SEN_BME280_DATA_NUM 3         // BME280のデータの数(温度,気圧,湿度)

// [ファイルスコープ変数]
static bool f_isBme280Inited = false;
static struct bme280_dev *f_dev = NULL;
static uint32_t f_req_delay = 0; // Variable to store minimum wait time between consecutive measurement in force mode
static char f_sendBuf[SEN_BME280_DATA_BUF_SIZE] = {0};

// [関数プロトタイプ宣言]
static void SEN_GetAdc(float *pDataAry);
static void SEN_GetBme280Data(float *pDataAry);

// センサのメイン処理
// センサデータを取得して送信する
void SEN_Main()
{
    char lvl_gp10, lvl_gp11, lvl_gp12, lvl_gp13, lvl_gp14, lvl_gp15;
    float aAdcData[SEN_ADC_CH_NUM] = {0}; // 電圧とオンボードの温度センサの温度
    float aBme280Data[SEN_BME280_DATA_NUM] = {0}; // BME280のデータ(温度,気圧,湿度)
    ULONG gpioValBits = 0;           

    // [センサデータを取得]
    // GPIO入力を取得
    gpioValBits = gpio_get_all();
    lvl_gp10 = (gpioValBits & (1UL << (ULONG)GP_10)) ? 'H' : 'L';  
    lvl_gp11 = (gpioValBits & (1UL << (ULONG)GP_11)) ? 'H' : 'L'; 
    lvl_gp12 = (gpioValBits & (1UL << (ULONG)GP_12)) ? 'H' : 'L';  
    lvl_gp13 = (gpioValBits & (1UL << (ULONG)GP_13)) ? 'H' : 'L'; 
    lvl_gp14 = (gpioValBits & (1UL << (ULONG)GP_14)) ? 'H' : 'L';   
    lvl_gp15 = (gpioValBits & (1UL << (ULONG)GP_15)) ? 'H' : 'L'; 

    // 電圧とオンボードの温度センサの温度を取得
    SEN_GetAdc(aAdcData);

    // BME280のデータ(温度,気圧,湿度)を取得
    SEN_GetBme280Data(aBme280Data);

    // [データをTCP送信]
    memset(f_sendBuf, 0, sizeof(f_sendBuf));
    sprintf(f_sendBuf, 
        "<GP10> %c <GP11> %c <GP12> %c <GP13> %c <GP14> %c <GP15> %c <ADC0[V]> %0.2f <ADC1[V]> %0.2f <ADC2[V]> %0.2f <ADC4[degC]> %0.2f <BME280[degC]> %0.2f <BME280[hPa]> %0.2f <BME280[%%]> %0.2f\r\n",
        lvl_gp10, lvl_gp11, lvl_gp12, lvl_gp13, lvl_gp14, lvl_gp15,
        aAdcData[0], aAdcData[1], aAdcData[2], aAdcData[3], 
        aBme280Data[0], aBme280Data[1], aBme280Data[2]);
    FRM_ReqToSend(E_FRM_LINE_TCP_SERVER, (PVOID)f_sendBuf, strlen(f_sendBuf));
}

// 電圧とオンボードの温度センサの温度を取得
static void SEN_GetAdc(float *pDataAry)
{
    ULONG i;
    float adcVal = 0; // AD変換値
    const float conversionFactor = 3.3f / (1 << 12); // 12-bit conversion, assume max value == ADC_VREF == 3.3 V

    // 電圧を取得
    for (i = 0; i < SEN_ADC_CH_NUM_WITHOUT_TEMP; i++) {
        adc_select_input(i);
        adcVal = (float)adc_read();
        pDataAry[i] = adcVal * conversionFactor;
    }
    // オンボードの温度センサの温度を取得
    adc_select_input(4);
    adcVal = (float)adc_read() * conversionFactor;
    pDataAry[SEN_ADC_CH_NUM_WITHOUT_TEMP] = 27.0f - (adcVal - 0.706f) / 0.001721f;
}

// BME280のデータ(温度,気圧,湿度)を取得
static void SEN_GetBme280Data(float *pDataAry)
{
    int8_t rslt = BME280_OK;
    float temp, press, hum;
    struct bme280_data comp_data; // Structure to get the pressure, temperature and humidity values

    if (!f_isBme280Inited) {
        goto __end;
    }

    /* Set the sensor to forced mode */
    rslt = bme280_set_sensor_mode(BME280_POWERMODE_FORCED, f_dev);
    if (rslt != BME280_OK) {
        goto __end;
    }

    /* Wait for the measurement to complete and print data */
    f_dev->delay_us(f_req_delay, f_dev->intf_ptr);
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, f_dev);
    if (rslt != BME280_OK)
    {
        goto __end;
    }

    // センサデータのUSB/無線送信要求を発行
    temp = comp_data.temperature;
    press = 0.01 * comp_data.pressure;
    hum = comp_data.humidity;

    pDataAry[0] = temp;
    pDataAry[1] = press;
    pDataAry[2] = hum;

__end:    
    return;
}

// センサを初期化
void SEN_Init()
{
    int8_t rslt = BME280_OK;
    struct bme280_settings settings = {0}; // Structure to store the settings
  
    // [ADCを初期化]
    adc_init();
    adc_gpio_init(GP_26);
    adc_gpio_init(GP_27);
    adc_gpio_init(GP_28);
    adc_set_temp_sensor_enabled(true);  

    // [BME280を初期化]
    // Initialize pico dependent parts for BME280 API 
    f_dev = bme280_user_init();

    /* Initialize the bme280 */
    rslt = bme280_init(f_dev);

    /* Get the current sensor settings */
    rslt = bme280_get_sensor_settings(&settings, f_dev);
    if (rslt != BME280_OK) {
        goto __end;
    }

    /* Recommended mode of operation: Indoor navigation */
    settings.filter = BME280_FILTER_COEFF_16;
    settings.osr_h = BME280_OVERSAMPLING_1X;
    settings.osr_p = BME280_OVERSAMPLING_16X;
    settings.osr_t = BME280_OVERSAMPLING_2X;

    /* Set the sensor settings */
    rslt = bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, f_dev);
    if (rslt != BME280_OK) {
        goto __end;
    }

    /*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
     *  and the oversampling configuration. */
    bme280_cal_meas_delay(&f_req_delay, &settings);
    f_isBme280Inited = true;

__end:  
    return;
}