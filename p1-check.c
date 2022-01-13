/*
 * CS 261 PA1: Mini-ELF header verifier
 *
 * Name: Nic Plybon
 */

#include "p1-check.h"

/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

bool read_header (FILE *file, elf_hdr_t *hdr)
{
    //check if file valid
    if (file == NULL || hdr == NULL) {
        printf("Failed to read file\n");
        return EXIT_FAILURE;
    }
    return fread(hdr, sizeof(elf_hdr_t), 1, file) == sizeof(elf_hdr_t);
}

/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p1 (char **argv)
{
    printf("Usage: %s <option(s)> mini-elf-file\n", argv[0]);
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
}

bool parse_command_line_p1 (int argc, char **argv, bool *print_header, char **filename)
{
    return true;
}

void dump_header (elf_hdr_t hdr)
{

}

