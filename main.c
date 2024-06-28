#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define ERROR -1

typedef struct s_file
{
	int fd;
	struct stat st;

} t_file;

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

int	parse_ehdr(uint8_t *mem, )
{

}

int main(int argc, char **argv)
{
	struct	stat st;
	uint8_t	*mem;

	if (argc == 1)
	{
		//one file "a.out"
		filehandler("a.out", &mem, &st);
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
