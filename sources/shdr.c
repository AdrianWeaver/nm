#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include "ft_nm.h"


	//ElfN_Off	e_shoff; 		-> triggers bfd errors
		//uint32_t	e_flags; 	-> not checked by nm
		//uint16_t	e_ehsize; 	-> not checked by nm
		//uint16_t	e_phentsize; -> not checked by nm
	//uint16_t	e_shentsize; -> format error/corrupt string table
	//uint16_t	e_shnum; -> leads to format error
	//if (ehdr->e_shnum == 0)
	//	goto format_error;
	//uint16_t	e_shstrndx;  -> this is complex
	/*
		if shstrnxd = SHN_UNDEF error message is
		nm: warning: file_name has a corrupt string table index - ignoring
		nm: file_name: no symbols
		same behaviour for broken shstrnxd but do not know how to check?
	*/

/*
 * @brief handler to check the correct shdr depending on class and endianness
 *
 * @param file t_mem storing mapped file and infos
 * @return non-zero in case of error
*/
int	check_shdr(const t_mem *file)
{
	printf("DEBUG: check shdr\n");
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2LSB)
	{
		if (_check_shdr_64lsb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2LSB)
	{
		if (_check_shdr_32lsb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2MSB)
	{
		if (_check_shdr_64msb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2MSB)
	{
		if (_check_shdr_32msb(file) != ERROR)
			return (0);
	}
	return (ERROR);
}

/*
 *	@brief	check validity of shdr table
 *
 *	@param file t_mem storing mapped file and infos
 *	@return non-zero in case of error in shdr table
*/
int	_check_shdr_64lsb(const t_mem *file)
{
	bool				section_too_long = false;
	const Elf64_Ehdr	*ehdr = (Elf64_Ehdr *)file->raw;
	const Elf64_Shdr	*shdr_table = (Elf64_Shdr *)&(file->raw[ehdr->e_shoff]);

	printf("called shdr 64 lsb\n");
	for (int i = 0; i < ehdr->e_shnum; i++)
	{
		const Elf64_Shdr shdr = shdr_table[i];
		//types will be checked laster. error is as follow
		//nm: %file: unknown type [0xval] section `.sectionname'\n then continuing on nm
		if (shdr.sh_type == SHT_NULL)
			continue;
		//check size of sections
		if (shdr.sh_offset + shdr.sh_size > file->size)
			section_too_long = true;
		//linked section is within section list
		if (shdr.sh_link > ehdr->e_shnum)
			return (fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
		//check that size of section and elements is not corrupt
		if (shdr.sh_entsize && (shdr.sh_size % shdr.sh_entsize) != 0)
			return (fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
	}
	if (section_too_long == true)
		fprintf(stderr, "nm: warning: %s: has a section extending past end of file\n", file->name);
	return (0);
}

/*
 *	@brief
 *
 *	@param
 *	@return
*/
int	_check_shdr_32lsb(const t_mem *file)
{
	(void)file;
	return (0);
}

/*
 *	@brief
 *
 *	@param
 *	@return
*/
int	_check_shdr_64msb(const t_mem *file)
{
	(void)file;
	return (0);
}

/*
 *	@brief
 *
 *	@param
 *	@return
*/
int	_check_shdr_32msb(const t_mem *file)
{
	(void)file;
	return (0);
}
