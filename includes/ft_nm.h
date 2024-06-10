#ifndef FT_NM_H
 #define FT_NM_H

typedef struct s_symbol {
	uint8_t		*value;
	char		type;
	char		*name;
} t_symbol;

char	*get_architecture_name(Elf64_Ehdr *ehdr);
void	print_ehdr(Elf64_Ehdr *ehdr);
char	*get_shtype(Elf64_Shdr *shdr);
void	print_shdr(Elf64_Shdr *shdr, unsigned char *StringTable);
#endif
