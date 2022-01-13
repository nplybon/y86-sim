#ifndef __CS261_ELF__
#define __CS261_ELF__

#include <stdio.h>
#include <stdint.h>

/*
   Mini-ELF file format (byte 0 = first byte of the file)
   +----------------------------------------------+
   | header (elf_hdr_t) - 16 bytes                |
   +----------------------------------------------+
   | program headers (elf_phdr_t) - 20 bytes each |
   +----------------------------------------------+
   | program segments - variable length of bytes  |
   +----------------------------------------------+
   | symbol table - each entry is 4 bytes each    |
   +----------------------------------------------+
   | string table - variable length of strings    |
   +----------------------------------------------+

   ELF header structure:
   +----------------------------------------------------------------------------+
   |  0   1  |  2   3  |  4   5  |  6   7  |  8   9  | 10  11  | 12  13  14  15 |
   | version | entry   | phdr    | numphdr | symtab  | strtab  | magic number   |
   +----------------------------------------------------------------------------+

   Sample ELF header (all entries in hex, format is little endian):
   +----------------------------------------------------------------------------+
   |  01  00 |  00  01 |  10  00 |  02  00 |  58  00 |  70  00 | 45  4c  46  00 |
   | version | entry   | phdr    | numphdr | symtab  | strtab  | magic number   |
   +----------------------------------------------------------------------------+

   version = 0x0001     entry = 0x0100      phdr = 0x0010     numphdr = 0x0002
   symtab = 0x0058      strtab = 0x0070     magic = "ELF\0"

   Interpretation:

   This file was created under version 1 of this format. When the program is
   loaded into memory, the instruct at address 0x100 (256) will be executed
   first. The first program header (which indicates segments in this file)
   starts at offset 0x10 (16) into the file, and there are 2 program headers
   total. The symbol table starts at offset 0x58 (88) into this file, and the
   string table starts at offset 0x70 (112). The magic number is the string
   "ELF\0", stored in the elf_hdr_t format as a 4-byte integer 0x00464c45
   (4607045) and is used for checking the validity of the header.
*/
typedef struct __attribute__((__packed__)) elf {
    uint16_t e_version;     /* version should be 1 */
    uint16_t e_entry;       /* entry point of program */
    uint16_t e_phdr_start;  /* start of program headers */
    uint16_t e_num_phdr;    /* number of program headers */
    uint16_t e_symtab;      /* start of symbol table */
    uint16_t e_strtab;      /* start of string table */
    uint32_t magic;         /* ELF */
} elf_hdr_t;

#endif
