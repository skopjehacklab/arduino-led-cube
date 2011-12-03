#include "effect.h"
#include "draw.h"
#include "font.h"
#include <math.h>
#include <avr/interrupt.h>

void effect_test (void)
{

	int x,y,i;

	for (i=0;i<1000;i++)
	{
		x = sin(i/8)*2+3.5;
		y = cos(i/8)*2+3.5;

		setvoxel(x,y,1);
		setvoxel(x,y,1);
		delay_ms(1000);
		fill(0x00);
	}

}


void effect_stringfly2(char * str)
{
	int x,y,i;
	unsigned char chr[5];
	
	while (*str)
	{
		font_getchar(*str++, chr);
		
        // Put a character on the back of the cube
		for (x = 0; x < 5; x++)
		{
			for (y = 0; y < 8; y++)
			{
				if ((chr[x] & (0x80>>y)))
				{
					setvoxel(7,x+2,y);
				}
			}
		}
		
        // Shift the entire contents of the cube forward by 6 steps
        // before placing the next character
		for (i = 0; i<6; i++)
		{
			delay_ms(1000);
            shift(AXIS_X,-1);
		}
	}
    // Shift the last character out of the cube.
	for (i = 0; i<8; i++)
	{
		delay_ms(1000);
        shift(AXIS_X,-1);
	}
	
}

// Draw a plane on one axis and send it back and forth once.
void effect_planboing (int plane, int speed)
{
	int i;
	for (i=0;i<8;i++)
	{
		fill(0x00);
        setplane(plane, i);
		delay_ms(speed);
	}
	
	for (i=7;i>=0;i--)
	{
		fill(0x00);
        setplane(plane,i);
		delay_ms(speed);
	}
}

void effect_blinky2()
{
	int i,r;
	fill(0x00);
	
	for (r=0;r<2;r++)
	{
		i = 750;
		while (i>0)
		{
			fill(0x00);
			delay_ms(i);
			
			fill(0xff);
			delay_ms(100);
			
			i = i - (15+(1000/(i/10)));
		}
		
		delay_ms(1000);
		
		i = 750;
		while (i>0)
		{
			fill(0x00);
			delay_ms(751-i);
			
			fill(0xff);
			delay_ms(100);
			
			i = i - (15+(1000/(i/10)));
		}
	}

}

void effect_box_shrink_grow (int iterations, int rot, int flip, uint16_t delay)
{
	int x, i, xyz;
	for (x=0;x<iterations;x++)
	{
		for (i=0;i<16;i++)
		{
            xyz = 7-i; // This reverses counter i between 0 and 7.
            if (i > 7) 
                xyz = i-8; // at i > 7, i 8-15 becomes xyz 0-7.
            
            fill(0x00); delay_ms(1);
            cli(); // disable interrupts while the cube is being rotated
			box_wireframe(0,0,0,xyz,xyz,xyz);

            if (flip > 0) // upside-down
                mirror_z();

            if (rot == 1 || rot == 3)
                mirror_y();

            if (rot == 2 || rot == 3)
                mirror_x();
            
            sei(); // enable interrupts
			delay_ms(delay);
	        fill(0x00);
        }
    }
}

// Creates a wireframe box that shrinks or grows out from the center of the cube.
void effect_box_woopwoop (int delay, int grow)
{
	int i,ii;
	
	fill(0x00);
	for (i=0;i<4;i++)
	{
        ii = i;
        if (grow > 0)
            ii = 3-i;

		box_wireframe(4+ii,4+ii,4+ii,3-ii,3-ii,3-ii);
		delay_ms(delay);
		fill(0x00);
	}
}


// Send a voxel flying from one side of the cube to the other
// If its at the bottom, send it to the top..
void sendvoxel_z (unsigned char x, unsigned char y, unsigned char z, int delay)
{
	int i, ii;
	for (i=0; i<8; i++)
	{
		if (z == 7)
		{
			ii = 7-i;
			clrvoxel(x,y,ii+1);
		} else
		{
			ii = i;
			clrvoxel(x,y,ii-1);
		}
		setvoxel(x,y,ii);
		delay_ms(delay);
	}
}

