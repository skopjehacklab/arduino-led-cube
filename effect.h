#ifndef EFFECT_H
#define EFFECT_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "cube.h"

void effect_box_shrink_grow (int iterations, int rot, int flip, uint16_t delay);

void effect_hollow_1 (int iterations, uint16_t delay);
void effect_hollow_2 (int iterations, int corner, uint16_t delay);

void sendvoxel_z (unsigned char x, unsigned char y, unsigned char z, int delay);
void sendplane_rand_z (unsigned char z, int delay, int wait);
void sendvoxels_rand_z (int iterations, int delay, int wait);
void boingboing(uint16_t iterations, int delay, unsigned char mode, unsigned char drawmode);

void effect_planboing (int plane, int speed);

void effect_random_filler (int delay, int state);

void effect_z_updown (int iterations, int delay);
void effect_rain(int iterations);
void effect_stringfly2(char * str);
void effect_blinky2(void);
void draw_positions_axis (char axis, unsigned char positions[64], int invert);
void effect_axis_updown_randsuspend (char axis, int delay, int sleep, int invert);

void effect_random_sparkle_flash (int iterations, int voxels, int delay);
void effect_random_sparkle (void);

void effect_box_woopwoop (int delay, int grow);
void effect_telcstairs (int invert, int delay, int val);
void effect_loadbar(int delay);

void effect_boxside_randsend_parallel (char axis, int origin, int delay, int mode);
void effect_smileyspin (int count, int delay, char bitmap);
void effect_pathmove (unsigned char *path, int length);
void effect_rand_patharound (int iterations, int delay);
void effect_pathspiral (int iterations, int delay);
void effect_path_text (int delay, char *str);
void effect_path_bitmap (int delay, char bitmap, int iterations);
void effect_wormsqueeze (int size, int axis, int direction, int iterations, int delay);

void effect_z_updown (int iterations, int delay);
void effect_z_updown_move (unsigned char positions[64], unsigned char destinations[64], char axis);




#endif

