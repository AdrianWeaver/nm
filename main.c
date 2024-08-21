#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <elf.h>

#define ERROR -1

typedef struct s_mem
{
	uint8_t	*raw;
	uint8_t	class;
	uint8_t	endianness;
} t_mem;

int	check_file_stat(char *target, struct stat *st)
{
	if (stat(target, st) < 0)
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

int filehandler(char *target, uint8_t **mem, struct stat *st)
{
	int fd;

	if (check_file_stat(target, st) == ERROR)
		return (ERROR);
	if ((fd = open(target, O_RDONLY)) < 0)
		return (ERROR);
	if ((mem = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (close(fd), ERROR);
	return (close(fd), 0);
}

int	e_ident_checker(t_mem *file)
{
	uint8_t	*e_ident = ((Elf_Ehdr32 *)file->raw)->e_ident;

	if (e_ident[EI_MAG0] != 0x7f || e_ident[EI_MAG1] != 'E'
		|| e_ident[EI_MAG2] != 'L' || e_ident[EI_MAG3] != 'F')
		return (ERROR);
	if (!(e_ident[EI_CLASS] == ELFCLASS32 || e_ident[EI_CLASS] == ELFCLASS64))
		return (ERROR);
	file->class = e_ident[EI_CLASS];
	if (!(e_ident[EI_DATA] == ELFDATA2LSB || e_ident[EI_DATA] == ELFDATA2MSB))
		return (ERROR);
	file->endianness = file->raw[EI_DATA];
	if (!(e_ident[EI_VERSION] == EV_CURRENT))
		return (ERROR);
	return (0);
}

int	parse_ehdr(t_mem *file, struct stat *st, char *target)
{
	if (st->st_size < (int) sizeof(Elf64_Ehdr))
		goto format_error;
	if (e_ident_checker(file) == ERROR)
		goto format_error;
	if (file->class == ELFCLASS32)
	{
		Elf32_Ehdr *ehdr = (Elf32_Ehdr *) file;
		if (ehdr->e_type == ET_CORE)
			goto format_error;
	}
	if (file->class == ELFCLASS64)
	{
		Elf64_Ehdr *ehdr = (Elf64_Ehdr *) file;
		if (ehdr->e_type == ET_CORE)
			goto format_error;
		//check sh number
		//check sh size
		//each section header should be readable so e_shnum * e_shentsize
		//should not be bigger than the actual size of the header
	}
	return (0);
	format_error:
		return (fprintf(stderr, "nm: %s: file format not recognized\n", target), ERROR);
}

int file_routine(t_mem *file, struct stat *st, char *taget)
{
	int ret;

	if ((ret = parse_ehdr(file, st, target)))
		return (ret);
	return (0);
}

int main(int argc, char **argv)
{
	struct	stat st;
	uint8_t	*file;
	int		ret;

	ret = 0;
	if (argc == 1)
	{
		//one file "a.out"
		if (filehandler("a.out", &file, &st) == ERROR)
			return (1);
		if (file_routine(file, &st, "a.out"))
			ret++;
		munmap(file, st.st_size);
		return (ret);
	}
	for (int i = 1; i < argc; i++)
	{
		//one file per loop "argv[i]"
		if (filedhandler(argv[i], &file, &st) == ERROR)
		{
			ret += 1;
			continue;
		}
		if (file_routine(file, &st, "a.out"))
			ret++;
		munmap(file, st.st_size);
	}
	return (ret);
}
