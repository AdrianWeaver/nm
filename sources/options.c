#include <stdio.h>
#include <stdlib.h>
#include "libft.h"
#include "ft_nm.h"

/*	@brief parse argc/argv stores filenames in char **, options in bitfield
 *
 *	@param argc
 *	@param argv
 *	@param to_read char** to store filename(s) to be read
 *	@param option_field bitfield to store options as flags
 *
 *	@return number of files to be read
*/
int get_options_and_file_list(int argc, char **argv, char ***files, uint8_t *option_field)
{
	int file_number = 0;
	if (argc == 1)
	{
		*files = malloc(sizeof(*files) * 1);
		if (!*files)
			exit(1);
		(*files)[0] = ft_strdup("a.out");
		return (1);
	}
	for (int i = 1; i < argc; i++)
	{
		if (*(argv[i]) != '-')
		{
			argv[file_number] = argv[i];
			file_number++;
			continue;
		}
		if (*(argv[i]) != '\0')
			_get_options(argv[i], option_field);
	}
	return (file_number);
}

void	_get_options(char *input, uint8_t *option_field)
{
	for (int i = 1; input[i] != '\0'; i++)
	{
		switch (input[i])
		{
			case 'a':
				*option_field |= (1 << OPTION_A);
				break;
			case 'g':
				*option_field |= (1 << OPTION_G);
				break;
			case 'u':
				*option_field |= (1 << OPTION_U);
				break;
			case 'r':
				*option_field |= (1 << OPTION_R);
				break;
			case 'p':
				*option_field |= (1 << OPTION_P);
				break;
			case 'h':
				*option_field |= (1 << OPTION_H);
				_printUsage('h');
				exit(0);;
			default:
				_printUsage(input[i]);
				exit(1);
		}
	}
	return ;
}

void	_printUsage(char invalidOption)
{
	if (invalidOption != 'h')
	{
		fprintf(stderr, "nm: invalid option -- '%c'\n", invalidOption);
	}
	fprintf(stderr, "%s%s%s%s%s%s%s%s%s\n",
		"Usage: nm [option(s)] [file(s)]\n",
		" List symbols in [file(s)] (a.out by default).\n",
		"  The options are:\n",
		"  -h Display this help\n"
		"  -a Display debugger-only symbols\n",
		"  -g Display all symbols even debugger-only symbols\n",
		"  -u Display only undefined symbols\n",
		"  -r Reverse the sense of the sort\n",
		"  -p Do not sort the symbols\n",
		"nm: supported targets: elf64-x86-64 elf32-x86-64"
	);
}
