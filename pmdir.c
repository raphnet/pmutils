#include <stdio.h>
#include <stdlib.h>
#include "pmcommon.h"

int main(int argc, char **argv)
{
	struct pm_dir *pmdir;
	int idx;

	if (argc < 2) {
		printf("Usage: ./pmshow FILE.SDR\n");
		printf("\n");
		return 1;
	}

	pmdir = pm_openDir(argv[1]);
	if (!pmdir) {
		fputs("Could not read directory file\n", stderr);
		return 1;
	}

	idx = 0;
	while (0 == pm_readDirEntry(pmdir)) {
		printf("%2d : %s\n", idx, (char*)pmdir->name.str);
		idx++;
	}

	pm_closeDir(pmdir);

	return 0;
}
