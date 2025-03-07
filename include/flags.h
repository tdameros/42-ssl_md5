/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flags.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 08:10:00 by tdameros          #+#    #+#             */
/*   Updated: 2025/03/07 08:10:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef FLAGS_H
# define FLAGS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char *command;
    char *string;
    char *stdin_input;
    char **files_list;
    uint32_t nb_files;
    struct {
        uint8_t help: 1;
        uint8_t print: 1;
        uint8_t quiet: 1;
        uint8_t reverse: 1;
        uint8_t string: 1;
        uint8_t _unused: 3;
    } options;
} flags_t;

int8_t parse_flags(int argc, char *argv[], flags_t *flags);
void free_flags(flags_t *flags);

#endif