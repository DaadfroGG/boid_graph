NAME            = draw_b

CC              = gcc  -Werror -Wextra -Wall -MMD

LIBS            = -lSDL2  -lSDL2_test -lm   # -fsanitize=address

OBJ             = $(SRCS:.c=.o)

DEP             = $(SRCS:.c=.d)

SRCS            = graph.c \
                        

SRCH_INCLDS     = -Iinclude

RM              = rm -rf

all: $(NAME)

-include $(DEP)

%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(LIBS) $(SRCH_INCLDS) -c $< -o $@
	@echo "  \033[92m[✔] Compiled $<\033[0m"

$(NAME): $(OBJ)
	@echo "Linking objects..."
	@$(CC) $(OBJ) $(LIBS) -o $(NAME)
	@echo "\n\033[92m[✔] Build successful! Executable: $(NAME)\033[0m"

clean:
	@echo "Cleaning up..."
	@$(RM) $(OBJ)
	@$(RM) $(DEP)
	@echo "\033[92m[✔] Cleaned up object files\033[0m"

fclean: clean
	@echo "Cleaning up..."
	@$(RM) $(NAME)
	@echo "\033[92m[✔] Cleaned up executable: $(NAME)\033[0m"

re: fclean $(NAME)
