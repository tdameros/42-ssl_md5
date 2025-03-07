/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   padding.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 13:59:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/05 13:59:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "padding.h"

#include <stdlib.h>
#include <string.h>

/**
 * Add md5 padding to the block with 0x80 and the length of the message in bits
 * @return 1 if a last block with the length of the message is needed, 0
 * otherwise
 */
uint8_t padding_block_512(const char *message, uint64_t bytes_len,
                          block_512_t block) {
  uint32_t message_len = strlen(message);
  memcpy(block, message, message_len);
  if (message_len <=
      BLOCK_512_SIZE_IN_BYTES - BLOCK_512_PADDING_META_DATA_SIZE) {
    block[message_len] = 0x80;
    memset(block + message_len + 1, 0,
           BLOCK_512_SIZE_IN_BYTES - message_len -
               BLOCK_512_PADDING_META_DATA_SIZE);
    memcpy(block + 56, &bytes_len, sizeof(uint64_t));
  } else {
    if (message_len < BLOCK_512_SIZE_IN_BYTES) {
      block[message_len] = 0x80;
      memset(block + message_len + 1, 0,
             BLOCK_512_SIZE_IN_BYTES - message_len - 1);
    }
    return 1;
  }
  return 0;
}

/**
 * Add md5 padding to the block with length of the message in bits at the end
 */
void padding_last_block_512(uint64_t bytes_len, block_512_t block) {
  memset(block, 0, 64);
  memcpy(block + 56, &bytes_len, sizeof(uint64_t));
}

// Debug functions

/*
#include <stdio.h>

static void print_block_512(block_512_t block);
static void print_uint32t_to_binary_bytes(uint32_t n);
static void print_byte_in_binary(uint8_t num);

static void print_block_512(block_512_t block) {
  for (int i = 0; i < 16; i++) {
    print_uint32t_to_binary_bytes(((uint32_t *)block)[i]);
    if (i % 2 == 1) {
      printf("\n");
    }
  }
  printf("\n");
}

static void print_uint32t_to_binary_bytes(uint32_t n) {
  print_byte_in_binary(n & 0xFF);
  printf(" ");
  print_byte_in_binary((n >> 8) & 0xFF);
  printf(" ");
  print_byte_in_binary((n >> 16) & 0xFF);
  printf(" ");
  print_byte_in_binary((n >> 24) & 0xFF);
  printf(" ");
}

static void print_byte_in_binary(uint8_t num) {
  for (int i = 7; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
  }
}
*/
