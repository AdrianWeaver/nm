NAME	= ft_nm

CC		=	gcc
INC		=	-I ./includes -I libft/includes

SRCS	=	main.c\
			utils.c\
			symbols.c

OBJS	=	$(addprefix $(OBJS_PATH),$(SRCS:.c=.o))
DEPS	= $(OBJS:.o=.d)

OBJSPATH	= ./objs/

CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -MMD -c


LIBFT	=   libft/libft.a

all:	$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJS_PATH)%.o:	%.c
			mkdir -p $(OBJSPATH)
			$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(INC)


libft:		$(LIBFT)

$(LIBFT):
			$(MAKE) -C libft/

clean:
			rm -rf $(OBJSPATH)

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
