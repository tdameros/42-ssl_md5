#*******************************  VARIABLES  **********************************#

NAME			=	libftssl.a
SHORT_NAME		=	ftssl
TEST_NAME		=	tests_bin

# --------------- FILES --------------- #

LIST_SRC		=	\
					md5.c		\
					padding.c	\
					sha256.c


LIST_TEST_SRC	=	\
					main.c		\
					md5.c		\
					sha256.c

# ------------ DIRECTORIES ------------ #

DIR_BUILD		=	.build/
DIR_SRC 		=	src/
DIR_INCLUDE		=	include/
DIR_TEST		=	test/
DIR_LIB			=	lib/

# ------------- SHORTCUTS ------------- #

OBJ				=	$(patsubst %.c, $(DIR_BUILD)%.o, $(SRC))
DEP				=	$(patsubst %.c, $(DIR_BUILD)%.d, $(SRC))
SRC				=	$(addprefix $(DIR_SRC), $(LIST_SRC))
TEST_SRC		=	$(addprefix $(DIR_TEST), $(LIST_TEST_SRC))
TEST_DEP		=	$(patsubst %.c, $(DIR_BUILD)%.d, $(TEST_SRC))
TEST_OBJ		=	$(patsubst %.c, $(DIR_BUILD)%.o, $(TEST_SRC))

# ------------ COMPILATION ------------ #

CFLAGS			=	-Wall -Wextra -Werror -g3

DEP_FLAGS		=	-MMD -MP

ARFLAGS			=	rcs

# -------------  COMMANDS ------------- #

RM				=	rm -rf
MKDIR			=	mkdir -p

#***********************************  RULES  **********************************#


.PHONY: all
all:			$(NAME)

.PHONY: tests
tests: $(TEST_NAME)
				valgrind ./$(TEST_NAME)

$(TEST_NAME):	$(TEST_OBJ) $(NAME)
				$(CC) $(CFLAGS) $(TEST_OBJ) -L. -l$(SHORT_NAME) -lcrypto -o $(TEST_NAME)

# ---------- VARIABLES RULES ---------- #

$(NAME):		$(OBJ)
				$(AR) $(ARFLAGS) $(NAME) $(OBJ)

# ---------- COMPILED RULES ----------- #

-include $(DEP)
$(DIR_BUILD)%.o: %.c
				mkdir -p $(shell dirname $@)
				$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@ -I $(DIR_INCLUDE)

-include $(TEST_DEP)
$(DIR_BUILD)%.o: %.c
				mkdir -p $(shell dirname $@)
				$(CC) $(CFLAGS) $(DEP_FLAGS) -c $< -o $@ -I $(DIR_INCLUDE) -I $(DIR_LIB) -lcrypto

.PHONY: clean
clean:
				$(RM) $(DIR_BUILD)

.PHONY: fclean
fclean: clean
				$(RM) $(NAME)
				$(RM) $(TEST_NAME)

.PHONY: re
re:				fclean
				$(MAKE) all

.PHONY: check-format
check-format:
				clang-format -style=file $(TEST_SRC) $(SRC) -n --Werror

.PHONY: format
format:
				clang-format -style=file $(TEST_SRC) $(SRC) -i