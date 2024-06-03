#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <stdlib.h>
#include "libft.h"
#include "libftprintf.h"

const char *osabi[] = {
	"System V",
	"HP-UX",
	"NetBSD",
	"Linux",
	"GNU Hurd", "",
	"Solaris",
	"AIX",
	"IRIX",
	"FreeBSD",
	"Tru64",
	"Novell Modesto",
	"OpenBSD",
	"OpenVMS",
	"NonStop Kernel",
	"AROS",
	"FenixOS",
	"Nuxi CloudABI",
	"Stratus Technologies OpenVOS" };

const char *ftype[] = {
	"ET_NONE: Unknown",
	"ET_REL: Relocatable file",
	"ET_EXEC: Executable file",
	"ET_DYN: Shared object",
	"ET_CORE: Core file" };

char *get_architecture_name(Elf64_Ehdr *ehdr)
{
	switch (ehdr->e_machine)
	{
		case 0x0:
			return ("No specific instruction set");
		case 0x01:
			return ("AT&T WE 32100");
		case 0x02:
			return ("SPARC");
		case 0x03:
			return ("x86");
		case 0x04:
			return ("Motorola 68000 (M68k)");
		case 0x05:
			return ("Motorola 88000 (M88k)");
		case 0x06:
			return ("Intel MCU");
		case 0x07:
			return ("Intel 80860");
		case 0x08:
			return ("MIPS");
		case 0x09:
			return ("IBM System/370");
		case 0x0A:
			return ("MIPS RS3000 Little-endian");
		case 0x0B:
			return ("- 0x0E	Reserved for future use");
		case 0x0F:
			return ("Hewlett-Packard PA-RISC");
		case 0x13:
			return ("Intel 80960");
		case 0x14:
			return ("PowerPC");
		case 0x15:
			return ("PowerPC (64-bit)");
		case 0x16:
			return ("S390, including S390x");
		case 0x17:
			return ("IBM SPU/SPC");
		case 0x18:
			return ("- 0x23	Reserved for future use");
		case 0x24:
			return ("NEC V800");
		case 0x25:
			return ("Fujitsu FR20");
		case 0x26:
			return ("TRW RH-32");
		case 0x27:
			return ("Motorola RCE");
		case 0x28:
			return ("Arm (up to Armv7/AArch32)");
		case 0x29:
			return ("Digital Alpha");
		case 0x2A:
			return ("SuperH");
		case 0x2B:
			return ("SPARC Version 9");
		case 0x2C:
			return ("Siemens TriCore embedded processor");
		case 0x2D:
			return ("Argonaut RISC Core");
		case 0x2E:
			return ("Hitachi H8/300");
		case 0x2F:
			return ("Hitachi H8/300H");
		case 0x30:
			return ("Hitachi H8S");
		case 0x31:
			return ("Hitachi H8/500");
		case 0x32:
			return ("IA-64");
		case 0x33:
			return ("Stanford MIPS-X");
		case 0x34:
			return ("Motorola ColdFire");
		case 0x35:
			return ("Motorola M68HC12");
		case 0x36:
			return ("Fujitsu MMA Multimedia Accelerator");
		case 0x37:
			return ("Siemens PCP");
		case 0x38:
			return ("Sony nCPU embedded RISC processor");
		case 0x39:
			return ("Denso NDR1 microprocessor");
		case 0x3A:
			return ("Motorola Star*Core processor");
		case 0x3B:
			return ("Toyota ME16 processor");
		case 0x3C:
			return ("STMicroelectronics ST100 processor");
		case 0x3D:
			return ("Advanced Logic Corp. TinyJ embedded processor family");
		case 0x3E:
			return ("AMD x86-64");
		case 0x3F:
			return ("Sony DSP Processor");
		case 0x40:
			return ("Digital Equipment Corp. PDP-10");
		case 0x41:
			return ("Digital Equipment Corp. PDP-11");
		case 0x42:
			return ("Siemens FX66 microcontroller");
		case 0x43:
			return ("STMicroelectronics ST9+ 8/16 bit microcontroller");
		case 0x44:
			return ("STMicroelectronics ST7 8-bit microcontroller");
		case 0x45:
			return ("Motorola MC68HC16 Microcontroller");
		case 0x46:
			return ("Motorola MC68HC11 Microcontroller");
		case 0x47:
			return ("Motorola MC68HC08 Microcontroller");
		case 0x48:
			return ("Motorola MC68HC05 Microcontroller");
		case 0x49:
			return ("Silicon Graphics SVx");
		case 0x4A:
			return ("STMicroelectronics ST19 8-bit microcontroller");
		case 0x4B:
			return ("Digital VAX");
		case 0x4C:
			return ("Axis Communications 32-bit embedded processor");
		case 0x4D:
			return ("Infineon Technologies 32-bit embedded processor");
		case 0x4E:
			return ("Element 14 64-bit DSP Processor");
		case 0x4F:
			return ("LSI Logic 16-bit DSP Processor");
		case 0x8C:
			return ("TMS320C6000 Family");
		case 0xAF:
			return ("MCST Elbrus e2k");
		case 0xB7:
			return ("Arm 64-bits (Armv8/AArch64)");
		case 0xDC:
			return ("Zilog Z80");
		case 0xF3:
			return ("RISC-V");
		case 0xF7:
			return ("Berkeley Packet Filter");
		case 0x101:
			return ("WDC 65C816");
		case 0x102:
			return ("LoongArch");
		default:
			return ("unknown");
	}
}

