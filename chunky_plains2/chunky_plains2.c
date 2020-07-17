
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


void clear_plains(struct chunky_plains *cp)
{
	int n;
	int size = cp -> bytes_per_row * cp -> height;

	for (n=0;n<8;n++)
	{
		memset( cp->data[n], 0, size );
	}
}

void plains_to_rp(struct chunky_plains *cp, struct RastPort *rp)
{
	int wpr =0;
	int x,y;
	unsigned int offset;

	int32 *w0,*w1,*w2,*w3,*w4,*w5,*w6,*w7;
	int32 *wdest;
	APTR lock;

	wpr = cp -> bytes_per_row / 4;

	offset = 0;
	wdest = (int32 *) cp -> f_buffer;

	for (y=0;y<cp -> height ;y++)
	{

		w0 = (int32 *) (cp->data[0] + offset);
		w1 = (int32 *) (cp->data[1] + offset);
		w2 = (int32 *) (cp->data[2] + offset);
		w3 = (int32 *) (cp->data[3] + offset);
		w4 = (int32 *) (cp->data[4] + offset);
		w5 = (int32 *) (cp->data[5] + offset);
		w6 = (int32 *) (cp->data[6] + offset);
		w7 = (int32 *) (cp->data[7] + offset);

		for (x=0;x<wpr;x++)
		{
			*wdest++ = (*w0++ | *w1++ | *w2++ | *w3++ | *w4++ | *w5++ | *w6++ | *w7++);
		}

		offset += cp -> bytes_per_row;
		wdest = (int32 *) (cp -> f_buffer + offset);
	}

	WriteChunkyPixels( rp ,0 ,0, cp -> width , cp -> height, cp -> f_buffer, cp -> bytes_per_row);
}


void plains_to_video(struct chunky_plains *cp, int h, struct BitMap *dest)
{
	int wpr =0;
	int x,y;
	int offset;

	int32 *w0,*w1,*w2,*w3,*w4,*w5,*w6,*w7;
	int32 *wdest;
	APTR lock;
	struct _RenderInfo ri;

//	if ( lock = p96LockBitMap( dest, (UBYTE *) &ri, (ULONG) sizeof(ri) ) )

	if ( lock = LockBitMapTags( dest, LBM_BaseAddress, &ri.Memory, LBM_BytesPerRow, &ri.BytesPerRow,  TAG_END) )
	{
		wpr = ri.BytesPerRow / 4;

		for (y=0;y<h;y++)
		{
			offset = y * ri.BytesPerRow;

			w0 = (int32 *) (cp->data[0] + offset);
			w1 = (int32 *) (cp->data[1] + offset);
			w2 = (int32 *) (cp->data[2] + offset);
			w3 = (int32 *) (cp->data[3] + offset);
			w4 = (int32 *) (cp->data[4] + offset);
			w5 = (int32 *) (cp->data[5] + offset);
			w6 = (int32 *) (cp->data[6] + offset);
			w7 = (int32 *) (cp->data[7] + offset);
			wdest = (int32 *) ( ((char *) ri.Memory) + offset);

			for (x=0;x<wpr;x++)
			{
				*wdest++ = (*w0++ | *w1++ | *w2++ | *w3++ | *w4++ | *w5++ | *w6++ | *w7++);
			}
		}

//		p96UnlockBitMap( dest, lock );

		UnlockBitMap( lock );
	}
}



void free_chunky_plains(struct chunky_plains *p)
{
	int n;

	if (p)
	{
		for (n=0;n<8;n++)
		{
			if (p -> data[n])
			{
				FreeVec(p -> data[n]);
				p -> data[n]=NULL;
			}
		}

		if (p -> f_buffer) FreeVec(p -> f_buffer);
		if (p -> n_buffer) FreeVec(p -> n_buffer);

		p -> f_buffer = NULL;
		p -> n_buffer = NULL;

		FreeVec(p);
	}
}

