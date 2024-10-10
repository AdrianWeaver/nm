#include <elf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ft_nm.h"

//sorting functions for symbol binary tree
static void	_print_symbol(void *node);
static void	_print_undefined_symbol(void *content);
static void	_print_global_symbol(void *content);
static void	_print_symbol32(void *node);
static void	_print_undefined_symbol32(void *content);
static void	_print_global_symbol32(void *content);

/*
 * @brief handler to check the correct shdr depending on class and endianness
 *
 * @param file t_mem storing mapped file and infos
 * @return non-zero in case of error
*/
int	get_symbols(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2LSB)
	{
		if (_get_symbols_64lsb(file, option_field, symbol_list) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2LSB)
	{
		if (_get_symbols_32lsb(file, option_field, symbol_list) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2MSB)
	{
		if (_get_symbols_64msb(file, option_field, symbol_list) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2MSB)
	{
		if (_get_symbols_32msb(file, option_field, symbol_list) != ERROR)
			return (0);
	}
	return (ERROR);
}

/*
 *	@brief this function gets symbols and prints them
 *
 *	@param file a t_mem* storing the file and infos
 *	@param option_field a bitfield storing options given by user
 *	@param symbol_list the address of the symbol list where the symbols are to be stored
 *
 *	@return non-zero in case of errors
*/
int	_get_symbols_64lsb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file->raw;
	Elf64_Shdr *shdr_table = (Elf64_Shdr *)&file->raw[ehdr->e_shoff];
	int (*sort_function)(void *, void*) = _symbol_sort_order;
	void (*iteration_function)(t_bst **, void (*f)(void*)) = ft_bstiter;

	//option -p does not sort
	if (option_field & OPTION_P)
		sort_function = _symbol_no_sort;
	for (uint16_t i = 0; i < ehdr->e_shnum; i++) //iterate on sections
	{
		Elf64_Shdr *shdr = &shdr_table[i];
		//real nm does not look at SHT_DYNSYM
		if (shdr->sh_type != SHT_SYMTAB && shdr->sh_type != SHT_SYMTAB_SHNDX) //if not a symbol section go next
			continue;
		Elf64_Sym *symbol_table = (Elf64_Sym *) &file->raw[shdr->sh_offset];
		if (shdr->sh_entsize == 0)
			continue;
		for (uint32_t j = 0; j < shdr->sh_size / shdr->sh_entsize; j++) //iterate on symbols
		{
			Elf64_Sym *symbol = &symbol_table[j];
			char *symbol_string_table = (char *)&file->raw[shdr_table[shdr->sh_link].sh_offset];
			if (!(option_field & OPTION_A))
			{
				if (symbol->st_shndx == SHN_ABS)
					continue;
			}
			t_symbol *tmp_symbol = malloc(sizeof(*tmp_symbol) * 1);
			if (!tmp_symbol)
			{
				ft_bstclear(symbol_list, free);
				fprintf(stderr, "nm: error: memory allocation failed\n");
				return (ERROR);
			}
			if (shdr_table[shdr->sh_link].sh_type != SHT_STRTAB)
			{
				fprintf(stderr, "nm: %s: attempt to load strings from a non-string section (number %d)\n", file->name, shdr->sh_link);
				tmp_symbol->name = "(null)";
			}
			else if (symbol->st_name == 0)
			{
				tmp_symbol->name = "";
				if (symbol->st_size == 0 && symbol->st_shndx != SHN_ABS)
				{
					free(tmp_symbol);
					continue;
				}
			}
			else
				tmp_symbol->name = &symbol_string_table[symbol->st_name];
			tmp_symbol->value = symbol->st_value;
			tmp_symbol->type = 'a';
			if (symbol->st_shndx <= ehdr->e_shnum)
				tmp_symbol->type = get_symbol_type_64lsb(file, symbol, tmp_symbol);
			//if symbol contains '@' search for duplicate without it and remove it.
			char *offset_of_at = ft_strchr(tmp_symbol->name, '@');
			if (offset_of_at)
			{
				*offset_of_at = '\0';
				//remove duplicate without @
				ft_bstremove(symbol_list, tmp_symbol, sort_function, free);
				*offset_of_at = '@';
			}
			t_bst *tmp_node = ft_bstnew(tmp_symbol);
			if (!ft_bstinsert(symbol_list, tmp_node, sort_function))
			{
				free(tmp_node);
				free(tmp_symbol);
			}
		}
	}
	if ((option_field & (OPTION_R | OPTION_P)) == OPTION_R) //reverse print
		iteration_function = ft_bstriter;
	//printing symbols
	if (*symbol_list == NULL && !(option_field & OPTION_MULTIPLE_FILES))
		fprintf(stderr, "nm: %s: no symbols\n", file->name);
	if (option_field & OPTION_MULTIPLE_FILES)
		printf("\n%s:\n", file->name);
	if (option_field & OPTION_U)
		(*iteration_function)(symbol_list, _print_undefined_symbol);
	else if (option_field & OPTION_G)
		(*iteration_function)(symbol_list, _print_global_symbol);
	else
		(*iteration_function)(symbol_list, _print_symbol);
	ft_bstclear(symbol_list, free);
	return (0);
}

int	_get_symbols_32lsb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)file->raw;
	Elf32_Shdr *shdr_table = (Elf32_Shdr *)&file->raw[ehdr->e_shoff];
	int (*sort_function)(void *, void*) = _symbol_sort_order;
	void (*iteration_function)(t_bst **, void (*f)(void*)) = ft_bstiter;

	//option -p does not sort
	if (option_field & OPTION_P)
		sort_function = _symbol_no_sort;
	for (uint16_t i = 0; i < ehdr->e_shnum; i++) //iterate on sections
	{
		Elf32_Shdr *shdr = &shdr_table[i];
		//real nm does not look at SHT_DYNSYM
		if (shdr->sh_type != SHT_SYMTAB && shdr->sh_type != SHT_SYMTAB_SHNDX) //if not a symbol section go next
			continue;
		Elf32_Sym *symbol_table = (Elf32_Sym *) &file->raw[shdr->sh_offset];
		if (shdr->sh_entsize == 0)
			continue;
		for (uint32_t j = 0; j < shdr->sh_size / shdr->sh_entsize; j++) //iterate on symbols
		{
			Elf32_Sym *symbol = &symbol_table[j];
			char *symbol_string_table = (char *)&file->raw[shdr_table[shdr->sh_link].sh_offset];
			if (!(option_field & OPTION_A))
			{
				if (symbol->st_shndx == SHN_ABS)
					continue;
			}
			t_symbol *tmp_symbol = malloc(sizeof(*tmp_symbol) * 1);
			if (!tmp_symbol)
			{
				ft_bstclear(symbol_list, free);
				fprintf(stderr, "nm: error: memory allocation failed\n");
				return (ERROR);
			}
			if (shdr_table[shdr->sh_link].sh_type != SHT_STRTAB)
			{
				fprintf(stderr, "nm: %s: attempt to load strings from a non-string section (number %d)\n", file->name, shdr->sh_link);
				tmp_symbol->name = "(null)";
			}
			else if (symbol->st_name == 0)
			{
				tmp_symbol->name = "";
				if (symbol->st_size == 0 && symbol->st_shndx != SHN_ABS)
				{
					free(tmp_symbol);
					continue;
				}
			}
			else
				tmp_symbol->name = &symbol_string_table[symbol->st_name];
			tmp_symbol->value = symbol->st_value;
			tmp_symbol->type = 'a';
			if (symbol->st_shndx <= ehdr->e_shnum)
				tmp_symbol->type = get_symbol_type_32lsb(file, symbol, tmp_symbol);
			//if symbol contains '@' search for duplicate without it and remove it.
			char *offset_of_at = ft_strchr(tmp_symbol->name, '@');
			if (offset_of_at)
			{
				*offset_of_at = '\0';
				//remove duplicate without @
				ft_bstremove(symbol_list, tmp_symbol, sort_function, free);
				*offset_of_at = '@';
			}
			t_bst *tmp_node = ft_bstnew(tmp_symbol);
			if (!ft_bstinsert(symbol_list, tmp_node, sort_function))
			{
				free(tmp_node);
				free(tmp_symbol);
			}
		}
	}
	if ((option_field & (OPTION_R | OPTION_P)) == OPTION_R) //reverse print
		iteration_function = ft_bstriter;
	//printing symbols
	if (*symbol_list == NULL && !(option_field & OPTION_MULTIPLE_FILES))
		fprintf(stderr, "nm: %s: no symbols\n", file->name);
	if (option_field & OPTION_MULTIPLE_FILES)
		printf("\n%s:\n", file->name);
	if (option_field & OPTION_U)
		(*iteration_function)(symbol_list, _print_undefined_symbol32);
	else if (option_field & OPTION_G)
		(*iteration_function)(symbol_list, _print_global_symbol32);
	else
		(*iteration_function)(symbol_list, _print_symbol32);
	ft_bstclear(symbol_list, free);
	return (0);
}

