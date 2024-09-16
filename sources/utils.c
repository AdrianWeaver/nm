
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