void	print_ehdr(Elf64_Ehdr *ehdr)
{
	ft_printf("EHDR:\n%-16s: 0x%x, '%c%c%c'\n" //e_ident Magic Bytes
		"%-16s: %s\n%-16s: %s\n%-16s: %s\n%-16s: %s\n%-16s: %s\n"
		"%-16s: %d\n%-16s: 0x%016x\n%-16s: 0x%x\n%-16s: 0x%x\n"
		"%-16s: 0x%x\n%-16s: 0x%x\n%-16s: %d\n%-16s: 0x%x\n"
		"%-16s: %d\n%-16s: %d\n", "e_ident", ehdr->e_ident[EI_MAG0],
		ehdr->e_ident[EI_MAG1],ehdr->e_ident[EI_MAG2],
		ehdr->e_ident[EI_MAG3],
		"32/64bits", (ehdr->e_ident[EI_CLASS] == 1 ? "32bits" : "64bits"),
		"endianness", (ehdr->e_ident[EI_DATA] == 1 ? "little endian" : "big endian"),
		"OS ABI:", osabi[ehdr->e_ident[EI_OSABI]],
		"e_type", ftype[ehdr->e_type], "e_machine", get_architecture_name(ehdr),
		"e_version", ehdr->e_version, "e_entry", ehdr->e_entry,
		"e_phoff", ehdr->e_phoff, "e_shoff", ehdr->e_shoff,
		"e_ehsize", ehdr->e_ehsize, "e_phentsize", ehdr->e_phentsize,
		"e_phnum", ehdr->e_phnum, "e_shentsize", ehdr->e_shentsize,
		"e_shnum", ehdr->e_shnum, "e_shstrndx", ehdr->e_shstrndx);
}

char	*get_shtype(Elf64_Shdr *shdr)
{
	switch (shdr->sh_type) {
		case SHT_NULL:
			return "SHT_NULL: inactive section header";
		case SHT_PROGBITS:
			return "SHT_PROGBITS: format and meaning determined by program";
		case SHT_SYMTAB:
			return "SHT_SYMTAB: symbol table section";
		case SHT_STRTAB:
			return "SHT_STRTAB: string table section";
		case SHT_RELA:
			return "SHT_RELA: relocation entries with explicit addends";
		case SHT_HASH:
			return "SHT_HASH: symbol hash table";
		case SHT_DYNAMIC:
			return "SHT_DYNAMIC: holds info for dynamic linking";
		case SHT_NOTE:
			return "SHT_NOTE: holds notes";
		case SHT_NOBITS:
			return "SHT_NOBITS: contains no bytes";
		case SHT_REL:
			return "SHT_REL: relocation offset without addends";
		case SHT_SHLIB:
			return "SHT_SHLIB: reserved, unspecified";
		case SHT_DYNSYM:
			return "SHT_DYNSYM: minimal set of dynamic linking symbols";
		case SHT_HIPROC:
			return "SHT_HIPROC: reserved for provessor-specific semantics";
		case SHT_LOPROC:
			return "SHT_LOPROC: reserved for provessor-specific semantics";
		case SHT_LOUSER:
			return "SHT_LOUSER: range's lower bound indices reserved for app program";
		case SHT_HIUSER:
			return "SHT_HIUSER: range's high bound incides reserved for app program";
		default:
			if (shdr->sh_type > SHT_LOUSER && shdr->sh_type < SHT_HIUSER)
				return "Inbound indices reserved for app program";
			return "unknown";
	}
}

