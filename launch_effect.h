#ifndef LAUNCH_H
#define LAUNCH_H

#include "cube.h"

// Total number of effects
// Used in the main loop to loop through all the effects one by bone.
// Set this number one higher than the highest number inside switch()
// in launch_effect() in launch_effect.c
#define EFFECTS_TOTAL 27

void launch_effect (int effect);

#endif

