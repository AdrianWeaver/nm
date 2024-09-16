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
	void	*mem;
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

	((Elf64_Ehdr*) mem)->e_phoff = PN_XNUM + 1;
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
