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

#define ERROR -1

typedef struct s_mem
{
	uint8_t		*raw;
	uint8_t		class;
	uint8_t	endianness;
} t_mem;

/*	@brief checking missing or forbidden files or directories
 *
 * @param target the name of the file to be read
 * @parem st an address to store the file stats in case of success
 *
 * @return non-zero in case of errors (writing on stderr)
 */
int	check_file_stat(char *target, struct stat *st)
{
	if (stat(target, st) < 0)
	{
		fprintf(stderr, "ft_nm: '%s': %s\n", target, strerror(errno));
		return (ERROR);
	}
	if (st->st_mode & S_IFDIR)
	{
		fprintf(stderr, "ft_nm: Warning '%s' is a directory\n", target);
		return (ERROR);
	}
	return (0);

}

/* @brief opens and check for errors in the file
 *
 * @param target name of the file
 * @param file t_mem address to store raw file obtained and infos
 * @return non-zero in case of error (without printing)
 */
int filehandler(char *target, t_mem *file, struct stat *st)
{
	int fd;

	if (check_file_stat(target, st) == ERROR)
		return (ERROR);
	if ((fd = open(target, O_RDONLY)) < 0)
		return (ERROR);
	if ((file->raw = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (close(fd), ERROR);
	return (close(fd), 0);
}

/* @brief checks if the file is an ELF, class: 32/64, endianness
 *
 * @param file t_mem storing raw file and infos
 *
 * @return non-zero in case of error (without printing)
 */
int	e_ident_checker(t_mem *file)
{
	uint8_t	*e_ident = ((Elf32_Ehdr *)file->raw)->e_ident;

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

/*
 * @brief checking elf header for errors
 *
 * @param file t_mem storing raw file and infos
 * @param st stats of the file being read
 * @target the name of the file
 *
 * @return non-zero in case of error
 */
int	parse_ehdr(t_mem *file, struct stat *st, char *target)
{
	//the file is too short
	if (st->st_size < (int) sizeof(Elf64_Ehdr))
		goto format_error;
	//the file is an elf
	if (e_ident_checker(file) == ERROR)
		goto format_error;
	//if the file is 32bits
	if (file->class == ELFCLASS32)
	{
		Elf32_Ehdr *ehdr = (Elf32_Ehdr *) file;
		//check e_type only core files are errors
		if (ehdr->e_type == ET_CORE)
			goto format_error;
		//uint16_t	e_machine (architecture) -> not checked by nm
			//uint16_t	e_version (current or invalid) -> not checked by nm
			//ElfN_Addr	e_entry;	-> not checked by nm
		//ElfN_Off	e_phoff;		-> if too much, check alignment if out of bounds format error
		//uint8_t	e_phnum;		-> cannot be greater than PN_XNUM, need to be checked
	if (check_32programheader(file->raw, ehdr->e_phoff, ehdr->e_phnum) < 0)
		goto format_error;
		//ElfN_Off	e_shoff; 		-> triggers bfd errors
			//uint32_t	e_flags; 	-> not checked by nm
			//uint16_t	e_ehsize; 	-> not checked by nm
			//uint16_t	e_phentsize; -> not checked by nm
		//uint16_t	e_phnum; -> checked by nm, max value is PN_XNUM and behaviour changes if it's the case
		//need to have a custom function check for this
	//uint16_t	e_shentsize; -> format error/corrupt string table
	//uint16_t	e_shnum; -> leads to format error
	if (ehdr->e_shnum == 0)
		goto format_error;
	//uint16_t	e_shstrndx;  -> this is complex
	/*
		if shstrnxd = SHN_UNDEF error message is
		nm: warning: file_name has a corrupt string table index - ignoring
		nm: file_name: no symbols
		same behaviour for broken shstrnxd but do not know how to check?
	*/
	if (ehdr->e_shstrndx == SHN_UNDEF)
		return (fprintf(stderr, "nm: %s: file has a corrupt string table index\n", target), ERROR);
	}
	//if the file is 64bits
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
	//TODO: check for errors on shoff shnum shentsize.
	//This can be a first check to see if the values are even possible
	//then the rest can be checked when the section headers are actually accessed
	return (0);
	format_error:
		return (fprintf(stderr, "nm: %s: file format not recognized\n", target), ERROR);
}

/* @brief main file routine, handling the launch of checks and features
 *
 * @param file t_mem storing raw file and infos
 * @param st stats of the file being read
 * @target the name of the file
 *
 * @return non-zero in case of error
 */
int file_routine(t_mem *file, struct stat *st, char *target)
{
	int ret;

	if ((ret = parse_ehdr(file, st, target)))
		return (ret);
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
		if (filehandler(to_read[i], &file, &st) == ERROR)
		{
			ret += 1;
			continue;
		}
		if (file_routine(&file, &st, "a.out"))
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