// Send all the voxels from one side of the cube to the other
// Start at z and send to the opposite side.
// Sends in random order.
void sendplane_rand_z (unsigned char z, int delay, int wait)
{
	unsigned char loop = 16;
	unsigned char x, y;

	fill(0x00);

	setplane_z(z);
	
	// Send voxels at random untill all 16 have crossed the cube.
	while(loop)
	{
		x = rand()%4;
		y = rand()%4;
		if (getvoxel(x,y,z))
		{
			// Send the voxel flying
			sendvoxel_z(x,y,z,delay);
			delay_ms(wait);
			loop--; // one down, loop-- to go. when this hits 0, the loop exits. 
		}
	}
}

// For each coordinate along X and Y, a voxel is set either at level 0 or at level 7
// for n iterations, a random voxel is sent to the opposite side of where it was.
void sendvoxels_rand_z (int iterations, int delay, int wait)
{
	unsigned char x, y, last_x = 0, last_y = 0, i;

	fill(0x00);

	// Loop through all the X and Y coordinates
	for (x=0;x<8;x++)
	{
		for (y=0;y<8;y++)
		{
			// Then set a voxel either at the top or at the bottom
            // rand()%2 returns either 0 or 1. multiplying by 7 gives either 0 or 7.
            setvoxel(x,y,((rand()%2)*7));
		}
	}

	for (i=0;i<iterations;i++)
	{
		// Pick a random x,y position
		x = rand()%8;
		y = rand()%8;
		// but not the sameone twice in a row
		if (y != last_y && x != last_x)
		{
			// If the voxel at this x,y is at the bottom
			if (getvoxel(x,y,0))
			{
				// send it to the top
				sendvoxel_z(x,y,0,delay);
			} else
			{
				// if its at the top, send it to the bottom
				sendvoxel_z(x,y,7,delay);
			}
			delay_ms(wait);
			
			// Remember the last move
			last_y = y;
			last_x = x;
		}
	}

}


