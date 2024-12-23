// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020, VMware, Tzvetomir Stoyanov <tz.stoyanov@gmail.com>
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <libgen.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "trace-utest.h"

const char *argv0;
bool show_output;

enum unit_tests {
	RUN_NONE	= 0,
	RUN_TRACECMD	= (1 << 0),
	RUN_ALL		= 0xFFFF
};

static void print_help(char **argv)
{
	printf("Usage: %s [OPTIONS]\n", basename(argv[0]));
	printf("\t-s, --silent\tPrint test summary\n");
	printf("\t-r, --run test\tRun specific test:\n");
	printf("\t\t  trace-cmd   run trace-cmd tests\n");
	printf("\t-h, --help\tPrint usage information\n");
	exit(0);
}

int main(int argc, char **argv)
{
	CU_BasicRunMode verbose = CU_BRM_VERBOSE;
	enum unit_tests tests = RUN_NONE;

	argv0 = argv[0];

	for (;;) {
		int c;
		int index = 0;
		const char *opts = "+hsr:v";
		static struct option long_options[] = {
			{"silent", no_argument, NULL, 's'},
			{"run", required_argument, NULL, 'r'},
			{"verbose", no_argument, NULL, 'v'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
		};

		c = getopt_long (argc, argv, opts, long_options, &index);
		if (c == -1)
			break;
		switch (c) {
		case 'r':
			if (strcmp(optarg, "trace-cmd") == 0)
				tests |= RUN_TRACECMD;
			else
				print_help(argv);
			break;
		case 's':
			verbose = CU_BRM_SILENT;
			break;
		case 'v':
			show_output = true;
			break;
		case 'h':
		default:
			print_help(argv);
			break;
		}
	}

	if (tests == RUN_NONE)
		tests = RUN_ALL;

	if (CU_initialize_registry() != CUE_SUCCESS) {
		printf("Test registry cannot be initialized\n");
		return -1;
	}

	if (tests & RUN_TRACECMD)
		test_tracecmd_lib();

	CU_basic_set_mode(verbose);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return 0;
}
