
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

struct shape
{
	double x;
	double y;
	struct xyz *data;
	struct matrix *matrix;
	int count;
};

extern int screenw;
extern int screenh;

