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

int main(int argc, char **argv)
{
	int			fd;
	uint8_t		*mem;
	struct stat	st;
	unsigned char		*StringTable;
	char		*interp;
	char		*target;
	static int	test;
	(void)test;

	Elf64_Ehdr	*ehdr;		//initial ELF header
	Elf64_Phdr	*phdr;		//program header
	Elf64_Shdr	*shdr;		//section header

	target = argv[1];
	if (argc < 2){
		target = "a.out";
	}
	if ((fd = open(target, O_RDONLY)) < 0){
		exit(1);
	}
	if (fstat(fd, &st) < 0)
	{
		perror("fstat");
		exit(1);
	}
	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mem == MAP_FAILED)
	{
		perror("mmap");
		exit(1);
	}
	ehdr = (Elf64_Ehdr *)mem;
	print_ehdr(ehdr);
	phdr = (Elf64_Phdr *)&mem[ehdr->e_phoff];	//program header offset
	shdr = (Elf64_Shdr *)&mem[ehdr->e_shoff];	//section header offset
	if (mem[2] != 0x7f && ft_strncmp((char *)&mem[1], "ELF",3))
	{
		ft_printf("%s: %s: file format not recognized\n", argv[0], argv[1]);
		exit(1);
	}
	printf("Program Entry point: 0x%016lx\n", ehdr->e_entry);

	/* the book states the following:
	 * we find the string table for the section header names
	 * with e_shstrndx
	 * which gives the index of which section holds the string table.
	*/
	StringTable = &mem[shdr[ehdr->e_shstrndx].sh_offset];
	//Section header list
	printf("ehdr->e_shnum: %d\n", ehdr->e_shnum);
	for (int i = 0; i < ehdr->e_shnum; i++)
	{
		printf("%-20s: 0x%016lx\n", &StringTable[shdr[i].sh_name], shdr[i].sh_addr);
		if (shdr[i].sh_type != SHT_SYMTAB && shdr[i].sh_type != SHT_DYNSYM)
			continue;
		for (Elf64_Addr j = 0; j < (shdr[i].sh_size / sizeof(Elf64_Sym));j++)
		{
			Elf64_Sym *symtab = (Elf64_Sym *)(mem + shdr[i].sh_offset);
			char *strtab = (char *)(mem + shdr[shdr[i].sh_link].sh_offset);
			//if (symtab[j].st_name == 0)
				//continue;
			printf("value: %016lx  - symbol: %s\n", symtab[j].st_value, strtab + symtab[j].st_name);
			printf("bind: %d - type: %d\n", ELF64_ST_BIND(symtab[j].st_info), ELF64_ST_TYPE(symtab[j].st_info));
			switch (symtab[j].st_shndx) {
			case SHN_ABS:
				printf("is absolute\n");
				break;
			case SHN_COMMON:
				printf("is common\n");
				break;
			case SHN_UNDEF:
				printf("is undefined\n");
				break;
			case SHN_XINDEX:
				printf("contained in extended SHT_SYMTAB_SHNDX\n");
				break;
			default:
			if (symtab[j].st_shndx < 0xff00)
				printf("contained in: %s\n", &StringTable[shdr[symtab[j].st_shndx].sh_name]);
			}
		}
		printf("SHN_ABS: %d\nSHN_COMMON: %d\nSHN_UNDEF: %d\nSHN_XINDEX: %d\n",SHN_ABS, SHN_COMMON, SHN_UNDEF, SHN_XINDEX);
	}
	//program header list
	for (int i = 0; i < ehdr->e_phnum; i++){
		switch (phdr[i].p_type)
		{
			case PT_LOAD: //loadable segment
				printf("%-20s: 0x%lx\n", (phdr[i].p_offset ? "Data segment" : "Text segment"), phdr[i].p_vaddr);
				break;
			case PT_INTERP: //null terminated pathname for interpreter
				interp = ((char *)(&mem[phdr[i].p_offset]));
				printf("%-20s: %s\n", "Interpreter", interp);
				break;
			case PT_NOTE:
				printf("%-20s: 0x%lx\n", "Note segment", phdr[i].p_vaddr);
				break;
			case PT_DYNAMIC:
				printf("%-20s: 0x%lx\n", "Dynamic segment", phdr[i].p_vaddr);
				break;
		}
	 }
	return (0);
}
