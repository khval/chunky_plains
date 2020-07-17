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

	float r90 =  M_PI / 2;
	float r180 =  M_PI;


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


struct p3d dots[8000];
struct p3d ddots[8000];


void rot( float r, int *a, int *b )
{
	float x1,y1,x2,y2;

	x1 = sin( r - r90) * (float) (*a);
	y1 = cos( r - r90) * (float) (*a);

	x2 = sin( r ) * (float) (*b);
	y2 = cos( r ) * (float) (*b);

	*a = (int) (x1 + x2);
	*b = (int) (y1 + y2);
}

int  MAXC = 3 ;
int  MAXR = 20;

void make_donut()
{
	float ra1,ra2;
	int n,nn;
	int x,y,z;
	int ro = 40;
	int ri =  120;
	int idx;

	for (nn=0;nn<MAXC;nn++)
	{
		ra2 = 2 * M_PI  * ((float) nn) / MAXC;

		for (n=0;n<MAXR;n++)
		{
			ra1 = 2*M_PI * ((float ) n) / ((float)  (MAXR-1));

			y = sin( ra1 ) * ro ;
			x = cos( ra1 ) * ro + ri;
			z = 0;

			idx = nn*MAXR+n;

			rot( ra2  ,&x,&z);

			dots[idx].x = x;
			dots[idx].y = y;
			dots[idx].z = z;
		}
	}
}

#define set_color_nice( n, r, g, b ) SetRGB32( &src -> ViewPort ,n , (unsigned int)  r * 0x01010101, (unsigned int) g * 0x01010101, (unsigned int) b * 0x01010101)


void set_palette(struct Screen *src)
{
	long long int r,g,b;
	int y;

	set_color_nice( 0 ,  000 ,  000 , 000 );
	set_color_nice( 1 ,  255 ,  000 , 000 );
	set_color_nice( 2 ,  000 ,  000 , 255 );
	set_color_nice( 3 ,  255 ,  255 , 255 );

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

	void *tmp;
	BPTR bmlock;
	int x,y,z;
	int n,nn,nnn;
	ULONG bpr;
	float g;
	float zoom;

	int a;
	int rx,ry;
	int cx;
	int cy;
	int cnt = 0;

	float x1,y1,x2,y2;
	float ra;
	

//	InitRastPort(&scroll_rp);


	cx = 45;
	cy = 45;

	screenw = 640;
	screenh = 480;
	cx = screenw / 2;
	cy = screenh / 2;

	src = OpenScreenTags( NULL,
		P96SA_DisplayID, 0x50051000,
		P96SA_Title, (ULONG) "666",
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

		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, &cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			int p0,p1,p2,p3;

			set_colour(cp,0,1);

			cnt = 0;

			for (g=0;g<360 * 4 + 45;g++)
			{
				plains_to_video(cp, screenh, win -> RPort -> BitMap );
				clear_plains( cp );

				ra = (2.0f * M_PI * (float) g / 360.0f);

				if (cnt == 40)
				{	
					if (MAXC<50) MAXC++;
					cnt = 0;
				}
				cnt++;

				make_donut();

				for (nnn=0;nnn<2;nnn++)
				{

				for (n=0;n<(MAXC*MAXR);n++)
				{
					x = dots[n].x;
					y = dots[n].y;
					z = dots[n].z;

					if (nnn==0)
					{
						rot(  ra  ,&x,&y);
						rot(  ra  ,&y,&z);
					}
					else
					{
						rot(  ra *2  ,&y,&x);
						rot(  ra  ,&x,&z);
					}

					zoom =(100 / (float) ( z + 200)) *2 ; 
					x1 = (float) x * zoom ;
					y1 = (float) y * zoom ;

					ddots[n].x = x + cx;
					ddots[n].y = y + cy;
					ddots[n].z = zoom;
				}

				for (nn=0;nn<MAXC;nn++)
				{
					for (n=0;n<MAXR;n++)
					{
						p0 = n;
						p1 = (n+1) % MAXR;
						p2 = n+MAXR;
						p3 = p1+MAXR;

						p0 = (p0 + (nn*MAXR)) % (MAXR*MAXC);
						p1 = (p1 + (nn*MAXR)) % (MAXR*MAXC);
						p2 = (p2 + (nn*MAXR)) % (MAXR*MAXC);
						p3 = (p3 + (nn*MAXR)) % (MAXR*MAXC);

					triangle(cp,
						ddots[ p0 ].x  , ddots[ p0 ].y ,
						ddots[ p1 ].x  , ddots[ p1 ].y ,
						ddots[ p2 ].x  , ddots[ p2 ].y ,
						nnn, 1 );

					}
				}
				}
			}

			for (n=0;n<400;n++)
			{
				plains_to_video(cp, screenh, win -> RPort -> BitMap );
				scroll(cp,0,0,2,screenw,screenh-2,0,-1);
				scroll(cp,1,0,2,screenw,screenh-2,0,1);
				Delay(1);
			}

			Delay(10);

			free_chunky_plains(cp);
		}

		FreeBitMap(bitmap);
		CloseWindow(win);
		CloseScreen(src);
	}
}