int	_get_symbols_64msb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file->raw;
	Elf64_Shdr *shdr_table = (Elf64_Shdr *)&file->raw[rev64(ehdr->e_shoff)];
	int (*sort_function)(void *, void*) = _symbol_sort_order;
	void (*iteration_function)(t_bst **, void (*f)(void*)) = ft_bstiter;

	//option -p does not sort
	if (option_field & OPTION_P)
		sort_function = _symbol_no_sort;
	for (uint16_t i = 0; i < rev16(ehdr->e_shnum); i++) //iterate on sections
	{
		Elf64_Shdr *shdr = &shdr_table[i];
		//real nm does not look at SHT_DYNSYM
		if (rev32(shdr->sh_type) != SHT_SYMTAB && rev32(shdr->sh_type) != SHT_SYMTAB_SHNDX) //if not a symbol section go next
			continue;
		Elf64_Sym *symbol_table = (Elf64_Sym *) &file->raw[rev64(shdr->sh_offset)];
		if (rev64(shdr->sh_entsize) == 0)
			continue;
		for (uint32_t j = 0; j < rev64(shdr->sh_size) / rev64(shdr->sh_entsize); j++) //iterate on symbols
		{
			Elf64_Sym *symbol = &symbol_table[j];
			char *symbol_string_table = (char *)&file->raw[rev64(shdr_table[rev32(shdr->sh_link)].sh_offset)];
			if (!(option_field & OPTION_A))
			{
				if (rev16(symbol->st_shndx) == SHN_ABS)
					continue;
			}
			t_symbol *tmp_symbol = malloc(sizeof(*tmp_symbol) * 1);
			if (!tmp_symbol)
			{
				ft_bstclear(symbol_list, free);
				fprintf(stderr, "nm: error: memory allocation failed\n");
				return (ERROR);
			}
			if (rev32(shdr_table[rev32(shdr->sh_link)].sh_type) != SHT_STRTAB)
			{
				fprintf(stderr, "nm: %s: attempt to load strings from a non-string section (number %d)\n", file->name, rev32(shdr->sh_link));
				tmp_symbol->name = "(null)";
			}
			else if (rev32(symbol->st_name) == 0)
			{
				tmp_symbol->name = "";
				if (rev64(symbol->st_size) == 0 && rev16(symbol->st_shndx) != SHN_ABS)
				{
					free(tmp_symbol);
					continue;
				}
			}
			else
				tmp_symbol->name = &symbol_string_table[rev32(symbol->st_name)];
			tmp_symbol->value = rev64(symbol->st_value);
			tmp_symbol->type = 'a';
			if (rev16(symbol->st_shndx) <= rev16(ehdr->e_shnum))
				tmp_symbol->type = get_symbol_type_64lsb(file, symbol, tmp_symbol);
			//if symbol contains '@' search for duplicate without it and remove it.
			char *offset_of_at = ft_strchr(tmp_symbol->name, '@');
			if (offset_of_at)
			{
				*offset_of_at = '\0';
				//remove duplicate without @
				ft_bstremove(symbol_list, tmp_symbol, sort_function, free);
				*offset_of_at = '@';
			}
			t_bst *tmp_node = ft_bstnew(tmp_symbol);
			if (!ft_bstinsert(symbol_list, tmp_node, sort_function))
			{
				free(tmp_node);
				free(tmp_symbol);
			}
		}
	}
	if ((option_field & (OPTION_R | OPTION_P)) == OPTION_R) //reverse print
		iteration_function = ft_bstriter;
	//printing symbols
	if (*symbol_list == NULL && !(option_field & OPTION_MULTIPLE_FILES))
		fprintf(stderr, "nm: %s: no symbols\n", file->name);
	if (option_field & OPTION_MULTIPLE_FILES)
		printf("\n%s:\n", file->name);
	if (option_field & OPTION_U)
		(*iteration_function)(symbol_list, _print_undefined_symbol);
	else if (option_field & OPTION_G)
		(*iteration_function)(symbol_list, _print_global_symbol);
	else
		(*iteration_function)(symbol_list, _print_symbol);
	ft_bstclear(symbol_list, free);
	return (0);
}

