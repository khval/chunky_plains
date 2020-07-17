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

int zb[640*480];
struct chunky_plains *cp;

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
	{ -80,-80,80 },
	{  80,-80,80 },
	{  80, 80,80 },
	{ -80, 80,80 },
	{ -80,-80,-80 },
	{  80,-80,-80 },
	{  80, 80,-80 },
	{ -80, 80,-80 }
};

struct p3d cdots[100];

struct matrix  object[] = {

	// top

	{0,1,2,0},
	{0,2,3,0},

	// bottom

	{4,5,6,1},
	{4,6,7,1},

	// north

	{1,5,6,2},
	{1,2,6,2},

	// south

	{4,0,3,5},
	{4,3,7,5},

	// west

	{1,4,5,4},
	{1,4,0,4},

	// east

	{3,2,7,3},
	{7,2,6,3}

};



void set_palette_gray(struct Screen *src)
{
	int n;

	SetRGB32( &src -> ViewPort ,0,0x0,0,0);

	for (n=0;n<255;n++)
	{
		SetRGB32( &src -> ViewPort ,n ,((unsigned int) n) * 0x01010101, ((unsigned int) n) * 0x01010101, ((unsigned int) n) * 0x01010101);
	}

}


void set_palette(struct Screen *src)
{
	long long int r,g,b;
	int y;

			SetRGB32( &src -> ViewPort ,0,0x0,0,0);

			for (y=0;y<255;y++)
			{
				r = 0;	g = 0;	b = 0;

				if (y&1)
				{
					r +=	255;
					g +=0;
					b +=0;
				}

				if (y&2)
				{
					r +=	0;
					g +=255;
					b +=0;
				}

				if (y&4)
				{
					r +=	0;
					g +=0;
					b +=255;
				}

				if (y&8)
				{
					r +=	255;
					g +=255;
					b +=0;
				}

				if (y&16)
				{
					r +=	255;
					g +=0;
					b +=255;
				}

				if (y&32)
				{
					r +=	0;
					g +=255;
					b +=255;
				}


				r/=6;
				g/=6;
				b/=6;

//				printf("%02X,%02X,%02X\n",(int) r, (int) g, (int) b);

				SetRGB32( &src -> ViewPort ,y ,((int) r) * 0x01010101, ((int) g) * 0x01010101, ((int) b) * 0x01010101);
			}

//			SetRGB32( &src -> ViewPort ,1, 255 * 0x01010101, 255 * 0x01010101, 255 * 0x01010101);

}



	float cx;
	float cy;

void conv_2d_to_3d(float x, float y, float z, float *xx,float *yy)
{
	float zoom;
	zoom =(100 / (float) (z+200)) *2 ; 
	*xx = (float) x * zoom + cx;
	*yy = (float) y * zoom + cy;
}

void _triangle(struct chunky_plains *cp, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, int cnt )
{
	float dx, dy, dz;
	float _x1,_y1,_z1,_x2,_y2,_z2,_x3,_y3,_z3;

	if (cnt == 0) return;

	conv_2d_to_3d(x1,y1,z1, &_x1,&_y1);
	conv_2d_to_3d(x2,y2,z2, &_x2,&_y2);
	conv_2d_to_3d(x3,y3,z3, &_x3,&_y3);

	line(cp, _x1,_y1, _x2, _y2, 0, cnt);
	line(cp, _x2,_y2, _x3, _y3, 0, cnt);
	line(cp, _x3,_y3, _x1, _y1, 0, cnt);


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

	_triangle(cp,_x1, _y1, _z1, _x2, _y2, _z2, _x3, _y3, _z3, cnt-1 );

}


void triangle3d(struct chunky_plains *cp, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, int plain )
{
	float _x1,_y1,_z1,_x2,_y2,_z2,_x3,_y3,_z3;

	conv_2d_to_3d(x1,y1,z1, &_x1,&_y1);
	conv_2d_to_3d(x2,y2,z2, &_x2,&_y2);
	conv_2d_to_3d(x3,y3,z3, &_x3,&_y3);
	triangle(cp, _x1, _y1,_x2, _y2, _x3, _y3, plain, 1 );
}


int minz = 1000000 ,maxz = -100000;

