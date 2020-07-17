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

void bitmap_to_plain( struct BitMap *bm , int x, int y, struct chunky_plains *cp , int number , int tx, int ty , int w, int h)
{
	struct RenderInfo ri;
	uint32 BMLock;
	register char *data = cp -> data[ number ];
	char *src;
	char *des;
	register int py;
	char *ptr;
	int bpr;

	if (BMLock = p96LockBitMap( bm, (UBYTE*) &ri, sizeof(ri)))
	{
		bpr = ri.BytesPerRow;
		ptr =(char *) ri.Memory;

		for (py=0;py<h;py++)
		{
			memcpy( data + ((py+ty) * bpr) + tx,  ptr + ( (py+y) *  bpr) + x, w );
		}

		p96UnlockBitMap(bm,BMLock);
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
	int a;
	int rx,ry;

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

		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		Delay(20);

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 4,2,2,0,0,0,0,0, &cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{

			for (rot=0;rot<1000;rot++)
			{
				Delay(1);
				// show buffer

				plains_to_video(cp, screenh, win -> RPort -> BitMap );
				clear_plains( cp );

				ScrollRaster( &scroll_rp, 2,0,0,0,320,20 );

				if (( (int) rot % (15/2))==0)
				{
					SetAPen( &scroll_rp, 2 << cp -> index[2] );
					Move( &scroll_rp, 300,10 );
					lazy_char( &scroll_rp, my_text[ (((int) rot) / (15/2))   % strlen(my_text)  ] );
				}

				for (x=0;x<300;x++)
				{
					bitmap_to_plain( scroll_rp.BitMap , x, 0, cp , 2, x, sin( (float) x / 100 *PI) * 50 + 120  , 1, 15 );
				}

				g = 2 * PI * rot / 500;

				star(cp,g,200,200,100,200,0,3);
				star(cp,-g,200,200,200,100,1,2);
			}

			Delay(100);
			Delay(100);

			free_chunky_plains(cp);
		}
		
		Delay(100);

		FreeBitMap(bitmap);

		CloseWindow(win);
		p96CloseScreen(src);
	}
}

