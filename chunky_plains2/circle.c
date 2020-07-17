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
	unsigned int r,g,b;
	unsigned int n;

			SetRGB32( &src -> ViewPort ,0,0x0,0,0);
			SetRGB32( &src -> ViewPort ,1, (unsigned int) 000 * 0x01010101, (unsigned int) 127 * 0x01010101, (unsigned int) 255 * 0x01010101);
			SetRGB32( &src -> ViewPort ,2, (unsigned int) 255 * 0x01010101, (unsigned int) 000 * 0x01010101, (unsigned int) 000 * 0x01010101);
			SetRGB32( &src -> ViewPort ,3, (unsigned int) 255 * 0x01010101, (unsigned int) 127 * 0x01010101, (unsigned int) 255 * 0x01010101);

			SetRGB32( &src -> ViewPort ,4, (unsigned int) 000 * 0x01010101, (unsigned int) 255 * 0x01010101, (unsigned int) 000 * 0x01010101);
			SetRGB32( &src -> ViewPort ,5, (unsigned int) 000 * 0x01010101, (unsigned int) 255 * 0x01010101, (unsigned int) 255 * 0x01010101);
			SetRGB32( &src -> ViewPort ,6, (unsigned int) 255 * 0x01010101, (unsigned int) 255 * 0x01010101, (unsigned int) 255 * 0x01010101);

}

int main()
{
	struct chunky_plains *cp;
	struct Screen *src = NULL;
	struct Window *win;
	struct RenderInfo ri;
	struct BitMap *bitmap;
	struct RastPort scroll_rp;

	char *buffer1,*buffer2, *buffer3;

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
	float zoom;


	float r90 = 1/4 * PI ;
	float r180 = 2/4 * PI;
	float r220 = 3/4 * PI;

	int a;
	int rx,ry;
	int cx;
	int cy;
	int ra;

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
			int xx,yy;

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


		buffer1 = AllocVecTags( 800 * 800, AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 0, TAG_END);
		buffer2 = AllocVecTags( 800 * 800 ,AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 0, TAG_END);
		buffer3 = AllocVecTags( 800 * 800 ,AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 0, TAG_END);

		cx = 800/2;
		cy = 800/2;

		for (y=0;y<800;y++)
		for (x=0;x<800;x++)
		{
			xx = cx -x  ;
			yy = cy -y  ;
			ra = sqrt((xx*xx)+(yy*yy));

			if ( ra & 0x10 )
			{
				buffer1[ y * 800 + x ] = 1;
				buffer2[ y * 800 + x ] = 2;
				buffer3[ y * 800 + x ] = 4;
			}
		}



		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, &cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{

		printf("%s:%d\n",__FUNCTION__,__LINE__);
			for (rot=0;rot<400;rot++)
			{
				Delay(1);

				plains_to_video(cp, screenh, win -> RPort -> BitMap );
				clear_plains( cp );


				cx = 800/2 + ( sin( 2 * M_PI * rot / 200) * 25 );
				cy = 800/2 + ( cos( 2 * M_PI * rot / 200) * 25 );

				yy = cy - (screenh/2);
				xx = cx - (screenw/2);

				for (y=0;y<screenh;y++)
				{
					memcpy( cp -> data[0] + (y * cp -> bytes_per_row)   , buffer1 + ((y+yy) * 800) + xx  , screenw );
				}

				cx = 800/2 + ( sin( 3 * M_PI * rot / 200) * 50 );
				cy = 800/2 + ( cos( 3 * M_PI * rot / 200) * 50 );

				yy = cy - (screenh/2);
				xx = cx - (screenw/2);

				for (y=0;y<screenh;y++)
				{
					memcpy( cp -> data[1] + (y * cp -> bytes_per_row)   , buffer2 + ((y+yy) * 800) + xx  , screenw );
				}


				cx = 800/2 + ( sin(  -4 * M_PI * rot / 200) * 50 );
				cy = 800/2 + ( cos( - 4 * M_PI * rot / 200) * 50 );

				yy = cy - (screenh/2);
				xx = cx - (screenw/2);

				for (y=0;y<screenh;y++)
				{
					memcpy( cp -> data[2] + (y * cp -> bytes_per_row)   , buffer3 + ((y+yy) * 800) + xx  , screenw );
				}

			}	

		printf("%s:%d\n",__FUNCTION__,__LINE__);

			Delay(25);

		printf("%s:%d\n",__FUNCTION__,__LINE__);

			free_chunky_plains(cp);
		}

		printf("%s:%d\n",__FUNCTION__,__LINE__);

		FreeVec(buffer1);
		FreeVec(buffer2);
		FreeVec(buffer3);

		printf("%s:%d\n",__FUNCTION__,__LINE__);
		
		Delay(100);

		FreeBitMap(bitmap);

		CloseWindow(win);
		p96CloseScreen(src);
	}
}

