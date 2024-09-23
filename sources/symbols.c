#include <elf.h>
#include <stdio.h>
#include "ft_nm.h"

/*
 * @brief handler to check the correct shdr depending on class and endianness
 *
 * @param file t_mem storing mapped file and infos
 * @return non-zero in case of error
*/
int	get_symbols(t_mem *file, uint8_t optionField)
{
	printf("DEBUG: check shdr\n");
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2LSB)
	{
		if (_get_symbols_64lsb(file, optionField) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2LSB)
	{
		if (_get_symbols_32lsb(file, optionField) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2MSB)
	{
		if (_get_symbols_64msb(file, optionField) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2MSB)
	{
		if (_get_symbols_32msb(file, optionField) != ERROR)
			return (0);
	}
	return (ERROR);
}

int	_get_symbols_64lsb(t_mem *file, uint8_t optionField)
{
	//get shdr_table
	//get symtab
	//get symstrtab
	//get symboltype
	(void)file; (void)optionField; return (0); //no compilation error
}

int	_get_symbols_32lsb(t_mem *file, uint8_t optionField)
{
	(void)file; (void)optionField; return (0); //no compilation error
}

int	_get_symbols_64msb(t_mem *file, uint8_t optionField)
{
	(void)file; (void)optionField; return (0); //no compilation error
}

int	_get_symbols_32msb(t_mem *file, uint8_t optionField)
{
	(void)file; (void)optionField; return (0); //no compilation error
}
