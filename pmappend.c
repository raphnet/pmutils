#include <stdio.h>
#include "pmcommon.h"

int main(int argc, char **argv)
{
	struct pm_image *pmimg;

	if (argc < 5) {
		printf("Usage: ./pmappend png_file description sdr_file shp_file\n");
		return 0;
	}

	pmimg = pm_loadPng(argv[1]);
	if (!pmimg) {
		return 1;
	}

	if (pm_appendToDirectory(argv[3], argv[4], pmimg, argv[2]) < 0) {
		return 1;
	}

	printf("Appended %s (%s) to %s:%s\n", argv[1], argv[2], argv[3], argv[4]);

	return 0;
}