// Big ugly function :p but it looks pretty
void boingboing(uint16_t iterations, int delay, unsigned char mode, unsigned char drawmode)
{
	fill(0x00);		// Blank the cube

	int x, y, z;		// Current coordinates for the point
	int dx, dy, dz;	// Direction of movement
	int lol, i;		// lol?
	unsigned char crash_x, crash_y, crash_z;

	y = rand()%8;
	x = rand()%8;
	z = rand()%8;

	// Coordinate array for the snake.
	int snake[8][3];
	for (i=0;i<8;i++)
	{
		snake[i][0] = x;
		snake[i][1] = y;
		snake[i][2] = z;
	}
	
	
	dx = 1;
	dy = 1;
	dz = 1;
	
	while(iterations)
	{
		crash_x = 0;
		crash_y = 0;
		crash_z = 0;
	

		// Let's mix things up a little:
		if (rand()%3 == 0)
		{
			// Pick a random axis, and set the speed to a random number.
			lol = rand()%3;
			if (lol == 0)
				dx = rand()%3 - 1;
			
			if (lol == 1)
				dy = rand()%3 - 1;
				
			if (lol == 2)
				dz = rand()%3 - 1;
		}

	    // The point has reached 0 on the x-axis and is trying to go to -1
        // aka a crash
		if (dx == -1 && x == 0)
		{
			crash_x = 0x01;
			if (rand()%3 == 1)
			{
				dx = 1;
			} else
			{
				dx = 0;
			}
		}
		
        // y axis 0 crash
		if (dy == -1 && y == 0)
		{
			crash_y = 0x01;
			if (rand()%3 == 1)
			{
				dy = 1;
			} else
			{
				dy = 0;
			}
		}
		
        // z axis 0 crash
		if (dz == -1 && z == 0)
		{
			crash_z = 0x01;
			if (rand()%3 == 1)
			{
				dz = 1;
			} else
			{
				dz = 0;
			}
		}
	    
        // x axis 7 crash
		if (dx == 1 && x == 7)
		{
			crash_x = 0x01;
			if (rand()%3 == 1)
			{
				dx = -1;
			} else
			{
				dx = 0;
			}
		}
		
        // y axis 7 crash
		if (dy == 1 && y == 7)
		{
			crash_y = 0x01;
			if (rand()%3 == 1)
			{
				dy = -1;
			} else
			{
				dy = 0;
			}
		}
		
        // z azis 7 crash
		if (dz == 1 && z == 7)
		{
			crash_z = 0x01;
			if (rand()%3 == 1)
			{
				dz = -1;
			} else
			{
				dz = 0;
			}
		}
		
		// mode bit 0 sets crash action enable
		if (mode | 0x01)
		{
			if (crash_x)
			{
				if (dy == 0)
				{
					if (y == 7)
					{
						dy = -1;
					} else if (y == 0)
					{
						dy = +1;
					} else
					{
						if (rand()%2 == 0)
						{
							dy = -1;
						} else
						{
							dy = 1;
						}
					}
				}
				if (dz == 0)
				{
					if (z == 7)
					{
						dz = -1;
					} else if (z == 0)
					{
						dz = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dz = -1;
						} else
						{
							dz = 1;
						}
					}	
				}
			}
			
			if (crash_y)
			{
				if (dx == 0)
				{
					if (x == 7)
					{
						dx = -1;
					} else if (x == 0)
					{
						dx = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dx = -1;
						} else
						{
							dx = 1;
						}
					}
				}
				if (dz == 0)
				{
					if (z == 3)
					{
						dz = -1;
					} else if (z == 0)
					{
						dz = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dz = -1;
						} else
						{
							dz = 1;
						}
					}	
				}
			}
			
			if (crash_z)
			{
				if (dy == 0)
				{
					if (y == 7)
					{
						dy = -1;
					} else if (y == 0)
					{
						dy = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dy = -1;
						} else
						{
							dy = 1;
						}
					}	
				}
				if (dx == 0)
				{
					if (x == 7)
					{
						dx = -1;
					} else if (x == 0)
					{
						dx = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dx = -1;
						} else
						{
							dx = 1;
						}
					}	
				}
			}
		}
		
		// mode bit 1 sets corner avoid enable
		if (mode | 0x02)
		{
			if (	// We are in one of 8 corner positions
				(x == 0 && y == 0 && z == 0) ||
				(x == 0 && y == 0 && z == 7) ||
				(x == 0 && y == 7 && z == 0) ||
				(x == 0 && y == 7 && z == 7) ||
				(x == 7 && y == 0 && z == 0) ||
				(x == 7 && y == 0 && z == 7) ||
				(x == 7 && y == 7 && z == 0) ||
				(x == 7 && y == 7 && z == 7)
			)
			{
				// At this point, the voxel would bounce
				// back and forth between this corner,
				// and the exact opposite corner
				// We don't want that!
			
				// So we alter the trajectory a bit,
				// to avoid corner stickyness
				lol = rand()%3;
				if (lol == 0)
					dx = 0;
				
				if (lol == 1)
					dy = 0;
					
				if (lol == 2)
					dz = 0;
			}
		}

        // one last sanity check
        if (x == 0 && dx == -1)
            dx = 1;
	
        if (y == 0 && dy == -1)
            dy = 1;
	
        if (z == 0 && dz == -1)
            dz = 1;
	
        if (x == 7 && dx == 1)
            dx = -1;
	
        if (y == 7 && dy == 1)
            dy = -1;
	
        if (z == 7 && dz == 1)
            dz = -1;
	
	
		// Finally, move the voxel.
		x = x + dx;
		y = y + dy;
		z = z + dz;
		
		if (drawmode == 0x01) // show one voxel at time
		{
			setvoxel(x,y,z);
			delay_ms(delay);
			clrvoxel(x,y,z);	
		} else if (drawmode == 0x02) // flip the voxel in question
		{
			flpvoxel(x,y,z);
			delay_ms(delay);
		} if (drawmode == 0x03) // draw a snake
		{
			for (i=7;i>=0;i--)
			{
				snake[i][0] = snake[i-1][0];
				snake[i][1] = snake[i-1][1];
				snake[i][2] = snake[i-1][2];
			}
			snake[0][0] = x;
			snake[0][1] = y;
			snake[0][2] = z;
				
			for (i=0;i<8;i++)
			{
				setvoxel(snake[i][0],snake[i][1],snake[i][2]);
			}
			delay_ms(delay);
			for (i=0;i<8;i++)
			{
				clrvoxel(snake[i][0],snake[i][1],snake[i][2]);
			}
		}
		
	
		iterations--;
	}
}

