#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/Picasso96API.h>

#include "../chunky_plains2.h"
#include "curves_ed.h"

double r90 =  M_PI / 2.0f ;
double r180 = M_PI ;

int screenw,screenh;
int quit = 0;
int selected_draw_mode = 0;


enum {
	d_edit = 0,
	d_rotate,
	d_move,
	d_zoom,
	d_xflip,
	d_load,
	d_save,
	d_quit
};


struct xyz Edit[1000];
struct xyz _Temp[1000];

struct matrix Matrix[1000];
struct matrix Matrix1[100];
struct matrix Matrix2[100];
struct matrix Matrix3[100];
struct matrix Matrix4[100];
struct matrix Matrix5[100];
struct matrix Matrix6[100];
struct matrix Matrix7[100];
struct matrix Matrix8[100];

struct shape edit_shape = {0,0,Edit,Matrix,0};

// 15
struct xyz Symbol_arrow[] =
{
	{-115.50,-83.00},
	{-94.50,-52.00},
	{8.50,83.00},
	{21.50,81.00},
	{37.50,39.00},
	{45.50,40.00},
	{90.50,77.00},
	{101.50,78.00},
	{115.50,46.00},
	{107.50,34.00},
	{58.50,3.00},
	{54.50,-6.00},
	{75.50,-50.00},
	{66.50,-63.00},
	{-83.50,-78.00}
};

// 32
struct xyz Symbol_rot[] =
{
	{23.00,-91.00},
	{34.00,-88.00},
	{66.00,-81.00},
	{97.00,-50.00},
	{106.00,11.00},
	{93.00,74.00},
	{57.00,117.00},
	{5.00,128.00},
	{-50.00,116.00},
	{-95.00,69.00},
	{-106.00,-14.00},
	{-81.00,-74.00},
	{-41.00,-95.00},
	{-36.00,-70.00},
	{-33.00,-51.00},
	{-64.00,-35.00},
	{-75.00,7.00},
	{-70.00,48.00},
	{-41.00,85.00},
	{6.00,97.00},
	{50.00,85.00},
	{78.00,48.00},
	{81.00,-9.00},
	{62.00,-46.00},
	{29.00,-57.00},
	{22.00,-54.00},
	{22.00,-26.00},
	{13.00,-15.00},
	{-20.00,-73.00},
	{-21.00,-77.00},
	{14.00,-128.00},
	{24.00,-121.00}
};

// 48
struct xyz Symbol_mov[] =
{
	{-82.00,-63.50},
	{-54.00,-65.50},
	{-26.00,-67.50},
	{-23.00,-28.50},
	{-22.00,-3.50},
	{-59.00,-2.50},
	{-98.00,-1.50},
	{-96.00,-32.50},
	{-102.00,-60.50},
	{-140.00,-22.50},
	{-179.00,14.50},
	{-138.00,52.50},
	{-90.00,86.50},
	{-89.00,56.50},
	{-92.00,26.50},
	{-55.00,25.50},
	{-19.00,23.50},
	{-18.00,47.50},
	{-16.00,80.50},
	{-46.00,84.50},
	{-75.00,88.50},
	{-40.00,139.50},
	{4.00,188.50},
	{34.00,138.50},
	{68.00,82.50},
	{43.00,82.50},
	{16.00,81.50},
	{15.00,52.50},
	{13.00,20.50},
	{46.00,20.50},
	{79.00,22.50},
	{81.00,47.50},
	{82.00,71.50},
	{130.00,39.50},
	{179.00,5.50},
	{125.00,-25.50},
	{74.00,-58.50},
	{75.00,-24.50},
	{78.00,-1.50},
	{44.00,-3.50},
	{10.00,-1.50},
	{8.00,-34.50},
	{8.00,-69.50},
	{36.00,-70.50},
	{62.00,-71.50},
	{27.00,-128.50},
	{-12.00,-188.50},
	{-49.00,-125.50}
};

