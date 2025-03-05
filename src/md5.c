/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   md5.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 13:21:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/02/18 13:21:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "md5.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "padding.h"

typedef struct {
  uint32_t a;
  uint32_t b;
  uint32_t c;
  uint32_t d;
} md5_context;

// shift amounts per round
const uint32_t S[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,   //    0..15
    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,   //    16..31
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,   //    32..47
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};  //   48..63

// K[i] = floor(2^32 × abs(sin(i + 1))) precomputed table
const uint32_t K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,   //  0..3
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,   //  4..7
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,   //  8..11
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,   //  12..15
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,   //  16..19
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,   //  20..23
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,   //  24..27
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,   //  28..31
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,   //  32..35
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,   //  36..39
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,   //  40..43
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,   //  44..47
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,   //  48..51
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,   //  52..55
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,   //  56..59
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};  // 60..63

static inline void init_context(md5_context *context);
static uint8_t *compute_block(block_512 block, md5_digest digest,
                              md5_context *context);
static inline uint32_t leftrotate(uint32_t x, uint32_t offset);
static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z);
static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z);
static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z);
static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z);

/**
 * Compute the md5 hash of a message and store it in the digest
 */
void md5_hash(const char *message, md5_digest digest) {
  md5_context context;
  block_512 block;
  uint64_t size = strlen(message);
  uint64_t i = 0;

  init_context(&context);

  // Process the message by blocks of 64 bytes
  while (size - i >= BLOCK_512_SIZE_IN_BYTES) {
    memcpy(block, message + i, BLOCK_512_SIZE_IN_BYTES);
    compute_block(block, digest, &context);
    i += BLOCK_512_SIZE_IN_BYTES;
  }
  uint8_t result = padding_block_512(message + i, size * 8, block);
  compute_block(block, digest, &context);
  // If the message is longer than 55 bytes, we need to add a last block
  if (result > 0) {
    padding_last_block_512(size * 8, block);
    compute_block(block, digest, &context);
  }
}

/**
 * Convert a md5 digest to a hexadecimal string
 */
void md5_convert_hex_digest(md5_digest digest, md5_hex_digest hex_digest) {
  for (int i = 0; i < MD5_DIGEST_SIZE; i++) {
    snprintf(hex_digest + i * 2, 3, "%02x", digest[i]);
  }
  hex_digest[MD5_HEX_DIGEST_SIZE - 1] = '\0';
}

/**
 * Initialize the md5 context with the default values
 */
static inline void init_context(md5_context *context) {
  context->a = 0x67452301;
  context->b = 0xEFCDAB89;
  context->c = 0x98BADCFE;
  context->d = 0x10325476;
}

/**
 * Compute the md5 hash of a block using context and store it in the digest
 */
static uint8_t *compute_block(block_512 block, md5_digest digest,
                              md5_context *context) {
  uint32_t A = context->a;
  uint32_t B = context->b;
  uint32_t C = context->c;
  uint32_t D = context->d;

  for (uint32_t i = 0; i < 64; i++) {
    uint32_t f, g;
    if (i < 16) {
      f = F(B, C, D);
      g = i;
    } else if (i < 32) {
      f = G(B, C, D);
      g = (5 * i + 1) % 16;
    } else if (i < 48) {
      f = H(B, C, D);
      g = (3 * i + 5) % 16;
    } else {
      f = I(B, C, D);
      g = (7 * i) % 16;
    }
    f = f + A + K[i] + ((uint32_t *)block)[g];
    A = D;
    D = C;
    C = B;
    B = B + leftrotate(f, S[i]);
  }

  context->a += A;
  context->b += B;
  context->c += C;
  context->d += D;

  memcpy(digest, &context->a, sizeof(uint32_t));
  memcpy(digest + 4, &context->b, sizeof(uint32_t));
  memcpy(digest + 8, &context->c, sizeof(uint32_t));
  memcpy(digest + 12, &context->d, sizeof(uint32_t));
  return digest;
}

static inline uint32_t leftrotate(uint32_t x, uint32_t offset) {
  return (x << offset) | (x >> (32 - offset));
}

static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) | (~x & z);
}

static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
  return (x & z) | (y & ~z);
}

static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
  return x ^ y ^ z;
}

static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
  return y ^ (x | ~z);
}
