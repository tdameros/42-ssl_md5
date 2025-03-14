/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   md5.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 13:35:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/02/18 13:35:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef MD5_H
# define MD5_H

#include <stdint.h>

#define MD5_DIGEST_SIZE 16
#define MD5_HEX_DIGEST_SIZE (MD5_DIGEST_SIZE * 2 + 1)

typedef uint8_t md5_digest_t[MD5_DIGEST_SIZE];
typedef char md5_hex_digest_t[MD5_HEX_DIGEST_SIZE];

void md5_hash(const char *message, md5_digest_t digest);
void md5_convert_hex_digest(md5_digest_t digest, md5_hex_digest_t hex_digest);
#endif