struct xyz Symbol_zoom[] =
{
	{-15.00,-23.50},
	{-15.00,-53.50},
	{-14.00,-78.50},
	{22.00,-78.50},
	{22.00,-50.50},
	{22.00,-21.50},
	{47.00,-18.50},
	{70.00,-19.50},
	{72.00,17.50},
	{45.00,17.50},
	{23.00,17.50},
	{24.00,49.50},
	{23.00,78.50},
	{-15.00,75.50},
	{-15.00,49.50},
	{-14.00,18.50},
	{-45.00,19.50},
	{-72.00,17.50},
	{-68.00,-23.50},
	{-42.00,-22.50},
};

// Count: 25
struct xyz Symbol_swap_horizontal[] = {
	{-37.00,-103.50,0.00},
	{-52.00,-81.50,0.00},
	{-96.00,-21.50,0.00},
	{-115.00,5.50,0.00},
	{-95.00,30.50,0.00},
	{-55.00,90.50,0.00},
	{-34.00,105.50,0.00},
	{-33.00,80.50,0.00},
	{-34.00,33.50,0.00},
	{-33.00,9.50,0.00},
	{-3.00,10.50,0.00},
	{32.00,9.50,0.00},
	{31.00,61.50,0.00},
	{31.00,109.50,0.00},
	{56.00,85.50,0.00},
	{95.00,34.50,0.00},
	{115.00,5.50,0.00},
	{97.00,-20.50,0.00},
	{57.00,-77.50,0.00},
	{33.00,-109.50,0.00},
	{34.00,-59.50,0.00},
	{32.00,-14.50,0.00},
	{-3.00,-16.50,0.00},
	{-36.00,-14.50,0.00},
	{-38.00,-57.50,0.00}
};

// 17
struct xyz Symbol_load[] = {
	{-13.50,56.50,0.00},
	{29.50,33.50,0.00},
	{39.50,16.50,0.00},
	{47.50,51.50,0.00},
	{42.50,66.50,0.00},
	{23.50,67.50,0.00},
	{0.50,68.50,0.00},
	{-23.50,66.50,0.00},
	{-47.50,62.50,0.00},
	{-36.50,23.50,0.00},
	{-31.50,-29.50,0.00},
	{-46.50,-66.50,0.00},
	{-30.50,-68.50,0.00},
	{-22.50,-67.50,0.00},
	{-17.50,-52.50,0.00},
	{-15.50,-11.50,0.00},
	{-24.50,28.50,0.00}
};

// Count: 16
struct xyz Symbol_save[] = {
	{23.50,-92.50,0.00},
	{64.50,-55.50,0.00},
	{47.50,-35.50,0.00},
	{21.50,-61.50,0.00},
	{-26.50,-62.50,0.00},
	{-29.50,-22.50,0.00},
	{56.50,-6.50,0.00},
	{55.50,82.50,0.00},
	{-26.50,92.50,0.00},
	{-64.50,53.50,0.00},
	{-53.50,24.50,0.00},
	{-19.50,62.50,0.00},
	{33.50,58.50,0.00},
	{29.50,10.50,0.00},
	{-60.50,-4.50,0.00},
	{-52.50,-81.50,0.00}
};


// Count: 31
struct xyz Symbol_quit[] = {
	{-58.00,-63.50,0.00},
	{-57.00,-52.50,0.00},
	{-25.00,-13.50,0.00},
	{-20.00,-3.50,0.00},
	{-28.00,10.50,0.00},
	{-62.00,41.50,0.00},
	{-73.00,54.50,0.00},
	{-64.00,66.50,0.00},
	{-52.00,81.50,0.00},
	{-44.00,73.50,0.00},
	{-15.00,40.50,0.00},
	{-6.00,30.50,0.00},
	{5.00,35.50,0.00},
	{29.00,71.50,0.00},
	{38.00,82.50,0.00},
	{52.00,65.50,0.00},
	{66.00,53.50,0.00},
	{55.00,41.50,0.00},
	{24.00,4.50,0.00},
	{28.00,-7.50,0.00},
	{66.00,-41.50,0.00},
	{73.00,-49.50,0.00},
	{67.00,-61.50,0.00},
	{55.00,-78.50,0.00},
	{46.00,-70.50,0.00},
	{12.00,-36.50,0.00},
	{4.00,-29.50,0.00},
	{-5.00,-36.50,0.00},
	{-32.00,-76.50,0.00},
	{-39.00,-82.50,0.00},
	{-49.00,-73.50,0.00}
};


