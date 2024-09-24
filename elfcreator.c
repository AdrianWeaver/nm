#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <elf.h>
#include <sys/mman.h>
#include <limits.h>
#ifndef INPUTFILE
# define INPUTFILE "a.out"
#endif

#ifndef MODIFIED_FILE
# define MODIFIED_FILE "modified_elf"
#endif

#include <stdio.h>
//main to explain segfaults
int main(void)
{
	struct	stat st;
	int		fd;
	void	*mem_head;
	uint8_t	*mem;
	int		ret;
	int		to_write;

	stat(INPUTFILE, &st);
	to_write = st.st_size;
	printf("to_write is: %d\n", to_write);
	fd = open(INPUTFILE, O_RDONLY);
	if (fd < 0)
		return (fprintf(stderr, "the file %s does not exist\n", INPUTFILE), 1);
	mem = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
	mem_head = mem;
	close(fd);
	fd = open(MODIFIED_FILE, O_CREAT | O_RDWR | O_TRUNC, st.st_mode);
	if (fd < 0)
		return (fprintf(stderr, "Something went wrong with open, aborting\n"), 1);
	// modify below
	Elf64_Ehdr *ehdr = (Elf64_Ehdr*)mem;
	Elf64_Shdr *shdr_tab = (Elf64_Shdr *)(mem + ehdr->e_shoff);
	char *string_table = (char *)&mem[shdr_tab[ehdr->e_shstrndx].sh_offset]; (void) string_table;
	for (int i = 0; i < ehdr->e_shnum; i++)
	{
		Elf64_Shdr *shdr = &shdr_tab[i];
		(void)shdr;
		if (shdr->sh_entsize == 24)
		{
			printf("sh_type: %d - sh_entsize: %lu\n", shdr->sh_type, shdr->sh_entsize);
		}
		//if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM)
		if (shdr->sh_type == SHT_RELA)
		{
			shdr->sh_entsize = 32;
		}
		if (shdr->sh_type == SHT_SYMTAB_SHNDX)
			printf("yup\n");
	}
	// modify above
	while ((ret = write(fd, mem, to_write)) > 0)
	{
		mem += ret;
		to_write -= ret;
	}
	munmap(mem_head, st.st_size);
	close(fd);
	return (0);
}
