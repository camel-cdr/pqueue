#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <unistd.h>

#include "arg.h"

static void
usage(char *argv0)
{
	printf("Usage: %s [OPTION]... CMD\n", argv0);
	puts("Execute CMD several times on multiple processes.\n");

	puts("Options:");
	puts("  -c, --count NUM       execute NUM times");
	puts("  -n, --num-procs NUM   quit after NUM outputs");
	puts("  -?, --help            display this help and exit\n");
}

int
main(int argc, char **argv)
{
	char *argv0 = argv[0];
	int i, loop = 1;
	long count = 0, nprocs = get_nprocs();

	ARG_BEGIN {
		if (ARG_LONG("help")) case 'h': case '?': {
			usage(argv0);
			return EXIT_SUCCESS;
		} else if (ARG_LONG("count")) case 'c': {
			count = atol(ARG_VAL());
			loop = (count <= 0);
		} else if (ARG_LONG("num-procs")) case 'n': {
			nprocs = atoi(ARG_VAL());
			if (nprocs <= 0)
				nprocs = get_nprocs();
		} else default: {
			fprintf(stderr,
			        "%s: invalid option '%s'\n"
			        "Try '%s --help' for more information.\n",
			        argv0, *argv, argv0);
			return EXIT_SUCCESS;
		}
	} ARG_END;

	if (argc != 1) {
		usage(argv0);
		return EXIT_SUCCESS;
	}

	/* we don't want to open more processes than count */
	if (count > 1 && count < nprocs)
		nprocs = count;

	/* open initial children */
	for (i = 0; i < nprocs; ++i) {
		--count;
		if (fork() == 0)
			return system(argv[0]);
	}

	/* reopen children */
	while (1) {
		int stat;
		(void)wait(&stat);

		if (WIFEXITED(stat) && WEXITSTATUS(stat) != 0)
			break;

		if (!loop && count-- <= 0)
			break;

		if (fork() == 0)
			return system(argv[0]);
	}

	/* wait for all children to close */
	while (wait(NULL) > 0);

	return EXIT_SUCCESS;
}

/*
--------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
--------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2020 Olaf Berstein
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
For more information, please refer to <http://unlicense.org/>
--------------------------------------------------------------------------------
*/
