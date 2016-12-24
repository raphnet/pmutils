#ifndef _pm_common_h__
#define _pm_common_h__

#include <stdint.h>

struct pm_image {
	uint8_t w, h;
	uint8_t image_data[];
};

struct pm_name {
	uint8_t str[17];
};

struct pm_dir {
	FILE *fptr;
	struct pm_name name;
};

struct pm_image *pm_newImage(int width, int height);
struct pm_image *pm_loadPng(const char *pngfile);
struct pm_image *pm_loadImage(const char *shapefile, int index);
void pm_freeImage(struct pm_image *img);

int pm_appendToDirectory(const char *sdr_file, const char *shp_file, struct pm_image *image, char *name);

void pm_displayImage(struct pm_image *img);

struct pm_dir *pm_openDir(const char *shapeDirFile);
int pm_readDirEntry(struct pm_dir *pmdir);
void pm_closeDir(struct pm_dir *pmdir);

#endif // _pm_common_h__
