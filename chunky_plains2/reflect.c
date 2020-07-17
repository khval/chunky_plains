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

			SetRGB32( &src -> ViewPort ,1 ,((unsigned int) 255) * 0x01010101, ((unsigned int) 000) * 0x01010101, ((unsigned int) 000) * 0x01010101);
			SetRGB32( &src -> ViewPort ,2 ,((unsigned int) 000) * 0x01010101, ((unsigned int) 255) * 0x01010101, ((unsigned int) 000) * 0x01010101);
			SetRGB32( &src -> ViewPort ,3 ,((unsigned int) 000) * 0x01010101, ((unsigned int) 000) * 0x01010101, ((unsigned int) 255) * 0x01010101);
			SetRGB32( &src -> ViewPort ,4 ,((unsigned int) 255) * 0x01010101, ((unsigned int) 255) * 0x01010101, ((unsigned int) 255) * 0x01010101);

}

	float cx;
	float cy;


struct lb
{
	int x;
	int y;
	int l;
	int r;
};

struct lb light[100];


int reflect( int r )
{
	while (r<0) r+=360;
	while (r>360) r-=360;

	if (r<=180)
	{
		return r = 180 -r;
	}
	else
	{
		return r = 180 + (360 -r);
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


	src = OpenScreenTags(NULL,
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

		ptr =  (char *) p96GetBitMapAttr( win -> RPort -> BitMap , P96BMA_MEMORY);
		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);


		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, &cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			int n;
			int x0,y0,x1,y1;
			int r0,r1;


		for (g=0;g<360*5;g++)
		{
			clear_plains( cp );

			for (n = 0; n<8; n++)
			{
				x0 = n * 60 + 50;
				y0 = 200;

				r0 = 360 * n / 14;

				light[n].x = x0;
				light[n].y = 300;
				light[n].l = 50;
				light[n].r = g;

				printf("%d\n",r0);
			}	


			for (n = 0; n<8; n++)
			{
				int c;
				int tr;

				for (c=0;c<3;c++)
				{

					x0 = light[n].x;
					y0 = light[n].y;

					r0 = 360 * n / 14;


					x1 = cos(2*M_PI * light[n].r/360) * light[n].l;
					y1 = -sin(2*M_PI * light[n].r/360) * light[n].l;

					line(cp,x0,y0,x0+x1,y0+y1,0,2);	

					light[n].x += x1;
					light[n].y += y1;
					tr = reflect(light[n].r - r0);

					light[n].r = r0 + tr - 180;

					x0 = x0 +x1;
					y0 = y0 +y1; 

					x1 = cos(2*M_PI * r0/360) * 5;
					y1 = -sin(2*M_PI * r0/360) * 5;

					line(cp,x0-x1,y0-y1,x0+x1,y0+y1,0,1);	

					if (c==0)	printf("%d : %d\n",n,tr);

				}
			}	


			plains_to_video(cp, screenh, win -> RPort -> BitMap );
			clear_plains( cp );
		}
			Delay(200);

			free_chunky_plains(cp);
		}
		
		Delay(50);

		FreeBitMap(bitmap);

		CloseWindow(win);
		CloseScreen(src);
	}
}

