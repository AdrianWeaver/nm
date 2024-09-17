	//ElfN_Off	e_shoff; 		-> triggers bfd errors
		//uint32_t	e_flags; 	-> not checked by nm
		//uint16_t	e_ehsize; 	-> not checked by nm
		//uint16_t	e_phentsize; -> not checked by nm
	//uint16_t	e_shentsize; -> format error/corrupt string table
	//uint16_t	e_shnum; -> leads to format error
	//if (ehdr->e_shnum == 0)
	//	goto format_error;
	//uint16_t	e_shstrndx;  -> this is complex
	/*
		if shstrnxd = SHN_UNDEF error message is
		nm: warning: file_name has a corrupt string table index - ignoring
		nm: file_name: no symbols
		same behaviour for broken shstrnxd but do not know how to check?
	*/
