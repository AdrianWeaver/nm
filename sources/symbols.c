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
	Elf64_Shdr *shdr_table = (Elf64_Shdr *)&ehdr->e_shoff;
	char	* const string_table = (char *)&file->raw[shdr_table[ehdr->e_shstrndx].sh_offset];
	int (*sort_function)(void *, void*) = _symbol_sort_order;
	void (*iteration_function)(t_bst **, void (*f)(void*)) = ft_bstiter;

	for (uint16_t i = 0; i < ehdr->e_shnum; i++) //iterate on sections
	{
		Elf64_Shdr *shdr = &shdr_table[i];
		if (shdr->sh_type != SHT_SYMTAB && shdr->sh_type != SHT_DYNSYM) //if not a symbol section go next
			continue;
		Elf64_Sym *symbol_table = (Elf64_Sym *) &shdr[i].sh_offset;
		for (uint32_t j = 0; j < shdr->sh_size / shdr->sh_entsize; j++) //iterate on symbols
		{
			Elf64_Sym *symbol = &symbol_table[j];
			t_symbol *tmp = malloc(sizeof(*tmp) * 1);
			tmp->name = &string_table[symbol->st_name];
			tmp->value = symbol->st_value;
			t_bst *node = ft_bstnew(tmp);
			if (option_field & (1 << OPTION_P)) //do not sort
			{
				ft_bstinsert(symbol_list, node, _symbol_no_sort);
				continue;
			}
			ft_bstinsert(symbol_list, node, _symbol_sort_order);
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

static int _symbol_sort_order(void *lhs, void *rhs)
{
	t_symbol *node_in_tree = (t_symbol *)lhs;
	t_symbol *node_to_be_added = (t_symbol *)rhs;
	if (node_in_tree->name && node_to_be_added->name &&
		ft_strcmp(node_in_tree->name, node_to_be_added->name))
	{
		return (ft_strcmp(node_in_tree->name, node_to_be_added->name));
	}
	if (node_in_tree->type != node_to_be_added->type)
	{
		return (node_in_tree->type < node_to_be_added->type);
	}
	return (node_in_tree->value < node_to_be_added->value);
}

static int _symbol_no_sort(void *lhs, void *rhs)
{
	(void)lhs;(void)rhs;
	return (true); //returning always true helps adding new nodes on the right side of the tree.
}

static void	_print_symbol(void *content)
{
	t_symbol *symbol = (t_symbol *)content;

	printf("oui\n");
	printf("%016lx %c %s\n", symbol->value, symbol->type, (symbol->name ? symbol->name : ""));
}
