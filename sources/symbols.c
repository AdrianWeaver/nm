#include <elf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ft_nm.h"

//sorting functions for symbol binary tree
static int _symbol_sort_order(void *lhs, void *rhs);
static int _symbol_no_sort(void *lhs, void *rhs);
static void	_print_symbol(void *node);

/*
 * @brief handler to check the correct shdr depending on class and endianness
 *
 * @param file t_mem storing mapped file and infos
 * @return non-zero in case of error
*/
int	get_symbols(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	printf("DEBUG: check shdr\n");
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

	for (uint16_t i = 0; i < ehdr->e_shnum; i++) //iterate on sections
	{
		Elf64_Shdr *shdr = &shdr_table[i];
		if (shdr->sh_type != SHT_SYMTAB && shdr->sh_type != SHT_DYNSYM) //if not a symbol section go next
			continue;
		Elf64_Sym *symbol_table = (Elf64_Sym *) &file->raw[shdr->sh_offset];
		for (uint32_t j = 0; j < shdr->sh_size / shdr->sh_entsize; j++) //iterate on symbols
		{
			Elf64_Sym *symbol = &symbol_table[j];
			if (symbol->st_shndx > ehdr->e_shnum)
				continue;
			char *symbol_string_table = (char *)&file->raw[shdr_table[shdr->sh_link].sh_offset];


			t_symbol *tmp_symbol = malloc(sizeof(*tmp_symbol) * 1);
			//TODO: Add malloc protection
			tmp_symbol->name = &symbol_string_table[symbol->st_name];
			if (symbol->st_name == 0)
			{
				tmp_symbol->name = "";
				if (symbol->st_size == 0)
					continue;
			}
			tmp_symbol->value = symbol->st_value;
			tmp_symbol->type = get_symbol_type_64lsb(file, symbol, tmp_symbol);
			t_bst *tmp_node = ft_bstnew(tmp_symbol);
			if (option_field & (1 << OPTION_P)) //do not sort
				sort_function = _symbol_no_sort;
			ft_bstinsert(symbol_list, tmp_node, sort_function);
		}
		//TODO: this could be better because name and value should be const
		//try to initialize a stack symbol then memcopy it in the malloced one.
		//use const qualifiers in the symbol
	}
	if (option_field & (1 << OPTION_R)) //reverse print
		iteration_function = ft_bstriter;
	//printing symbols
	(*iteration_function)(symbol_list, _print_symbol);
	//TODO: add clear bst_tree here
	return (0);
}

int	_get_symbols_32lsb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	(void)symbol_list;(void)file; (void)option_field; return (0); //no compilation error
}

int	_get_symbols_64msb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	(void)symbol_list;(void)file; (void)option_field; return (0); //no compilation error
}

int	_get_symbols_32msb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	(void)symbol_list;(void)file; (void)option_field; return (0); //no compilation error
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
static int _symbol_sort_order(void *new, void *inplace)
{
	t_symbol *new_content = (t_symbol *)new;
	t_symbol *old_content = (t_symbol *)inplace;
	int diff = 0;

	if ((diff = ft_strcmp(new_content->name, old_content->name)))
		return (diff);
	if ((diff = new_content->type - old_content->type))
		return (diff);
	return (new_content->value - old_content->value);
}

static int _symbol_no_sort(void *lhs, void *rhs)
{
	(void)lhs;(void)rhs;
	return (1); //returning always true helps adding new nodes on the right side of the tree.
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
