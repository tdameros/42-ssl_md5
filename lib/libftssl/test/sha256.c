/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha256.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:06:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/05 16:06:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "sha256.h"

#include <openssl/evp.h>
#include <stdio.h>

#include "utest/utest.h"

#define ASSERT_SHA256_HEX_DIGEST(input)             \
  do {                                              \
    sha256_hex_digest_t ftssl_digest;               \
    sha256_hex_digest_t official_digest;            \
    ftssl_sha256_digest(input, ftssl_digest);       \
    official_sha256_digest(input, official_digest); \
    ASSERT_STREQ(official_digest, ftssl_digest);    \
  } while (0)

static int8_t sha256_official_hash(const char *str, uint8_t result[32]);

static void ftssl_sha256_digest(const char *input,
                                sha256_hex_digest_t hex_digest) {
  sha256_digest_t digest;
  memset(digest, 0, 32);
  sha256_hash(input, digest);
  sha256_convert_hex_digest(digest, hex_digest);
}

static void official_sha256_digest(const char *input,
                                   sha256_hex_digest_t hex_digest) {
  uint8_t result[32];
  memset(result, 0, 32);
  if (sha256_official_hash(input, result) < 0) {
    return;
  }
  sha256_convert_hex_digest(result, hex_digest);
}

static int8_t sha256_official_hash(const char *str, uint8_t result[32]) {
  unsigned char sha256_hash[EVP_MAX_MD_SIZE];
  unsigned int sha256_len;

  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (mdctx == NULL) {
    return -1;
  }

  if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  if (EVP_DigestUpdate(mdctx, str, strlen(str)) != 1) {
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  if (EVP_DigestFinal_ex(mdctx, sha256_hash, &sha256_len) != 1) {
    EVP_MD_CTX_free(mdctx);
    return -1;
  }

  EVP_MD_CTX_free(mdctx);
  memcpy(result, sha256_hash, sha256_len);
  return 0;
}

UTEST(sha256, empty_string) {
  ASSERT_SHA256_HEX_DIGEST("");  // 0 chars, 0 bits
}

UTEST(sha256, numeric_string) {
  ASSERT_SHA256_HEX_DIGEST("1");  // 10 chars, 80 bits
}

UTEST(sha256, special_chars) {
  ASSERT_SHA256_HEX_DIGEST("!@#$%^&*()");  // 10 chars, 80 bits
}

UTEST(sha256, mixed_case) {
  ASSERT_SHA256_HEX_DIGEST("AbCdEfGhIjKlMnOpQrStUvWxYz");  // 26 chars, 208 bits
}

UTEST(sha256, alphanumeric) {
  ASSERT_SHA256_HEX_DIGEST("abc123XYZ987");  // 12 chars, 96 bits
}

UTEST(sha256, repeated_pattern) {
  ASSERT_SHA256_HEX_DIGEST(
      "abcdabcdabcdabcdabcdabcdabcdabcd");  // 32 chars, 256 bits
}

UTEST(sha256, long_repeated_char) {
  ASSERT_SHA256_HEX_DIGEST(
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbb");  // 64 chars, 512 bits
}

UTEST(sha256, unicode_string) {
  ASSERT_SHA256_HEX_DIGEST(
      "你好世界");  // 4 chars (UTF-8 encoding may vary), approx. 32 bits
}

UTEST(sha256, with_128_chars) {
  ASSERT_SHA256_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");  // 128
                                                                    // chars,
                                                                    // 1024 bits
}

UTEST(sha256, with_192_chars) {
  ASSERT_SHA256_HEX_DIGEST(
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");  // 192 chars, 1536
                                                            // bits
}

UTEST(sha256, padding_edge_case_448_bits) {
  ASSERT_SHA256_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");  // 56 chars,
                                                                    // 448 bits
}

UTEST(sha256, padding_edge_case_440_bits) {
  ASSERT_SHA256_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");  // 55 chars,
                                                                   // 440 bits
}

UTEST(sha256, two_full_blocks) {
  ASSERT_SHA256_HEX_DIGEST(
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");  // 128
                                                                    // chars,
                                                                    // 1024 bits
}

UTEST(sha256, just_under_512_bits) {
  ASSERT_SHA256_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaa");  // 63 chars, 504 bits
}

UTEST(sha256, exact_512_bits) {
  ASSERT_SHA256_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaa");  // 64 chars, 512 bits
}

UTEST(sha256, just_over_512_bits) {
  ASSERT_SHA256_HEX_DIGEST(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaa");  // 65 chars, 520 bits
}
