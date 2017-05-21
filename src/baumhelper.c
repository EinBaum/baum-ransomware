#include "baumhelper.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>

#include "baumcommon.h"

int helper_putfile(const char *name, off_t len, void *data) {
	FILE *file = fopen(name, "wb");
	if (!file) {
		printf_v("Failed to open file for writing '%s'", name);
		return 1;
	}
	fwrite(data, len, 1, file);
	fclose(file);
	return 0;
}

int helper_openfile(const char *name, int *pfd, off_t *plen, void **pdata) {
	int fd = open(name, O_RDONLY);
	if (fd == -1) {
		printf_v("Failed to open '%s'", name);
		return 1;
	}
	off_t len = lseek(fd, 0, SEEK_END);
	if (len == (off_t)-1) {
		printf_v("Can't get length of '%s'", name);
		close(fd);
		return 1;
	}
	void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == (void*)-1) {
		printf_v("Failed to read '%s'", name);
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

int helper_get_own_name(char *buf, size_t buflen) {
	int helper_ret = readlink("/proc/self/exe", buf, buflen);
	if (helper_ret < 0) {
		printf_v("Failed to get own filename");
		return 1;
	}
	return 0;
}

int helper_chdir_home(void) {
	const char *home = getenv("HOME");
	if (!home) {
		printf_v("Failed to get HOME");
		return 1;
	}
	int ret = chdir(home);
	if (ret != 0) {
		printf_v("Failed to chdir to HOME");
		return 1;
	}

	return 0;
}

static list_callback helper_list_current_cb;
static void *helper_list_current_arg;

static int helper_list_cb(const char *name, const struct stat *status, int type) {
	switch (type) {
	case FTW_F:
		printf_v("-> File '%s'", name);
		helper_list_current_cb(name, helper_list_current_arg);
		break;
	}
	return 0;
}

int helper_list(const char *dir, list_callback cb, void *cb_arg) {
	helper_list_current_cb = cb;
	helper_list_current_arg = cb_arg;

	printf_v("Listing dir '%s'", dir);
	int helper_ret = ftw(dir, helper_list_cb, 1);
	if (helper_ret == -1) {
		printf_v("Error listing dir '%s'", dir);
		return 1;
	}
	return 0;
}