void 	draw_hline_zb(struct chunky_plains *cp,int x0,int x1, int y, int z0, int z1)
{
	int dx,dz;
	int dm;
	int n;
	int x;
	int z;
	int p;
	int dzz;

	dx = x1- x0;
	dz = z1- z0;

	if (dx<0) dx--;
	if (dx>0) dx++;

	dm = abs(dx);

	if (minz>z0) minz=z0;
	if (maxz<z0) maxz=z0;
	if (minz>z1) minz=z1;
	if (maxz<z1) maxz=z1;
	
	dzz=maxz-minz;

	if (dm==0)
	{
		z = z0 - minz;
		z = 255 - (255 * z / dzz);
		p = y * 640 +  x0;
		if (zb[p]<z) 
		{
			zb[p] = z;
		}
		return;
	}

	for (n=0;n<dm;n++)
	{
		x = (dx * n / dm) + x0;
		z = (dz * n / dm) + z0 ;

		z = z - minz;
		z = 255 - (255 * z / dzz);

		p = y * 640 +  x;
		
		if (zb[p]<z) 
		{
			zb[p] = z;
//			set_pixel(cp, 0,z & 255, x, y );
		}
	}
}

void _triangleZB(void *cp,int x1,int y1,int z1,int x2,int y2,int z2, int x3, int y3, int z3)
{
	int ymin,ymax;
	struct p3 p1 = {x1,y1,z1};
	struct p3 p2 = {x2,y2,z2};
	struct p3 p3 = {x3,y3,z3};
	struct p3 *temp;
	struct p3 *pt[3] ;
	int x,xa,xb,y,za,zb;
	int dxa,dya,dza;
	int dxb,dyb,dzb;
	int my1,my3;

	pt[2] = &p3;

	// sort first two
	if (y1 < y2)  { pt[0] = &p1;  pt[1] = &p2; } else { pt[1] = &p1;  pt[0] = &p2; } 

	// sort 2en and 3rd
	if ( pt[1] -> y > pt[2] -> y ) {	temp = pt[2];	pt[2] = pt[1];	pt[1] = temp; 	};

	// sort 1en and 2rd
	if ( pt[0] -> y > pt[1] -> y ) {	temp = pt[1];	pt[1] = pt[0];	pt[0] = temp; 	};

	// we are done whit pointers!!!
	x1 = pt[0] -> x;	
	y1 = pt[0] -> y;
	z1 = pt[0] -> z;

	x2 = pt[1] -> x;
	y2 = pt[1] -> y;
	z2 = pt[1] -> z;

	x3 = pt[2] -> x;
	y3 = pt[2] -> y;
	z3 = pt[2] -> z;


	xa = x1;
	xb = x1;

	dzb = z2 - z1+1;
	dyb = y2 - y1+1;
	dxb = x2 - x1;

	dxb = (dxb >= 0 ? dxb+1 : dxb-1 );

	dza = z3 - z1+1;
	dya = y3 - y1+1;
	dxa = x3 - x1;

	dxa = (dxa >= 0 ? dxa+1 : dxa-1 );

	my1 = y1>=0 ? y1 : 0;
	my3 = y3>=0 ? y3 : 0;

	my1 = my1< 640 ? my1 : 480-1;
	my3 = my3< 640 ? my3 : 480-1;

	if (y1<y3)
	for (y= my1 ; y<= my3;y++)
	{

		xa = (dya ? (y-y1) *  dxa / dya : dxa) + x1;
		za = (dya ? (y-y1) *  dza / dya : dza) + z1;

		if (y<y2)
		{
			xb = (dyb ? (y-y1) *  dxb / dyb : dxb) + x1;
			zb = (dyb ? (y-y1) *  dzb / dyb : dzb) + z1;
		}
		else 	if (y==y2)
		{
			dzb = z3 - z2;
			dzb = (dzb >= 0 ? dzb+1 : dzb-1 );

			dxb = x3 - x2;
			dxb = (dxb >= 0 ? dxb+1 : dxb-1 );

			dyb = y3 - y2+1;

			xb = x2;
			zb = z2;
		}
		else
		{
			xb = (dyb ? (y-y2) *  dxb / dyb : dxb) + x2;
			zb = (dzb ? (y-y2) *  dzb / dyb : dzb) + z2;
		}

		draw_hline_zb(cp,xa,xb,y,za,zb);
	}
}



void triangleZB(struct chunky_plains *cp, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, int plain )
{
	float _x1,_y1,_x2,_y2,_x3,_y3;
	int z;

	conv_2d_to_3d(x1,y1,z1, &_x1,&_y1);
	conv_2d_to_3d(x2,y2,z2, &_x2,&_y2);
	conv_2d_to_3d(x3,y3,z3, &_x3,&_y3);

	_triangleZB(cp, _x1, _y1,z1,_x2, _y2,z2, _x3, _y3,z3 );
}