void	print_shdr(Elf64_Shdr *shdr, unsigned char *StringTable)
{
	ft_printf("%-16s: %s\n"		//name
			"%-16s: %s\n"		//type
			"%-16s: %o\n"		//flags
			"%-16s: %x\n"		//addr
			"%-16s: %x\n"		//offset
			"%-16s: %s\n"		//size
			"%-16s: %s\n"		//link
			"%-16s: %s\n"		//info
			"%-16s: %s\n"		//addralign
			"%-16s: %s\n",		//entsize
			StringTable[shdr->sh_name],
			get_shtype(shdr),
			shdr->sh_flags,
			shdr->sh_addr,
			shdr->sh_offset,
			shdr->sh_size,
			shdr->sh_link,
			shdr->sh_info,
			shdr->sh_addralign,
			shdr->sh_entsize);
}

int main(int argc, char **argv)
{
	int			fd;
	uint8_t		*mem;
	struct stat	st;
	unsigned char		*StringTable;
	char		*interp;
	char		*target;

	Elf64_Ehdr	*ehdr;		//initial ELF header
	Elf64_Phdr	*phdr;		//program header
	Elf64_Shdr	*shdr;		//section header

	target = argv[1];
	if (argc < 2){
		target = "a.out";
	}
	if ((fd = open(target, O_RDONLY)) < 0){
		exit(1);
	}
	if (fstat(fd, &st) < 0)
	{
		perror("fstat");
		exit(1);
	}
	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mem == MAP_FAILED)
	{
		perror("mmap");
		exit(1);
	}
	ehdr = (Elf64_Ehdr *)mem;
	print_ehdr(ehdr);
	phdr = (Elf64_Phdr *)&mem[ehdr->e_phoff];	//program header offset
	shdr = (Elf64_Shdr *)&mem[ehdr->e_shoff];	//section header offset
	if (mem[2] != 0x7f && ft_strncmp((char *)&mem[1], "ELF",3))
	{
		ft_printf("%s: %s: file format not recognized\n", argv[0], argv[1]);
		exit(1);
	}
	(void)shdr;
	(void)phdr;
	(void)interp;
	printf("Program Entry point: 0x%016lx\n", ehdr->e_entry);

	/* the book states the following:
	 * we find the string table for the section header names
	 * with e_shstrndx
	 * which gives the index of which section holds the string table.
	*/
	StringTable = &mem[shdr[ehdr->e_shstrndx].sh_offset];
	//Section header list
	printf("ehdr->e_shnum: %d\n", ehdr->e_shnum);
	for (int i = 0; i < ehdr->e_shnum; i++){
		printf("%-20s: 0x%016lx\n", &StringTable[shdr[i].sh_name], shdr[i].sh_addr);
	}
	//program header list
	for (int i = 0; i < ehdr->e_phnum; i++){
		switch (phdr[i].p_type)
		{
			case PT_LOAD: //loadable segment
				printf("%-20s: 0x%lx\n", (phdr[i].p_offset ? "Data segment" : "Text segment"), phdr[i].p_vaddr);
				break;
			case PT_INTERP: //null terminated pathname for interpreter
				interp = ((char *)(&mem[phdr[i].p_offset]));
				printf("%-20s: %s\n", "Interpreter", interp);
				break;
			case PT_NOTE:
				printf("%-20s: 0x%lx\n", "Note segment", phdr[i].p_vaddr);
				break;
			case PT_DYNAMIC:
				printf("%-20s: 0x%lx\n", "Dynamic segment", phdr[i].p_vaddr);
				break;
		}
	 }
	return (0);
}
