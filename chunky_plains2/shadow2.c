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

int h[640];

void set_palette(struct Screen *src)
{
	long long int r,g,b;
	int y;
			SetRGB32( &src -> ViewPort ,0,0x0,0,0);

			SetRGB32( &src -> ViewPort ,1 ,((int) 255) * 0x01010101, ((int) 000) * 0x01010101, ((int) 000) * 0x01010101);
			SetRGB32( &src -> ViewPort ,2 ,((int) 000) * 0x01010101, ((int) 255) * 0x01010101, ((int) 000) * 0x01010101);
			SetRGB32( &src -> ViewPort ,3 ,((int) 000) * 0x01010101, ((int) 000) * 0x01010101, ((int) 255) * 0x01010101);
			SetRGB32( &src -> ViewPort ,4 ,((int) 255) * 0x01010101, ((int) 255) * 0x01010101, ((int) 255) * 0x01010101);

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
	cx = screenw / 2;
	cy = screenh / 2;


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
			set_palette(src);

			printf("Bitmap %x\n",src -> BitMap);
			printf("BytesPerRow %d\n", src -> BitMap.BytesPerRow); 
			printf("Rows %d\n", src -> BitMap.Rows);
		}

		ptr =  (char *) p96GetBitMapAttr( win -> RPort -> BitMap , P96BMA_MEMORY);
		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		for (x = 0; x<320; x++) h[x]=0;


		for (x = 50; x<100; x++) h[x]=50;

		for (x = 150; x<200; x++) h[x]=100;

		for (x = 250; x<300; x++) h[x]=50;

		for (x = 350; x<400; x++) h[x]= 50 * sin( M_PI * (x - 350) / 50 );

		for (x = 450; x<500; x++) h[x]= 100 * sin( M_PI * (x - 450) / 50 );


		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, 320,200,&cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			int lx,ly;
			int dx,dy;
			int ax,ay,xx,yy;
			int light;

			clear_plains( cp, bpr, screenh );

		for (g=0;g<640*1;g++)
		{

			lx =  screenw/2 + (sin(2*M_PI*g/360) * 200);
			ly = 50 + sin( g / 2.0f ) * 20;

			set_pixel( cp , 0, 1, lx, ly  );

			for (x = 0; x<640; x++)
			{
				y = 200 - h[x];

				dx = lx - x ;
				dy = ly - y ; 

				ax = 1;
				ay = 1;

				if (dx<0) { ax=-1; }
 				if (dy<0) { ay=-1; }


				light = 4;

				yy = y;
				for (xx=x ;(ax>0) ?  (xx<=lx) : (xx>=lx) ;xx+=ax)
				{
					if (xx!=x)
					{
						if (dx>0)
						{ yy = dy * (xx-x) / dx + y ; }
						else if (dx<0)
						{ yy = dy * (x-xx) / -dx + y ; 


						}
						 else { yy = ly; };

//						set_pixel( cp , 0, light, xx, yy-2  );

						if ( (200 - h[xx]) < yy )
						{
							// hit the wall.
//							set_pixel( cp , 0, 1, xx, yy  );
							light = 1;
							break;
						}
					}
				}

				set_pixel( cp , 0, light, x, y+2  );
			}


			plains_to_video(cp, screenh, win -> RPort -> BitMap );
			clear_plains( cp, bpr, screenh );
			Delay(1);
		}

			Delay(200);

			free_chunky_plains(cp);
		}
		
		Delay(50);

		FreeBitMap(bitmap);

		CloseWindow(win);
		p96CloseScreen(src);
	}
}

