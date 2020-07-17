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

	double r90 =  M_PI / 2.0f ;

	int screenw,screenh;


struct xyz
{
	double x;
	double y;
	double z;
};

struct matrix
{
	struct xyz *a;
	struct xyz *b;
	struct xyz *c;
};

struct xyz pos[100];

struct matrix Matrix[] = {

	{&pos[0],&pos[1],&pos[2]},	
	{&pos[1],&pos[2],&pos[3]},	
	{&pos[2],&pos[3],&pos[4]},	
	{&pos[3],&pos[4],&pos[5]},	
	{&pos[4],&pos[5],&pos[6]},	
	{&pos[5],&pos[6],&pos[7]},	
	{&pos[6],&pos[7],&pos[0]},	
	{&pos[7],&pos[0],&pos[1]},	

	{&pos[16],&pos[17],&pos[18]},	
	{&pos[17],&pos[18],&pos[19]},	
	{&pos[18],&pos[19],&pos[20]},	
	{&pos[19],&pos[20],&pos[21]},	
	{&pos[20],&pos[21],&pos[22]},	
	{&pos[21],&pos[22],&pos[23]},	
	{&pos[22],&pos[23],&pos[16]},	
	{&pos[23],&pos[16],&pos[17]},	

	{&pos[0],&pos[1],&pos[2]},	
	{&pos[1],&pos[2],&pos[10]},	
	{&pos[2],&pos[10],&pos[18]},	
	{&pos[10],&pos[18],&pos[17]},	
	{&pos[18],&pos[17],&pos[16]},	
	{&pos[17],&pos[16],&pos[8]},	
	{&pos[16],&pos[8],&pos[0]},	
	{&pos[8],&pos[0],&pos[1]},

	{&pos[2],&pos[3],&pos[4]},	
	{&pos[3],&pos[4],&pos[12]},	
	{&pos[4],&pos[12],&pos[20]},	
	{&pos[12],&pos[20],&pos[19]},	
	{&pos[20],&pos[19],&pos[18]},	
	{&pos[19],&pos[18],&pos[10]},	
	{&pos[18],&pos[10],&pos[2]},	
	{&pos[10],&pos[2],&pos[3]},

	{&pos[4],&pos[5],&pos[6]},	
	{&pos[5],&pos[6],&pos[14]},	
	{&pos[6],&pos[14],&pos[22]},	
	{&pos[14],&pos[22],&pos[21]},	
	{&pos[22],&pos[21],&pos[20]},	
	{&pos[21],&pos[20],&pos[12]},	
	{&pos[20],&pos[12],&pos[4]},	
	{&pos[12],&pos[4],&pos[5]},

	{&pos[0],&pos[8],&pos[16]},	
	{&pos[8],&pos[16],&pos[23]},	
	{&pos[16],&pos[23],&pos[22]},	
	{&pos[23],&pos[22],&pos[14]},	
	{&pos[22],&pos[14],&pos[6]},	
	{&pos[14],&pos[6],&pos[7]},	
	{&pos[6],&pos[7],&pos[0]},	
	{&pos[7],&pos[0],&pos[8]}	

};

int matrix_max = sizeof(Matrix) /sizeof(struct matrix);

struct xyz org[10000];

struct xyz des[10000];
struct xyz des2[10000];

struct xyz ha,hb;


struct matrix *_matrix;

int pixels = 10000;
int pos_max = 10;

int max_len = 0;

void setxyz( struct xyz *set, double x, double y, double z )
{
	set -> x = x;
	set -> y = y;
	set -> z = z;
}

void movexyz( struct xyz *set, double x, double y, double z)
{
	set -> x += x;
	set -> y += y;
	set -> z += z;
}

void rotxy( struct xyz *set, double r)
{
	double x1,y1,x2,y2;

	x1 = sin( r - r90) * set -> x;
	y1 = cos( r - r90) * set -> x;
	x2 = sin( r ) * set -> y;
	y2 = cos( r ) * set -> y;

	set -> x = x1 + x2;
	set -> y = y1 + y2;

}

void rotxz( struct xyz *set, double r)
{
	double x1,z1,x2,z2;

	x1 = sin( r - r90) * set -> x;
	z1 = cos( r - r90) * set -> x;
	x2 = sin( r ) * set -> z;
	z2 = cos( r ) * set -> z;

	set -> x = x1 + x2;
	set -> z = z1 + z2;

}

/*
void conv_3d_to_2d(double cx, double cy, struct xyz *src, struct xyz *des)
{
	double zoom =(200.0f / (double) ( src -> z+400.0f)) ; 
	des -> x = src -> x * zoom + cx;
	des -> y = src -> y * zoom + cy;
}
*/

void conv_3d_to_2d(double cx, double cy, struct xyz *src, struct xyz *des)
{

	double zoom = 400.0f;
	double zoom_fac;

	zoom_fac = (400 + src -> z) / 400;

	des -> x = src -> x * zoom_fac + cx;
	des -> y = src -> y * zoom_fac + cy;
}


