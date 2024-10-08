#include <stdint.h>
#include <elf.h>
#include "libft.h"

#ifndef FT_NM_H
 #define FT_NM_H

#include <sys/stat.h>

#define PAGESIZE 4096

#ifndef ERROR
# define ERROR -1
#endif

#define SHOW_TYPE_DETAILS 0
#define SHOW_SECTION_AS_DEBUG 0

typedef struct s_mem
{
	char		*name;
	uint8_t		*raw;
	uint8_t		class;		//ELFCLASS32 or ELFCLASS64
	uint8_t		endianness;	//ELFDATA2LSB or ELFDATA2MSB
	uint64_t	size;
} t_mem;

typedef struct s_symbol {
	unsigned long int	value;
	char				type;
	char		*name;
} t_symbol;

//defined values for option parsing
enum OptionReturns {
	NOTANOPTION,
	OPTION_SUCCESS,
	OPTION_HELP,
	OPTION_ERROR,
};

enum OptionParser {
	OPTION_NONE = 0,
	OPTION_A = (1 << 1),
	OPTION_G = (1 << 2),
	OPTION_U = (1 << 3),
	OPTION_R = (1 << 4),
	OPTION_P = (1 << 5),
	OPTION_H = (1 << 6),
	OPTION_MULTIPLE_FILES = (1 << 6),
};

uint8_t	*handleFile(char *target, int *fd, struct stat *st);
int	file_routine(t_mem *file, uint8_t optionField, t_bst **symbol_list);

//read protections
void *protected_read(const t_mem *mem, const uint8_t *addr, const uint8_t buffersize);
const char *protected_read_str(const t_mem *mem, const char *addr);

//check ehdr
int	check_ehdr(t_mem *file);
int	e_ident_checker(t_mem *file);
int	_check_ehdr_64lsb(const t_mem *file);
int	_check_ehdr_32lsb(const t_mem *file);
int	_check_ehdr_64msb(const t_mem *file);
int	_check_ehdr_32msb(const t_mem *file);

//check phdr
int	check_phdr(const t_mem *file);
int	_check_phdr_64lsb(const t_mem *file);
int	_check_phdr_32lsb(const t_mem *file);
int	_check_phdr_64msb(const t_mem *file);
int	_check_phdr_32msb(const t_mem *file);

//check shdr
int	check_shdr(const t_mem *file);
int	_check_shdr_64lsb(const t_mem *file);
int	_check_shdr_32lsb(const t_mem *file);
int	_check_shdr_64msb(const t_mem *file);
int	_check_shdr_32msb(const t_mem *file);
int	_check_sh_type_64lsb(const t_mem *file, const Elf64_Shdr *shdr, const unsigned char *section_name);
int	_check_sh_type_32lsb(const t_mem *file, const Elf32_Shdr *shdr, const unsigned char *section_name);
int	_check_sh_type_64msb(const t_mem *file, const Elf64_Shdr *shdr, const unsigned char *section_name);
int	_check_sh_type_32msb(const t_mem *file, const Elf32_Shdr *shdr, const unsigned char *section_name);

//sections
int	get_section_as_symbol(const t_mem *file, uint8_t option_field, t_bst **symbol_list);
int	_get_section_as_symbol_64lsb(const t_mem *file, uint8_t option_field, t_bst **symbol_list);
int	_get_section_as_symbol_32lsb(const t_mem *file, uint8_t option_field, t_bst **symbol_list);
int	_get_section_as_symbol_64msb(const t_mem *file, uint8_t option_field, t_bst **symbol_list);
int	_get_section_as_symbol_32msb(const t_mem *file, uint8_t option_field, t_bst **symbol_list);

//symbols
int	get_symbols(t_mem *file, uint8_t optionField, t_bst **symbol_list);
int	_get_symbols_64lsb(t_mem *file, uint8_t option_field, t_bst **symbol_list);
int	_get_symbols_32lsb(t_mem *file, uint8_t option_field, t_bst **symbol_list);
int	_get_symbols_64msb(t_mem *file, uint8_t option_field, t_bst **symbol_list);
int	_get_symbols_32msb(t_mem *file, uint8_t option_field, t_bst **symbol_list);
//sort order
int _symbol_sort_order(void *new, void *inplace);
int _symbol_no_sort(void *lhs, void *rhs);

//type
char get_symbol_type_64lsb(t_mem *file, Elf64_Sym *symbol, t_symbol *sym);
char get_symbol_type_32lsb(t_mem *file, Elf32_Sym *symbol, t_symbol *sym);
char get_symbol_type_64msb(t_mem *file, Elf64_Sym *symbol);
char get_symbol_type_32msb(t_mem *file, Elf32_Sym *symbol);

//options
int		get_options_and_file_list(int argc, char **argv, char ***files, uint8_t *bitfield);
void	_get_options(char *input, uint8_t *optionField);
void	_printUsage(char invalidOption);

//reverse endianness
uint16_t	rev16(uint16_t origin);
uint32_t	rev32(uint32_t origin);
uint64_t	rev64(uint64_t origin);

#endif