// Set or clear exactly 512 voxels in a random order.
void effect_random_filler (int delay, int state)
{
	int x,y,z;
	int loop = 0;
	
	
	if (state == 1)
	{
		fill(0x00);
	} else
	{
		fill(0xff);
	}
	
	while (loop<511)
	{
		x = rand()%8;
		y = rand()%8;
		z = rand()%8;

		if ((state == 0 && getvoxel(x,y,z) == 0x01) || (state == 1 && getvoxel(x,y,z) == 0x00))
		{
			altervoxel(x,y,z,state);
			delay_ms(delay);
			loop++;
		}	
	}
}


void effect_rain (int iterations)
{
	int i, ii;
	int rnd_x;
	int rnd_y;
	int rnd_num;
	
	for (ii=0;ii<iterations;ii++)
	{
		rnd_num = rand()%4;
		
		for (i=0; i < rnd_num;i++)
		{
			rnd_x = rand()%8;
			rnd_y = rand()%8;
			setvoxel(rnd_x,rnd_y,7);
		}
		
		delay_ms(1000);
		shift(AXIS_Z,-1);
	}
}

void effect_z_updown (int iterations, int delay)
{
	unsigned char positions[64];
	unsigned char destinations[64];

	int i,y,move;
	
	for (i=0; i<64; i++)
	{
		positions[i] = 4;
		destinations[i] = rand()%8;
	}

	for (i=0; i<8; i++)
	{
		effect_z_updown_move(positions, destinations, AXIS_Z);
		delay_ms(delay);
	}
	
	for (i=0;i<iterations;i++)
	{
		for (move=0;move<8;move++)
		{
			effect_z_updown_move(positions, destinations, AXIS_Z);
			delay_ms(delay);
		}

		delay_ms(delay*4);


		for (y=0;y<32;y++)
		{
				destinations[rand()%64] = rand()%8;
		}
		
	}

}

void effect_z_updown_move (unsigned char positions[64], unsigned char destinations[64], char axis)
{
	int px;
	for (px=0; px<64; px++)
	{
		if (positions[px]<destinations[px])
		{
			positions[px]++;
		}
		if (positions[px]>destinations[px])
		{
			positions[px]--;
		}
	}
		
	draw_positions_axis (AXIS_Z, positions,0);
}

void effect_axis_updown_randsuspend (char axis, int delay, int sleep, int invert)
{
	unsigned char positions[64];
	unsigned char destinations[64];

	int i,px;
	
    // Set 64 random positions
	for (i=0; i<64; i++)
	{
		positions[i] = 0; // Set all starting positions to 0
		destinations[i] = rand()%8;
	}

    // Loop 8 times to allow destination 7 to reach all the way
	for (i=0; i<8; i++)
	{
        // For every iteration, move all position one step closer to their destination
		for (px=0; px<64; px++)
		{
			if (positions[px]<destinations[px])
			{
				positions[px]++;
			}
		}
        // Draw the positions and take a nap
		draw_positions_axis (axis, positions,invert);
		delay_ms(delay);
	}
	
    // Set all destinations to 7 (opposite from the side they started out)
	for (i=0; i<64; i++)
	{
		destinations[i] = 7;
	}
	
    // Suspend the positions in mid-air for a while
	delay_ms(sleep);
	
    // Then do the same thing one more time
	for (i=0; i<8; i++)
	{
		for (px=0; px<64; px++)
		{
			if (positions[px]<destinations[px])
			{
				positions[px]++;
			}
			if (positions[px]>destinations[px])
			{
				positions[px]--;
			}
		}
		draw_positions_axis (axis, positions,invert);
		delay_ms(delay);
	}
}