int	_get_symbols_32msb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)file->raw;
	Elf32_Shdr *shdr_table = (Elf32_Shdr *)&file->raw[rev32(ehdr->e_shoff)];
	int (*sort_function)(void *, void*) = _symbol_sort_order;
	void (*iteration_function)(t_bst **, void (*f)(void*)) = ft_bstiter;

	//option -p does not sort
	if (option_field & OPTION_P)
		sort_function = _symbol_no_sort;
	for (uint16_t i = 0; i < rev16(ehdr->e_shnum); i++) //iterate on sections
	{
		Elf32_Shdr *shdr = &shdr_table[i];
		//real nm does not look at SHT_DYNSYM
		if (rev32(shdr->sh_type) != SHT_SYMTAB
			&& rev32(shdr->sh_type) != SHT_SYMTAB_SHNDX) //if not a symbol section go next
			continue;
		Elf32_Sym *symbol_table = (Elf32_Sym *) &file->raw[rev32(shdr->sh_offset)];
		if (rev32(shdr->sh_entsize) == 0)
			continue;
		for (uint32_t j = 0; j < rev32(shdr->sh_size) / rev32(shdr->sh_entsize); j++) //iterate on symbols
		{
			Elf32_Sym *symbol = &symbol_table[j];
			char *symbol_string_table = (char *)&file->raw[rev32(shdr_table[rev32(shdr->sh_link)].sh_offset)];
			if (!(option_field & OPTION_A))
			{
				if (rev16(symbol->st_shndx) == SHN_ABS)
					continue;
			}
			t_symbol *tmp_symbol = malloc(sizeof(*tmp_symbol) * 1);
			if (!tmp_symbol)
			{
				ft_bstclear(symbol_list, free);
				fprintf(stderr, "nm: error: memory allocation failed\n");
				return (ERROR);
			}
			if (rev32(shdr_table[rev32(shdr->sh_link)].sh_type) != SHT_STRTAB)
			{
				fprintf(stderr, "nm: %s: attempt to load strings from a non-string section (number %d)\n", file->name, rev32(shdr->sh_link));
				tmp_symbol->name = "(null)";
			}
			else if (rev32(symbol->st_name) == 0)
			{
				tmp_symbol->name = "";
				if (rev32(symbol->st_size) == 0 && rev16(symbol->st_shndx) != SHN_ABS)
				{
					free(tmp_symbol);
					continue;
				}
			}
			else
				tmp_symbol->name = &symbol_string_table[rev32(symbol->st_name)];
			tmp_symbol->name = &symbol_string_table[symbol->st_name];
			if (rev32(symbol->st_name) == 0)
			{
				tmp_symbol->name = "";
				if (rev32(symbol->st_size) == 0 && rev16(symbol->st_shndx) != SHN_ABS)
				{
					free(tmp_symbol);
					continue;
				}
			}
			tmp_symbol->value = rev32(symbol->st_value);
			tmp_symbol->type = 'a';
			if (symbol->st_shndx <= ehdr->e_shnum)
				tmp_symbol->type = get_symbol_type_32lsb(file, symbol, tmp_symbol);
			//if symbol contains '@' search for duplicate without it and remove it.
			char *offset_of_at = ft_strchr(tmp_symbol->name, '@');
			if (offset_of_at)
			{
				*offset_of_at = '\0';
				//remove duplicate without @
				ft_bstremove(symbol_list, tmp_symbol, sort_function, free);
				*offset_of_at = '@';
			}
			t_bst *tmp_node = ft_bstnew(tmp_symbol);
			if (!ft_bstinsert(symbol_list, tmp_node, sort_function))
			{
				free(tmp_node);
				free(tmp_symbol);
			}
		}
	}
	if ((option_field & (OPTION_R | OPTION_P)) == OPTION_R) //reverse print
		iteration_function = ft_bstriter;
	//printing symbols
	if (*symbol_list == NULL && !(option_field & OPTION_MULTIPLE_FILES))
		fprintf(stderr, "nm: %s: no symbols\n", file->name);
	if (option_field & OPTION_MULTIPLE_FILES)
		printf("\n%s:\n", file->name);
	if (option_field & OPTION_U)
		(*iteration_function)(symbol_list, _print_undefined_symbol32);
	else if (option_field & OPTION_G)
		(*iteration_function)(symbol_list, _print_global_symbol32);
	else
		(*iteration_function)(symbol_list, _print_symbol32);
	ft_bstclear(symbol_list, free);
	return (0);
}

