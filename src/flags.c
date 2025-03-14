/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 22:12:00 by tdameros          #+#    #+#             */
/*   Updated: 2024/08/27 22:12:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "flags.h"
#include "utils.h"

#define ERROR_MSG_TRY_HELP "Try 'ft_ssl --help' for more information."

static void print_help(const char *program_name);

int8_t parse_flags(int argc, char *argv[], flags_t *flags) {
  int opt;
  int option_index = 0;

  struct option long_options[] = {
      {"help", no_argument, 0, '?'},         {"print", no_argument, 0, 'p'},
      {"quiet", no_argument, 0, 'q'},        {"reverse", no_argument, 0, 'r'},
      {"string", required_argument, 0, 's'}, {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "?pqrs:", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case '?':
      print_help(argv[0]);
      return 0;
    case 'p':
      flags->options.print = true;
      break;
    case 'q':
      flags->options.quiet = true;
      break;
    case 'r':
      flags->options.reverse = true;
      break;
    case 's':
      if (flags->options.string) {
        break;
      }
      flags->string = optarg;
      flags->options.string = true;
      break;
    default:
      fprintf(stderr, ERROR_MSG_TRY_HELP "\n");
      return -1;
    }
  }
  if (optind < argc) {
    flags->command = str_to_upper(argv[optind]);
    if (!is_valid_command(flags->command)) {
      fprintf(stderr, "ft_ssl: Error: '%s' is an invalid command.\n",
              flags->command);
      fprintf(stderr, ERROR_MSG_TRY_HELP "\n");
      return -1;
    }
    optind++;
    flags->nb_files += argc - optind;
    flags->files_list = malloc(sizeof(char *) * flags->nb_files);
    if (NULL == flags->files_list) {
      fprintf(stderr, "ft_ssl: %s\n", strerror(errno));
      return -1;
    }
    for (uint32_t i = 0; i < flags->nb_files; i++) {
      flags->files_list[i] = argv[optind + i];
    }
    return 1;
  } else {
    fprintf(stderr, "ft_ssl: missing command\n");
    fprintf(stderr, ERROR_MSG_TRY_HELP "\n");
    return -1;
  }
}

static void print_help(const char *program_name) {
  printf("Usage: %s [command] [options] [files]\n", program_name);
  printf("Options:\n");
  printf("  -p, --print\t\tEcho STDIN to STDOUT and append the checksum to "
         "STDOUT\n");
  printf("  -q, --quiet\t\tQuiet mode\n");
  printf("  -r, --reverse\t\tReverse the output format\n");
  printf("  -s, --string\t\tHash the string\n");
  printf("  -?, --help\t\tDisplay this help\n");
  printf("\nCommands:\n");
  printf("  md5\t\t\tCompute the MD5 hash\n");
  printf("  sha256\t\tCompute the SHA256 hash\n");
}

void free_flags(flags_t *flags) {
  free(flags->stdin_input);
  free(flags->files_list);
}
