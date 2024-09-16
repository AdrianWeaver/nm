#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <stdlib.h>
#include "libft.h"
#include "libftprintf.h"
#include "ft_nm.h"

Elf64_Shdr *getSymbolShdr(Elf64_Addr *mem, Elf64_Sym sym)
{
	Elf64_Shdr	*shdr;

	shdr = NULL;
	// common = c

	// undefined section
		//and weak = w
		//and weak and object
	(void)shdr; (void)mem; (void)sym;	//no errors at compilation
	return (shdr);

}

char get32SymbolType(Elf32_Sym sym, Elf64_Shdr *shdr)
{
	(void) sym;	(void) shdr;	//no errors at compilation
	return ('z');
}

char get64SymbolType(Elf64_Sym sym, Elf64_Shdr *shdr)
{
	(void) sym;	(void) shdr;	//no errors at compilation
	return ('z');
}

/* @brief prints the symbols given as a list.
 *
 * @param symList a t_list of symbols.
 * @return void
 */
void	printSymbols(t_list *symList)
{
	int j = 0;
	while (symList)
	{
		t_symbol *tmp;

		tmp = (t_symbol *) symList->content;
		if (!tmp)
		{
			symList = symList->next;
			continue;
		}
		printf("%016lx %c %s\n", tmp->value, tmp->type, (tmp->name ? tmp->name : ""));
		symList = symList->next;
		j++;
	}
}

uint8_t		*handleFile(char *target, int *fd, struct stat *st)
{
	uint8_t		*mem = NULL;

	if ((*fd = open(target, O_RDONLY)) < 0)
	{
		perror(target);
		return (mem);
	}
	if (fstat(*fd, st) < 0)
	{
		perror("nm: fstat'");
		close(*fd);
		return (mem);
	}
	mem = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, *fd, 0);
	close(*fd);
	if (mem == MAP_FAILED)
		return(NULL);
	return (mem);
}

t_list	*get64SymbolList(uint8_t *mem, struct stat *st, char *target)
{
	Elf64_Ehdr	*ehdr;
	Elf64_Shdr	*shdr;
	char		*stringTable;
	t_list		*symList = NULL;

	ehdr = (Elf64_Ehdr *)mem;
	#if PRINT_EHDR
	print_ehdr(ehdr);
	#endif
	shdr = (Elf64_Shdr *)&mem[ehdr->e_shoff];
	if (mem[2] != 0x7f && ft_strncmp((char *)&mem[1], "ELF",3))
	{
		ft_printf("ft_nm: %s: file format not recognized\n", target);
		munmap(mem, st->st_size);
		return (NULL);
	}
	stringTable = (char *)&mem[shdr[ehdr->e_shstrndx].sh_offset];
	for (Elf64_Off i = 0; i < ehdr->e_shnum; i++)
	{
		Elf64_Sym	*symTab;
		char		*symStrTab;

		symTab = (Elf64_Sym *)(mem + shdr[i].sh_offset);
		symStrTab = (char *) (mem + shdr[shdr[i].sh_link].sh_offset);

		print_64shdr(&shdr[i], stringTable);
		(void)stringTable;

		if (shdr[i].sh_type != SHT_SYMTAB && shdr[i].sh_type != SHT_DYNSYM)
			continue;
		for (Elf64_Off j = 0; j < (shdr[i].sh_size / sizeof(Elf64_Sym));j++)
		{
			t_symbol	*tmp;
			t_list		*new;
			tmp = malloc(sizeof(*tmp));
			if (!tmp)
			{
				perror("malloc");
				ft_lstclear(&symList, NULL);
				return (NULL);
			}
			tmp->value = symTab[j].st_value;
			tmp->type = get64SymbolType(symTab[j], getSymbolShdr((Elf64_Addr *)mem, symTab[j]));
			tmp->name = (symTab[j].st_name != 0 ? &symStrTab[symTab[j].st_name] : "");
			new = ft_lstnew(tmp);
			ft_lstadd_front(&symList, new);
		}
	}
	return (symList);
}
