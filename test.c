#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <elf.h>
#include <sys/mman.h>


//main to explain segfaults
int main(void)
{
	struct	stat st;
	int		fd;
	uint8_t	*mem;
	int		ret;
	int		to_write;

	stat("a.out", &st);
	to_write = st.st_size;
	fd = open("a.out", O_RDONLY);
	mem = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	fd = open("truncated", O_CREAT | O_RDWR, 0750);
	((Elf64_Ehdr*) mem)->e_shoff = 0;
	while ((ret = write(fd, mem, to_write) > 0))
	{
		to_write -= ret;
	}
	munmap(mem, st.st_size);
	close(fd);
	return (0);
}
