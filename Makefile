NAME	= ft_nm

CC		=	gcc
INC		=	-I ./includes -I libft/includes


SRCS_PATH	= ./sources/
SRCS	=	main.c\
			utils.c\
			ehdr.c\
			phdr.c\
			shdr.c\
			options.c\
			symbols.c\
			types.c

OBJS_PATH	= ./objects/
OBJS	=	$(addprefix $(OBJS_PATH), $(SRCS:.c=.o))
DEPS	= 	$(OBJS:.o=.d)


CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -MMD -c -g3


LIBFT		=   libft/libft.a

all:	$(NAME)

$(NAME):	$(OBJS)	libft
			$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $@

$(OBJS_PATH)%.o:	$(SRCS_PATH)%.c
			mkdir -p $(OBJS_PATH)
			$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(INC)

libft:
			@$(MAKE) --no-print-directory -C libft/

clean:
			rm -rf $(OBJS_PATH)

fclean:		clean
			rm -rf $(NAME)

re:			fclean
			$(MAKE) all

.PHONY: all, clean, fclean, re, libft

-include $(DEPS)
