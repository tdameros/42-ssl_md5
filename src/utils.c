/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 11:22:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/07 11:22:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *read_fd_until_end(const int32_t fd) {
  char buffer[1024];
  char *input = NULL;
  size_t read_bytes = read(fd, buffer, sizeof(buffer));
  size_t total_bytes = 0;

  while (read_bytes > 0) {
    input = realloc(input, total_bytes + read_bytes + 1);
    if (NULL == input) {
      return NULL;
    }
    memcpy(input + total_bytes, buffer, read_bytes);
    input[total_bytes + read_bytes] = '\0';
    total_bytes += read_bytes;
    read_bytes = read(fd, buffer, sizeof(buffer));
  }
  return input;
}

char *str_to_upper(char *str) {
  for (uint32_t i = 0; i < strlen(str); i++) {
    str[i] = toupper(str[i]);
  }
  return str;
}
