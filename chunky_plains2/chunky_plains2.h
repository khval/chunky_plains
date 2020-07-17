
#ifndef PI
#define PI (3.141592653589793)
#endif

struct p { int x ; int y ; };
struct p3 { int x ; int y ; int z; };

struct _RenderInfo
{
	char *Memory;
	uint32 BytesPerRow;
};

struct chunky_plains {
	char bits[8];					// bits on plain
	char index[8];					// index bit for color
	char palette[3][8][256];			// hemm...
	char current_color;
	char current_plane;
	int width;
	int height;
	int bytes_per_row;				// copy of bitmap bytes per row
	char *data[8];					// ptr to plain buffer
	char *f_buffer;					// buffer so we can do filtering.
	char *n_buffer;					// buffer so we can do filtering.
};

enum {
	cp_no_error = 0,
	cp_error_no_mem,
	cp_error_too_many_bits
};


void clear_plains(struct chunky_plains *cp);
void plains_to_rp(struct chunky_plains *cp, struct RastPort *rp);
void plains_to_video(struct chunky_plains *cp, int h, struct BitMap *dest);
void free_chunky_plains(struct chunky_plains *p);
void set_colour( struct chunky_plains *cp, char number, char color );
void set_pixel_fast_nomask(struct chunky_plains *cp, int x,int y);
void set_pixel_fast(struct chunky_plains *cp, int x,int y);
void set_pixel(struct chunky_plains *cp, char number, char color, int x,int y);
void draw_hline_fast_x4(struct chunky_plains *cp, int x,int y,int w);
void __draw_hline_fast(struct chunky_plains *cp, int x,int y,int w);
void draw_hline_fast(struct chunky_plains *cp, int x,int y,int w);
void draw_box(struct chunky_plains *cp, int x,int y,int w, int h);
int get_pixel(struct chunky_plains *cp, char number, int x,int y);
void scroll(struct chunky_plains *cp, int number, int x1,int y1,int x2,int y2, int offset_x, int offset_y );
void triangle(void *cp,int x1,int y1,int x2,int y2, int x3, int y3, int p, int c);
void line(void *cp,int x1,int y1,int x2,int y2, int p, int c);

