#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <elf.h>
#include <stdlib.h>
#include "libft.h"
#include "ft_nm.h"

/*	@brief checking missing or forbidden files or directories
 *
 * @param filename the name of the file to be read
 * @param st an address to store the file stats in case of success
 * @return non-zero in case of errors (writing on stderr)
*/
int	check_file_stat(char *filename, struct stat *st)
{
	if (stat(filename, st) < 0)
	{
		fprintf(stderr, "ft_nm: '%s': %s\n", filename, strerror(errno));
		return (ERROR);
	}
	if (st->st_mode & S_IFDIR)
	{
		fprintf(stderr, "ft_nm: Warning '%s' is a directory\n", filename);
		return (ERROR);
	}
	return (0);
}

/* @brief opens and check for errors in the file
 *
 * @param file t_mem address to store mapped file obtained and infos
 * @return non-zero in case of error (without printing)
*/
int filehandler(t_mem *file, struct stat *st)
{
	int fd;

	if (check_file_stat(file->name, st) == ERROR)
		return (ERROR);
	if ((fd = open(file->name, O_RDONLY)) < 0)
		return (ERROR);
	if ((file->raw = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (close(fd), ERROR);
	//the file is too short to even have an ehdr
	close(fd);
	if (st->st_size < (int) sizeof(Elf64_Ehdr))
		return(fprintf(stderr, "nm: %s: file format not recognized\n", file->name), ERROR);
	file->size = st->st_size;
	return (0);
}

/* @brief main file routine, handling the launch of checks and features
 *
 * @param file t_mem storing mapped file and infos
 * @param st stats of the file being read
 *
 * @return non-zero in case of error
 */
int file_routine(t_mem *file)
{
	if (check_ehdr(file) == ERROR)
		return (ERROR);
	if (check_phdr(file) == ERROR)
		return (ERROR);
	if (check_shdr(file) == ERROR)
		return (ERROR);
	return (0);
}

int main(int argc, char **argv)
{
	struct	stat st;
	t_mem	file;
	int		ret;
	char	**to_read;

	ret = 0;
	to_read = argv;
	if (argc == 1)
	{

		to_read = malloc(sizeof(*to_read) * 2);
		to_read[0] = argv[0];
		to_read[1] = ft_strdup("a.out");
		argc++;
	}
	for (int i = 1; i < argc; i++)
	{
		//one file per loop "argv[i]"
		file.name = to_read[i];
		if (filehandler(&file, &st) == ERROR)
		{
			ret++;
			continue;
		}
		if (file_routine(&file))
			ret++;
		munmap(file.raw, st.st_size);
	}
	if (to_read != argv)
	{
		free(to_read[1]);
		free(to_read);
	}
	return (ret);
}
