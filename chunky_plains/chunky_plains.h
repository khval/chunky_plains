
#ifndef PI
#define PI (3.141592653589793)
#endif

struct p { int x ; int y ; };

struct chunky_plains {
	char bits[8];					// bits on plain
	char index[8];					// index bit for color
	char mask[8];					// mask for color value
	char palette[3][8][256];			// hemm...
	char current_color;
	char current_mask;
	int bytes_per_row;				// copy of bitmap bytes per row
	char *data;					// ptr to the Bitmap memory
	char *f_buffer;					// buffer so we can do filtering.
	char *n_buffer;					// buffer so we can do filtering.
};

enum {
	cp_error_no_mem = 0,
	cp_error_too_many_bits
};

void free_chunky_plains(struct chunky_plains *p);
int alloc_cunky_plains(struct BitMap *bm, char n0,char n1,char n2,char n3,char n4,char n5, char n6,char n7, int w,int h,struct chunky_plains **new_cp);
void set_colour( struct chunky_plains *cp, char number, char color );
void set_pixel_fast_nomask(struct chunky_plains *cp, int x,int y);
void set_pixel_fast(struct chunky_plains *cp, int x,int y);
void draw_hline_fast(struct chunky_plains *cp, int x,int y,int w);
void set_pixel(struct chunky_plains *cp, char number, char color, int x,int y);
int get_pixel(struct chunky_plains *cp, char number, int x,int y);
void scroll(struct chunky_plains *cp, int number, int x1,int y1,int x2,int y2, int offset_x, int offset_y );
void triangle(void *cp,int x1,int y1,int x2,int y2, int x3, int y3, int p, int c);
void line(void *cp,int x1,int y1,int x2,int y2, int p, int c);

