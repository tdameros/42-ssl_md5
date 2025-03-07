/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 15:08:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/06 15:08:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"
#include "flags.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  flags_t flags = {0};

  int8_t return_code = parse_flags(argc, argv, &flags);
  if (return_code < 0) {
    return 1;
  } else if (return_code == 0) {
    return 0;
  }

  if (flags.options.print || (!flags.options.string && 0 == flags.nb_files)) {
    flags.stdin_input = read_fd_until_end(STDIN_FILENO);
  }
  execute_command(&flags);
  free_flags(&flags);
}