void draw_positions_axis (char axis, unsigned char positions[64], int invert)
{
	int x, y, p;
	
	fill(0x00);
	
	for (x=0; x<8; x++)
	{
		for (y=0; y<8; y++)
		{
			if (invert)
			{
				p = (7-positions[(x*8)+y]);
			} else
			{
				p = positions[(x*8)+y];
			}
		
			if (axis == AXIS_Z)
				setvoxel(x,y,p);
				
			if (axis == AXIS_Y)
				setvoxel(x,p,y);
				
			if (axis == AXIS_X)
				setvoxel(p,y,x);
		}
	}
	
}


void effect_boxside_randsend_parallel (char axis, int origin, int delay, int mode)
{
	int i;
	int done;
	unsigned char cubepos[64];
	unsigned char pos[64];
	int notdone = 1;
	int notdone2 = 1;
	int sent = 0;
	
	for (i=0;i<64;i++)
	{
		pos[i] = 0;
	}
	
	while (notdone)
	{
		if (mode == 1)
		{
			notdone2 = 1;
			while (notdone2 && sent<64)
			{
				i = rand()%64;
				if (pos[i] == 0)
				{
					sent++;
					pos[i] += 1;
					notdone2 = 0;
				}
			}
		} else if (mode == 2)
		{
			if (sent<64)
			{
				pos[sent] += 1;
				sent++;
			}
		}
		
		done = 0;
		for (i=0;i<64;i++)
		{
			if (pos[i] > 0 && pos[i] <7)
			{
				pos[i] += 1;
			}
				
			if (pos[i] == 7)
				done++;
		}
		
		if (done == 64)
			notdone = 0;
		
		for (i=0;i<64;i++)
		{
			if (origin == 0)
			{
				cubepos[i] = pos[i];
			} else
			{
				cubepos[i] = (7-pos[i]);
			}
		}
		
		
		delay_ms(delay);
		draw_positions_axis(axis,cubepos,0);
		LED_PORT ^= LED_RED;
	}
	
}




// Light all leds layer by layer,
// then unset layer by layer
void effect_loadbar(int delay)
{
	fill(0x00);
	
	int z,y;
	
	for (z=0;z<8;z++)
	{
		for (y=0;y<8;y++)
			cube[z][y] = 0xff;
			
		delay_ms(delay);
	}
	
	delay_ms(delay*3);
	
	for (z=0;z<8;z++)
	{
		for (y=0;y<8;y++)
			cube[z][y] = 0x00;
			
		delay_ms(delay);
	}
}


// Set n number of voxels at random positions
void effect_random_sparkle_flash (int iterations, int voxels, int delay)
{
	int i;
	int v;
	for (i = 0; i < iterations; i++)
	{
		for (v=0;v<=voxels;v++)
			setvoxel(rand()%8,rand()%8,rand()%8);
			
		delay_ms(delay);
		fill(0x00);
	}
}

// blink 1 random voxel, blink 2 random voxels..... blink 20 random voxels
// and back to 1 again.
void effect_random_sparkle (void)
{
	int i;
	
	for (i=1;i<20;i++)
	{
		effect_random_sparkle_flash(5,i,200);
	}
	
	for (i=20;i>=1;i--)
	{
		effect_random_sparkle_flash(5,i,200);
	}
	
}

int effect_telcstairs_do(int x, int val, int delay)
{
	int y,z;

	for(y = 0, z = x; y <= z; y++, x--)
	{
		if(x < CUBE_SIZE && y < CUBE_SIZE)
		{
			cube[x][y] = val;
		}
	}
	delay_ms(delay);
	return z;
}

