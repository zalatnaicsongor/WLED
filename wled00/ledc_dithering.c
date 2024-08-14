#include "soc/soc_caps.h"
#include "driver/ledc.h"
#include "hal/ledc_hal.h"

//from Arduino
#ifdef SOC_LEDC_SUPPORT_HS_MODE
#define LEDC_CHANNELS           (SOC_LEDC_CHANNEL_NUM<<1)
#else
#define LEDC_CHANNELS           (SOC_LEDC_CHANNEL_NUM)
#endif

//Use XTAL clock if possible to avoid timer frequency error when setting APB clock < 80 Mhz
//Need to be fixed in ESP-IDF
#ifdef SOC_LEDC_SUPPORT_XTAL_CLOCK
#define LEDC_DEFAULT_CLK        LEDC_USE_XTAL_CLK
#else
#define LEDC_DEFAULT_CLK        LEDC_AUTO_CLK
#endif

#define LEDC_MAX_BIT_WIDTH      SOC_LEDC_TIMER_BIT_WIDE_NUM

uint8_t ledc_dithering_channels_resolution[LEDC_CHANNELS] = {0};

uint32_t ledcWithDitheringSetup(uint8_t chan, uint32_t freq, uint8_t bit_num)
{
    if(chan >= LEDC_CHANNELS || bit_num > LEDC_MAX_BIT_WIDTH){
        return 0;
    }

    uint8_t group=(chan/8), timer=((chan/2)%4);

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = group,
        .timer_num        = timer,
        .duty_resolution  = bit_num,
        .freq_hz          = freq,
        .clk_cfg          = LEDC_DEFAULT_CLK
    };
    if(ledc_timer_config(&ledc_timer) != ESP_OK)
    {
        return 0;
    }
    ledc_dithering_channels_resolution[chan] = bit_num;
    return ledc_get_freq(group,timer);
}

void ledcWithDitheringWrite(uint8_t chan, uint32_t duty)
{
    if(chan >= LEDC_CHANNELS){
        return;
    }
    uint8_t group=(chan/8), channel=(chan%8);

    //Fixing if all bits in resolution is set = LEDC FULL ON
    uint32_t max_duty = (1 << (ledc_dithering_channels_resolution[chan] + 4)) - 1;

    if((duty == max_duty) && (max_duty != 1)){
        duty = max_duty + 1;
    }

    ledc_dev_t *dev;
    dev = LEDC_LL_GET_HW();

    dev->channel_group[group].channel[channel].duty.duty = duty;

    ledc_update_duty(group, channel);
}