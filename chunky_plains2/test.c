#include <stdlib.h>
#include <stdio.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/Picasso96API.h>
#include "chunky_plains2.h"

/*
int vga_setpalette(int index, int red, int green, int blue)
{
	red = (red<<2)		* 0x01010101;
	green = (green<<2)	* 0x01010101;
	blue = (blue<<2)		* 0x01010101;

	SetRGB32(src -> ViewPort ,index,red,green,blue);
}
*/

int main()
{
	struct chunky_plains *cp;
	struct Screen *src = NULL;
	struct Window *win;
	struct RenderInfo ri;
	int err;
	void *tmp;
	BPTR bmlock;
	int x,y;
	int n,anim;
	ULONG bpr;
	int screenw,screenh;


	screenw = 640;
	screenh = 480;

	// 1 = 2
	// 2 = 4
	// 3 = 8
	// 4 = 16

	// 2 + 6 = 8bits ( 4 colors * 64 colors ) = 256 colors
	// 4 + 4 = 8bits (16 colors * 16 colors) = 256 colors


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

		if (src)
		{
			SetRGB32( &src -> ViewPort ,0,0xFFFFFFFF,0,0);

			for (y=0;y<255;y++)
			{
				SetRGB32( &src -> ViewPort ,1,y * 0x01010101, (255-y)* 0x01010101,y*0x01010101);
			}

			printf("Bitmap %x\n",src -> BitMap);
			printf("BytesPerRow %d\n", src -> BitMap.BytesPerRow); 
			printf("Rows %d\n", src -> BitMap.Rows);
//			printf("plains %x\n", src -> BitMap.Planes[0]);


		}


		bpr = p96GetBitMapAttr(  win -> RPort -> BitMap, P96BMA_BYTESPERROW);

		Delay(20);

		if (err = alloc_cunky_plains(win -> RPort -> BitMap, 4,4,0,0,0,0,0,0,&cp))
		{
			printf("error code: %d\n",err);
		}
		else
		{

			for (y=0;y<255;y++)
			{
				for (x=0;x<255;x++)
				{
					set_pixel(cp,0, (y^x) & 8 ? (x & 3) : (y & 3) , x ,y );
				}
			}
			plains_to_video(cp, screenh, win -> RPort -> BitMap );

			Delay(100);

			for (anim = 0; anim < 5; anim++)
			for (n=0;n<32;n++)
			{

				for (y=0;y<255;y++)
				{
					for (x=0;x<255;x++)
					{
						set_pixel(cp,1, (y^(x+n)) & 16 ? 2 : 0 , x ,y );
					}
				}

				plains_to_video(cp, screenh, win -> RPort -> BitMap );

				Delay(1);
			}

			Delay(100);

			for (anim = 0; anim < 100; anim++)
			{
				scroll(cp,0,50,50,100,100,0,1);
				scroll(cp,1,50,50,100,100,1,0);

				plains_to_video(cp, screenh, win -> RPort -> BitMap );

				Delay(2);
			}

			Delay(100);

			free_chunky_plains(cp);
		}
		
		Delay(100);

		CloseWindow(win);
		p96CloseScreen(src);
	}
}

