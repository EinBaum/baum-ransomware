#include "baumhelper.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <ftw.h>

#include "baumcommon.h"

char helper_fileexists(const char *name) {
	return access(name, F_OK) == 0;
}

int helper_putfile(const char *name, off_t len, void *data) {
	FILE *file = fopen(name, "wb");
	if (!file) {
		bp("Failed to open file for writing '%s'", name);
		return 1;
	}
	fwrite(data, len, 1, file);
	fclose(file);
	return 0;
}

int helper_openfile(const char *name, int *pfd, off_t *plen, void **pdata) {
	int fd = open(name, O_RDONLY);
	if (fd == -1) {
		bp("Failed to open '%s'", name);
		return 1;
	}
	off_t len = lseek(fd, 0, SEEK_END);
	if (len == (off_t)-1) {
		bp("Can't get length of '%s'", name);
		close(fd);
		return 1;
	}
	void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == (void*)-1) {
		bp("Failed to read '%s'", name);
		close(fd);
		return 1;
	}

	*pfd = fd;
	*plen = len;
	*pdata = data;

	return 0;
}

void helper_closefile(int fd, off_t len, void *data) {
	munmap(data, len);
	close(fd);
}

static char my_filename[BUFSIZ];
static char my_filename_init = 0;
static const char *my_basename = NULL;

const char *helper_get_own_name(void) {
	if (!my_filename_init) {
		int hret = readlink("/proc/self/exe",
			my_filename, sizeof(my_filename));
		if (hret < 0) {
			bp("Failed to get own filename");
			return NULL;
		}
		my_filename_init = 1;
	}
	return my_filename;
}

const char *helper_get_own_basename(void) {
	if (!my_basename) {
		const char *filename = helper_get_own_name();
		if (!filename) {
			return NULL;
		}
		const char *occ = strrchr(filename, '/');
		if (!occ) {
			my_basename = filename;
		} else {
			my_basename = (occ + 1);
		}
	}
	return my_basename;
}

int helper_chdir_home(void) {
	const char *home = getenv("HOME");
	if (!home) {
		bp("Failed to get HOME");
		return 1;
	}
	int ret = chdir(home);
	if (ret != 0) {
		bp("Failed to chdir to HOME");
		return 1;
	}

	return 0;
}

static list_callback helper_list_current_cb;
static void *helper_list_current_arg;

static int helper_list_cb(const char *name, const struct stat *status, int type) {
	switch (type) {
	case FTW_F:
		bp("-> File '%s'", name);
		helper_list_current_cb(name, helper_list_current_arg);
		break;
	}
	return 0;
}

int helper_list(const char *dir, list_callback cb, void *cb_arg) {
	helper_list_current_cb = cb;
	helper_list_current_arg = cb_arg;

	bp("Listing dir '%s'", dir);
	int hret = ftw(dir, helper_list_cb, 1);
	if (hret == -1) {
		bp("Error listing dir '%s'", dir);
		return 1;
	}
	return 0;
}

