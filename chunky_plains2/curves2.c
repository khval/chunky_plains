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
	int x;
	int y;
};

struct xy org[10000];

struct xy des[10000];
struct xy des2[10000];

struct xy pos[100];

int pixels = 10000;
int pos_max = 8;

int max_len = 0;

void init()
{
	int n;

	for (n=0;n<pos_max;n++)
	{
		pos[n].x = rand() % 640;
		pos[n].y = rand() % 480;
	}
}



void larp( int n, struct xy *p0, struct xy *p1, struct xy *dest )
{
	dest->x = (n * (p1->x-p0->x) / 100) + p0->x;
	dest->y = (n * (p1->y-p0->y) / 100) + p0->y;
}



/*
void draw_curv(struct chunky_plains *cp, int p, int c, int len, struct xy *tab)
{
	int n;
	set_colour(cp,p,c);

	for (n=0;n<len;n++)
	{
		if ((tab[n].x>0) && (tab[n].x<screenw) &&
		  (tab[n].y>0) &&( tab[n].y<screenh))
		{
			set_pixel_fast(cp, tab[n].x , tab[n].y );
		}
	}
}
*/


void set_palette(struct Screen *src)
{
	long long int r,g,b;
	int y;

	SetRGB32( &src -> ViewPort ,0,0x0,0,0);



	for (y=0;y<256;y++)
	{

		SetRGB32( &src -> ViewPort ,y ,y * 0x01010101, y * 0x01010101, y * 0x01010101);
	}


	SetRGB32( &src -> ViewPort ,1 ,255 * 0x01010101, 0 * 0x01010101, 0 * 0x01010101);
	SetRGB32( &src -> ViewPort ,2 ,0 * 0x01010101, 255 * 0x01010101, 0 * 0x01010101);

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

	int a,zz;
	int rx,ry;

	int cnt = 0;
	int idx = 0;

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
				Delay(50);

				for( zz=0;zz<10;zz++)
				{
					init();

					for (n=0;n<100;n++)
					{
						z = pos_max -1;

						set_colour(cp,0,1);

						for (a=0;a<z-1;a++)
						{					
							larp(n,&pos[a],&pos[a+1],&org[a]);
							set_pixel_fast(cp, org[a].x, org[a].y );
						}
					}

					for (n=0;n<100;n++)
					{
						z = pos_max -1;

						set_colour(cp,0,1);

						for (a=0;a<z-1;a++)
						{					
							larp(n,&pos[a],&pos[a+1],&org[a]);
							set_pixel_fast(cp, org[a].x, org[a].y );
						}

						z --;

						set_colour(cp,0,2);

						for (a=0;a<z-1;a++)
						{					
							larp(n,&org[a],&org[a+1],&des[a]);
						}

						z --;

						set_colour(cp,0,200);

						for (a=0;a<z-1;a++)
						{					
							larp(n,&des[a],&des[a+1],&des2[a]);
						}

						z--;

						set_colour(cp,0,200);

						for (a=0;a<z-1;a++)
						{					
							larp(n,&des2[a],&des2[a+1],&des[a]);
						}

						z --;

						set_colour(cp,0,200);

						for (a=0;a<z-1;a++)
						{					
							larp(n,&des[a],&des[a+1],&des2[a]);
						}

						z--;

						set_colour(cp,0,200);

						for (a=0;a<z-1;a++)
						{					
							larp(n,&des2[a],&des2[a+1],&des[a]);
							set_pixel_fast(cp, des[a].x, des[a].y );

							plains_to_video(cp, screenh, win -> RPort -> BitMap );
							Delay(1);
						}
					}

					plains_to_video(cp, screenh, win -> RPort -> BitMap );
					clear_plains( cp, bpr, screenh );
					Delay(40);
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

