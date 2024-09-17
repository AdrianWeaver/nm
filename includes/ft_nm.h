#ifndef FT_NM_H
 #define FT_NM_H

#include <sys/stat.h>

#define PAGESIZE 4096

#define ERROR -1

typedef struct s_mem
{
	char		*name;
	uint8_t		*raw;
	uint8_t		class;		//ELFCLASS32 or ELFCLASS64
	uint8_t		endianness;	//ELFDATA2LSB or ELFDATA2MSB
} t_mem;

typedef struct s_symbol {
	unsigned long int	value;
	char				type;
	char				*name;
} t_symbol;

uint8_t	*handleFile(char *target, int *fd, struct stat *st);

//utils
int	check_32programheader(uint8_t *raw, Elf32_Off offset, uint8_t phnum);
int	check_64programheader(uint8_t *raw, Elf64_Off offset, uint8_t phnum);

//check ehdr
int	e_ident_checker(t_mem *file);
int	check_ehdr(t_mem *file, struct stat *st);
int	check_ehdr_64lsb(t_mem *file, struct stat *st);
int	check_ehdr_32lsb(t_mem *file, struct stat *st);
int	check_ehdr_64msb(t_mem *file, struct stat *st);
int	check_ehdr_32msb(t_mem *file, struct stat *st);

//check phdr
int	check_phdr_64lsb(t_mem *file, Elf64_Ehdr *ehdr);
int	check_phdr_32lsb(t_mem *file, Elf32_Ehdr *ehdr);
int	check_phdr_64msb(t_mem *file, Elf64_Ehdr *ehdr);
int	check_phdr_32msb(t_mem *file, Elf32_Ehdr *ehdr);

//reverse endianness

uint16_t rev16(uint16_t origin);
uint32_t rev32(uint32_t origin);
uint64_t rev64(uint64_t origin);
#endif
