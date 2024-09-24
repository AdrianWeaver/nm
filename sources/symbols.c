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

int	_get_symbols_64lsb(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file->raw;
	Elf64_Shdr *shdr_table = (Elf64_Shdr *)&file->raw[ehdr->e_shoff];
	char	* const string_table = (char *)&file->raw[shdr_table[ehdr->e_shstrndx].sh_offset];
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
			printf("loop %d\n", j);

			Elf64_Sym *symbol = &symbol_table[j];
			char *symbol_string_table = (char *)&file->raw[shdr_table[shdr->sh_link].sh_offset];


			t_symbol *tmp_symbol = malloc(sizeof(*tmp_symbol) * 1);
			tmp_symbol->name = &symbol_string_table[symbol->st_name];
			if (symbol->st_name == 0)
				tmp_symbol->name = "";
			tmp_symbol->value = symbol->st_value;
			t_bst *tmp_node = ft_bstnew(tmp_symbol);
			printf("--- symbol_name = %s\n", tmp_symbol->name);
			printf("--- node_name = %s\n", ((t_symbol *)tmp_node->content)->name);
			printf("--- symbol_value = %lu\n", tmp_symbol->value);
			printf("--- node_value = %lu\n", ((t_symbol *)tmp_node->content)->value);
			tmp_symbol->value = symbol->st_value;
			//printf("symbol test = %s\n", ((t_symbol *)tmp_node->content)->name);
			if (option_field & (1 << OPTION_P)) //do not sort
			{
				printf("DEBUG: option -p detected\n");
				ft_bstinsert(symbol_list, tmp_node, _symbol_no_sort);
				continue;
			}
			ft_bstinsert(symbol_list, tmp_node, _symbol_sort_order);
		}
		//TODO: this could be better because name and value should be const
		//try to initialize a stack symbol then memcopy it in the malloced one.
		//use const qualifiers in the symbol
	}
	if (option_field & (1 << OPTION_R)) //reverse print
	{
		iteration_function = ft_bstriter;
	}
	printf("SHOULD PRINT THE SYMBOLS HERE\n");
	(*iteration_function)(symbol_list, _print_symbol);
	//get shdr_table
	//get symtab
	//get symstrtab
	//get symboltype

	(void)iteration_function;(void)sort_function;(void)symbol_list;(void) ehdr; (void) string_table; (void)file; (void)option_field; return (0); //no compilation error
	//print
	if (option_field & (1 << OPTION_R)) //print in reverse
	{;}
	if (option_field & (1 << OPTION_R)) //print in reverse
	{;}
	//print
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
 *	@param lhs most likely the new node to be added
 *	@param rhs most likely one of the old nodes for comparison
 *
 *	@return 
*/
//TODO: there is a clear error here
//STAF START HERE
static int _symbol_sort_order(void *lhs, void *rhs)
{
	t_symbol *new_content = (t_symbol *)lhs;
	t_symbol *old_content = (t_symbol *)rhs;

	//special case:
	//null name
	//if strcmp != 0 return strcmp
	//if strcmp == 0 && type != return type
	//else return addr
	printf("--- CALL SORT ORDER\n\tnew_content->name: %s\n\told_content->name: %s\n", new_content->name, old_content->name);
	if (!lhs || !rhs)
	{
		printf("!!! REFUSING INSERTION !!!\n");
		return (0);
	}
	if (new_content->name && old_content->name &&
		ft_strcmp(new_content->name, old_content->name))
	{
		printf("inserted using name\n");
		printf("new name : %s - old name: %s, diff: %d\n", new_content->name, old_content->name, ft_strcmp(new_content->name, old_content->name));
		return (ft_strcmp(new_content->name, old_content->name));
	}
	if (new_content->type != old_content->type)
	{
		printf("inserted using type\n");
		return (new_content->type - old_content->type);
	}
		printf("inserted using value: %ld\n", new_content->value - old_content->value);
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
	symbol->type = 'R';

	printf("%016lx %c %s\n", symbol->value, symbol->type, (symbol->name ? symbol->name : ""));
}
