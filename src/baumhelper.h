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

#ifndef BAUMHELPER_H
#define BAUMHELPER_H

#define _DEFAULT_SOURCE

#include <sys/types.h>

char helper_fileexists(const char *name);

int helper_putfile(const char *name, off_t len, void *data);
int helper_openfile(const char *name, int *pfd, off_t *plen, void **pdata);
void helper_closefile(int fd, off_t len, void *data);

int helper_get_own_name(char *buf, size_t buflen);
int helper_chdir_home(void);

typedef int (*list_callback)(const char *name, void *arg);
int helper_list(const char *dir, list_callback cb, void *cb_arg);

#endif