void init()
{
	pos_max = 0;

	setxyz( &pos[0],	-50,	-50,	-50 );
	setxyz( &pos[1],	0,	-50,	-50 );
	setxyz( &pos[2],	50,	-50,	-50 );
	setxyz( &pos[3],	50,	0,	-50 );
	setxyz( &pos[4],	50,	50,	-50 );
	setxyz( &pos[5],	0,	50,	-50 );
	setxyz( &pos[6],	-50,	50,	-50 );
	setxyz( &pos[7],	-50,	0,	-50 );

	setxyz( &pos[8],	-50,	-50,  0 );
	setxyz( &pos[9],	0,	-50,	0 );
	setxyz( &pos[10],	50,	-50,  0 );
	setxyz( &pos[11],	50,	0,	0 );
	setxyz( &pos[12],	50,	50,	0 );
	setxyz( &pos[13],	0,	50,	0 );
	setxyz( &pos[14],	-50,	50,	0 );
	setxyz( &pos[15],	-50,	0,	0 );

	setxyz( &pos[16],	-50,	-50,	50 );
	setxyz( &pos[17],	0,	-50,	50 );
	setxyz( &pos[18],	50,	-50,	50 );
	setxyz( &pos[19],	50,	0,	50 );
	setxyz( &pos[20],	50,	50,	50 );
	setxyz( &pos[21],	0,	50,	50 );
	setxyz( &pos[22],	-50,	50,	50 );
	setxyz( &pos[23],	-50,	0,	50 );

	pos_max = 24;
}


void larp( double n, struct xyz *p0, struct xyz *p1, struct xyz *dest )
{
	dest->x = (n * (p1->x-p0->x))  + p0->x;
	dest->y = (n * (p1->y-p0->y))  + p0->y;
	dest->z = (n * (p1->z-p0->z))  + p0->z;
}

#define set_color_nice( n, r, g, b ) SetRGB32( &src -> ViewPort ,n , (unsigned int)  r * 0x01010101, (unsigned int) g * 0x01010101, (unsigned int) b * 0x01010101)



void set_palette(struct Screen *src)
{
	long long int r,g,b;
	unsigned int y;

	set_color_nice( 0 ,  000 ,  000 , 000 );
	set_color_nice( 1 ,  255 ,  000 , 000 );
	set_color_nice( 2 ,  000 ,  255 , 000 );

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
	double r;

	int txt_pos = 0;

	void *tmp;
	BPTR bmlock;
	double x,y,z;
	int n,nn,nnn;
	ULONG bpr;
	double g;
	double zoom;

	int p,zz;
	int rx,ry;

	int cnt = 0;
	int idx = 0;

	double dice_x = 0;
	double dice_dir_x = 2.0f;
	double dice_y = 0;


	double l;

	src = OpenScreenTags( NULL,
		SA_DisplayID, 0x50051000,
		SA_Title, (ULONG) "666",
		SA_FullPalette, TRUE,
		SA_Exclusive, TRUE,
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
	       		bitmap = AllocBitMap( screenw, screenh, 8, BMF_CLEAR, win -> RPort -> BitMap);

			scroll_rp.Font =  win -> RPort -> Font;
			scroll_rp.BitMap = bitmap;

			bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

			if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, &cp))
			{
				printf("error code: %d\n",err);
			}
			else
			{
				Delay(25);

				set_colour(cp,0,200);

		for (z=0;z<2400;z++)
		{
			r=r+0.01f;

				init();

				dice_x += dice_dir_x;
				dice_y = sin(r) * 50; 

				if (dice_x>400) dice_dir_x=-1;
				if (dice_x<-400) dice_dir_x=1;


				set_colour(cp,0,1);
				for (p=0;p<pos_max;p++)
				{
					rotxz( &pos[p], r);
					rotxy( &pos[p], r*2.0f);

					movexyz( &pos[p],  dice_x, dice_y, 0);

					conv_3d_to_2d( screenw / 2, screenh / 2, &pos[p], &pos[p] );

					set_pixel_fast(cp, pos[p].x, pos[p].y );
				}
			
				for (p=0;p<matrix_max;p++)
				{
					_matrix = &Matrix[ p ];

					larp( 0.5f, _matrix -> a , _matrix -> b , &ha );
					larp( 0.5f, _matrix -> b , _matrix -> c , &hb );	

					set_colour(cp,0,200);

					for (l=0.0f;l<1.0f;l+=0.01f)
					{
						larp( l, &ha, _matrix -> b , &des[0] );
						larp( l, _matrix -> b, &hb , &des[1] );
						larp( l, &des[0], &des[1], &des2[0] );

						set_pixel_fast(cp, des2[0].x, des2[0].y );
					}
				}

				plains_to_rp(cp, win -> RPort );
				clear_plains( cp );
		}

				Delay(50 * 8);

				free_chunky_plains(cp);

			}

			FreeBitMap(bitmap);

			CloseWindow(win);
		}

		CloseScreen(src);
	}
}

