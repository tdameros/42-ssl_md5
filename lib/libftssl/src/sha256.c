/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha256.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 15:06:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/05 15:06:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "sha256.h"

#include <stdio.h>
#include <string.h>

#include "padding.h"

typedef struct {
  uint32_t a;
  uint32_t b;
  uint32_t c;
  uint32_t d;
  uint32_t e;
  uint32_t f;
  uint32_t g;
  uint32_t h;
} sha256_context;

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

static inline void init_context(sha256_context *context);
static uint8_t *compute_block(block_512 block, sha256_digest digest,
                              sha256_context *context);

static inline uint32_t swap_endian_32(uint32_t x);
static inline uint64_t swap_endian_64(uint64_t x);
static inline uint32_t right_rotate(uint32_t x, uint32_t n);

static inline uint32_t Ch(uint32_t e, uint32_t f, uint32_t g);
static inline uint32_t Maj(uint32_t a, uint32_t b, uint32_t c);
static inline uint32_t Sigma0(uint32_t a);
static inline uint32_t Sigma1(uint32_t e);

/**
 * Compute the SHA256 hash of a message and store it in the digest
 */
void sha256_hash(const char *message, sha256_digest digest) {
  sha256_context context;
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
  uint8_t result =
      padding_block_512(message + i, swap_endian_64(size * 8), block);
  compute_block(block, digest, &context);
  // If the message is longer than 55 bytes, we need to add a last block
  if (result > 0) {
    padding_last_block_512(swap_endian_64(size * 8), block);
    compute_block(block, digest, &context);
  }
}

/**
 * Convert a SHA256 digest to a hexadecimal string
 */
void sha256_convert_hex_digest(sha256_digest digest,
                               sha256_hex_digest hex_digest) {
  for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
    snprintf(hex_digest + i * 2, 3, "%02x", digest[i]);
  }
  hex_digest[SHA256_HEX_DIGEST_SIZE - 1] = '\0';
}

/**
 * Initialize the SHA256 context with the default values
 */
static inline void init_context(sha256_context *context) {
  context->a = 0x6a09e667;
  context->b = 0xbb67ae85;
  context->c = 0x3c6ef372;
  context->d = 0xa54ff53a;
  context->e = 0x510e527f;
  context->f = 0x9b05688c;
  context->g = 0x1f83d9ab;
  context->h = 0x5be0cd19;
}

/**
 * Compute the SHA256 hash of a block using context and store it in the digest
 */
static uint8_t *compute_block(block_512 block, sha256_digest digest,
                              sha256_context *context) {
  uint32_t A = context->a;
  uint32_t B = context->b;
  uint32_t C = context->c;
  uint32_t D = context->d;
  uint32_t E = context->e;
  uint32_t F = context->f;
  uint32_t G = context->g;
  uint32_t H = context->h;

  uint32_t W[64];

  for (uint32_t i = 0; i < 16; i++) {
    W[i] = swap_endian_32(((uint32_t *)block)[i]);
  }
  for (uint32_t i = 16; i < 64; i++) {
    uint32_t s0 = right_rotate(W[i - 15], 7) ^ right_rotate(W[i - 15], 18) ^
                  (W[i - 15] >> 3);
    uint32_t s1 = right_rotate(W[i - 2], 17) ^ right_rotate(W[i - 2], 19) ^
                  (W[i - 2] >> 10);
    W[i] = W[i - 16] + s0 + W[i - 7] + s1;
  }

  for (uint32_t i = 0; i < 64; i++) {
    uint32_t temp1 = K[i] + W[i] + H + Ch(E, F, G) + Sigma1(E);
    uint32_t temp2 = Maj(A, B, C) + Sigma0(A);

    H = G;
    G = F;
    F = E;
    E = D + temp1;
    D = C;
    C = B;
    B = A;
    A = temp1 + temp2;
  }

  context->a += A;
  context->b += B;
  context->c += C;
  context->d += D;
  context->e += E;
  context->f += F;
  context->g += G;
  context->h += H;

  ((uint32_t *)digest)[0] = swap_endian_32(context->a);
  ((uint32_t *)digest)[1] = swap_endian_32(context->b);
  ((uint32_t *)digest)[2] = swap_endian_32(context->c);
  ((uint32_t *)digest)[3] = swap_endian_32(context->d);
  ((uint32_t *)digest)[4] = swap_endian_32(context->e);
  ((uint32_t *)digest)[5] = swap_endian_32(context->f);
  ((uint32_t *)digest)[6] = swap_endian_32(context->g);
  ((uint32_t *)digest)[7] = swap_endian_32(context->h);
  return digest;
}

static inline uint32_t right_rotate(uint32_t x, uint32_t n) {
  return (x >> n) | (x << (32 - n));
}

static inline uint32_t swap_endian_32(uint32_t x) {
  return ((x >> 24) & 0xff) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) |
         ((x << 24) & 0xff000000);
}

static inline uint64_t swap_endian_64(uint64_t x) {
  return (((x >> 56) & 0x00000000000000FF) | ((x >> 40) & 0x000000000000FF00) |
          ((x >> 24) & 0x0000000000FF0000) | ((x >> 8) & 0x00000000FF000000) |
          ((x << 8) & 0x000000FF00000000) | ((x << 24) & 0x0000FF0000000000) |
          ((x << 40) & 0x00FF000000000000) | ((x << 56) & 0xFF00000000000000));
}

static inline uint32_t Ch(uint32_t e, uint32_t f, uint32_t g) {
  return (e & f) ^ (~e & g);
}

static inline uint32_t Maj(uint32_t a, uint32_t b, uint32_t c) {
  return (a & b) ^ (a & c) ^ (b & c);
}

static inline uint32_t Sigma0(uint32_t a) {
  return right_rotate(a, 2) ^ right_rotate(a, 13) ^ right_rotate(a, 22);
}

static inline uint32_t Sigma1(uint32_t e) {
  return right_rotate(e, 6) ^ right_rotate(e, 11) ^ right_rotate(e, 25);
}
