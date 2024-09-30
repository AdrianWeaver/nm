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
int file_handler(t_mem *file, struct stat *st)
{
	int fd;

	if (check_file_stat(file->name, st) == ERROR)
		return (ERROR);
	if ((fd = open(file->name, O_RDONLY)) < 0)
		return (ERROR);
	if ((file->raw = mmap(NULL, st->st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
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
int file_routine(t_mem *file, uint8_t option_field, t_bst **symbol_list)
{
	if (check_ehdr(file) == ERROR)
		return (ERROR);
	if (check_phdr(file) == ERROR)
		return (ERROR);
	if (check_shdr(file) == ERROR)
		return (ERROR);
	if (option_field & OPTION_A)
		get_section_as_symbol(file, option_field, symbol_list);
	if (get_symbols(file, option_field, symbol_list) == ERROR)
		return (ERROR);
	return (0);
}

//options:
// -a : display all symbols even debugger-only symbols
// -g : display only external (global) symbols
// -u : display only undefined symbols (external to each object file)
// -r : reverse sort
// -p : no-sort
int main(int argc, char **argv)
{
	int		ret = 0;
	char	**files = argv;
	int		file_number;
	uint8_t	option_field = 0;

	file_number = get_options_and_file_list(argc, argv, &files, &option_field);
	if (file_number > 1)
		option_field |= OPTION_MULTIPLE_FILES;
	for (int i = 0; i < file_number; i++)
	{
		struct	stat st;
		t_mem	file;
		t_bst	*symbol_list = NULL;
		//one file per loop "argv[i]"
		file.name = files[i];
		if (file_handler(&file, &st) == ERROR)
		{
			ret++;
			continue;
		}
		if (file_routine(&file, option_field, &symbol_list))
			ret++;
		munmap(file.raw, st.st_size);
	}
	if (argc == 1)
	{
		free(files[1]);
		free(files);
	}
	return (ret);
}
