#include <elf.h>
#include <stdio.h>
#include "ft_nm.h"

/*	@brief handler to check the correct phdr depending on class and endianness
 *
 *	@param file t_mem storing mapped file and infos
 *	@return non-zero in case of error
*/
int	check_phdr(const t_mem *file)
{
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2LSB)
	{
		if (check_phdr_64lsb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2LSB)
	{
		if (check_phdr_32lsb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2MSB)
	{
		if (check_phdr_64msb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2MSB)
	{
		if (check_phdr_32msb(file) != ERROR)
			return (0);
	}
	return (ERROR);
}

/*	@brief checking that all phdr (if any) are correct
 *	for 64bits and little-endian files
 *	check for uniqueness of PT_HDR, then alignment
 *
 *	@param file t_mem storing mapped file and infos
 *	@return non-zero in case of errors (writing on stderr)
*/
int	check_phdr_64lsb(const t_mem *file)
{
	int					unique = 0;
	int					pt_load_count = 0;
	const Elf64_Ehdr	*ehdr = (Elf64_Ehdr *)file->raw;
	const Elf64_Phdr	*phdr_table = (Elf64_Phdr *)&(file->raw[ehdr->e_phoff]);

	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		const Elf64_Phdr phdr = phdr_table[i];
		if (phdr.p_type == PT_LOAD)
			pt_load_count++;
		if (phdr.p_type == PT_PHDR && (++unique > 1 || pt_load_count > 0))
			return (ERROR);
		if (phdr.p_align == 0 || phdr.p_align == 1)
			continue;
		//returns non-zero if multiple bytes are set
		if (phdr.p_align & (phdr.p_align - 1))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		//PAGESIZE should not be hardcoded and should call for getpagesize() but not allowed in the subject
		if (!((phdr.p_vaddr % PAGESIZE) == phdr.p_align
				&& (phdr.p_offset % PAGESIZE) == phdr.p_align))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
	}
	return (0);
}

/*	@brief checking that all phdr (if any) are correct
 *	for 32bits and little-endian files
 *	check for uniqueness of PT_HDR, then alignment
 *
 *	@param file t_mem storing mapped file and infos
 *	@return non-zero in case of errors (writing on stderr)
*/
int	check_phdr_32lsb(const t_mem *file)
{
	Elf32_Ehdr	*ehdr;
	Elf32_Phdr	*phdr;
	int	unique = 0;

	ehdr = (Elf32_Ehdr *)file->raw;
	phdr = (Elf32_Phdr *)&(file->raw[ehdr->e_phoff]);
	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		if (phdr[i].p_type == PT_PHDR && ++unique > 1)
			return (ERROR);
		if (phdr->p_align == 0 || phdr->p_align == 1)
			continue;
		//returns non-zero if multiple bytes are set
		if (phdr->p_align & (phdr->p_align - 1))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		//PAGESIZE should not be hardcoded and should call for getpagesize() but not allowed in the subject
		if (!((phdr->p_vaddr % PAGESIZE) == phdr->p_align
				&& (phdr->p_offset % PAGESIZE) == phdr->p_align))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
	}
	return (0);
}


/*	@brief checking that all phdr (if any) are correct
 *	for 32bits and little-endian files
 *	check for uniqueness of PT_HDR, then alignment
 *
 *	@param file t_mem storing mapped file and infos
 *	@return non-zero in case of errors (writing on stderr)
*/
int	check_phdr_64msb(const t_mem *file)
{
	//TODO: do this function.
	(void)file;
	return (0);
}

/*	@brief checking that all phdr (if any) are correct
 *	for 32bits and little-endian files
 *	check for uniqueness of PT_HDR, then alignment
 *
 *	@param file t_mem storing mapped file and infos
 *	@return non-zero in case of errors (writing on stderr)
*/
int	check_phdr_32msb(const t_mem *file)
{
	//TODO: do this function
	(void)file;
	return (0);
}