static inline int _case_sensitive(int a)
{ return (a);}

/* 	@brief case insentitive comparison of symbol names skipping leading '_'
 *
 *	@param s1 new node
 *	@param s2 old node
 *	@return 0 if identical non-zero if different
*/
static int	_compare_symbol_names(const char *s1, const char *s2)
{
	int (*case_sensitiveness)(int) = ft_tolower;
	bool	skip_symbols = true;
	size_t	s1_iterator = 0;
	size_t	s2_iterator = 0;
	//special case leading dot
	for (;;)
	{
		//skip non alphanum
		while (skip_symbols && s1[s1_iterator] && ft_strchr("()[]{}*._/-:@", s1[s1_iterator]))
			s1_iterator++;
		while (skip_symbols && s2[s2_iterator] && ft_strchr("()[]{}*._/-:@", s2[s2_iterator]))
			s2_iterator++;
		//if strings are different
		if ((*case_sensitiveness)(s1[s1_iterator]) != (*case_sensitiveness)(s2[s2_iterator]))
			break;
		//reached end of both strings
		if (s1[s1_iterator] == '\0' && s2[s2_iterator] == '\0')
		{
			s1_iterator = 0;
			s2_iterator = 0;
			//if strings equal with case insensitive, restart with case sensitive
			if (case_sensitiveness == ft_tolower)
			{
				case_sensitiveness = _case_sensitive;
				continue;
			}
			//if strings equal with symbol skipped, restart compare with symbols
			if (skip_symbols)
			{
				skip_symbols = false;
				continue;
			}
			return (0);
		}
		s1_iterator++;
		s2_iterator++;
	}
	//do strings contain numbers ?
	bool contains_number = false;
	for (int i = 0; s1[i]; i++)
	{
		if (ft_isdigit(s1[i]))
		{
			contains_number = true;
			break;
		}
	}
	//if different because of symbols
	//trying to insert with symbol while one without symbol is a duplicate
	if (ft_strchr("()[]{}*/:@", s1[s1_iterator]) && s1[s1_iterator])
	{
		if (contains_number)
			return (1);
		return (-1);
	}
	//trying to insert without symbol while one with symbol is a duplicate
	if (ft_strchr("()[]{}*/@", s2[s2_iterator]) && s2[s2_iterator])
	{
		if (s2[s2_iterator] == '@')
			return (0);		//do not insert duplicate without @
		//if both are symbols TODO
		if (contains_number)	//first the symbol
			return (-1);			//if number no symbol last
		return (1);			//if no number no symbol first
	}
	//if different because of case
	if (ft_tolower(s1[s1_iterator]) == ft_tolower(s2[s2_iterator]))
	{
		if (ft_islower(s1[s1_iterator]))
			return (-1);			//insert lowercase first
		return (1);					//insert uppercase last
	}
	//if different not because of symbols nor case -> just return difference
	return ((unsigned char)((*case_sensitiveness)(s1[s1_iterator])) - (unsigned char)((*case_sensitiveness)(s2[s2_iterator])));
}

