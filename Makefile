NAME	= ft_nm

CC		=	gcc
INC		=	-I ./includes -I libft/includes


SRCS_PATH	= ./sources/
SRCS	=	main.c\
			utils.c\
			symbols.c

OBJS_PATH	= ./objects/
OBJS	=	$(addprefix $(OBJS_PATH), $(SRCS:.c=.o))
DEPS	= 	$(OBJS:.o=.d)


CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -MMD -c


LIBFT	=   libft/libft.a

all:	$(NAME)

$(NAME):	$(OBJS) $(LIBFT)
			@echo "called linker"
			$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $@

$(OBJS_PATH)%.o:	$(SRCS_PATH)%.c
			mkdir -p $(OBJS_PATH)
			@echo "called this rule"
			$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(INC)


libft:		$(LIBFT)

$(LIBFT):
			$(MAKE) -C libft/

clean:
			rm -rf $(OBJS_PATH)

fclean:		clean
			rm -rf $(NAME)

re:			fclean
			$(MAKE) all

test:		$(NAME)
			./$(NAME) $(NAME)

vtest:		$(NAME)
			valgrind --leak-check=full ./$(NAME) $(NAME)

create:		
			$(CC) $(CFLAGS) elfcreator.c -o ELFcreator
			@./ELFcreator
			@$(MAKE) --no-print-directory nm

nm:			
			-nm modified_elf
			
.PHONY: all, clean, fclean, re, test, vtest, create, libft

-include $(DEPS)
