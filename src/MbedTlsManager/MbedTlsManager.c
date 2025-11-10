#include "MbedTlsManager.h"
LOG_MODULE_REGISTER(MbedTls_Debug);
unsigned char sharedKey[16];
void generateSharedKey(const char *text) {
  LOG_INF("getting shared keys");
  struct mbedtls_md5_context context;
  mbedtls_md5_init(&context);
  mbedtls_md5_starts(&context); // <-- REQUIRED

  mbedtls_md5_update(&context, (const unsigned char *)text, strlen(text));
  mbedtls_md5_finish(&context, sharedKey);
  mbedtls_md5_free(&context);
  LOG_INF("Generating MD5 hash for text: '%s'", text);
  char md5_str[16 * 3 + 1]; // "xx xx xx ... xx "
  char *p = md5_str;
  for (int i = 0; i < 16; i++) {
    p += sprintf(p, "%02x ", sharedKey[i]);
  }
  LOG_INF("the value of key is ");
  LOG_INF("MD5('%s') = %s", text, md5_str);
}
int aes_cfb128(unsigned char *input, int crypt_len, unsigned char *output) {
  LOG_INF("Decrypting data");
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  int ret = mbedtls_aes_setkey_enc(&aes, sharedKey, 128);
  if (ret != 0) {
    LOG_ERR("setkey_enc failed: %d", ret);
    mbedtls_aes_free(&aes);
    return 0;
  }
  ret = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, input, output);
  if (ret != 0) {
    LOG_INF("mbedtls_aes_crypt_cfb128(MBEDTLS_AES_DECRYPT) failed %d", ret);
    return 0;
  } else {
    LOG_INF("mbedtls_aes_crypt_cfb128(MBEDTLS_AES_DECRYPT) ok");
  }
  mbedtls_aes_free(&aes);
  LOG_INF("decryption completed");

  return 1;
}