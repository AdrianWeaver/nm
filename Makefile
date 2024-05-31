NAME	= ft_nm

CC		=	gcc
INC		=	-I ./includes -I libft/includes

SRCS	=	test.c
OBJS	=	$(addprefix $(OBJS_PATH),$(SRCS:.c=.o))
DEPS	= $(OBJS:.o=.d)

OBJSPATH	= ./objs/

CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -MMD -c


LIBFT	=   libft/libft.a

all:	$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(OBJS):	$(SRCS)
			mkdir -p $(OBJSPATH)
			$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(INC)


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
			valgrind --check-leaks=full ./$(NAME)

.PHONY: all, clean, fclean, re, test, vtest, libft

