
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

/*
 *	Here is a short explanation of the bitshifting
 *	to swap bytes in order to reverse endianness
 *	Before swap:
 *		byte_4		byte_3		byte_2		byte_1
 *	|________|	|________|	|________|	|________|
 *
 *	After swap
 *		byte_1		byte_2		byte_3		byte_4
 *	|________|	|________|	|________|	|________|
 *
 */

/*
 *	@brief used to swap endianness on 16bits data.
 *
 *	@param origin the 16bits wide value to swap
 *	@return 16bits value with swapped endianness.
*/
uint16_t rev16(uint16_t origin)
{
	uint16_t byte_1;		//leftmost byte after swap
	uint16_t byte_2;		//leftmost byte after swap

	byte_1 = origin & 0xff;
	byte_2 = (origin >> 8) & 0xff;
	return ((uint16_t)(byte_1 << 8) | (byte_2));
}

/*
 *	@brief used to swap endianness on 16bits data.
 *
 *	@param origin the 16bits wide value to swap
 *	@return 32bits value with swapped endianness.
*/
uint32_t rev32(uint32_t origin)
{
	uint8_t byte_1;	//leftmost byte after reverse
	uint8_t byte_2;	//middle leftmost byte after reverse
	uint8_t byte_3;	//midle rightmost byte after reverse
	uint8_t byte_4;	//rightmost byte after reverse

	byte_1 = origin & 0xff;
	byte_2 = (origin >> 8) & 0xff;
	byte_3 = (origin >> 16) & 0xff;
	byte_4 = (origin >> 24) & 0xff;
	return ((uint32_t)(byte_1 << 24) | (uint32_t)(byte_2 << 16)
		| (uint32_t)(byte_3 << 8)  | (uint32_t)(byte_4));
}

/*
 *	@brief used to swap endianness on 16bits data.
 *
 *	@param origin the 16bits wide value to swap
 *	@return 32bits value with swapped endianness.
*/
uint64_t rev64(uint64_t origin)
{
	uint8_t byte_1;	//leftmost byte after reverse
	uint8_t byte_2;
	uint8_t byte_3;
	uint8_t byte_4;
	uint8_t byte_5;
	uint8_t byte_6;
	uint8_t byte_7;
	uint8_t byte_8;	//rightmost byte after reverse

	byte_1 = origin & 0xff;
	byte_2 = (origin >> 8) & 0xff;
	byte_3 = (origin >> 16) & 0xff;
	byte_4 = (origin >> 24) & 0xff;
	byte_5 = (origin >> 32) & 0xff;
	byte_6 = (origin >> 40) & 0xff;
	byte_7 = (origin >> 48) & 0xff;
	byte_8 = (origin >> 56) & 0xff;
	return (((uint64_t)byte_1 << 56) | ((uint64_t)byte_2 << 48)
		| ((uint64_t)byte_3 << 40)  | ((uint64_t)byte_4 << 32)
		| ((uint64_t)byte_5 << 24) | ((uint64_t)byte_6 << 16)
		| ((uint64_t)byte_7 << 8) | ((uint64_t)byte_8));
}
