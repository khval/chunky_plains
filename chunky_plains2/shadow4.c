#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/Picasso96API.h>
#include <proto/DiskFont.h>

#include "chunky_plains2.h"

float zb[640*480];

void set_palette(struct Screen *src)
{
	long long int r,g,b;
	unsigned int n;
	SetRGB32( &src -> ViewPort ,0,0x0,0,0);


	for (n=0;n<256;n++)
		SetRGB32( &src -> ViewPort ,n ,((int) n) * 0x01010101, ((int) 0) * 0x01010101, ((int) 0) * 0x01010101);

}

	float cx;
	float cy;

struct TextFont *load_font( char const *filename, int size, int flags )
{
	struct TextAttr ta;
	struct TextFont *font;

	if (!filename) return NULL;

	ta.ta_Name = filename;  
	ta.ta_YSize = size; 
	ta.ta_Style = 0; 
	ta.ta_Flags =  FPF_DISKFONT; 

	font = OpenDiskFont(&ta);

	return font;
}

	struct chunky_plains *cp;


void setbox(int x0,int y0, int x1, int y1, int z)
{
	int x,y;

	for (x=x0;x<=x1;x++)
	{
		for (y=y0;y<=y1;y++)
		{
			zb[y*640+x] = z;
		}
	}
}

void setcrl(int x,int y, int r)
{
	int cx,cy;
	int px,py;
	int z;
	int x0,y0,x1,y1;

	x0 = x-r;
	y0 = y-r;
	x1 = x+r;
	y1 = y+r;


	cx = (x1 - x0) / 2 ;
	cy = (y1 - y0) / 2 ;

	for (x=x0;x<=x1;x++)
	{
		for (y=y0;y<=y1;y++)
		{
			px = (x-x0) - cx;
			py = (y-y0) - cy;

			r = sqrt((px*px)+(py*+py));

			if (r<=cx)
			{
				z = sqrt((cx*cx)-(r*r));
				if (zb[y*640+x] < z) zb[y*640+x] = z;
			}
		}
	}
}

int _chk_ (register int x, register int y, register float z)
{
	if ( zb[y*640+x] > z )
		return 1;
	else
		return 0;
}


int chk_light(int x1, int y1, float z1, int lx, int ly, float lz )
{
	register int dx,dy,dz,md;
	register int n;
	int rx,ry;
	register float rz;

	dx = lx - x1;
	dy = ly - y1;
	dz = lz - z1;

//	md = abs(dx);
//	if (abs(dy)>md) md = abs(dy);
//	if (abs(dz)>md)
 md = abs(dz);

	md/=4;

	for (n=0;n<md;n++)
	{
		rx = n * dx / md + x1;
		ry = n * dy / md + y1;
		rz = n * dz / md + z1;

		if ( zb[ry*640+rx] > rz )	return 1;

//		if (hit(rx,ry,rz))	return 1;


	}	
	return 0;
}


int main()
{

	struct Screen *src = NULL;
	struct Window *win;
	struct RenderInfo ri;
	struct BitMap *bitmap;
	struct RastPort scroll_rp;

	struct TextFont *font;

	int err;
	int screenw,screenh;

	int txt_pos = 0;

	void *tmp;
	PLANEPTR ptr;
	BPTR bmlock;
	int x,y;
	int n,anim;
	ULONG bpr;
	float g;
	float rot,r;
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

       		bitmap = AllocBitMap( screenw, screenh, 8, BMF_CLEAR, win -> RPort -> BitMap);

		font = load_font("Century Schoolbook L Bold.font", 70, 0);

		scroll_rp.Font =  win -> RPort -> Font;
		scroll_rp.BitMap = bitmap;
		SetFont(&scroll_rp, font);


		printf("Bitmap %x\n",src -> BitMap);
		printf("BytesPerRow %d\n", src -> BitMap.BytesPerRow); 
		printf("Rows %d\n", src -> BitMap.Rows);


		ptr =  (char *) p96GetBitMapAttr( win -> RPort -> BitMap , P96BMA_MEMORY);
		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		setbox(0,0,640-1,480-1, -100); 
		setbox(150,150,200,200, 65); 
		setbox(250,150,300,200, 120); 
		setbox(350,150,400,200, 60); 

		setbox(150,250,200,300, 60); 
		setbox(250,250,300,300, 120); 
		setbox(350,250,375,275, 70); 

		setcrl(350,250, 100);

		setcrl(250,300, 100);


		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, 320,200,&cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{
			int lx,ly,lz;
			register int dx,dy,dz;
			int ax,ay,xx,yy;
			int light;
			float z;

			clear_plains( cp, bpr, screenh );

		for (g=0;g<640*2;g+=4)
		{

			lx =  screenw/2 + (sin(2*M_PI*g/360) * 200);
			ly =  screenh/2 + (cos(2*M_PI*g/360) * 200);
			lz = 150;


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

					if (chk_light(x,y, z , lx, ly, lz ))
					{
						light = light * 3 / 4;
					}

					set_pixel( cp ,0, light, x, y  );
					}

				}
			}


			plains_to_video(cp, screenh, win -> RPort -> BitMap );
			clear_plains( cp, bpr, screenh );
			Delay(1);
		}

			Delay(100);

			free_chunky_plains(cp);
		}
		

		CloseFont(font);
		FreeBitMap(bitmap);

		CloseWindow(win);
		p96CloseScreen(src);
	}
}