struct shape symbol_arrow_shape = {0,0,Symbol_arrow,Matrix1,15};
struct shape symbol_rot_shape = {0,0,Symbol_rot,Matrix2,32};
struct shape symbol_mov_shape = {0,0,Symbol_mov,Matrix3,48};
struct shape symbol_zoom_shape = {0,0,Symbol_zoom,Matrix4,20};
struct shape symbol_swap_horizontal_shape = {0,0,Symbol_swap_horizontal,Matrix5,25};
struct shape symbol_load_shape = {0,0,Symbol_load,Matrix6,17};
struct shape symbol_save_shape = {0,0,Symbol_save,Matrix7,16};
struct shape symbol_quit_shape = {0,0,Symbol_quit,Matrix8,31};

struct shape *icons[]={ 
		&symbol_arrow_shape, 
		&symbol_rot_shape, 
		&symbol_mov_shape , 
		&symbol_zoom_shape,
		&symbol_swap_horizontal_shape,
		&symbol_load_shape,
		&symbol_save_shape,
		&symbol_quit_shape,
		NULL };

int matrix_max =0;
int pixels = 10000;


void pre_rotate(struct shape *obj, int x, int y)
{
	int p;

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x -= x;
		obj->data[p].y -= y;
	}
}

void cord_to_shape(struct shape *obj)
{
	int p;

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x += obj -> x;
		obj->data[p].y += obj -> y;
	}

	obj -> x = 0;
	obj -> y = 0;
}

void zoom_shape(struct shape *obj, double zoom )
{
	int p;
	double sx;
	double sy;

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x -= (screenw/2.0f);
		obj->data[p].y -= (screenh/2.0f);
	}

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x *= zoom;
		obj->data[p].y *= zoom;
	}

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x += (screenw/2.0f);
		obj->data[p].y += (screenh/2.0f);
	}
}

void scale_shape(struct shape *obj, double r )
{
	int p;
	double minx,maxx,miny,maxy;
	double ww,hh;
	double _max;
	
	minx=0;	maxx=0;
	miny=0;	maxy=0;

	for (p=0;p<obj -> count;p++)
	{
		if (obj->data[p].x<minx) minx = obj->data[p].x;
		if (obj->data[p].y<miny) miny = obj->data[p].y;

		if (obj->data[p].x>maxx) maxx = obj->data[p].x;
		if (obj->data[p].y>maxy) maxy = obj->data[p].y;
	}

	ww = maxx - minx;		hh = maxy - miny;

	_max = maxx > maxy ? maxx : maxy;

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x = r * obj->data[p].x / _max;
		obj->data[p].y = r * obj->data[p].y / _max;
	}
}

void flip_shape(struct shape *obj, double sx, double sy )
{
	int p;

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x -= (screenw/2.0f);
		obj->data[p].y -= (screenh/2.0f);
	}

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x *= sx;
		obj->data[p].y *= sy;
	}

	for (p=0;p<obj -> count;p++)
	{
		obj->data[p].x += (screenw/2.0f);
		obj->data[p].y += (screenh/2.0f);
	}
}

// xflip_to_shape(&edit_shape);



