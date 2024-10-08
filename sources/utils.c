
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
//it seems like this could be replace by builtin __bswap_16
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
//it seems like this could be replace by builtin __bswap_32
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
//it seems like this could be replace by builtin __bswap_64
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
