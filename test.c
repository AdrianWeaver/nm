#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <stdlib.h>
#include "libft.h"
#include "libftprintf.h"

int main(int argc, char **argv)
{
	int			fd;
	uint8_t		*mem;
	struct stat	st;
	unsigned char		*StringTable;
	char		*interp;
	char		*target;

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
	phdr = (Elf64_Phdr *)&mem[ehdr->e_phoff];	//program header offset
	shdr = (Elf64_Shdr *)&mem[ehdr->e_shoff];	//section header offset
	if (mem[2] != 0x7f && ft_strncmp((char *)&mem[1], "ELF",3))
	{
		ft_printf("%s: %s: file format not recognized\n", argv[0], argv[1]);
		exit(1);
	}
	//check for executable format
	printf("ET_NONE: %d\n", ET_NONE);
	printf("ET_REL: %d\n", ET_REL);
	printf("ET_EXEC: %d\n", ET_EXEC);
	printf("ET_DYN:	%d\n", ET_DYN);
	printf("ET_CORE: %d\n", ET_CORE);
	printf("e_type : %d\n", ehdr->e_type);

	//if (ehdr->e_type != ET_EXEC) {
		//ft_printf("%s is not an executable\n", argv[1]);
		//exit(1);
	//}
	(void)shdr;
	(void)phdr;
	(void)interp;
	printf("Program Entry point: 0x%016lx\n", ehdr->e_entry);

	/* the book states the following:
	 * we find the string table for the section header names
	 * with e_shstrndx
	 * which gives the index of which section holds the string table.
	*/
	StringTable = &mem[shdr[ehdr->e_shstrndx].sh_offset];
	//then we print each section header name and address:
	printf("ehdr->e_shnum: %d\n", ehdr->e_shnum);
	for (int i = 0; i < ehdr->e_shnum; i++){
		printf("%s: 0x%016lx\n", &StringTable[shdr[i].sh_name], shdr[i].sh_addr);
	}
	return (0);
}
