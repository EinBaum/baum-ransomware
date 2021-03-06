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

#ifndef BAUMCOMMON_H
#define BAUMCOMMON_H

#include <stdio.h>

extern char baumcommon_verbose;
extern char baumcommon_test;
extern char baumcommon_force;

void baumcommon_setverbose(char b);
void baumcommon_settest(char b);
void baumcommon_setforce(char b);

#define bp(...) do {if (baumcommon_verbose) {printf(__VA_ARGS__); printf("\n");}} while (0);

#define be(...) do {fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");} while (0);

#endif