int alloc_cunky_plains(struct BitMap *bm, char n0,char n1,char n2,char n3,char n4,char n5, char n6,char n7, struct chunky_plains **new_cp)
{
	struct chunky_plains *p;
	int n;
	int cnt;
	int bpr;
	int error_code = 0;
	int screenh;

	p = (struct chunky_plains *) AllocVecTags( sizeof(struct chunky_plains) , AVT_Type, MEMF_PRIVATE, TAG_END );

	if (p)
	{
		p -> height = GetBitMapAttr(  bm, BMA_HEIGHT);	
		p -> width = GetBitMapAttr(  bm, BMA_ACTUALWIDTH );

		bpr = GetBitMapAttr(  bm, BMA_WIDTH ) * GetBitMapAttr(  bm, BMA_BYTESPERPIXEL );

		printf("screenh %d, bpr %d\n",screenh,bpr);

		p -> f_buffer = (char *)  AllocVecTags( bpr * p -> height , AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 0, TAG_END);
		p -> n_buffer = (char *)  AllocVecTags( bpr * p -> height , AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 0, TAG_END);
		p -> bytes_per_row = bpr;

		p -> bits[0]=n0;
		p -> bits[1]=n1;
		p -> bits[2]=n2;
		p -> bits[3]=n3;
		p -> bits[4]=n4;
		p -> bits[5]=n5;
		p -> bits[6]=n6;
		p -> bits[7]=n7;

		p -> index[0] = 0;

		cnt = 0;
		for (n=1;n<8;n++)
		{
			p -> index[n] = p -> index[n-1] + p -> bits[n-1];

			cnt += p -> bits[n];
		}

		for (n=0;n<8;n++)
		{
			printf("bits %02x  index %d\n",p -> bits[n] , p -> index[n]);
		}

		if (cnt > 8) error_code = cp_error_too_many_bits;


		for (n=0;n<8;n++)
		{	
			p -> data[n] = AllocVecTags( bpr * p -> height, AVT_Type, MEMF_PRIVATE,  AVT_ClearWithValue, 0, TAG_END);

			if (!p -> data[n])	error_code = cp_error_no_mem;
		}

		if (!p -> f_buffer) error_code = cp_error_no_mem;
		if (!p -> n_buffer) error_code = cp_error_no_mem;

	} else {
		error_code = cp_error_no_mem;
	}

	if (error_code)
	{
		free_chunky_plains(p);
		p = NULL;
	}

	*new_cp = p;

	return error_code;
}


void set_colour( struct chunky_plains *cp, char number, char color )
{
	cp -> current_color = color << cp -> index[number];
	cp -> current_plane =  number;
}

void set_pixel_fast_nomask(struct chunky_plains *cp, int x,int y)
{
	cp -> data[cp -> current_plane] [ x + (cp -> bytes_per_row * y)] = cp -> current_color;
}

void set_pixel_fast(struct chunky_plains *cp, int x,int y)
{
	if ((x>=0)&&(x<640)&&(y>=0)&&(y<480))
	{
		cp -> data[cp -> current_plane] [ x + (cp -> bytes_per_row * y)] = cp -> current_color;
	}
}

void set_pixel(struct chunky_plains *cp, char number, char color, int x,int y)
{
	cp -> data[number] [ x + (cp -> bytes_per_row * y)] = color << cp -> index[number]; 
}


void draw_hline_fast_x4(struct chunky_plains *cp, int x,int y,int w)
{
	int n0 = x % 4;
	int w0;
	int _x;
	register int color = cp -> current_color; 
	register int colorw = cp -> current_color * 0x01010101;
	register unsigned char *pbyte;
	register unsigned int *plong;
	register n;

	pbyte = &cp -> data[cp -> current_plane < 8 ? cp -> current_plane : 0 ][ x + (cp -> bytes_per_row * y)];

	w-=n0;

	for (n=0;n<n0;n++)
	{
		*pbyte++=color;
	}

	w0 = w / 4;
	w -= (w0*4);

	plong = (unsigned int *) pbyte;
	for (n=0;n<w0;n++)
	{
		*plong++=colorw;
	}

	pbyte = (unsigned char *) plong;
	for (n=0;n<w;n++)
	{
		*pbyte++=color;
	}
}


void __draw_hline_fast(struct chunky_plains *cp, int x,int y,int w)
{
	char *pixel = &cp -> data[cp -> current_plane < 8 ? cp -> current_plane : 0 ][ x + (cp -> bytes_per_row * y)];
	register int color;

	color = cp -> current_color; 

	for (;w>0;w--) { *pixel = color; pixel++; }
}

void draw_hline_fast(struct chunky_plains *cp, int x,int y,int w)
{
	if ((x>-1)&&(x+w<cp->width))
	{
		__draw_hline_fast(cp, x,y,w);
	}
}


void draw_box(struct chunky_plains *cp, int x,int y,int w, int h)
{
	int _y;

	if (x+w>cp->width) w = cp->width - x ;
	if (y+h>cp->height) h = cp->height - y ;

	if ((x>-1)&&(y>-1)&&(h>0)&&(w>0))
	{
		for (_y=y;_y<y+h;_y++)
		{
			__draw_hline_fast(cp, x,_y, w);
		}
	}
}


