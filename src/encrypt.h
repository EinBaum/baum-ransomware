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

#ifndef ENCRYPT_H
#define ENCRYPT_H

int baum_createkey(const char *keyfile);
int baum_createkey_stdout(void);

int baum_encrypt(const char *directories[], const char *extension,
	const char *keyfile);
int baum_encrypt_stdin(const char *directories[], const char *extension);

int baum_decrypt(const char *directories[], const char *extension,
	const char *keyfile);
int baum_decrypt_stdin(const char *directories[], const char *extension);

#endif
