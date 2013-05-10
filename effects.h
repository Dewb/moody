// effects.h

#include <stdint.h>

void initEffects(int width, int height);

void runWaterfallEffect(uint8_t* data);
void runFlameEffect(uint8_t* data);

void clearEffect(uint8_t* data);
void runTestEffect(uint8_t* data);
