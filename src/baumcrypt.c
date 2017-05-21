#include "baumcrypt.h"

#include <stdio.h>

#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#include "baumcommon.h"
#include "baumhelper.h"

#define IV_LEN (16)
#define BUFSIZE (1024)

int baumcrypt_makekey(unsigned char *key_out) {
	int hret = RAND_bytes(key_out, BAUMCRYPT_KEYLEN);
	if (hret == 0) {
		bp("RAND_bytes failed");
		return 1;
	}
	return 0;
}

static int baumcrypt_helper2(FILE *file_in, FILE *file_out,
	unsigned char *key, unsigned char *iv, char mode_encrypt) {

	int hret = 0;

	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		bp("EVP_CIPHER_CTX_new failed");
		return 1;
	}
	hret = EVP_CipherInit(ctx, EVP_aes_256_cbc(), key, iv, 			mode_encrypt);
	if (hret == 0) {
		bp("EVP_CipherInit failed");
		ret = 1;
		goto end;
	}

	unsigned char *buf_in = alloca(BUFSIZE);
	unsigned char *buf_out = alloca(BUFSIZE + EVP_MAX_BLOCK_LENGTH);
	int out_len = 0;

	int ret = 0;

	for (;;) {
		size_t num_read = fread(buf_in, 1, BUFSIZE, file_in);
		hret = EVP_CipherUpdate(ctx, buf_out, &out_len, buf_in, num_read);
		if (hret == 0) {
			bp("EVP_CipherUpdate failed");
			ret = 1;
			goto end;
		}
		hret = fwrite(buf_out, 1, out_len, file_out);
		if (hret != out_len) {
			bp("fwrite failed");
			ret = 1;
			goto end;
		}
		if (num_read < BUFSIZE) {
			break;
		}
	}

	hret = EVP_CipherFinal_ex(ctx, buf_out, &out_len);
	if (hret == 0) {
		bp("EVP_CipherFinal_ex failed");
		ret = 1;
		goto end;
	}
	hret = fwrite(buf_out, 1, out_len, file_out);
	if (hret != out_len) {
		bp("final fwrite failed");
		ret = 1;
		goto end;
	}
end:
	EVP_CIPHER_CTX_free(ctx);
	return ret;
}

static int baumcrypt_helper1(const char *name_in, const char *name_out,
	unsigned char *key, char mode_encrypt) {

	int hret = 0;
	unsigned char iv[IV_LEN];

	FILE *file_in = fopen(name_in, "rb");
	if (!file_in) {
		bp("cannot open for reading '%s'", name_in);
		return 1;
	}

	int ret = 0;

	FILE *file_out = fopen(name_out, "wb");
	if (!file_out) {
		bp("cannot open for writing '%s'", name_out);
		ret = 1;
		goto end1;
	}

	if (mode_encrypt) {
		hret = RAND_bytes(iv, IV_LEN);
		if (hret == 0) {
			bp("RAND_bytes failed");
			ret = 1;
			goto end2;
		}
		hret = fwrite(iv, IV_LEN, 1, file_out);
		if (hret != 1) {
			bp("failed to write IV");
			ret = 1;
			goto end2;
		}
	} else {
		hret = fread(iv, IV_LEN, 1, file_in);
		if (hret != 1) {
			bp("failed to read IV");
			ret = 1;
			goto end2;
		}
	}

	hret = baumcrypt_helper2(file_in, file_out, key, iv, mode_encrypt);
	if (hret != 0) {
		bp("operation failed");
		ret = 1;
		goto end2;
	}

end2:
	fclose(file_out);
end1:
	fclose(file_in);
	return ret;
}

int baumcrypt_encrypt(const char *name_in, const char *name_out, unsigned char *key) {
	return baumcrypt_helper1(name_in, name_out, key, 1);
}
int baumcrypt_decrypt(const char *name_in, const char *name_out, unsigned char *key) {
	return baumcrypt_helper1(name_in, name_out, key, 0);
}
