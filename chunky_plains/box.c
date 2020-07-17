#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/Picasso96API.h>

#include "chunky_plains.h"

struct matrix {
	int p0;
	int p1;
	int p2;
	int plain;
};

struct p3d {
	int x;
	int y;
	int z;
};

struct p3d dots[] = {
	{ -40,-40,40 },
	{  40,-40,40 },
	{  40, 40,40 },
	{ -40, 40,40 },
	{ -40,-40,-40 },
	{  40,-40,-40 },
	{  40, 40,-40 },
	{ -40, 40,-40 }
};

struct p3d cdots[100];

struct matrix  object[] = {
	{0,1,2,0},
	{0,2,3,0},

	{4,5,6,1},
	{4,6,7,1},

	{1,5,6,2},
	{1,2,6,2},

	{3,2,7,1},
	{7,2,6,1},

	{3,2,7,1},
	{7,2,6,1},

	{4,0,3,0},
	{4,3,7,0}

};


void star(void *cp,float g,int sx,int sy,int r1,int r2,int p, int color)
{
	float r;
	int x1,y1,x2,y2;
	int rx,ry;
	int a,n;
	struct p pt[30];

	int radius;

	r = 2 * PI * 0 / 10;		

	radius =  (0&1 ? r1 : r2 );

	rx = sin( r +g ) * radius;
	ry = cos( r +g ) * radius;

	for (a=0;a<11;a++)
	{		
		r = 2 * PI * a / 10;	
		radius =  (a&1 ? r1 : r2 );				
								
		rx = sin( r +g) * radius;
		ry = cos( r +g) * radius;
		
		pt[a].x = sx + rx;
		pt[a].y = sy + ry;
	}

	for (a=0;a<(10/2);a++)
	{
		n = a * 2;
		triangle(cp,pt[n].x,pt[n].y,pt[n+1].x,pt[n+1].y,pt[n+2].x,pt[n+2].y, p,color );
		triangle(cp,pt[n].x,pt[n].y,sx,sy,pt[n+2].x,pt[n+2].y, p,color );
	}
}

void lazy_text(struct RastPort *rp, char *txt)
{
	if (txt) Text(rp,txt,strlen(txt));
}

void lazy_char(struct RastPort *rp, char c)
{
	char cc[2]={c,0};
	Text(rp,cc,1);
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
	PLANEPTR ptr;
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

	int x1,y1,x2,y2;

	InitRastPort(&scroll_rp);

	// 1 = 2
	// 2 = 4
	// 3 = 8
	// 4 = 16

	// 2 + 6 = 8bits ( 4 colors * 64 colors ) = 256 colors
	// 4 + 4 = 8bits (16 colors * 16 colors) = 256 colors

	screenw = 640;
	screenh = 480;
	cx = 320 / 2;
	cy = 200 / 2;


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
			SetRGB32( &src -> ViewPort ,0,0xFFFFFFFF,0,0);

			for (y=0;y<255;y++)
			{
				SetRGB32( &src -> ViewPort ,1,y * 0x01010101, (255-y)* 0x01010101,y*0x01010101);
			}

			printf("Bitmap %x\n",src -> BitMap);
			printf("BytesPerRow %d\n", src -> BitMap.BytesPerRow); 
			printf("Rows %d\n", src -> BitMap.Rows);
		}

		ptr =  (char *) p96GetBitMapAttr( win -> RPort -> BitMap , P96BMA_MEMORY);
//		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		Delay(20);

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 4,2,2,0,0,0,0,0, 320,200,&cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			cp -> data = AllocVec(cp -> bytes_per_row * 480,MEMF_ANY);

			for (rot=0;rot<400;rot++)
			{
				Delay(1);
				// show buffer
				memcpy( ptr, cp->data,cp -> bytes_per_row * 200 );

				// clear buffer
				bzero(cp -> data, cp -> bytes_per_row * 200); 

				memcpy( cp->data, p96GetBitMapAttr( bitmap , P96BMA_MEMORY) + (cp -> bytes_per_row * 20 ) ,cp -> bytes_per_row * 200 );

				// calc rotx

				r = (float) rot / 100 * PI;

					r90 = (2*PI) / 4;

				// rot xy

				for (n=0;n<8;n++)
				{
					x1 = sin( r - r90) * dots[n].x;
					y1 = cos( r - r90) * dots[n].x;
					x2 = sin( r ) * dots[n].y;
					y2 = cos( r ) * dots[n].y;

					cdots[n].x = x1 + x2;
					cdots[n].y = y1 + y2;
					cdots[n].z = dots[n].z;
				}

				// rot xz

				for (n=0;n<8;n++)
				{
					x1 = sin( r - r90) * cdots[n].x;
					y1 = cos( r - r90) * cdots[n].x;
					x2 = sin( r ) * cdots[n].z;
					y2 = cos( r ) * cdots[n].z;

					cdots[n].x = x1 + x2;
					cdots[n].z = y1 + y2;
				}

				// do 

				for (n=0;n<8;n++)
				{
					zoom =50 / (float) (cdots[n].z+100) ; 

					x1 = (float) cdots[n].x * zoom ;
					y1 = (float) cdots[n].y * zoom ;

//					printf("%d %d %02f\n",x1,y1,zoom);

					cdots[n].x = x1;
					cdots[n].y = y1;
				}


				// draw faces

				for (n=0;n<12;n++)
				{

					triangle(cp,

						cdots[ object[n].p0 ].x  + cx , cdots[ object[n].p0 ].y + cy,
						cdots[ object[n].p1 ].x  + cx , cdots[ object[n].p1 ].y + cy,
						cdots[ object[n].p2 ].x  + cx , cdots[ object[n].p2 ].y + cy,
						object[n].plain,1);
						
/*
					line(cp,	cdots[ object[n].p0 ].x  + cx , cdots[ object[n].p0 ].y + cy,
							cdots[ object[n].p1 ].x  + cx , cdots[ object[n].p1 ].y + cy, 0, 2 );

					line(cp,	cdots[ object[n].p1 ].x  + cx , cdots[ object[n].p1 ].y + cy,
							cdots[ object[n].p2 ].x  + cx , cdots[ object[n].p2 ].y + cy, 0, 2 );

					line(cp,	cdots[ object[n].p2 ].x  + cx , cdots[ object[n].p2 ].y + cy,
							cdots[ object[n].p0 ].x  + cx , cdots[ object[n].p0 ].y + cy, 0, 2 );
*/
				}

			}

			memcpy( ptr, cp->data,cp -> bytes_per_row * 200 );

			Delay(100);
			Delay(100);

			FreeVec(cp -> data);

			free_chunky_plains(cp);
		}
		
		Delay(100);

		FreeBitMap(bitmap);

		CloseWindow(win);
		p96CloseScreen(src);
	}
}

