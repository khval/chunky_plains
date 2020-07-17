#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/Picasso96API.h>

#include "chunky_plains2.h"

	double r90 =  M_PI / 2;
	double r180 =  M_PI;

	int screenw,screenh;


struct xy  
{
	double x;
	double y;
};

struct xy org[10000];

struct xy des[10000];
struct xy des2[10000];

struct xy pos[100];

int pixels = 10000;
int pos_max = 10;

int max_len = 0;

void init()
{
	int n;

	for (n=0;n<pos_max;n++)
	{
		pos[n].x = (double) (rand() % 640);
		pos[n].y = (double) (rand() % 480);
	}
}


void larp( double n, struct xy *p0, struct xy *p1, struct xy *dest )
{
	dest->x = (n * (p1->x-p0->x))  + p0->x;
	dest->y = (n * (p1->y-p0->y))  + p0->y;
}

#define set_color_nice( n, r, g, b ) SetRGB32( &src -> ViewPort ,n , (unsigned int)  r * 0x01010101, (unsigned int) g * 0x01010101, (unsigned int) b * 0x01010101)



void set_palette(struct Screen *src)
{
	long long int r,g,b;
	unsigned int y;

	SetRGB32( &src -> ViewPort ,0,0x0,0,0);

	for (y=0;y<256;y++)
	{
		set_color_nice( y ,y , y , y );
	}

	set_color_nice( 1 ,  255 ,  000 , 000 );
	set_color_nice( 2 ,  000 ,  255 , 000 );

}

int main()
{
	struct chunky_plains *cp;
	struct Screen *src = NULL;
	struct Window *win;
	struct RenderInfo ri;
	struct BitMap *bitmap;
	struct RastPort scroll_rp;

	int err;


	int txt_pos = 0;

	void *tmp;
	BPTR bmlock;
	double x,y,z;
	int n,nn,nnn;
	ULONG bpr;
	double g;
	double zoom;

	int p,zz;
	int rx,ry;

	int cnt = 0;
	int idx = 0;

	double l;

	src = p96OpenScreenTags(
		P96SA_DisplayID, 0x50051000,
		P96SA_Title, (ULONG) "666",
		P96SA_Quiet, TRUE,
		P96SA_NoMemory, TRUE,
		P96SA_NoSprite, TRUE,
		P96SA_Exclusive, TRUE,
		TAG_END	);

	if ( src )
	{
		screenw=640;
		screenh=480;


		set_palette(src);

		win = OpenWindowTags(NULL,
			WA_Left, 0, WA_Top, 0,
			WA_Width, screenw, WA_Height, screenh,
			WA_NoCareRefresh, TRUE,
			WA_Borderless, TRUE,
			WA_Activate, TRUE,
			WA_RMBTrap, TRUE,
			WA_ReportMouse, TRUE,
			WA_CustomScreen, (ULONG) src,
			TAG_END
		);


		if (win)
		{
	       		bitmap = AllocBitMap( screenw, screenh, 8, BMF_CLEAR, win -> RPort -> BitMap);

			scroll_rp.Font =  win -> RPort -> Font;
			scroll_rp.BitMap = bitmap;

			bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

			if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, screenw,screenh,&cp))
			{
				printf("error code: %d\n",err);
			}
			else
			{
				Delay(25);

				set_colour(cp,0,200);

	for (z=0;z<10;z++)
	{

				init();

				for (p=0;p<pos_max-2;p++)
				{
					larp( 0.5f, &pos[p+0], &pos[p+1], &org[0] );
					larp( 0.5f, &pos[p+1], &pos[p+2], &org[1] );	

					set_colour(cp,0,1);

					set_pixel_fast(cp, pos[p+0].x, pos[p+0].y );
					set_pixel_fast(cp, pos[p+1].x, pos[p+1].y );
					set_pixel_fast(cp, pos[p+2].x, pos[p+2].y );

					set_colour(cp,0,200);

					set_pixel_fast(cp, org[0].x, org[0].y );
					set_pixel_fast(cp, org[1].x, org[1].y );


					for (l=0.0f;l<1.0f;l+=0.01f)
					{
						larp( l, &org[0], &pos[p+1], &des[0] );
						larp( l, &pos[p+1], &org[1], &des[1] );
						larp( l, &des[0], &des[1], &des2[0] );

						set_pixel_fast(cp, des2[0].x, des2[0].y );
					}
					plains_to_video(cp, screenh, win -> RPort -> BitMap );
					Delay(10);
				}

				plains_to_video(cp, screenh, win -> RPort -> BitMap );
				clear_plains( cp, bpr, screenh );

	}

				Delay(50 * 8);

				free_chunky_plains(cp);

			}

			FreeBitMap(bitmap);

			CloseWindow(win);
		}

		p96CloseScreen(src);
	}
}