void effect_telcstairs (int invert, int delay, int val)
{
	int x;

	if(invert)
	{
		for(x = CUBE_SIZE*2; x >= 0; x--)
		{
			x = effect_telcstairs_do(x,val,delay);
		}
	}
	else
	{
		for(x = 0; x < CUBE_SIZE*2; x++)
		{
			x = effect_telcstairs_do(x,val,delay);
		}
	}
}

void effect_wormsqueeze (int size, int axis, int direction, int iterations, int delay)
{
	int x, y, i,j,k, dx, dy;
	int cube_size;
	int origin = 0;
	
	if (direction == -1)
		origin = 7;
	
	cube_size = 8-(size-1);
	
	x = rand()%cube_size;
	y = rand()%cube_size;
	
	for (i=0; i<iterations; i++)
	{
		dx = ((rand()%3)-1);
		dy = ((rand()%3)-1);
	
		if ((x+dx) > 0 && (x+dx) < cube_size)
			x += dx;
			
		if ((y+dy) > 0 && (y+dy) < cube_size)
			y += dy;
	
		shift(axis, direction);
		

		for (j=0; j<size;j++)
		{
			for (k=0; k<size;k++)
			{
				if (axis == AXIS_Z)
					setvoxel(x+j,y+k,origin);
					
				if (axis == AXIS_Y)
					setvoxel(x+j,origin,y+k);
					
				if (axis == AXIS_X)
					setvoxel(origin,y+j,x+k);
			}
		}
		
		delay_ms(delay);
	}
}

void effect_smileyspin (int count, int delay, char bitmap)
{
	unsigned char dybde[] = {0,1,2,3,4,5,6,7,1,1,2,3,4,5,6,6,2,2,3,3,4,4,5,5,3,3,3,3,4,4,4,4};
	int d = 0;
	int flip = 0;
	int x, y, off;
	for(int i = 0; i<count; i++)
	{
		flip = 0;
		d = 0;
		off = 0;
		// front:
		for (int s=0;s<7;s++){
			if(!flip){
				off++;
				if (off == 4){
					flip = 1;
					off = 0;
				}
			} else {
				off++;
			}
		        for (x=0; x<8; x++)
        		{
				d = 0;
                		for (y=0; y<8; y++)
	                	{
					if (font_getbitmappixel ( bitmap, 7-x, y)){
						if (!flip)
							setvoxel(y,dybde[8 * off + d++],x);
						else
							setvoxel(y,dybde[31 - 8 * off - d++],x);
					} else {
						d++;
					}
				}
			}
			delay_ms(delay);
			fill(0x00);
		}

		// side:
		off = 0;
		flip = 0;
		d = 0;
		for (int s=0;s<7;s++){
			if(!flip){
				off++;
				if (off == 4){
					flip = 1;
					off = 0;
				}
			} else {
				off++;
			}
		        for (x=0; x<8; x++)
        		{
				d = 0;
                		for (y=0; y<8; y++)
	                	{
					if (font_getbitmappixel ( bitmap, 7-x, y)){
						if (!flip)
							setvoxel(dybde[8 * off + d++], 7 - y,x);
						else
							setvoxel(dybde[31 - 8 * off - d++],7 - y,x);
					} else {
						d++;
					}
				}
			}
			delay_ms(delay);
			fill(0x00);
		}


		flip = 0;
		d = 0;
		off = 0;
		// back:
		for (int s=0;s<7;s++){
			if(!flip){
				off++;
				if (off == 4){
					flip = 1;
					off = 0;
				}
			} else {
				off++;
			}
		        for (x=0; x<8; x++)
        		{
				d = 0;
                		for (y=0; y<8; y++)
	                	{
					if (font_getbitmappixel ( bitmap, 7-x, 7-y)){
						if (!flip)
							setvoxel(y,dybde[8 * off + d++],x);
						else
							setvoxel(y,dybde[31 - 8 * off - d++],x);
					} else {
						d++;
					}
				}
			}
			delay_ms(delay);
			fill(0x00);
		}

		// other side:
		off = 0;
		flip = 0;
		d = 0;
		for (int s=0;s<7;s++){
			if(!flip){
				off++;
				if (off == 4){
					flip = 1;
					off = 0;
				}
			} else {
				off++;
			}
		        for (x=0; x<8; x++)
        		{
				d = 0;
                		for (y=0; y<8; y++)
	                	{
					if (font_getbitmappixel ( bitmap, 7-x, 7-y)){
						if (!flip)
							setvoxel(dybde[8 * off + d++], 7 - y,x);
						else
							setvoxel(dybde[31 - 8 * off - d++],7 - y,x);
					} else {
						d++;
					}
				}
			}
			delay_ms(delay);
			fill(0x00);
		}

	}
}


