#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pmcommon.h"

#ifdef HAVE_PNG
#include "png.h"
#endif

struct pm_image *pm_newImage(int width, int height)
{
	struct pm_image *pmimg;
	int img_size;

	img_size = width * height / 8;
	pmimg = calloc(1, sizeof(struct pm_image) + img_size);
	if (!pmimg) {
		perror("calloc");
		return NULL;
	}

	pmimg->w = width;
	pmimg->h = height;

	return pmimg;
}

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

void pm_displayImage(struct pm_image *img)
{
	int y, x;
	unsigned char b;

	for (y=0; y<img->h; y++) {
		for (x=0; x<img->w/8; x++) {
			for (b=0x80; b; b>>=1) {
				if (img->image_data[y * img->w/8 + x] & b) {
					fputs(" ", stdout);
				} else {
					fputs("\033[7m \033[0m", stdout);
				}
			}
		}
		fputs("\n", stdout);
	}
}

int pm_appendToDirectory(const char *sdr_file, const char *shp_file, struct pm_image *image, char *name)
{
	FILE *fptr_sdr = NULL, *fptr_shp = NULL;
	unsigned char header[4];
	unsigned char sdr_entry[16];
	int name_length;

	name_length = strlen(name);
	if (name_length > 16) {
		fputs("Name too long\n", stderr);
		return -1;
	}

	fptr_sdr = fopen(sdr_file, "wb");
	if (!fptr_sdr) {
		perror(sdr_file);
		goto err;
	}

	fptr_shp = fopen(shp_file, "wb");
	if (!fptr_shp) {
		perror(shp_file);
		goto err;
	}

	fseek(fptr_sdr, 0, SEEK_END);
	fseek(fptr_shp, 0, SEEK_END);

	/* Write to the SHP file */
	header[0] = 0x0b;
	header[1] = image->h;
	header[2] = image->w;
	header[3] = 0; // extra (unknown use)
	fwrite(header, sizeof(header), 1, fptr_shp);
	fwrite(image->image_data, image->w / 8 * image->h, 1, fptr_shp);
	fwrite(header + 3, 1, 1, fptr_shp); // second unknown extra byte

	/* Write to the SDR file */
	memset(sdr_entry, 0, sizeof(sdr_entry));
	memcpy(sdr_entry, name, name_length);
	fwrite(sdr_entry, sizeof(sdr_entry), 1, fptr_sdr);

	return 0;
err:
	if (fptr_shp)
		fclose(fptr_shp);
	if (fptr_sdr)
		fclose(fptr_sdr);

	return -1;
}

#ifdef HAVE_PNG
struct pm_image *pm_loadPng(const char *pngfile)
{
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep *row_pointers;
	int w,h,depth,color,y;
	struct pm_image *pmimg = NULL;
	FILE *fptr_in = NULL;
	uint8_t header[8];

	fptr_in = fopen(pngfile, "rb");
	if (!fptr_in) {
		perror(pngfile);
		return NULL;
	}

	if (8 != fread(header, 1, 8, fptr_in)) {
		perror("fread");
		goto err;
	}

	if (png_sig_cmp(header, 0, 8)) {
		fputs("Not a PNG file\n", stderr);
		goto err;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fputs("Failed to create read struct\n", stderr);
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fputs("Failed to create info struct\n", stderr);
		goto err;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		goto err;
	}

	png_init_io(png_ptr, fptr_in);
	png_set_sig_bytes(png_ptr, 8);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_PACKING, NULL);

	w = png_get_image_width(png_ptr, info_ptr);
	h = png_get_image_height(png_ptr, info_ptr);
	depth = png_get_bit_depth(png_ptr, info_ptr);
	color = png_get_color_type(png_ptr, info_ptr);

	if (color != PNG_COLOR_TYPE_PALETTE) {
		fputs("PNG image must be palletized\n", stderr);
		goto err;
	}
	if (depth != 1) {
		fputs("PNG image must use a 1-bit (BW) palette\n", stderr);
		goto err;
	}

	printf("Image: %d x %d, ",w,h);

	pmimg = pm_newImage(w, h);

	row_pointers = png_get_rows(png_ptr, info_ptr);

	for (y=0; y<h; y++) {
		memcpy(&pmimg->image_data[y * w/8], row_pointers[y], w/8);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return pmimg;

err:
	if (png_ptr) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	}
	if (pmimg) {
		pm_freeImage(pmimg);
	}
	if (fptr_in) {
		fclose(fptr_in);
	}
	return NULL;
}
#endif
