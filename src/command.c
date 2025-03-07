/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 11:22:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/07 11:22:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "errno.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "flags.h"
#include "md5.h"
#include "sha256.h"
#include "utils.h"

static void print_stdint_hash(const flags_t *flags);
static void print_string_hash(const flags_t *flags);
static void print_files_hash(const flags_t *flags);
static char *hash_using_command(const char *command, const char *string);

void execute_command(const flags_t *flags) {
  print_stdint_hash(flags);
  print_string_hash(flags);
  print_files_hash(flags);
}

static void print_stdint_hash(const flags_t *flags) {
  if (NULL != flags->stdin_input) {
    char *hash = hash_using_command(flags->command, flags->stdin_input);
    if (NULL == hash) {
      fprintf(stderr, "ft_ssl: %s: %s\n", flags->command, strerror(errno));
      return;
    }
    if (flags->options.quiet) {
      printf("%s\n", hash);
    } else if (flags->options.print) {

      char tmp = flags->stdin_input[strlen(flags->stdin_input) - 1];
      if ('\n' == tmp) {
        flags->stdin_input[strlen(flags->stdin_input) - 1] = '\0';
      }
      printf("(\"%s\")= %s\n", flags->stdin_input, hash);
      if ('\n' == tmp) {
        flags->stdin_input[strlen(flags->stdin_input)] = '\n';
      }
    } else {
      printf("(stdin)= %s\n", hash);
    }
    free(hash);
  }
}

static void print_string_hash(const flags_t *flags) {
  if (NULL != flags->string) {
    char *hash = hash_using_command(flags->command, flags->string);
    if (NULL == hash) {
      fprintf(stderr, "ft_ssl: %s: %s\n", flags->command, strerror(errno));
      return;
    }
    if (flags->options.quiet) {
      printf("%s\n", hash);
    } else if (flags->options.reverse) {
      printf("%s \"%s\"\n", hash, flags->string);
    } else {
      printf("%s(\"%s\")= %s\n", flags->command, flags->string, hash);
    }
    free(hash);
  }
}

static void print_files_hash(const flags_t *flags) {
  for (uint32_t i = 0; i < flags->nb_files; i++) {
    int32_t fd = open(flags->files_list[i], O_RDONLY);
    if (fd < 0) {
      fprintf(stderr, "ft_ssl: %s: %s: No such file or directory\n",
              flags->command, flags->files_list[i]);
      continue;
    }
    char *file_content = read_fd_until_end(fd);
    close(fd);
    if (NULL != file_content) {
      char *hash = hash_using_command(flags->command, file_content);
      if (NULL == hash) {
        fprintf(stderr, "ft_ssl: %s: %s: %s\n", flags->command,
                flags->files_list[i], strerror(errno));
        free(file_content);
        continue;
      }
      if (flags->options.quiet) {
        printf("%s\n", hash);
      } else if (flags->options.reverse) {
        printf("%s %s\n", hash, flags->files_list[i]);
      } else {
        printf("%s(%s)= %s\n", flags->command, flags->files_list[i], hash);
      }
      free(hash);
    } else {
      fprintf(stderr, "ft_ssl: %s: %s: %s\n", flags->command,
              flags->files_list[i], strerror(errno));
    }
    free(file_content);
  }
}

static char *hash_using_command(const char *command, const char *string) {
  if (0 == strcmp(command, "MD5")) {
    md5_digest_t md5_digest;
    md5_hex_digest_t md5_hex_digest;
    md5_hash(string, md5_digest);
    md5_convert_hex_digest(md5_digest, md5_hex_digest);
    return strdup(md5_hex_digest);
  } else if (0 == strcmp(command, "SHA256")) {
    sha256_digest_t sha256_digest;
    sha256_hex_digest_t sha256_hex_digest;
    sha256_hash(string, sha256_digest);
    sha256_convert_hex_digest(sha256_digest, sha256_hex_digest);
    return strdup(sha256_hex_digest);
  }
  return NULL;
}
