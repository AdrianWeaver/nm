#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <elf.h>
#include <stdlib.h>
#include "libft.h"
#include "ft_nm.h"

#define ERROR -1

/*	@brief making sure the file is an ELF
 *	populating the t_mem* with class and endianness
 *
 * @param file a t_mem*	with a raw pointer on the file mapped in memory 
 * @return non-zero if not an ELF
 */
int	e_ident_checker(t_mem *file)
{
	uint8_t	*e_ident = ((Elf32_Ehdr *)file->raw)->e_ident;

	if (e_ident[EI_MAG0] != 0x7f || e_ident[EI_MAG1] != 'E'
		|| e_ident[EI_MAG2] != 'L' || e_ident[EI_MAG3] != 'F')
		goto format_error;
	if (!(e_ident[EI_CLASS] == ELFCLASS32 || e_ident[EI_CLASS] == ELFCLASS64))
		goto format_error;
	file->class = e_ident[EI_CLASS];
	if (!(e_ident[EI_DATA] == ELFDATA2LSB || e_ident[EI_DATA] == ELFDATA2MSB))
		goto format_error;
	file->endianness = file->raw[EI_DATA];
	if (!(e_ident[EI_VERSION] == EV_CURRENT))
		goto format_error;
	return (0);
	format_error:
		return (fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
}

/*
 * @brief handler to check the correct ehdr depending on class and endianness
 *
 * @param file t_mem storing mapped file and infos
 * @param st stats of the file being read
 * @return non-zero in case of error
 */
int	check_ehdr(t_mem *file, struct stat *st)
{
	//checking ident and filling t_mem struct with infos
	if (e_ident_checker(file) == ERROR)
		return (ERROR);
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2LSB)
	{
		if (check_ehdr_64lsb(file, st) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2LSB)
	{
		if (check_ehdr_32lsb(file, st) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2MSB)
	{
		if (check_ehdr_64msb(file, st) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2MSB)
	{
		if (check_ehdr_32msb(file, st) != ERROR)
			return (0);
	}
	return (ERROR);
}

/*
 * @brief check ehdr for 64 little endian
 *
 * @param file t_mem storing mapped file and infos
 * @param st stats of the file being read
 * @return non-zero in case of error
 */

int	check_ehdr_64lsb(t_mem *file, struct stat *st)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *) &file->raw;
	//check e_type only core files are errors
	if (ehdr->e_type == ET_CORE)
		goto format_error;
	//check if the program header table is not too big for the file
	if ((uint8_t)(ehdr->e_phoff + (ehdr->e_phnum * ehdr->e_phentsize)) > st->st_size)
		goto format_error;
	//check if the section header table is not too big for the file
	if ((uint8_t)(ehdr->e_shoff + (ehdr->e_shnum * ehdr->e_shentsize)) > st->st_size)
		goto format_error;
	if (ehdr->e_shstrndx == SHN_UNDEF)
		return (fprintf(stderr, "nm: %s: file has a corrupt string table index\n", file->name), ERROR);
	format_error:
		return (fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
	return (0);
}

/*
 * @brief check ehdr for 64 little endian
 *
 * @param file t_mem storing mapped file and infos
 * @param st stats of the file being read
 * @return non-zero in case of error
 */
int	check_ehdr_32lsb(t_mem *file, struct stat *st)
{
	(void)file; (void)st; //no compilation errors
	return (0);
}
/*
 * @brief check ehdr for 64 little endian
 *
 * @param file t_mem storing mapped file and infos
 * @param st stats of the file being read
 * @return non-zero in case of error
 */int	check_ehdr_64msb(t_mem *file, struct stat *st)
{
	(void)file; (void)st; //no compilation errors
	return (0);
}

/*
 * @brief check ehdr for 64 little endian
 *
 * @param file t_mem storing mapped file and infos
 * @param st stats of the file being read
 * @return non-zero in case of error
 */int	check_ehdr_32msb(t_mem *file, struct stat *st)
{
	(void)file; (void)st; //no compilation errors
	return (0);
}
