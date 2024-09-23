#include <stdio.h>
#include "ft_nm.h"

/*	@brief fils a uint8_t bitfield for activated options
 *	used to parse the inputs.
 *
 *	@param input a word to parse
 *	@param optionField the address of a bitfield used to store options
 *	@return 0 if the word can be a file, non-zero otherwise
 *	@retval ERROR if option does not exist, prints usage doc
 *	@retval NOTANOPTION if the word is not an option and can be a file
 *	@retval HELP if the -h was asked
 *	@retval SUCCESS if the option is allowed and registered
*/
int getOptions(char *input, uint8_t *optionField)
{
	if (input == NULL)
		return (ERROR);
	for (int i = 0; i != '\0'; i++)
	{
		if (i == 0 && input[i] != '-')
			return (NOTANOPTION);
		switch (input[i])
		{
			case 'a':
				*optionField |= (1 << OPTION_A);
				break;
			case 'g':
				*optionField |= (1 << OPTION_G);
				break;
			case 'u':
				*optionField |= (1 << OPTION_U);
				break;
			case 'r':
				*optionField |= (1 << OPTION_R);
				break;
			case 'p':
				*optionField |= (1 << OPTION_P);
				break;
			case 'h':
				*optionField |= (1 << OPTION_H);
				printUsage('h');
				return (HELP);
				break;
			default:
				printUsage(input[i]);
				return (ERROR);
		}
	}
	return (SUCCESS);
}

void	printUsage(char invalidOption)
{
	fprintf(stderr, "%s '%c'\n%s%s%s%s%s%s%s%s%s\n",
		(invalidOption == 'h') ? "" : "nm: invalid option --",
		invalidOption,
		"Usage: nm [option(s)] [file(s)]\n",
		"\tList symbols in [file(s)] (a.out by default).\n",
		"\tThe options are:\n",
		"-h\tDisplay this help\n"
		"-a\tDisplay debugger-only symbols\n",
		"-g\tDisplay all symbols even debugger-only symbols\n",
		"-u\tDisplay only undefined symbols\n",
		"-r\tReverse the sense of the sort\n",
		"-p\tDo not sort the symbols\n",
		"nm: supported targets: elf64-x86-64 elf32-x86-64"
	);
}
