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
	return (fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
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
	const unsigned char	*string_table = &file->raw[shdr_table[ehdr->e_shstrndx].sh_offset];

	for (int i = 0; i < ehdr->e_shnum; i++)
	{
		const Elf64_Shdr *shdr = &shdr_table[i];
		//types will be checked laster. error is as follow
		//nm: %file: unknown type [0xval] section `.sectionname'\n then continuing on nm
		if (shdr->sh_type == SHT_NULL)
			continue;
		//check size of sections
		if (shdr->sh_offset + shdr->sh_size > file->size)
			section_too_long = true;
		//linked section is within section list
		if (shdr->sh_link > ehdr->e_shnum)
			return (ERROR);
		//check that size of section and elements is not corrupt
		if (shdr->sh_entsize && (shdr->sh_size % shdr->sh_entsize) != 0)
			return (ERROR);
		//checking that section name is a valid string
		if (shdr->sh_name > shdr_table[ehdr->e_shstrndx].sh_size)
			return (fprintf(stderr, "nm: %s: invalid string offset %u >= %lu for section `.shstrtab'\n",
						file->name, shdr->sh_name, shdr_table[ehdr->e_shstrndx].sh_size), ERROR);
		//checking that section type is valid
		_check_sh_type_64lsb(file, shdr, &string_table[shdr->sh_name]);
		printf("%d: section_type: %d\n", i, shdr->sh_type);
	}
	if (section_too_long == true)
		fprintf(stderr, "nm: warning: %s: has a section extending past end of file\n", file->name);
	return (0);
}

/*	@brief checks if the section type is valid
 *	
 *	@param file the t_mem* storing mapped file and info
 *	@param sh_type a uint32_t the type of the section
 *	@param section_name the valid string storing the section name
 *	@return non-zero in case of incorrect type
*/
int	_check_sh_type_64lsb(const t_mem *file, const Elf64_Shdr *shdr, const unsigned char *section_name)
{
	const uint32_t sh_type = shdr->sh_type;

	if (sh_type > SHT_LOPROC && sh_type < SHT_HIPROC)
		return (0);
	if (sh_type > SHT_LOUSER && sh_type < SHT_HIUSER)
		return (0);
	switch (sh_type)
	{
		case SHT_NULL:
		case SHT_PROGBITS:
			break;
		case SHT_SYMTAB:
			if (shdr->sh_entsize != sizeof(Elf64_Sym))
				return (fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
			break;
		case SHT_STRTAB:
		case SHT_RELA:
		case SHT_HASH:
		case SHT_DYNAMIC:
		case SHT_NOTE:
		case SHT_NOBITS:
		case SHT_REL:
		case SHT_SHLIB:
			break;
		case SHT_DYNSYM:
			if (shdr->sh_entsize != sizeof(Elf64_Sym))
				return (fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
			break;
		case SHT_INIT_ARRAY:
		case SHT_FINI_ARRAY:
		case SHT_PREINIT_ARRAY:
		case SHT_GROUP:
		case SHT_SYMTAB_SHNDX:
		case SHT_NUM:
		case SHT_LOOS:
		case SHT_GNU_ATTRIBUTES:
		case SHT_GNU_HASH:
		case SHT_GNU_LIBLIST:
		case SHT_CHECKSUM:
		//case SHT_LOSUNW:	//adapt this in case SHTSUNW_move != SHT_LOSUNW
		//case SHT_HISUNW:	//adapt this in case SHT_GNU_versym != SHT_HISUNW
		case SHT_SUNW_move:
		case SHT_SUNW_COMDAT:
		case SHT_SUNW_syminfo:
		case SHT_GNU_verdef:
		case SHT_GNU_verneed:
		case SHT_GNU_versym:
		//case SHT_HIOS:	//adapt this in case SHT_GNU_versym != SHT_HIOS
			break;
		default:
			//nm: %file: unknown type [0xval] section `.sectionname'\n then continuing on nm
			fprintf(stderr, "nm: %s: unknown type [0x%x] section `%s'\n", file->name, sh_type, section_name);
			return (ERROR);
	}
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