void dump_xy(struct shape *obj)
{
	int p;
	double minx,maxx,miny,maxy;
	double ww,hh;
	
	minx = (double) screenw; 	maxx = 0;
	miny = (double) screenh;	maxy = 0;

	for (p=0;p<obj -> count;p++)
	{
		if (obj->data[p].x<minx) minx = obj->data[p].x;
		if (obj->data[p].y<miny) miny = obj->data[p].y;

		if (obj->data[p].x>maxx) maxx = obj->data[p].x;
		if (obj->data[p].y>maxy) maxy = obj->data[p].y;
	}

	ww = maxx - minx;		hh = maxy - miny;

	printf("// Count: %d\n",obj -> count);

	printf("struct xyz myshape[] = {\n");
	for (p=0;p<obj -> count;p++)
	{

		if (p == obj -> count-1)
		{
			printf("	{%0.2lf,%0.2lf,%0.2lf}\n",(obj->data[p].x-minx)-(ww/2),(obj->data[p].y -miny)-(hh/2), obj->data[p].z );
		}
		else
		{
			printf("	{%0.2lf,%0.2lf,%0.2lf},\n",(obj->data[p].x-minx)-(ww/2),(obj->data[p].y -miny)-(hh/2), obj->data[p].z );
		}
	}
	printf("};\n");

}

void update_matrix(struct shape *obj)
{
	int p;
	int count = obj->count;

	for (p=0;p<obj->count;p++)
	{
		obj->matrix[p].a = &obj->data[(p+0)%count];
		obj->matrix[p].b = &obj->data[(p+1)%count];
		obj->matrix[p].c = &obj->data[(p+2)%count];
	}
}

struct xyz org[10000];
struct xyz point_a,point_b;
struct xyz des_point;

struct xyz ha,hb;

struct matrix *_matrix;

int max_len = 0;
int relx,rely;
int sel = -1;

struct chunky_plains *cp;

#define set_color_nice( n, r, g, b ) SetRGB32( &src -> ViewPort ,n , (unsigned int)  r * 0x01010101, (unsigned int) g * 0x01010101, (unsigned int) b * 0x01010101)

void set_palette(struct Screen *src)
{
	long long int r,g,b;
	unsigned int y;

	set_color_nice( 0 ,  000 ,  000 , 000 );
	set_color_nice( 1 ,  255 ,  000 , 000 );
	set_color_nice( 2 ,  000 ,  255 , 000 );
}

void setxyz( struct xyz *set, double x, double y, double z )
{
	set -> x = x;	set -> y = y;	set -> z = z;
}

void movexyz( struct xyz *set, double x, double y, double z)
{
	set -> x += x;	set -> y += y;	set -> z += z;
}

void move_shape( struct shape *obj, double x, double y, double z)
{
	obj -> x = x;	obj -> y = y;
}