void effect_pathmove (unsigned char *path, int length)
{
	int i,z;
	unsigned char state;
	
	for (i=(length-1);i>=1;i--)
	{
		for (z=0;z<8;z++)
		{
		
			state = getvoxel(((path[(i-1)]>>4) & 0x0f), (path[(i-1)] & 0x0f), z);
			altervoxel(((path[i]>>4) & 0x0f), (path[i] & 0x0f), z, state);
		}
	}
	for (i=0;i<8;i++)
		clrvoxel(((path[0]>>4) & 0x0f), (path[0] & 0x0f),i);
}

void effect_rand_patharound (int iterations, int delay)
{
	int z, dz, i;
	z = 4;
	unsigned char path[28];
	
	font_getpath(0,path,28);
	
	for (i = 0; i < iterations; i++)
	{
		dz = ((rand()%3)-1);
		z += dz;
		
		if (z>7)
			z = 7;
			
		if (z<0)
			z = 0;
		
		effect_pathmove(path, 28);
		setvoxel(0,7,z);
		delay_ms(delay);
	}
}

void effect_pathspiral (int iterations, int delay)
{
	int z, i;
	z = 4;
	unsigned char path[16];
	
	font_getpath(1,path,16);
	
	for (i = 0; i < iterations; i++)
	{
		setvoxel(4,0,i%8);
		delay_ms(delay);
		effect_pathmove(path, 28);
		
	}
}

void effect_path_text (int delay, char *str)
{
	int z, i,ii;
	z = 4;
	unsigned char path[28];
	font_getpath(0,path,28);
	
	unsigned char chr[5];
	unsigned char stripe;
	
	while (*str)
	{
		//charfly(*str++, direction, axis, mode, delay);
		
		
		font_getchar(*str++, chr);
		
		for (ii=0;ii<5;ii++)
		{
			//stripe = pgm_read_byte(&font[(chr*5)+ii]);
			stripe = chr[ii];
			
			for (z=0;z<8;z++)
			{
				if ((stripe>>(7-z)) & 0x01)
				{
					setvoxel(0,7,z);
				} else
				{
					clrvoxel(0,7,z);
				}
				
			}
			effect_pathmove(path, 28);
			delay_ms(delay);
		}
	
		effect_pathmove(path, 28);
		delay_ms(delay);
	}
	for (i=0;i<28;i++)
	{
		effect_pathmove(path, 28);
		delay_ms(delay);
	}
}

void effect_path_bitmap (int delay, char bitmap, int iterations)
{
	int z, i, ii;
	z = 4;
	unsigned char path[28];
	font_getpath(0,path,28);
	
	for (i=0; i < iterations; i++)
	{	
		for (ii=0;ii<8;ii++)
		{		
			for (z=0;z<8;z++)
			{
				if (font_getbitmappixel(bitmap,(7-z),ii))
				{
					setvoxel(0,7,z);
				} else
				{
					clrvoxel(0,7,z);
				}
				
			}
			delay_ms(delay);
			effect_pathmove(path, 28);
		}
	
		for (ii=0;ii<20;ii++)
		{
			delay_ms(delay);
			effect_pathmove(path, 28);
		}
	}
	for (ii=0;ii<10;ii++)
	{
		delay_ms(delay);
		effect_pathmove(path, 28);
	}
}



