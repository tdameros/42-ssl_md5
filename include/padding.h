/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   padding.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 13:58:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/05 13:58:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PADDING_H
# define PADDING_H

#define BLOCK_512_SIZE_IN_BYTES 64
#define BLOCK_512_PADDING_META_DATA_SIZE 9

#include <stdint.h>

typedef uint8_t block_512[BLOCK_512_SIZE_IN_BYTES];

uint8_t padding_block_512(const char *message, uint64_t bytes_len, block_512 block);
void padding_last_block_512(uint64_t bytes_len, block_512 block);

#endif