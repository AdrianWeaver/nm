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
		if (_check_phdr_64lsb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2LSB)
	{
		if (_check_phdr_32lsb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2MSB)
	{
		if (_check_phdr_64msb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2MSB)
	{
		if (_check_phdr_32msb(file) != ERROR)
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
int	_check_phdr_64lsb(const t_mem *file)
{
	int					unique_phdr = 0;
	int					unique_interp = 0;
	int					pt_load_count = 0;
	const Elf64_Ehdr	*ehdr = (Elf64_Ehdr *)file->raw;
	const Elf64_Phdr	*phdr_table = (Elf64_Phdr *)&(file->raw[ehdr->e_phoff]);

	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		const Elf64_Phdr phdr = phdr_table[i];
		if (phdr.p_type == PT_LOAD)
		{
			pt_load_count++;
			if (phdr.p_align > 1 && ((phdr.p_vaddr % PAGESIZE) != (phdr.p_offset % PAGESIZE)))
				fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		}
		if (phdr.p_type == PT_PHDR && (++unique_phdr > 1 || pt_load_count > 0))
			return (ERROR);
		if (phdr.p_type == PT_INTERP && (++unique_interp > 1 || pt_load_count > 0))
			return (ERROR);
		if (phdr.p_align == 0 || phdr.p_align == 1)
			continue;
		//returns non-zero if multiple bytes are set
		if (phdr.p_align & (phdr.p_align - 1))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		//PAGESIZE should not be hardcoded and should call for getpagesize() but not allowed in the subject
		if ((phdr.p_vaddr % phdr.p_align) != (phdr.p_offset % phdr.p_align))
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
int	_check_phdr_32lsb(const t_mem *file)
{
	int					unique_phdr = 0;
	int					unique_interp = 0;
	int					pt_load_count = 0;
	const Elf32_Ehdr	*ehdr = (Elf32_Ehdr *)file->raw;
	const Elf32_Phdr	*phdr_table = (Elf32_Phdr *)&(file->raw[ehdr->e_phoff]);

	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		const Elf32_Phdr phdr = phdr_table[i];
		if (phdr.p_type == PT_LOAD)
		{
			pt_load_count++;
			if (phdr.p_align > 1 && ((phdr.p_vaddr % PAGESIZE) != (phdr.p_offset % PAGESIZE)))
				fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		}
		if (phdr.p_type == PT_PHDR && (++unique_phdr > 1 || pt_load_count > 0))
			return (ERROR);
		if (phdr.p_type == PT_INTERP && (++unique_interp > 1 || pt_load_count > 0))
			return (ERROR);
		if (phdr.p_align == 0 || phdr.p_align == 1)
			continue;
		//returns non-zero if multiple bytes are set
		if (phdr.p_align & (phdr.p_align - 1))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		//PAGESIZE should not be hardcoded and should call for getpagesize() but not allowed in the subject
		if ((phdr.p_vaddr % phdr.p_align) != (phdr.p_offset % phdr.p_align))
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
int	_check_phdr_64msb(const t_mem *file)
{
	int					unique_phdr = 0;
	int					unique_interp = 0;
	int					pt_load_count = 0;
	const Elf64_Ehdr	*ehdr = (Elf64_Ehdr *)file->raw;
	const Elf64_Phdr	*phdr_table = (Elf64_Phdr *)&(file->raw[rev64(ehdr->e_phoff)]);

	for (int i = 0; i < rev16(ehdr->e_phnum); i++)
	{
		const Elf64_Phdr phdr = phdr_table[i];
		if (rev32(phdr.p_type) == PT_LOAD)
		{
			pt_load_count++;
			if (rev32(phdr.p_align) > 1 && ((rev64(phdr.p_vaddr) % PAGESIZE) != (rev64(phdr.p_offset) % PAGESIZE)))
				fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		}
		if (rev32(phdr.p_type) == PT_PHDR && (++unique_phdr > 1 || pt_load_count > 0))
			return (ERROR);
		if (rev32(phdr.p_type) == PT_INTERP && (++unique_interp > 1 || pt_load_count > 0))
			return (ERROR);
		if (rev64(phdr.p_align) == 0 || rev64(phdr.p_align) == 1)
			continue;
		//returns non-zero if multiple bytes are set
		if (rev64(phdr.p_align) & (rev64(phdr.p_align) - 1))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		//PAGESIZE should not be hardcoded and should call for getpagesize() but not allowed in the subject
		if ((rev64(phdr.p_vaddr) % rev64(phdr.p_align)) != (rev64(phdr.p_offset) % rev64(phdr.p_align)))
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
int	_check_phdr_32msb(const t_mem *file)
{
	int					unique_phdr = 0;
	int					unique_interp = 0;
	int					pt_load_count = 0;
	const Elf32_Ehdr	*ehdr = (Elf32_Ehdr *)file->raw;
	const Elf32_Phdr	*phdr_table = (Elf32_Phdr *)&(file->raw[rev32(ehdr->e_phoff)]);

	for (int i = 0; i < rev16(ehdr->e_phnum); i++)
	{
		const Elf32_Phdr phdr = phdr_table[i];
		if (rev32(phdr.p_type) == PT_LOAD)
		{
			pt_load_count++;
			if (rev32(phdr.p_align) > 1 && ((rev32(phdr.p_vaddr) % PAGESIZE) != (rev32(phdr.p_offset) % PAGESIZE)))
				fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		}
		if (rev32(phdr.p_type) == PT_PHDR && (++unique_phdr > 1 || pt_load_count > 0))
			return (ERROR);
		if (rev32(phdr.p_type) == PT_INTERP && (++unique_interp > 1 || pt_load_count > 0))
			return (ERROR);
		if (rev32(phdr.p_align) == 0 || rev32(phdr.p_align) == 1)
			continue;
		//returns non-zero if multiple bytes are set
		if (rev32(phdr.p_align) & (rev32(phdr.p_align) - 1))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
		//PAGESIZE should not be hardcoded and should call for getpagesize() but not allowed in the subject
		if ((rev32(phdr.p_vaddr) % rev32(phdr.p_align)) != (rev32(phdr.p_offset) % rev32(phdr.p_align)))
			fprintf(stderr, "nm: warning: %s has a program header with invalid alignment\n", file->name);
	}
	return (0);
}
