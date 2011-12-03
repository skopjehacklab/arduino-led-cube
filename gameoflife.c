#include <avr/io.h>
#include "gameoflife.h"
#include "cube.h"
#include "draw.h"

// Game of Life for the 4x4x4 and 8x8x8 led cube

#define GOL_CREATE_MIN 3
#define GOL_CREATE_MAX 3

#define GOL_TERMINATE_LONELY 1
#define GOL_TERMINATE_CROWDED 4

#define GOL_X 8
#define GOL_Y 8
#define GOL_Z 8

#define GOL_WRAP 0x01



void gol_play (int iterations, uint16_t delay)
{
	int i;
	
	for (i = 0; i < iterations; i++)
	{
		LED_PORT ^= LED_GREEN;
	
		gol_nextgen();
		
		if (gol_count_changes() == 0)
			return;
		
		tmp2cube();
		
		delay_ms(delay);
		
		//led_red(1);
	}
}

void gol_nextgen (void)
{
	int x,y,z;
	unsigned char neigh;
	
	tmpfill(0x00);
	
	for (x = 0; x < GOL_X; x++)
	{
		for (y = 0; y < GOL_Y; y++)
		{
			for (z = 0; z < GOL_Z; z++)
			{
				neigh = gol_count_neighbors(x, y, z);
				
				// Current voxel is alive.
				if (getvoxel(x,y,z) == 0x01)
				{
					if (neigh <= GOL_TERMINATE_LONELY)
					{
						tmpclrvoxel(x,y,z);
					} else if(neigh >= GOL_TERMINATE_CROWDED)
					{
						tmpclrvoxel(x,y,z);
					} else
					{
						tmpsetvoxel(x,y,z);
					}
				// Current voxel is dead.
				} else
				{
					if (neigh >= GOL_CREATE_MIN && neigh <= GOL_CREATE_MAX)
						tmpsetvoxel(x,y,z);
				}
			}
		}
	}
}

unsigned char gol_count_neighbors (int x, int y, int z)
{
	int ix, iy, iz; // offset 1 in each direction in each dimension
	int nx, ny, nz; // neighbours address.
	
	unsigned char neigh = 0; // number of alive neighbours.
	
	for (ix = -1; ix < 2; ix++)
	{
		for (iy = -1; iy < 2; iy++)
		{
			for (iz = -1; iz < 2; iz++)
			{
				// Your not your own neighbour, exclude 0,0,0, offset.
				if ( !(ix == 0 && iy == 0 && iz == 0) )
				{
					if (GOL_WRAP == 0x01)
					{
						nx = (x+ix)%GOL_X;
						ny = (y+iy)%GOL_Y;
						nz = (z+iz)%GOL_Z;
					} else
					{
						nx = x+ix;
						ny = y+iy;
						nz = z+iz;
					}
					
					if ( getvoxel(nx, ny, nz) )
						neigh++;
				}
			}
		}
	}
	return neigh;
}

int gol_count_changes (void)
{
	int x,y;
	int i = 0;
	
	for (x = 0; x < GOL_X; x++)
	{
		for (y = 0; y < GOL_Y; y++)
		{
			if (fb[x][y] != cube[x][y])
				i++;
		}
	}
	
	return i;
}

