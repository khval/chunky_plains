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

struct matrix {
	int p0;
	int p1;
	int p2;
	int plain;
};

struct p3d {
	float x;
	float y;
	float z;
};

struct p3d dots[] = {
	{ 0,-80,0 },
	{  80,80, 80 },	
	{  80,80,-80 },	
	{  -80, 80, 0 },
};

struct p3d cdots[100];

struct matrix  object[] = {
	{0,1,2,0},
	{0,2,3,0},
	{0,3,1,0},
	{1,2,3,0},
};



void set_palette(struct Screen *src)
{
	long long int r,g,b;
	int y;

	SetRGB32( &src -> ViewPort ,0,0x0,0,0);
	SetRGB32( &src -> ViewPort ,1 ,255 * 0x01010101, 255 * 0x01010101, 255 * 0x01010101);
}


void __triangle(struct chunky_plains *cp, int x1, int y1, int x2, int y2, int x3, int y3 )
{
	int dx;
	int dy;
	int _x1,_y1,_x2,_y2,_x3,_y3;

	line(cp, x1,y1,x2, y2, 0, 1);
	line(cp, x2,y2,x3, y3, 0, 1);
	line(cp, x3,y3,x1, y1, 0, 1);

	dx = (x2 - x1) / 2;
	dx = (y2 - y1) / 2;

	_x1 = x1 + dx;
	_y1 = y1 + dy;

	dx = (x3 - x2) / 2;
	dx = (y3 - y2) / 2;

	_x2 = x2 + dx;
	_y2 = y2 + dy;

	dx = (x1 - x3) / 2;
	dx = (y1 - y3) / 2;

	_x3 = x3 + dx;
	_y3 = y3 + dy;

}


void _triangle(struct chunky_plains *cp, int x1, int y1, int x2, int y2, int x3, int y3, int cnt )
{
	int dx;
	int dy;
	int _x1,_y1,_x2,_y2,_x3,_y3;

	if (cnt == 0) return;

	line(cp, x1,y1,x2, y2, 0, 1);
	line(cp, x2,y2,x3, y3, 0, 1);
	line(cp, x3,y3,x1, y1, 0, 1);

	dx = (x2 - x1) / 2;
	dy = (y2 - y1) / 2;

	_x1 = x1 + dx;
	_y1 = y1 + dy;

	dx = (x3 - x2) / 2;
	dy = (y3 - y2) / 2;

	_x2 = x2 + dx;
	_y2 = y2 + dy;

	dx = (x1 - x3) / 2;
	dy = (y1 - y3) / 2;

	_x3 = x3 + dx;
	_y3 = y3 + dy;

	_triangle(cp,_x1, _y1, _x2, _y2, _x3, _y3, cnt-1 );

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
	int screenw,screenh;

	int txt_pos = 0;

	char my_text[]="The Demo king is back..... whit a system frendly demo ...... :-)                       ";

	void *tmp;
	BPTR bmlock;
	int x,y;
	int n,anim;
	ULONG bpr;
	float g;
	float rot,r;
	float zoom;


	float r90 = 1/4 * PI ;
	float r180 = 2/4 * PI;
	float r220 = 3/4 * PI;

	int a;
	int rx,ry;
	int cx;
	int cy;

	float x1,y1,x2,y2;

	InitRastPort(&scroll_rp);

	// 1 = 2
	// 2 = 4
	// 3 = 8
	// 4 = 16

	// 2 + 6 = 8bits ( 4 colors * 64 colors ) = 256 colors
	// 4 + 4 = 8bits (16 colors * 16 colors) = 256 colors

	screenw = 640;
	screenh = 480;
	cx = screenw / 2;
	cy = screenh / 2;


	src = OpenScreenTags( NULL,
		SA_DisplayID, 0x50051000,
		SA_Title, (ULONG) "666",
		SA_Exclusive, TRUE,
		TAG_END	);

	if ( src )
	{
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

       		bitmap = AllocBitMap( screenw, screenh, 8, BMF_CLEAR, win -> RPort -> BitMap);

		scroll_rp.Font =  win -> RPort -> Font;
		scroll_rp.BitMap = bitmap;

		if (src)
		{
			set_palette(src);

			printf("Bitmap %x\n",src -> BitMap);
			printf("BytesPerRow %d\n", src -> BitMap.BytesPerRow); 
			printf("Rows %d\n", src -> BitMap.Rows);
		}

		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		Delay(20);

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1,&cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			for (rot=0;rot<400;rot++)
			{
				Delay(1);
				// show buffer


				plains_to_rp(cp, win -> RPort );

				// clear buffer

				clear_plains( cp );


				// calc rotx

				r = (float) rot / 100 * PI;

					r90 = (2*PI) / 4;

				for (n=0;n<4;n++)
				{
					cdots[n].x = dots[n].x;
					cdots[n].y = dots[n].y;
					cdots[n].z = dots[n].z;
				}

				// rot xz

				for (n=0;n<4;n++)
				{
					x1 = sin( r - r90) * cdots[n].x;
					y1 = cos( r - r90) * cdots[n].x;
					x2 = sin( r ) * cdots[n].z;
					y2 = cos( r ) * cdots[n].z;

					cdots[n].x = x1 + x2;
					cdots[n].z = y1 + y2;
				}

				// do 

				for (n=0;n<4;n++)
				{
					zoom =(100 / (float) (cdots[n].z+200)) *2 ; 

					x1 = (float) cdots[n].x * zoom ;
					y1 = (float) cdots[n].y * zoom ;

					cdots[n].x = x1;
					cdots[n].y = y1;
				}

				// draw faces

				for (n=0;n<4;n++)
				{
					printf("%d,%d,%d\n", object[n].p0, object[n].p1, object[n].p2 );


					_triangle(	cp, cdots[ object[n].p0 ].x  + cx , cdots[ object[n].p0 ].y + cy,
						cdots[ object[n].p1 ].x  + cx , cdots[ object[n].p1 ].y + cy,
						cdots[ object[n].p2 ].x  + cx , cdots[ object[n].p2 ].y + cy , 8 );
						
				}
			}

			Delay(100);
			free_chunky_plains(cp);
		}
		
		Delay(100);

		FreeBitMap(bitmap);
		CloseWindow(win);
		CloseScreen(src);
	}
}

