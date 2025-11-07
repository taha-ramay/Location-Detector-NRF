#ifndef MbedTlsManager_H
#define MbedTlsManager_H
#include "mbedtls/aes.h"
#include "mbedtls/cipher.h"
#include "mbedtls/md5.h"
#include <stdio.h>
#include <zephyr/logging/log.h>

void generateSharedKey(const char *text);
int aes_cfb128(unsigned char *input, int crypt_len, unsigned char *output);
#endif
