#include <stdio.h>
#include <stdlib.h>
#include "pmcommon.h"

int main(int argc, char **argv)
{
	int idx;
	struct pm_image *pmimg;

	if (argc < 3) {
		printf("Usage: ./pmshow FILE.SHP index\n");
		printf("\n");
		printf("Image index can be found by running pmdir on the associated .SDR file.\n");
		return 1;
	}

	idx = atoi(argv[2]);

	pmimg = pm_loadImage(argv[1], idx);
	if (!pmimg) {
		fprintf(stderr, "Could not find image index %d\n", idx);
		return 1;
	}

	printf("Image size: %d x %d\n", pmimg->w, pmimg->h);
	pm_displayImage(pmimg);
	printf("\n\n");

	pm_freeImage(pmimg);

	return 0;
}