int get_pixel(struct chunky_plains *cp, char number, int x,int y)
{
	return cp -> data[cp -> current_plane] [ x + (cp -> bytes_per_row * y)] >> cp -> index[number] ; 
}

void scroll(struct chunky_plains *cp, int number, int x1,int y1,int x2,int y2, int offset_x, int offset_y )
{
	int bytes_per_row = cp -> bytes_per_row;
	register int x = 0;
	register int y;
	register int line_offset;
	register int offset;
	register int doffset;
	register char *data = cp -> data[cp -> current_plane];

	// we filter out the other plains into the buffer, so we can put it back in.

	for (y =y1;y<=y2;y++)
	{
		line_offset = y *  bytes_per_row;
		for (x=x1;x<=x2;x++)
		{
			offset = line_offset +x;
			cp ->n_buffer[offset] = cp -> data[number] [ offset ] ;
		}
	}

	for (y =y1;y<=y2;y++)
	{
		line_offset = y * bytes_per_row;
		for (x=x1;x<=x2;x++)
		{
			offset = line_offset +x;
			doffset = (y + offset_y) * bytes_per_row + (x + offset_x);
			cp -> data[number] [ offset ] = cp ->n_buffer[ doffset ] ;
		}
	}
}

void triangle(void *cp,int x1,int y1,int x2,int y2, int x3, int y3, int p, int c)
{
	int ymin,ymax;
	struct p p1 = {x1,y1};
	struct p p2 = {x2,y2};
	struct p p3 = {x3,y3};
	struct p *temp;
	struct p *pt[3] ;
	int x,xa,xb,y;
	int dxa,dya;
	int dxb,dyb;
	int my1,my3;

	pt[2] = &p3;

	set_colour(cp,p,c);

	// sort first two
	if (y1 < y2)  { pt[0] = &p1;  pt[1] = &p2; } else { pt[1] = &p1;  pt[0] = &p2; } 

	// sort 2en and 3rd
	if ( pt[1] -> y > pt[2] -> y ) {	temp = pt[2];	pt[2] = pt[1];	pt[1] = temp; 	};

	// sort 1en and 2rd
	if ( pt[0] -> y > pt[1] -> y ) {	temp = pt[1];	pt[1] = pt[0];	pt[0] = temp; 	};

	// we are done whit pointers!!!
	x1 = pt[0] -> x;	y1 = pt[0] -> y;
	x2 = pt[1] -> x;	y2 = pt[1] -> y;
	x3 = pt[2] -> x;	y3 = pt[2] -> y;


	xa = x1;
	xb = x1;

	dyb = y2 - y1+1;
	dxb = x2 - x1;
	dxb = (dxb >= 0 ? dxb+1 : dxb-1 );

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

		if (y<y2)
		{
			xb = (dyb ? (y-y1) *  dxb / dyb : dxb) + x1;
		}
		else 	if (y==y2)
		{
			dxb = x3 - x2;
			dxb = (dxb >= 0 ? dxb+1 : dxb-1 );
			dyb = y3 - y2+1;
			xb = x2;
		}
		else
		{
			xb = (dyb ? (y-y2) *  dxb / dyb : dxb) + x2;
		}


		draw_hline_fast(cp,xa,y,xb-xa);
		draw_hline_fast(cp,xb,y,xa-xb);
	}
}

void line(void *cp,int x1,int y1,int x2,int y2, int p, int c)
{
	int dx,dy;
	int x,y;
	int ax = 1,ay =1;

	dx = x2-x1+1;
	dy = y2-y1+1;

	if (dx<0) { dx=-dx; ax=-1; }
 	if (dy<0) { dy=-dy; ay=-1; }

	set_colour(cp,p,c);

	if (dx>dy)
	{
		dx = x2-x1; 	dy = y2-y1;
		y = y1;
		for (x=x1;(ax>0) ?  (x<=x2) : (x>=x2) ;x+=ax)
		{
			if (dx) { y = dy * (x-x1) / dx + y1 ; } else { y = y2; };
			set_pixel_fast(cp, x , y );
		}
	}
	else
	{
		dx = x2-x1;  dy = y2-y1;
		x = x1;
		for (y=y1;(ay>0) ? (y<=y2) : (y>=y2) ;y+=ay)
		{
			if (dy) { x = dx * (y-y1) / dy + x1; } else { x=x2; };
			set_pixel_fast(cp, x , y );
		}
	}
}
