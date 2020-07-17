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

	int cx;
	int cy;

struct matrix {
	int p0;
	int p1;
	int p2;
	int plain;
	int z;
};

struct p3d {

	// display 

	double dx;
	double dy;
	double dz;

	// vector

	double x;
	double y;
	double z;
};

struct pa 
{
	struct matrix *ma;
};

struct p3d dots[8000];
struct p3d ddots[8000];

struct matrix ma[8000];
struct pa	 paint[8000];



void rot( double r, double *a, double *b )
{
	double x1,y1,x2,y2;

	x1 = sin( r - r90) *  (*a);
	y1 = cos( r - r90) * (*a);

	x2 = sin( r ) * (*b);
	y2 = cos( r ) * (*b);

	*a = (int) (x1 + x2);
	*b = (int) (y1 + y2);
}

int  MAXC = 3 ;
int  MAXR = 20;

void make_donut()
{
	double ra1,ra2;
	int n,nn;
	double x,y,z;
	int ro = 40;
	int ri =  100;
	int idx;

	for (nn=0;nn<MAXC;nn++)
	{
		ra2 = 2 * M_PI  * ((double) nn) / MAXC;

		for (n=0;n<MAXR;n++)
		{
			ra1 = 2*M_PI * ((double ) n) / ((double)  (MAXR-1));

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

void insert_into_paint(struct matrix *ma)
{
	int n=0;
	int c;
	int size;

	while (paint[n].ma)
	{
		if (ma -> z < paint[n].ma -> z)
		{
			size = sizeof(paint) - ( sizeof(struct pa) * (n+1) );

			if (n<8000)
			{
				for (c=8000;c>n;c--) paint[c] = paint[c-1];
				paint[n].ma = ma;
				return;
			}
			else
			{
				printf("bad idx value\n");
				return;
			}
		}
		n++;
	}

	if (n<8000)
	{
		for (c=8000;c>n;c--) paint[c] = paint[c-1];
		paint[n].ma = ma;
	}
}


void diff_paint(int *mi, int *ma)
{
	int p0,p1,p2;
	int n;

	for (n=0;paint[n].ma;n++)
	{
		if (*mi < paint[n].ma -> z) *mi = paint[n].ma -> z;
		if (*ma > paint[n].ma -> z) *ma = paint[n].ma -> z;

	}
}

void conv_2d_to_3d(double x, double y, double z, double *xx,double *yy)
{

	double zoom;
	zoom =(100 / (double) (z+200)) *2 ; 
	*xx = (double) x * zoom + cx;
	*yy = (double) y * zoom + cy;
}


void _triangle(struct chunky_plains *cp, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, int cnt ,  int color )
{
	double dx, dy, dz;
	double _x1,_y1,_z1,_x2,_y2,_z2,_x3,_y3,_z3;

	if (cnt == 0) return;

	conv_2d_to_3d(x1,y1,z1, &_x1,&_y1);
	conv_2d_to_3d(x2,y2,z2, &_x2,&_y2);
	conv_2d_to_3d(x3,y3,z3, &_x3,&_y3);

	line(cp, _x1,_y1, _x2, _y2, 0, color);
	line(cp, _x2,_y2, _x3, _y3, 0, color);
	line(cp, _x3,_y3, _x1, _y1, 0, color);

	dx = (x2 - x1) / 2;
	dy = (y2 - y1) / 2;
	dz = (z2 - z1) / 2; 

	_x1 = x1 + dx;
	_y1 = y1 + dy;
	_z1 = z1 + dz;

	dx = (x3 - x2) / 2;
	dy = (y3 - y2) / 2;
	dz = (z3 - z2) / 2; 

	_x2 = x2 + dx;
	_y2 = y2 + dy;
	_z2 = z2 + dz;

	dx = (x1 - x3) / 2;
	dy = (y1 - y3) / 2;
	dz = (z1 - z3) / 2; 

	_x3 = x3 + dx;
	_y3 = y3 + dy;
	_z3 = z3 + dz;

	_triangle(cp,_x1, _y1, _z1, _x2, _y2, _z2, _x3, _y3, _z3, cnt-1 , color );

}

void display_paint(struct chunky_plains *cp)
{
	int p0,p1,p2;
	int n;
	int c;
	int mi,ma;

	diff_paint(&mi,  &ma);

	for (n=0;paint[n].ma;n++)
	{
		p0 = paint[n].ma -> p0;
		p1 = paint[n].ma -> p1;
		p2 = paint[n].ma -> p2;

		c = (255 - (paint[n].ma -> z - mi) * 255 / (ma-mi));

		_triangle(cp,
			ddots[ p0 ].x  , ddots[ p0 ].y , ddots[ p0 ].z ,
			ddots[ p1 ].x  , ddots[ p1 ].y , ddots[ p1 ].z ,
			ddots[ p2 ].x  , ddots[ p2 ].y , ddots[ p2 ].z ,
			3 ,  c );

/*
		triangle(cp,
			ddots[ p0 ].dx  , ddots[ p0 ].dy ,
			ddots[ p1 ].dx  , ddots[ p1 ].dy ,
			ddots[ p2 ].dx  , ddots[ p2 ].dy ,
			0, c );
*/
	}
}


int  count_paint()
{
	int c;
	int n = 0;

	for (c=0;c<8000;c++)
	{
		if (paint[c].ma) n++;
	}

	printf("paint count %d\n",n);

	return n;
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

int calc_z(int p0,int p1,int p2, int cx,int cy,int cz)
{
	int x,y,z;

	x = ddots[ p0 ].x;
	x += ddots[ p1 ].x;
	x += ddots[ p2 ].x;
	x /= 3;

	y = ddots[ p0 ].y;
	y += ddots[ p1 ].y;
	y += ddots[ p2 ].y;
	y /= 3;

	z = ddots[ p0 ].z;
	z += ddots[ p1 ].z;
	z += ddots[ p2 ].z;
	z /= 3;

	x-=cx;
	y-=cy;
	z-=cz;

	return sqrt( (x*x) + (y*y) + (z*z) );
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
	double x,y,z;
	int n,nn,nnn;
	ULONG bpr;
	double g;
	double zoom;

	int a;
	int rx,ry;

	int cnt = 0;
	int idx = 0;

	double x1,y1,x2,y2;
	double ra;
	

//	InitRastPort(&scroll_rp);


	cx = 45;
	cy = 45;

	screenw = 640;
	screenh = 480;
	cx = screenw / 2;
	cy = screenh / 2;


	src = OpenScreenTags(NULL,
		SA_DisplayID, 0x50051000,
		SA_Title, (ULONG) "666",
		SA_Quiet, TRUE,
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
				plains_to_rp(cp, win -> RPort );
				clear_plains( cp );

				ra = (2.0f * M_PI * (double) g / 360.0f);

				if (cnt == 40)
				{	
					if (MAXC<50) MAXC++;
					cnt = 0;
				}
				cnt++;

				make_donut();

				for (nnn=0;nnn<1;nnn++)
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

						ddots[n].x = x ;
						ddots[n].y = y ;
						ddots[n].z = z ;

						ddots[n].dx = x + cx;
						ddots[n].dy = y + cy;
						ddots[n].dz = zoom;
					}

					bzero( paint, sizeof(paint) ) ;

					for (nn=0;nn<MAXC;nn++)
					{
						for (n=0;n<MAXR;n++)
						{
							idx = (nn * MAXR) + n;

							p0 = n;
							p1 = (n+1) % MAXR;
							p2 = n+MAXR;
							p3 = p1+MAXR;

							p0 = (p0 + (nn*MAXR)) % (MAXR*MAXC);
							p1 = (p1 + (nn*MAXR)) % (MAXR*MAXC);
							p2 = (p2 + (nn*MAXR)) % (MAXR*MAXC);
							p3 = (p3 + (nn*MAXR)) % (MAXR*MAXC);

							if (idx<800)
							{
								ma[idx].p0 = p0;
							 	ma[idx].p1 = p1;
							 	ma[idx].p2 = p2;
							 	ma[idx].z = calc_z(p0,p1,p2, 0,0, 30);

								insert_into_paint(&ma[idx]);
							}
						}
						display_paint(cp);
					}
				}
			}
			Delay(10);

			free_chunky_plains(cp);
		}

		FreeBitMap(bitmap);
		CloseWindow(win);
		CloseScreen(src);
	}
}