/*
 *	@brief function used to compare symbols and sort them used by ft_bstinsert
 *
 *	@param new a pointer on the content being added
 *	@param inplace a pointer on the content already in the tree for the comparison
 *
 * 	@return an int to represent if the new content is >,<,== compared to the old one
 *	@retval > 0 if newcontent should go to the right
 *	@retval < 0 if newcontent should go to the left
 *	@retval = 0 if newcontent is already in the binary tree
*/
int _symbol_sort_order(void *new, void *inplace)
{
	t_symbol *new_content = (t_symbol *)new;
	t_symbol *old_content = (t_symbol *)inplace;
	int diff = 0;

	if ((diff = _compare_symbol_names(new_content->name, old_content->name)))
		return (diff);
	if ((diff = new_content->type - old_content->type))
		return (diff);
	return (new_content->value - old_content->value);
}

int _symbol_no_sort(void *lhs, void *rhs)
{
	//returning either 0 if duplicate or 1 if not duplicate
	return (!!_symbol_sort_order(lhs,rhs));
}

static void	_print_symbol(void *content)
{
	t_symbol *symbol = (t_symbol *)content;

	if (symbol->type == 'w' || symbol->type == 'U')
	{
		printf("                 %c %s\n", symbol->type, (symbol->name ? symbol->name : ""));
		return ;
	}
	printf("%016lx %c %s\n", symbol->value, symbol->type, (symbol->name ? symbol->name : ""));
}

