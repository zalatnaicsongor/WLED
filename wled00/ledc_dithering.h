#ifndef LEDCDITHERING_h
#define LEDCDITHERING_h


#ifdef __cplusplus
extern "C" {
#endif

uint32_t ledcWithDitheringSetup(uint8_t chan, uint32_t freq, uint8_t bit_num);
void ledcWithDitheringWrite(uint8_t chan, uint32_t duty);

#ifdef __cplusplus
}
#endif
#endif