#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pmcommon.h"

struct pm_image *pm_loadImage(const char *shapefile, int index)
{
	FILE *fptr;
	static unsigned char header[4];
	unsigned char width, height;
	int img_size, i;
	struct pm_image *pmimg;

	fptr = fopen(shapefile, "rb");
	if (!fptr) {
		perror("fopen");
		return NULL;
	}

	i = 0;
	while (1 == fread(header, sizeof(header), 1, fptr))
	{
		if (header[0] != 0x0b) {
			fprintf(stderr, "Unexpected magic number\n");
			break;
		}

		height = header[1];
		width = header[2];

		if (width % 8) {
			fprintf(stderr, "Image width is not byte-aligned\n");
			break;
		}

		img_size = width * height / 8;

		if (i != index) {
			// Not the image we are looking for. Skip it.
			fseek(fptr, img_size, SEEK_CUR);
		}
		else {
			pmimg = calloc(1, sizeof(struct pm_image) + img_size);
			if (!pmimg) {
				perror("could not allocate image");
				break;
			}

			if (1 != fread(pmimg->image_data, img_size, 1, fptr)) {
				perror("read error");
				free(pmimg);
				break;
			}

			pmimg->w = width;
			pmimg->h = height;

			fclose(fptr);
			return pmimg;
		}

		/* skip the padding byte */
		fseek(fptr, 1, SEEK_CUR);
		i++;
	}

	fclose(fptr);

	return NULL;
}

void pm_freeImage(struct pm_image *img)
{
	if (img)
		free(img);
}

struct pm_dir *pm_openDir(const char *shapeDirFile)
{
	struct pm_dir *pmdir;

	pmdir = calloc(1, sizeof(struct pm_dir));
	if (!pmdir) {
		perror("calloc");
		return NULL;
	}

	pmdir->fptr = fopen(shapeDirFile, "rb");
	if (!pmdir->fptr) {
		perror("fopen");
		free(pmdir);
		return NULL;
	}

	return pmdir;
}

void pm_closeDir(struct pm_dir *pmdir)
{
	if (pmdir) {
		if (pmdir->fptr) {
			fclose(pmdir->fptr);
		}
		free(pmdir);
	}
}

int pm_readDirEntry(struct pm_dir *pmdir)
{
	memset(pmdir->name.str, 0, sizeof(pmdir->name.str));
	if (1 != fread(pmdir->name.str, 16, 1, pmdir->fptr)) {
		return -1;
	}
	return 0;
}

