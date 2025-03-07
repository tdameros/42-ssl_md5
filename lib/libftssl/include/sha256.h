/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sha256.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 15:06:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/05 15:06:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHA256_H
# define SHA256_H

#define SHA256_DIGEST_SIZE 32
#define SHA256_HEX_DIGEST_SIZE (SHA256_DIGEST_SIZE * 2 + 1)

#include <stdint.h>

typedef uint8_t sha256_digest[SHA256_DIGEST_SIZE];
typedef char sha256_hex_digest[SHA256_HEX_DIGEST_SIZE];

void sha256_hash(const char *message, sha256_digest digest);
void sha256_convert_hex_digest(sha256_digest digest, sha256_hex_digest hex_digest);

#endif