static void	_print_global_symbol(void *content)
{
	t_symbol *symbol = (t_symbol *)content;

	if (symbol->type == 'w' || symbol->type == 'U')
	{
		printf("                 %c %s\n", symbol->type, (symbol->name ? symbol->name : ""));
		return ;
	}
	if (ft_isupper(symbol->type))
	printf("%016lx %c %s\n", symbol->value, symbol->type, (symbol->name ? symbol->name : ""));
}

static void	_print_undefined_symbol(void *content)
{
	t_symbol *symbol = (t_symbol *)content;

	if (symbol->type == 'w' || symbol->type == 'U')
	{
		printf("                 %c %s\n", symbol->type, (symbol->name ? symbol->name : ""));
		return ;
	}
}

static void	_print_symbol32(void *content)
{
	t_symbol *symbol = (t_symbol *)content;

	if (symbol->type == 'w' || symbol->type == 'U')
	{
		printf("         %c %s\n", symbol->type, (symbol->name ? symbol->name : ""));
		return ;
	}
	printf("%08lx %c %s\n", symbol->value, symbol->type, (symbol->name ? symbol->name : ""));
}

static void	_print_global_symbol32(void *content)
{
	t_symbol *symbol = (t_symbol *)content;

	if (symbol->type == 'w' || symbol->type == 'U')
	{
		printf("         %c %s\n", symbol->type, (symbol->name ? symbol->name : ""));
		return ;
	}
	if (ft_isupper(symbol->type))
	printf("%08lx %c %s\n", symbol->value, symbol->type, (symbol->name ? symbol->name : ""));
}

static void	_print_undefined_symbol32(void *content)
{
	t_symbol *symbol = (t_symbol *)content;

	if (symbol->type == 'w' || symbol->type == 'U')
	{
		printf("         %c %s\n", symbol->type, (symbol->name ? symbol->name : ""));
		return ;
	}
}
