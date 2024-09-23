#include <stdint.h>

#ifndef FT_NM_H
 #define FT_NM_H

#include <sys/stat.h>

#define PAGESIZE 4096

#ifndef ERROR
# define ERROR -1
#endif

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
	char				*name;
} t_symbol;

//defined values for option parsing
enum OptionReturns {
	NOTANOPTION,
	OPTION_SUCCESS,
	OPTION_HELP,
	OPTION_ERROR,
};

enum OptionParser {
	OPTION_A,
	OPTION_G,
	OPTION_U,
	OPTION_R,
	OPTION_P,
	OPTION_H,
};

uint8_t	*handleFile(char *target, int *fd, struct stat *st);
int	file_routine(t_mem *file, uint8_t optionField);

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

//symbols
int	get_symbols(t_mem *file, uint8_t optionField);
int	_get_symbols_64lsb(t_mem *file, uint8_t optionField);
int	_get_symbols_32lsb(t_mem *file, uint8_t optionField);
int	_get_symbols_64msb(t_mem *file, uint8_t optionField);
int	_get_symbols_32msb(t_mem *file, uint8_t optionField);

//options
int		get_options_and_file_list(int argc, char **argv, char ***files, uint8_t *bitfield);
void	_get_options(char *input, uint8_t *optionField);
void	_printUsage(char invalidOption);

//reverse endianness
uint16_t	rev16(uint16_t origin);
uint32_t	rev32(uint32_t origin);
uint64_t	rev64(uint64_t origin);

#endif
