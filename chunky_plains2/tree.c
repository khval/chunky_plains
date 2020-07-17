#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

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
	int n;


	SetRGB32( &src -> ViewPort ,0,0x0,0,0);

	for (n=1;n<10;n++)
	{
			SetRGB32( &src -> ViewPort ,n ,000 * 0x01010101, ((n*10)+100) * 0x01010101, 000 * 0x01010101);
	}	

	set_color_nice( 1 ,  255 ,  000 , 000 );
	set_color_nice( 2 ,  000 ,  255 , 000 );
	set_color_nice( 3 ,  000 ,  000 , 255 );


}

	struct chunky_plains *cp;
	struct Window *win;
	int screenw,screenh;

int v1(int x, int y, int g, int l, int n, int nn)
{
	float ra;
	int x1,y1;

	int t,i;

	ra = 2 * M_PI * g / 360;

	x1 = cos( ra ) * l;
	y1 = - sin( ra ) * l;

	line(cp,x, y, x + x1, y + y1,0,nn );
	if (nn==5)	plains_to_video(cp, screenh, win -> RPort -> BitMap );

	if (n>0)
	{
		t = 4;
//		t = 1 << nn ;
	
		for (i=0;i<t;i++)
		{
			v1(x+x1,y+y1,g + ( (i * 90 / (t-1)) - 45),l * 2 / 3,n-1,nn+1);
		}
	}
}

int main()
{
	struct Screen *src = NULL;
	struct BitMap *bitmap;
	struct RastPort scroll_rp;

	int err;

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
	

	cx = 45;
	cy = 45;

	screenw = 640;
	screenh = 480;
	cx = screenw / 2;
	cy = screenh / 2;


	src = OpenScreenTags( NULL,
		SA_DisplayID, 0x50051000,
		SA_Title, (ULONG) "666",
		SA_Quiet, FALSE,
		TAG_END	);

	if ( src )
	{
		set_palette(src);

		win = OpenWindowTags(NULL,
			WA_Left, 0, WA_Top, 0,
			WA_Width, screenw, WA_Height, screenh,
			WA_NoCareRefresh, FALSE,
			WA_Borderless, FALSE,
			WA_Activate, TRUE,
			WA_RMBTrap, TRUE,
			WA_ReportMouse, TRUE,
			WA_CustomScreen, (ULONG) src,
			TAG_END
		);
/*
       		bitmap = AllocBitMap( screenw, screenh, 8, BMF_CLEAR, win -> RPort -> BitMap);

		scroll_rp.Font =  win -> RPort -> Font;
		scroll_rp.BitMap = bitmap;

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, 320,200,&cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			int p0,p1,p2,p3;

			set_colour(cp,0,1);

			clear_plains( cp, bpr, screenh );

			v1(screenw/2,screenh/2, 90, 50, 15 / 2,1 );

//			plains_to_video(cp, screenh, win -> RPort -> BitMap );
			plains_to_rp(cp, screenh, win -> RPort );

			Delay(200);

			free_chunky_plains(cp);
		}
		
		FreeBitMap(bitmap);
*/


	RectFill( win -> RPort, 50, 50, 100, 100);


	char *f_buffer = AllocVecTags( 100 * 100 , AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 3, TAG_END);

	if (f_buffer)
	{
/*
		WritePixelArray(  f_buffer, 
					0,0, 
					100, PIXF_ALPHA8, 
					win -> RPort,55,55,
					100,100	 );
*/
		WriteChunkyPixels( win->RPort ,0 ,0, 100 , 100, f_buffer, 100);

//PIXF_CLUT
//PIXF_ALPHA8


		FreeVec(f_buffer);
	}

		Delay(200);


		CloseWindow(win);
		CloseScreen(src);
	}
}

