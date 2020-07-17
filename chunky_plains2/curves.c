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
struct xy pixel[10000];
struct xy curv[10000];
struct xy pos[100];

int pixels = 10000;
int pos_max = 30;

int max_len = 0;



void avg_dot( struct xy *from,struct xy *to, int n, int dif)
{
	int min_n;
	int max_n;
	int x;
	int y;
	int dx;
	int dy;

	min_n = n-dif;
	max_n = n+dif;

	if (min_n<0) min_n = 0;
	if (max_n>max_len-1) max_n = max_len -1;

	to[n].x = (from[ max_n ].x + from[ min_n ].x) / 2;
	to[n].y = (from[ max_n ].y + from[ min_n ].y) / 2;


/*
	dx = (from[ max_n ].x - from[ min_n ].x) / 2;
	dy = (from[ max_n ].y - from[ min_n ].y) / 2;

	to[n].x = from[ min_n ].x + dx;
	to[n].y = from[ min_n ].y + dy;
*/
}

void insert_line( struct xy *at, int part_len, int x, int y,int dx,int dy )
{
	int px;
	int py;
	int n;

	for (n=0;n<part_len;n++)
	{
		at->x = (n * dx / part_len) + x;
		at->y = (n * dy / part_len) + y;
		at++;
	}

}

void init()
{
	int n;
	int dx,dy;

	int part_len =0;

	max_len = 0;
	part_len = 0;

	for (n=0;n<pos_max;n++)
	{
		pos[n].x = rand() % 640;
		pos[n].y = rand() % 480;

		if (n>0)
		{
			dx = pos[n].x - pos[n-1].x;
			dy = pos[n].y - pos[n-1].y;

			part_len= sqrt( (dx*dx) + (dy*dy) );
		
			insert_line( org + max_len, part_len, pos[n-1].x, pos[n-1].y, dx, dy );

			max_len += part_len;
		}
	}

}


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



void set_palette(struct Screen *src)
{
	long long int r,g,b;
	int y;

	SetRGB32( &src -> ViewPort ,0,0x0,0,0);

	for (y=0;y<256;y++)
	{

		SetRGB32( &src -> ViewPort ,y ,y * 0x01010101, y * 0x01010101, y * 0x01010101);
	}
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

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	       		bitmap = AllocBitMap( screenw, screenh, 8, BMF_CLEAR, win -> RPort -> BitMap);

	printf("%s:%d\n",__FUNCTION__,__LINE__);

			scroll_rp.Font =  win -> RPort -> Font;
			scroll_rp.BitMap = bitmap;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

			bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

	printf("%s:%d\n",__FUNCTION__,__LINE__);

			if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, screenw,screenh,&cp))
			{
				printf("error code: %d\n",err);
			}
			else
			{

				Delay(50);

				set_colour(cp,0,1);

	for(a=0;a<40;a++)
	{

				init();

				for (z=0;z<50;z++)
				{
					draw_curv(cp, 0, 200, pos_max, pos);

//					draw_curv(cp, 0, 200, max_len , org);

					for (n=0;n<max_len;n++)
					{
						avg_dot(org,pixel,n, abs(z - 100) );
					} 

		
					for ( zz=0;zz<z;zz++)
					{
						for (n=0;n<max_len;n++)
						{
							avg_dot(pixel,curv,n, abs(50) );
						} 

						for (n=0;n<max_len;n++)
						{
							avg_dot(curv,pixel,n, abs(50) );
						} 
					}


					draw_curv(cp, 0, 200, max_len , curv);


					plains_to_video(cp, screenh, win -> RPort -> BitMap );

					Delay(1);

					clear_plains( cp, bpr, screenh );

				}
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