void rotxy( struct xyz *set, double r)
{
	double x1,y1,x2,y2;

	x1 = -sin( r - r90) * set -> x;
	y1 = cos( r - r90) * set -> x;
	x2 = -sin( r ) * set -> y;
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

void conv_3d_to_2d(double cx, double cy, struct xyz *src, struct xyz *des)
{
	double zoom = 400.0f;
	double zoom_fac;

	zoom_fac = (400 + src -> z) / 400;

	des -> x = src -> x * zoom_fac + cx;
	des -> y = src -> y * zoom_fac + cy;
}

void init(struct shape *obj)
{
	obj -> count = 0;
}

void larp( double n, struct xyz *p0, struct xyz *p1, struct xyz *dest )
{
	dest->x = (n * (p1->x-p0->x))  + p0->x;
	dest->y = (n * (p1->y-p0->y))  + p0->y;
	dest->z = (n * (p1->z-p0->z))  + p0->z;
}

void draw_controls(struct shape *obj)
{
	struct xyz *pos;
	int p;
	double l;
	double x, y;
	int count = obj -> count;

	pos = obj -> data;

	for (p=0;p<count;p++)
	{
		set_colour(cp,0,1);

		draw_box(cp,pos[p].x-2, pos[p].y-2, 4, 4);

		x = (pos[p].x + pos[ (p+1) % count ].x) / 2 ;
		y = (pos[p].y + pos[ (p+1) % count ].y) / 2;

		set_colour(cp,0,2);

		draw_box(cp, x -2 , y -2, 4, 4);
	}
}

void draw_triangles(struct shape *obj)
{
	struct xyz *pos;
	struct xyz a,b,c;
	int p;
	double l;

	int count = obj -> count;

	for (p=0;p<count;p++)
	{
		_matrix = &obj->matrix[ p ];

		a.x = _matrix -> a -> x + obj -> x;
		a.y = _matrix -> a -> y + obj -> y;

		b.x = _matrix -> b -> x + obj -> x;
		b.y = _matrix -> b -> y + obj -> y;

		c.x = _matrix -> c -> x + obj -> x;
		c.y = _matrix -> c -> y + obj -> y;


		larp( 0.5f, &a , &b , &ha );
		larp( 0.5f, &b , &c , &hb );	

		for (l=0.0f;l<1.0f;l+=0.01f)
		{
			larp( l, &ha, &b , &point_a );
			larp( l, &b, &hb , &point_b );
			larp( l, &point_a, &point_b, &des_point );

			set_pixel_fast(cp, des_point.x, des_point.y );
		}
	}
}

int get_sel(struct shape *obj,int mx,int my)
{
	int dx,dy,p;
	int r;
	int sel_r = 10000;
	int sel = -1;
	struct xyz *pos = obj -> data;
	
	for (p=0;p<obj -> count;p++)
	{
		dx = pos[p].x - mx; 
 		dy =pos[p].y - my;
		r = sqrt( (dx*dx) + (dy*dy) ); 

		if ((sel_r>r)&&(r<20))
		{
			sel_r = r;
			sel = p;
		}
	}
	return sel;
}

void insert_into(struct shape *obj,int mx,int my)
{
	double x,y;
	int dx,dy,p;
	int r;
	int sel_r = 10000;
	int sel = -1;
	int count = obj -> count;
	struct xyz *pos = obj -> data;
	
	for (p=0;p<count;p++)
	{
		x = (pos[p].x + pos[ (p+1) % count ].x) / 2 ;
		y = (pos[p].y + pos[ (p+1) % count ].y) / 2;

		dx = x - mx; 
 		dy =y - my;
		r = sqrt( (dx*dx) + (dy*dy) ); 

		if (sel_r>r)
		{
			sel_r = r;
			sel = p;
		}
	}

	if (sel>-1)
	{
		for (p = count; p>=sel+1; p--)
		{
			pos[p].x = pos[p-1].x;
			pos[p].y = pos[p-1].y;
		}

		pos[sel+1].x = mx;
		pos[sel+1].y = my;
		pos[sel+1].z = 0.0f;
		obj -> count++;
	}

	if (count==0)
	{
		pos[0].x = mx;
		pos[0].y = my;
		pos[0].z = 0.0f;
		obj -> count=1;
	}
}

void delete_dot( struct shape *obj, int sel )
{
	int p;

	if (obj->count>0)
	{
		for (p = sel; p<obj->count-1; p++)
		{
			obj->data[p].x = obj->data[p+1].x;
			obj->data[p].y = obj->data[p+1].y;
		}
		obj->count--;
	}
}

int menu_pick(int mx, int my)
{
	int bx, by;
	int id;
	bx = mx / 60;
	by = my / 50;
	id =  by + (screenh / 50) * bx;
	return (id < 7) ? id : -1;
}

void init_icons()
{
	struct shape **icon;
	int n = 0;

	for (icon = icons; *icon != NULL; icon++ )
	{
		update_matrix( *icon );
		scale_shape( *icon, 15.0f );
		move_shape( *icon, 30.0f,25.0f + (50.0f * n), 0.0f);
		n++;
	}

}

void draw_menu(int mx,int my)
{
	struct shape **icon;

	int id = menu_pick( mx, my);
	int n;

	n = 0;
	for (icon = icons; *icon != NULL; icon++ )
	{
		set_colour(cp,0,  (selected_draw_mode == n)  ?  1 : ( id == n ?  230 : 150)  );

		draw_triangles( *icon );
		n++;
	}
}

void menu_actions(struct Window *win,struct IntuiMessage *msg)
{
	switch (msg -> Class)
	{
		case IDCMP_MOUSEBUTTONS:

			switch (msg -> Code)
			{
				case SELECTDOWN:
					selected_draw_mode = menu_pick( win -> MouseX, win -> MouseY );
					break;
			}
			break;
	}
}

void (*tool_action) (struct Window *win);

void tool_move_shape(struct Window *win)
{
	edit_shape.x = win -> MouseX - relx;
	edit_shape.y = win -> MouseY - rely;
}

void tool_move_dot(struct Window *win)
{
	if (sel>-1)
	{
		edit_shape.data[sel].x = win -> MouseX;
		edit_shape.data[sel].y = win -> MouseY;
	}
}

void tool_rotate_shape(struct Window *win)
{
	double angel;
	int dx;
	int dy;
	int p;
	double hyp;

	dx = win -> MouseX - relx;
	dy = win -> MouseY - rely;

	hyp = sqrt( (dx*dx) + (dy*dy) );

	if (hyp<1.0f) hyp = 1.0f;

	angel = asin(dy/hyp);

	if (dx<=0)
	{
		angel = r180 - angel;
	}

	edit_shape.x = relx;
	edit_shape.y = rely;

	for (p=0;p<edit_shape.count;p++)
	{
		edit_shape.data[p].x = Edit[p].x - relx;
		edit_shape.data[p].y = Edit[p].y - rely;
	}

	for (p=0;p<edit_shape.count;p++)
	{
		rotxy( &edit_shape.data[p], angel);
	}
}

void paint_actions(struct Window *win,struct IntuiMessage *msg)
{
	switch (msg -> Class)
	{
		case IDCMP_MOUSEBUTTONS:

			switch (msg -> Code)
			{
				case SELECTDOWN:
					sel = get_sel( &edit_shape, win -> MouseX, win -> MouseY );

					if (sel==-1)
					{
						insert_into( &edit_shape, win -> MouseX,win -> MouseY);
						update_matrix( &edit_shape );
					}
					else
					{
						tool_action = tool_move_dot;
					}
					break;

				case SELECTUP:
					sel = -1;
					break;

				case MENUUP:
					sel = get_sel( &edit_shape, win -> MouseX, win -> MouseY );
									
					if (sel>-1)
					{
						delete_dot( &edit_shape, sel);
						update_matrix( &edit_shape );
					}
					sel = -1;
					tool_action = NULL;
					break;

				case MIDDLEDOWN: 
					quit= 1;
					break;
		}
		break;
	}
}

void move_actions(struct Window *win,struct IntuiMessage *msg)
{
	switch (msg -> Class)
	{
		case IDCMP_MOUSEBUTTONS:

			switch (msg -> Code)
			{
				case SELECTDOWN:
					relx = win -> MouseX;
					rely = win -> MouseY;	

					tool_action = tool_move_shape;
					break;

				case SELECTUP:
					sel = -1;
					cord_to_shape(&edit_shape);
					tool_action = NULL;
					break;
			}
			break;
	}
}

void zoom_actions(struct Window *win,struct IntuiMessage *msg)
{
	switch (msg -> Class)
	{
		case IDCMP_MOUSEBUTTONS:

			switch (msg -> Code)
			{
				case SELECTDOWN:
					zoom_shape(&edit_shape,2.0f);
					break;

				case MENUDOWN:
					zoom_shape(&edit_shape,1.0f / 2.0f);
					break;
			}
			break;
	}
}

void rotate_actions(struct Window *win,struct IntuiMessage *msg)
{
	switch (msg -> Class)
	{
		case IDCMP_MOUSEBUTTONS:

			switch (msg -> Code)
			{
				case SELECTDOWN:
					relx = win -> MouseX;
					rely = win -> MouseY;	

					memcpy( _Temp, Edit,sizeof(Edit) );
					edit_shape.data = _Temp;

					pre_rotate(&edit_shape,relx,rely);

					tool_action = tool_rotate_shape;
					break;

				case SELECTUP:
					sel = -1;

					memcpy( Edit, _Temp,sizeof(Edit) );
					edit_shape.data = Edit;
					cord_to_shape(&edit_shape);

					tool_action = NULL;

					break;
		}
		break;
	}
}

void xflip_actions(struct Window *win,struct IntuiMessage *msg)
{
	switch (msg -> Class)
	{
		case IDCMP_MOUSEBUTTONS:

			switch (msg -> Code)
			{
				case SELECTDOWN:
					break;

				case SELECTUP:
					flip_shape(&edit_shape, -1.0f, 1.0f );
					break;
		}
		break;
	}
}


int main()
{
	struct Screen *src = NULL;
	struct Window *win;
	struct RenderInfo ri;
	struct BitMap *bitmap;
	struct RastPort scroll_rp;
	struct IntuiMessage *msg;

	int err;
	double r;

	int txt_pos = 0;

	void *tmp;
	BPTR bmlock;
	double x,y,z;

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
			WA_Borderless, FALSE,
			WA_Activate, TRUE,
			WA_RMBTrap, TRUE,
			WA_ReportMouse, TRUE,
			WA_CustomScreen, (ULONG) src,

			WA_IDCMP,  IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS,
			TAG_END
		);


		if (win)
		{
	       		bitmap = AllocBitMap( screenw, screenh, 8, BMF_CLEAR, win -> RPort -> BitMap);

			scroll_rp.Font =  win -> RPort -> Font;
			scroll_rp.BitMap = bitmap;

			if (err = alloc_cunky_plains(win -> RPort -> BitMap, 1,1,1,1,1,1,1,1, &cp))
			{
				printf("error code: %d\n",err);
			}
			else
			{
				Delay(5);

				set_colour(cp,0,200);

				init( &edit_shape );

				init_icons();

				while (quit == 0)
				{
					while (msg = (struct IntuiMessage *) GetMsg( win -> UserPort))
					{
						if  (win -> MouseX< 60)
						{
							menu_actions(win,msg);
						}
						else
						{
							switch ( selected_draw_mode)
							{
								case d_edit: 	paint_actions(win,msg); break;
								case d_rotate: rotate_actions(win,msg); break;
								case d_move:	move_actions(win,msg); break;
								case d_zoom:  zoom_actions(win,msg); break;
								case d_xflip:  xflip_actions(win,msg); break;

								case d_save: 	
											selected_draw_mode = d_edit;
											break;
								case d_load:	
											load( &edit_shape );
											update_matrix( &edit_shape );
											selected_draw_mode = d_edit;
											break;
							}
						}

						ReplyMsg( (struct Message *) msg);
					}

					if (tool_action)
					{
						tool_action(win);
					}

					update_matrix( &edit_shape );


					if (selected_draw_mode == d_edit )
					{
						set_colour(cp,0,1);
						draw_controls( &edit_shape );
					}


					set_colour(cp,0,200);
					draw_triangles( &edit_shape );

					draw_menu(win -> MouseX, win -> MouseY);

					plains_to_rp(cp, win -> RPort );
					clear_plains( cp );

					Delay(1);
				}

				free_chunky_plains(cp);

			}

			FreeBitMap(bitmap);
			CloseWindow(win);
		}

		CloseScreen(src);
	}

	dump_xy( &edit_shape );
}