int chk_light(int x1, int y1, float z1, int lx, int ly, float lz )
{
	register int dx,dy,dz,md;
	register int n;
	register int rx,ry;
	register float rz;

	dx = lx - x1;
	dy = ly - y1;
	dz = lz - z1;

	md = abs(dz);
	md/=4;
	for (n=0;n<md;n++)
	{
		rx = n * dx / md + x1;
		ry = n * dy / md + y1;
		rz = n * dz / md + z1;
		if ( zb[ry*640+rx] > rz )	return 1;
	}	
	return 0;
}

void render_zb(int lx,int ly,int lz)
{
	register int x,y;
	register int dx,dy,dz,z;
	register int light;

	for (y=0; y<480;y+=1)
	{
		dy = ly - y ; 

		for (x = 100; x<540; x++)
		{
			z = zb[y*640+x];

			if (z>0)
			{
				dx = lx - x ;
				dz = lz - z;

				light = 255 - (sqrt((dx*dx)+ (dy*dy) + (dz*dz))*3/4);
				light = light < 0 ? 0 : light;
/*
				if (chk_light(x,y, z , lx, ly, lz ))
				{
					light = light * 3 / 4;
				}
*/
				set_pixel( cp ,0, light, x, y  );
			}
		}
	}
}



int main()
{

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


	src = OpenScreenTags( NULL,
		SA_DisplayID, 0x50051000,
		SA_Title, (ULONG) "666",
		SA_Quiet, FALSE,
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
			printf("Bitmap %x\n",src -> BitMap);
			printf("BytesPerRow %d\n", src -> BitMap.BytesPerRow); 
			printf("Rows %d\n", src -> BitMap.Rows);
		}

		ptr =  (char *) p96GetBitMapAttr( win -> RPort -> BitMap , P96BMA_MEMORY);
		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		Delay(20);

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, &cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{

		for (effect = 0;effect<3; effect ++)
		{

			switch (effect)
			{
				case 1:	set_palette(src);
						break;
				case 0:	set_palette_gray(src);
						break;
				case 2:	set_palette(src);
						break;
			}

			for (rot=0;rot<400;rot++)
			{
				Delay(1);
				// show buffer

				memset(zb,0,sizeof(zb));
				plains_to_rp(cp, win -> RPort );

				// clear buffer

				clear_plains( cp );


				// calc rotx

				r = (float) rot / 100 * PI;

					r90 = (2*PI) / 4;

				for (n=0;n<sizeof(dots)/sizeof(struct p3d) ;n++)
				{
					cdots[n].x = dots[n].x;
					cdots[n].y = dots[n].y;
					cdots[n].z = dots[n].z;
				}

				// rot xy

				for (n=0;n<8;n++)
				{
					x1 = sin( r - r90) * dots[n].x;
					y1 = cos( r - r90) * dots[n].x;
					x2 = sin( r ) * dots[n].y;
					y2 = cos( r ) * dots[n].y;

					cdots[n].x = x1 + x2;
					cdots[n].y = y1 + y2;
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

				// draw faces


				for (n=0;n<12;n++)
				{
					switch (effect)
					{

						case 1:
							triangle3d(cp,
								cdots[ object[n].p0 ].x  , cdots[ object[n].p0 ].y , cdots[ object[n].p0 ].z ,
								cdots[ object[n].p1 ].x  , cdots[ object[n].p1 ].y , cdots[ object[n].p1 ].z ,
								cdots[ object[n].p2 ].x  , cdots[ object[n].p2 ].y , cdots[ object[n].p2 ].z ,
								object[n].plain );
							break;

						case 0:
							triangleZB(cp,
								cdots[ object[n].p0 ].x  , cdots[ object[n].p0 ].y , cdots[ object[n].p0 ].z ,
								cdots[ object[n].p1 ].x  , cdots[ object[n].p1 ].y , cdots[ object[n].p1 ].z ,
								cdots[ object[n].p2 ].x  , cdots[ object[n].p2 ].y , cdots[ object[n].p2 ].z ,
								object[n].plain );
							break;

						case 2:
							_triangle(cp,
								cdots[ object[n].p0 ].x  , cdots[ object[n].p0 ].y , cdots[ object[n].p0 ].z ,
								cdots[ object[n].p1 ].x  , cdots[ object[n].p1 ].y , cdots[ object[n].p1 ].z ,
								cdots[ object[n].p2 ].x  , cdots[ object[n].p2 ].y , cdots[ object[n].p2 ].z ,
								20 );
							break;
					}
				}

				if (effect == 0)
				{
					render_zb(screenw/2+(sin(rot/25*M_PI) * 100),screenh/2,300+(cos(rot/25*M_PI) * 100));
				}

			}


		}
			Delay(100);
			free_chunky_plains(cp);
		}
		
		Delay(50);

		if (bitmap) FreeBitMap(bitmap);
		if (win) CloseWindow(win);
		if (src) CloseScreen(src);
	}
}

