#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define ERROR -1

typedef struct s_mem
{
	uint8_t	*mem;
	uint8_t	class;
	uint8_t	endianness;
} t_mem;

int	check_file_stat(char *target, struct stat *st)
{
	if (stat(argv[i], &st) < 0)
	{
		printf("ft_nm: '%s': %s\n", target, strerror(errno));
		return (ERROR);
	}
	if (st->st_mode & S_IFDIR)
	{
		printf("ft_nm: Warning '%s' is a directory\n", target);
		return (ERROR);
	}
	return (0);
}

int	e_ident_checker(t_mem *file, char *target)
{
	if (file->mem[EI_MAG0] != 0x7f || file->mem[EI_MAG1] != 'E'
		|| file->mem[EI_MAG2] != 'L' || file->mem[EI_MAG3] != 'F')
		return (printf("nm: %s: file format not recognized\n", target), ERROR);
	if (!(file->mem[EI_CLASS] == ELFCLASS32 || file->mem[EI_CLASS] == ELFCLASS64))
		return (printf("nm: %s: file format not recognized\n", target), ERROR);
	file->class = file->mem[EI_CLASS];
	if (!(file->mem[EI_DATA] == ELFDATA2LSB || file->mem[EI_DATA] == ELFDATA2MSB))
		return (printf("nm: %s: file format not recognized\n", target), ERROR);
	file->endianness = file->mem[EI_DATA];
	return (0);
}

int filehandler(char *target, uint8_t **mem, struct stat *st)
{
	int fd;

	if (check_file_stat(target, st) == ERROR)
		return (ERROR);
	if ((fd = open(target, O_RDONLY)) < 0)
		return (ERROR);
	if ((mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (close(fd), ERROR);
	return (close(fd), 0);
}

int	parse_ehdr(t_mem *file, struct stat *st, char *target)
{
	if (st->st_size < sizeof(Elf64_Ehdr))
		return (printf("nm: %s: file format not recognized\n", target), ERROR);
	if (e_ident_checker(file, target) == ERROR)
		return (ERROR);
	if (file->class == ELFCLASS32)
	{
		Elf32_Ehdr *ehdr = (Elf32_Ehdr *) mem;
		ehdr->e_type;
	}
	if (file->class = ELFCLASS64)
	{
		Elf64_Ehdr *ehdr = (Elf32_Ehdr *) mem;
		//check shoff
		//check sh number
		//check sh size
		//each section header should be readable so e_shnum * e_shentsize
		//should not be bigger than the actual size of the header
	}
}

int main(int argc, char **argv)
{
	struct	stat st;
	uint8_t	*mem;

	if (argc == 1)
	{
		//one file "a.out"
		filehandler("a.out", &mem, &st);
		munmap(mem, st.st_size);
		return (0);
	}
	for (int i = 1; i < argc; i++)
	{
		//one file per loop "argv[i]"
		if (filedhandler(argv[i], &mem, &st) == ERROR)
			continue;
		munmap(mem, st.st_size);
	}
	return (0);
}
