#*******************************  VARIABLES  **********************************#

NAME			=	ft_ssl

# --------------- FILES --------------- #

LIST_SRC		=	\
					command.c	\
					flags.c		\
					main.c		\
					utils.c

# ------------ DIRECTORIES ------------ #

DIR_BUILD		=	.build/
DIR_SRC 		=	src/
DIR_INCLUDE		=	include/
DIR_LIB			=	lib/

DIR_LIBFTSSL	=	$(DIR_LIB)libftssl/
LIBFTSSL		=	$(DIR_LIBFTSSL)libftssl.a

# ------------- SHORTCUTS ------------- #

OBJ				=	$(patsubst %.c, $(DIR_BUILD)%.o, $(SRC))
DEP				=	$(patsubst %.c, $(DIR_BUILD)%.d, $(SRC))
SRC				=	$(addprefix $(DIR_SRC), $(LIST_SRC))

# ------------ COMPILATION ------------ #

CFLAGS			=	-Wall -Wextra -Werror

DEP_FLAGS		=	-MMD -MP

# -------------  COMMANDS ------------- #

RM				=	rm -rf
MKDIR			=	mkdir -p

#***********************************  RULES  **********************************#


.PHONY: all
all:			$(NAME)

# ---------- VARIABLES RULES ---------- #

$(NAME):		$(LIBFTSSL) $(OBJ)
				$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -L $(DIR_LIBFTSSL) -lftssl

$(LIBFTSSL): FORCE
				$(MAKE) -C $(DIR_LIBFTSSL)

# ---------- COMPILED RULES ----------- #

-include $(DEP)
$(DIR_BUILD)%.o: %.c
				mkdir -p $(shell dirname $@)
				$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@ -I $(DIR_INCLUDE) -I $(DIR_LIBFTSSL)include

.PHONY: clean
clean:
				$(MAKE) -C $(DIR_LIBFTSSL) clean
				$(RM) $(DIR_BUILD)

.PHONY: fclean
fclean: clean
				$(MAKE) -C $(DIR_LIBFTSSL) fclean
				$(RM) $(NAME)

.PHONY: re
re:				fclean
				$(MAKE) all

.PHONY: check-format
check-format:
				clang-format -style=file $(SRC) -n --Werror

.PHONY: format
format:
				clang-format -style=file $(SRC) -i

.PHONY: FORCE
FORCE:

.PHONY: tests
tests:
				$(MAKE) -C $(DIR_LIBFTSSL) tests
