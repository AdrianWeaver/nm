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

test:		$(NAME)
			./$(NAME) modified_elf
			nm modified_elf
			#./$(NAME) $(NAME)

vtest:		$(NAME)
			#valgrind --leak-check=full ./$(NAME) $(NAME)
			valgrind --leak-check=full ./$(NAME) modified_elf

create:		
			$(CC) $(CFLAGS) elfcreator.c -o ELFcreator
			@./ELFcreator
			@$(MAKE) --no-print-directory nm

nm:			
			@echo "nm modified_elf"
			-@nm modified_elf || echo "nm returned $$?"
			
.PHONY: all, clean, fclean, re, test, vtest, create, libft

-include $(DEPS)
