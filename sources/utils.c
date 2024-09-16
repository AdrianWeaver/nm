
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <stdlib.h>
#include "libft.h"
#include "libftprintf.h"
#include "ft_nm.h"

void	print_64shdr(Elf64_Shdr *shdr, char *stringTable)
{
	char *str_flags;

	str_flags = get_sh_flags(shdr->sh_flags);
	ft_printf("\n%-16s: %s\n"		//name
			"%-16s: %s\n"		//type
			"%-16s: %s\n"		//flags
			"%-16s: 0x%016x\n"		//addr
			"%-16s: 0x%016x\n"		//offset
			"%-16s: %d\n"		//size
			"%-16s: %d\n"		//link
			"%-16s: %d\n"		//info
			"%-16s: %d\n"		//addralign
			"%-16s: %d\n",		//entsize
			"SECTION", &stringTable[shdr->sh_name],
			"sh_type", get_shtype(shdr),
			"sh_flag", str_flags,
			"sh_addr", shdr->sh_addr,
			"sh_offset", shdr->sh_offset,
			"sh_size", shdr->sh_size,
			"sh_link", shdr->sh_link,
			"sh_info", shdr->sh_info,
			"sh_addralign", shdr->sh_addralign,
			"sh_entsize", shdr->sh_entsize);
	free(str_flags);
}

//TODO: Reimplement this part, this is a core function and needs
//to be used everywhere I need to access the raw file.
void *protected_read(uint8_t *mem, size_t max, int offset, size_t buffer)
{
	if (offset < 0)
		return (NULL);
	if (mem + offset + buffer > mem + max)
		return (NULL);
	return (mem + offset);
}

//TODO: Reimplement this part, this is a core function and needs
//to be used everywhere I need to access the raw file for strings
char *protected_read_str(uint8_t *mem, size_t max, int offset)
{
	if (offset < 0)
		return (NULL);
	if (mem + offset < mem + max)
	{
		for (size_t i = 0; mem + i < mem + max; i++)
		{
			if (mem[i] == '\0')
				return ((char *)mem + offset);
		}
	}
	return (NULL);
}

/*	@brief check validity of phdr(s) in 32bits programs
 *	@param raw the file mapped in memory
 *	@param phoff the offset for the process header(s) found in ehdr
 *	@param phnum the number of process header(s) found in ehdr
 *
 *	@return non-zero if error
 */
int	check_32programheader(uint8_t *raw, Elf32_Off phoff, uint8_t phnum)
{
	(void)raw;(void)phoff;(void)phnum;
	return (0);
}

/*	@brief check validity of phdr(s) in 64bits programs
 *	@param raw the file mapped in memory
 *	@param phoff the offset for the process header(s) found in ehdr
 *	@param phnum the number of process header(s) found in ehdr
 *
 *	@return non-zero if error
 */
int	check_64programheader(uint8_t *raw, Elf64_Off phoff, uint8_t phnum)
{
	(void)raw;(void)phoff;(void)phnum;
	return (0);
}
