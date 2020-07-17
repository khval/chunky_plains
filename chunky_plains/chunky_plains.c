#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/Picasso96API.h>

#include "chunky_plains.h"


void free_chunky_plains(struct chunky_plains *p)
{
	if (p)
	{
		if (p -> f_buffer) FreeVec(p -> f_buffer);
		if (p -> n_buffer) FreeVec(p -> n_buffer);
		FreeVec(p);
	}
}

int alloc_cunky_plains(struct BitMap *bm, char n0,char n1,char n2,char n3,char n4,char n5, char n6,char n7, int w,int h,struct chunky_plains **new_cp)
{
	struct chunky_plains *p;
	int n;
	int cnt;
	int bpr;
	int error_code = 0;

	p = (struct chunky_plains *) AllocVec( sizeof(struct chunky_plains) , MEMF_ANY );

	if (p)
	{
		bpr = p96GetBitMapAttr(  bm, P96BMA_BYTESPERROW);	

		p -> data =  (char *) p96GetBitMapAttr( bm , P96BMA_MEMORY);
		p -> f_buffer = (char *)  AllocVec( bpr * h , MEMF_ANY | MEMF_CLEAR);
		p -> n_buffer = (char *)  AllocVec( bpr * h , MEMF_ANY | MEMF_CLEAR);
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
			p -> mask[n] = ((1 << p -> bits[n]) -1) << p -> index[n]; 

			printf("bits %02x mask %02x index %d\n",p -> bits[n] ,p->mask[n], p -> index[n]);
		}

		if (cnt > 8) error_code = cp_error_too_many_bits;
		
		if (!p -> data)	error_code = cp_error_no_mem;
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
	cp -> current_mask =  cp -> mask[number];
}

void set_pixel_fast_nomask(struct chunky_plains *cp, int x,int y)
{
	char *pixel = &cp -> data[ x + (cp -> bytes_per_row * y)];
	*pixel =  *pixel | cp -> current_color; 
}

void set_pixel_fast(struct chunky_plains *cp, int x,int y)
{
	char *pixel = &cp -> data[ x + (cp -> bytes_per_row * y)];
	*pixel =  (~cp -> current_mask & *pixel) | cp -> current_color; 
}

void draw_hline_fast(struct chunky_plains *cp, int x,int y,int w)
{
	char *pixel = &cp -> data[ x + (cp -> bytes_per_row * y)];
	register int color;
	register int mask;

	color = cp -> current_color; 	mask = cp -> current_mask;
	
	for (;w>0;w--) { *pixel =  (~ mask & *pixel) | color; pixel++; }
}

void set_pixel(struct chunky_plains *cp, char number, char color, int x,int y)
{
	char *pixel = &cp -> data[ x + (cp -> bytes_per_row * y)];
	*pixel = (~cp -> mask[number]) &  *pixel | (color << cp -> index[number] ); 
}

int get_pixel(struct chunky_plains *cp, char number, int x,int y)
{
	return (cp -> mask[number] &  cp -> data[ x + (cp -> bytes_per_row * y)]) >> cp -> index[number] ; 
}

void scroll(struct chunky_plains *cp, int number, int x1,int y1,int x2,int y2, int offset_x, int offset_y )
{
	int fmask = ~ cp -> mask[number];
	int mask = cp -> mask[number];
	int bytes_per_row = cp -> bytes_per_row;
	register int x = 0;
	register int y;
	register int line_offset;
	register int offset;
	register int doffset;

	// we filter out the other plains into the buffer, so we can put it back in.

	for (y =y1;y<=y2;y++)
	{
		line_offset = y *  bytes_per_row;
		for (x=x1;x<=x2;x++)
		{
			offset = line_offset +x;
			cp -> f_buffer[offset] = cp -> data[ offset ] & fmask;
			cp ->n_buffer[offset] = cp -> data[ offset ] & mask;
		}
	}

	for (y =y1;y<=y2;y++)
	{
		line_offset = y * bytes_per_row;
		for (x=x1;x<=x2;x++)
		{
			offset = line_offset +x;

			doffset = (y + offset_y) * bytes_per_row + (x + offset_x);

			cp -> data[ offset ] = cp -> f_buffer[offset] | cp ->n_buffer[ doffset ] ;
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

	if (y1<y3)

	for (y=y1;y<=y3;y++)
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


//		for (x=xa;x<=xb;x++)	set_pixel_fast(cp, x , y );
//		for (x=xb;x<=xa;x++)	set_pixel_fast(cp, x , y );
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
