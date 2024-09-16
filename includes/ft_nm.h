#ifndef FT_NM_H
 #define FT_NM_H

typedef struct s_symbol {
	unsigned long int	value;
	char				type;
	char				*name;
} t_symbol;

uint8_t	*handleFile(char *target, int *fd, struct stat *st);
void	printSymbols(t_list *symList);
char	*get_architecture_name(Elf64_Ehdr *ehdr);
void	print_ehdr(Elf64_Ehdr *ehdr);
char	*get_shtype(Elf64_Shdr *shdr);
void	print_64shdr(Elf64_Shdr *shdr, char *stringTable);
t_list	*get64SymbolList(uint8_t *mem, struct stat *st, char *target);

//utils
int	check_32programheader(uint8_t *raw, Elf32_Off offset, uint8_t phnum);
int	check_64programheader(uint8_t *raw, Elf64_Off offset, uint8_t phnum);
#endif
