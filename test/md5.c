#include "md5.h"

#include <openssl/evp.h>

#include "utest.h"

#define ASSERT_MD5_HEX_DIGEST(input)             \
  do {                                           \
    md5_hex_digest ftssl_digest;                 \
    md5_hex_digest official_digest;              \
    ftssl_md5_digest(input, ftssl_digest);       \
    official_md5_digest(input, official_digest); \
    ASSERT_STREQ(official_digest, ftssl_digest); \
  } while (0)

static int8_t md5_official_hash(const char *str, uint8_t result[16]);

static void ftssl_md5_digest(const char *input, md5_hex_digest hex_digest) {
  md5_digest digest;
  memset(digest, 0, 16);
  md5_hash(input, digest);
  md5_convert_hex_digest(digest, hex_digest);
}

static void official_md5_digest(const char *input, md5_hex_digest hex_digest) {
  uint8_t result[16];
  memset(result, 0, 16);
  if (md5_official_hash(input, result) < 0) {
    return;
  }
  md5_convert_hex_digest(result, hex_digest);
}

static int8_t md5_official_hash(const char *str, uint8_t result[16]) {
  unsigned char md5_hash[EVP_MAX_MD_SIZE];
  unsigned int md5_len;

  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (mdctx == NULL) {
    return -1;
  }

  if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  if (EVP_DigestUpdate(mdctx, str, strlen(str)) != 1) {
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  if (EVP_DigestFinal_ex(mdctx, md5_hash, &md5_len) != 1) {
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  EVP_MD_CTX_free(mdctx);
  memcpy(result, md5_hash, md5_len);
  return 0;
}

UTEST(md5, empty_string) {
  ASSERT_MD5_HEX_DIGEST("");  // 0 chars, 0 bits
}

UTEST(md5, numeric_string) {
  ASSERT_MD5_HEX_DIGEST("9876543210");  // 10 chars, 80 bits
}

UTEST(md5, special_chars) {
  ASSERT_MD5_HEX_DIGEST("!@#$%^&*()");  // 10 chars, 80 bits
}

UTEST(md5, mixed_case) {
  ASSERT_MD5_HEX_DIGEST("AbCdEfGhIjKlMnOpQrStUvWxYz");  // 26 chars, 208 bits
}

UTEST(md5, alphanumeric) {
  ASSERT_MD5_HEX_DIGEST("abc123XYZ987");  // 12 chars, 96 bits
}

UTEST(md5, repeated_pattern) {
  ASSERT_MD5_HEX_DIGEST(
      "abcdabcdabcdabcdabcdabcdabcdabcd");  // 32 chars, 256 bits
}

UTEST(md5, long_repeated_char) {
  ASSERT_MD5_HEX_DIGEST(
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbb");  // 64 chars, 512 bits
}

UTEST(md5, unicode_string) {
  ASSERT_MD5_HEX_DIGEST(
      "你好世界");  // 4 chars (UTF-8 encoding may vary), approx. 32 bits
}

UTEST(md5, with_128_chars) {
  ASSERT_MD5_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");  // 128
                                                                    // chars,
                                                                    // 1024 bits
}

UTEST(md5, with_192_chars) {
  ASSERT_MD5_HEX_DIGEST(
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");  // 192 chars, 1536
                                                            // bits
}

UTEST(md5, padding_edge_case_448_bits) {
  ASSERT_MD5_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");  // 56 chars,
                                                                    // 448 bits
}

UTEST(md5, padding_edge_case_440_bits) {
  ASSERT_MD5_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");  // 55 chars,
                                                                   // 440 bits
}

UTEST(md5, two_full_blocks) {
  ASSERT_MD5_HEX_DIGEST(
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");  // 128
                                                                    // chars,
                                                                    // 1024 bits
}

UTEST(md5, just_under_512_bits) {
  ASSERT_MD5_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaa");  // 63 chars, 504 bits
}

UTEST(md5, exact_512_bits) {
  ASSERT_MD5_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaa");  // 64 chars, 512 bits
}

UTEST(md5, just_over_512_bits) {
  ASSERT_MD5_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaa");  // 65 chars, 520 bits
}
