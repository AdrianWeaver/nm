#include <elf.h>
#include <stdio.h>
#include "ft_nm.h"

//#define DEBUG 1
const char *sh_type[] = { "SHT_NULL", "SHT_PROGBITS", "SHT_SYMTAB", "SHT_STRTAB", "SHT_RELA", "SHT_HASH", "SHT_DYNAMIC", "SHT_NOTE", "SHT_NOBITS", "SHT_REL", "SHT_SHLIB", "SHT_DYNSYM" , "12", "13", "SHT_INIT_ARRAY", "SHT_FINI_ARRAY", "SHT_PREINIT_ARRAY", "SHT_GROUP", "SHT_SYMTAB_SHNDX", "SHT_NUM" };
const char *st_type[] = { "STT_NOTYPE", "STT_OBJECT", "STT_FUNC", "STT_SECTION", "STT_FILE", "STT_COMMON", "STT_TLS", "STT_NUM" };
const char *st_bind[] = { "STB_LOCAL", "STB_GLOBAL", "STB_WEAK", "STB_NUM" };


char get_symbol_type_64lsb(t_mem *file, Elf64_Sym *symbol, t_symbol *sym)
{
	static int i;
		Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file->raw;
	Elf64_Shdr *shdr_table = (Elf64_Shdr *)&file->raw[ehdr->e_shoff];
	Elf64_Shdr *shdr = &shdr_table[symbol->st_shndx];

	unsigned char	symbol_type = ELF64_ST_TYPE(symbol->st_info);
	unsigned char	symbol_bind = ELF64_ST_BIND(symbol->st_info);
	uint64_t		executable_section = shdr->sh_flags & SHF_EXECINSTR;
	uint64_t		writable_section = shdr->sh_flags & SHF_WRITE;
	uint64_t		allocated_section = shdr->sh_flags & SHF_ALLOC;
	uint64_t		mergeable_section = shdr->sh_flags & SHF_MERGE;

	(void)sym; //kept for debuggin reasons
#if DEBUG
	printf("%3d: %39s section: %3u type: %16u %14s %5s %8s %9s %9s symbol type: %13s bind: %s\n", i, sym->name, symbol->st_shndx, shdr->sh_type, (shdr->sh_type < 20) ? sh_type[shdr->sh_type]: "type offlimit", (executable_section) ? "exec" : "", (writable_section)? "writable" : "", (allocated_section) ? "allocated" : "", (mergeable_section) ? "mergeable" : "", (symbol_type < 8) ? st_type[symbol_type]: "st type offlimit", st_bind[symbol_bind]);
#endif
	i++;
	if (symbol->st_shndx == SHN_UNDEF)
	{
		if (symbol_bind == STB_WEAK)
		{
			if (symbol_type == STT_OBJECT)
				return ('v');
			return ('w');
		}
		return ('U');
	}
	if (symbol->st_shndx == SHN_ABS)
		return ('A');
	if (symbol->st_shndx == SHN_COMMON)
	{
		if (symbol_bind == STB_LOCAL)
			return ('c');
		return ('C');
	}
	if (symbol_bind == STB_WEAK)
	{
		if (symbol_type == STT_OBJECT)
			return ('V');
		return ('W');
	}
	//.bss
	if (symbol_bind >= STB_LOOS && symbol_type <= STB_HIOS)
		return ('u');
	if (shdr->sh_type == SHT_PROGBITS && symbol_type == STT_FUNC)
	{
		if (symbol_bind == STB_LOCAL)
			return ('t');
		return ('T');
	}	if (writable_section && allocated_section && !executable_section && !mergeable_section)
	{
		if (shdr->sh_type == SHT_NOBITS)
		{
			if (symbol_bind == STB_LOCAL)
				return ('b');
			return ('B');
		}

		if (symbol_bind == STB_LOCAL)
			return ('d');
		return ('D');
	}
	if (allocated_section && !writable_section && !mergeable_section && !executable_section)
	{
		if (symbol_bind == STB_LOCAL)
			return ('r');
		return ('R');
	}
	if (mergeable_section && allocated_section)
	{
		if (symbol_bind == STB_LOCAL)
			return ('g');
		return ('G');
	}
	if (shdr->sh_type == SHT_NOBITS && allocated_section)
	{
		if (symbol_bind == STB_LOCAL)
			return ('s');
		return ('S');
	}
	if (!writable_section && !executable_section && !mergeable_section)
	{
		if (symbol_bind == STB_LOCAL)
			return ('n');
		return ('N');
	}
	//GNU SPECIFIC UNTESTED
	if ((shdr->sh_type == SHT_REL || shdr->sh_type == SHT_RELA)
			&& allocated_section && symbol_type == STT_FUNC)
	{
		if (symbol_bind == STB_LOCAL)
			return ('i');
		return ('I');
	}
	return ('?');
}

char get_symbol_type_32lsb(t_mem *file, Elf32_Sym *symbol)
{
	(void) file; (void) symbol; return ('0'); //no compilation errors
}

char get_symbol_type_64msb(t_mem *file, Elf64_Sym *symbol)
{
	(void) file; (void) symbol; return ('0'); //no compilation errors
}

char get_symbol_type_32msb(t_mem *file, Elf32_Sym *symbol)
{
	(void) file; (void) symbol; return ('0'); //no compilation errors
}

