
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define __USE_INLINE__
#include <proto/asl.h>
#include <proto/exec.h>

#include "curves_ed.h"
#include "curves_ed_load.h"

struct Library 			 *AslBase = NULL;
struct AslIFace 		 *IAsl = NULL;

#define dprintf printf

BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface)
{
	*interface = NULL;
	*base = OpenLibrary( name , ver);
	if (*base)
	{
		 *interface = GetInterface( *base,  iname , iver, TAG_END );
		if (!*interface) dprintf("Unable to getInterface %s for %s %ld!\n",iname,name,ver);
	}
	else
	{
	   	dprintf("Unable to open the %s %ld!\n",name,ver);
	}
	return (*interface) ? TRUE : FALSE;
}

char *asl()
{
	struct FileRequester	 *filereq;
	char *ret = NULL;
	char c;
	int l;

	if (filereq = (struct FileRequester	 *) AllocAslRequest( ASL_FileRequest, TAG_DONE ))
	{
		if (AslRequestTags( (void *) filereq, ASLFR_DrawersOnly, FALSE,	TAG_DONE ))
		{
			if ((filereq -> fr_File)&&(filereq -> fr_Drawer))
			{
				if (l = strlen(filereq -> fr_Drawer))
				{
					c = filereq -> fr_Drawer[l-1];
					if (ret = malloc( strlen(filereq -> fr_Drawer) + strlen(filereq -> fr_File) +2 ))
					{
						sprintf( ret, ((c == '/') || (c==':')) ? "%s%s" : "%s/%s",  filereq -> fr_Drawer, filereq -> fr_File ) ;
					}
				}
				else ret = strdup(filereq -> fr_File);
			}
		}
		 FreeAslRequest( filereq );
	}

	return ret;
}

char *get_name(FILE *fd)
{
	char	rbuffer[1000];
	char	buffer[1000];
	int ret ;
	char *name = NULL;

	if (!feof(fd))
	{
		if (fgets(rbuffer,sizeof(rbuffer),fd))
		{
			ret = sscanf(rbuffer,"struct xyz %[^[[[] = {", buffer);

			if (ret>0) 
			{
				printf("%s\n",buffer);
				name = (char *) strdup(buffer);
			}
		}
	}

	return name;
}

void get_data(FILE *fd,struct shape *obj)
{
	char	buffer[1000];
	double x,y,z;
	int ret ;


	obj -> count = 0;

	while (!feof(fd))
	{
		ret = fscanf(fd," {%lf,%lf,%lf%[^\n]",
				&obj -> data[ obj -> count ].x,
				&obj -> data[ obj -> count ].y,
				&obj -> data[ obj -> count ].z,
				buffer);

		if (ret == 4)
		{

			obj -> data[ obj -> count ].x += screenw/2;
			obj -> data[ obj -> count ].y += screenh/2;

			printf("	%lf,%lf,%lf\n",
				obj -> data[ obj -> count ].x,
				obj -> data[ obj -> count ].y,
				obj -> data[ obj -> count ].z);

			obj -> count ++;

			if (strcmp(buffer,"}") == 0 )
			{
				ret = fscanf(fd,"%[^\n]",buffer);
				ret = 0;
				break;
			}
		}
		else
		{
			printf("file has bad format, only %d arg\n",ret);
			break;
		}
	}
}


void load(struct shape *obj)
{
	char	buffer[1000];
	char *fname = NULL;
	char *name = NULL;
	FILE *fd;

	if ( open_lib( "asl.library", 0L , "main", 1, &AslBase, (struct Interface **) &IAsl  ) ) 
	{
		if (fname = asl())
		{
			if (fd = fopen(fname,"r"))
			{
				printf("file is open\n");

				while (!feof(fd))
				{
					name = get_name(fd);
					if (name)
					{
						get_data(fd, obj);
						free(name);
					}
				}
				fclose (fd);
			}
			free(fname);
		}

		if (AslBase) CloseLibrary(AslBase); AslBase = 0;
		if (IAsl) DropInterface((struct Interface*)IAsl); IAsl = 0;
	}
}


