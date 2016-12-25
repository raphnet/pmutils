#define _GNU_SOURCE // for strcasestr
#include <stdio.h>
#include <string.h>
#include "pmcommon.h"

int main(int argc, char **argv)
{
	struct pm_image *pmimg;
	const char *png_filename, *description, *sdr_filename, *shp_filename;

	if (argc < 5) {
		printf("Usage: ./pmappend png_file description sdr_file shp_file\n");
		return 0;
	}

	png_filename = argv[1];
	description = argv[2];
	sdr_filename = argv[3];
	shp_filename = argv[4];

	if (!strcasestr(sdr_filename, ".SDR")) {
		fputs("Expected a file ending by .SDR\n", stderr);
		return 1;
	}

	if (!strcasestr(shp_filename, ".SHP")) {
		fputs("Expected a file ending by .SHP\n", stderr);
		return 1;
	}

	pmimg = pm_loadPng(png_filename);
	if (!pmimg) {
		return 1;
	}

	if (pm_appendToDirectory(sdr_filename, shp_filename, pmimg, description) < 0) {
		return 1;
	}

	printf("Appended %s (%s) to %s:%s\n", png_filename, description, sdr_filename, shp_filename);

	return 0;
}
