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
int	check_shdr(t_mem *file)
{
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2LSB)
	{
		if (check_shdr_64lsb(file) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2LSB)
	{
		if (check_shdr_32lsb(file, (Elf32_Phdr *)file->raw) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS64 && file->endianness == ELFDATA2MSB)
	{
		if (check_shdr_64msb(file, (Elf64_Phdr *)file->raw) != ERROR)
			return (0);
	}
	if (file->class == ELFCLASS32 && file->endianness == ELFDATA2MSB)
	{
		if (check_shdr_32msb(file, (Elf32_Phdr *)file->raw) != ERROR)
			return (0);
	}
	return (ERROR);
}



