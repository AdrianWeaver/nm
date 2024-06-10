#include <elf.h>

char get64SymbolType(Elf64_Sym *sym)
{
	(void) sym;
	return ('z');
}
