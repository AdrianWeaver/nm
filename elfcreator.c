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


//main to explain segfaults
int main(void)
{
	struct	stat st;
	int		fd;
	uint8_t	*mem;
	int		ret;
	int		to_write;

	stat(INPUTFILE, &st);
	to_write = st.st_size;
	fd = open(INPUTFILE, O_RDONLY);
	if (fd < 0)
		return (fprintf(stderr, "the file %s does not exist\n", INPUTFILE), 1);
	mem = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	fd = open(MODIFIED_FILE, O_CREAT | O_RDWR, 0750);
	// modify below

	((Elf64_Ehdr*) mem)->e_shoff = 0;
	// modify above
	while ((ret = write(fd, mem, to_write) > 0))
	{
		to_write -= ret;
	}
	munmap(mem, st.st_size);
	close(fd);
	return (0);
}
