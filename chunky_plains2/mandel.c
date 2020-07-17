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


void set_palette(struct Screen *src)
{
	long long int r,g,b;
	long long int rgb;
	int y;

	if (src)
	{
		SetRGB32( &src -> ViewPort ,0,0x0,0,0);	

		for (y=0;y<255;y++)
		{
			r = y;	g = y;	b = y;
	
			SetRGB32( &src -> ViewPort ,y ,((int) r) * 0x01010101, ((int) g) * 0x01010101, ((int) b) * 0x01010101);
		}
	}
}



	float cx;
	float cy;



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

	float r90 = 1/4 * PI ;
	float r180 = 2/4 * PI;
	float r220 = 3/4 * PI;

	int a;
	int rx,ry;

	int effect;

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

		Delay(20);

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 8,0,0,0,0,0,0,0, &cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			float ax,ay,scale;
			float a1,b1,limit;
			float a2,b2;
			int lp;

			cx = 0.5f;
			cy =  0.3f;
			scale = 0.02f;
			limit = 4.0f;

			plains_to_video(cp, screenh, win -> RPort -> BitMap );
			// clear buffer
			clear_plains( cp );
			// draw faces

			for (n=0;n<500;n++)
			{


			for (x =-(screenw/2);x<(screenw/2);x++)
				for (y=-(screenh/2);y<(screenh/2);y++)
			{

				ax =  (float) x * scale + cx; 
				ay =  (float) y * scale + cy;

				a1 = ax; 
				b1 = ay;
				lp = 0;

				do
				{
					lp ++;
					a2 = (a1*a1) - (b1*b1) + ax;
					b2 = (2*a1*b1) + ay;
					a1 = a2;
					b1 = b2;
				} while  (!((lp>255) || ((a1*a1)+(b1*b1)>limit  ) ));


				set_pixel( cp, 0, lp, (screenw/2) + x , (screenh/2) + y );
			}

			plains_to_rp(cp,  win -> RPort );
			// clear buffer
			clear_plains( cp );
			// draw faces


				scale -= 0.0001f;

			}

			plains_to_rp(cp, win -> RPort  );
			// clear buffer
			clear_plains( cp );
			// draw faces


			Delay(100);

			free_chunky_plains(cp);
		}
		
		Delay(50);

		if (bitmap) FreeBitMap(bitmap);
		if (win) CloseWindow(win);
		if (src) CloseScreen(src);
	}
}

