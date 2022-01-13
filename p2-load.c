/*
 * CS 261 PA2: Mini-ELF loader
 *
 * Name: Nic Plybon
 */

#include "p2-load.h"

/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

bool read_phdr (FILE *file, uint16_t offset, elf_phdr_t *phdr)
{
    if (file == NULL || phdr == NULL) {
        return false;
    }
    fseek(file, offset, SEEK_SET);
    if (fread(phdr, sizeof(elf_phdr_t), 1, file) != 1) {
        return false;
    }
    //check magic number is valid
    if (phdr->magic != 0xDEADBEEF) {
        return false;
    }



    return true;
}

bool load_segment (FILE *file, byte_t *memory, elf_phdr_t phdr)
{
    if (file == NULL || memory == NULL) {
        return false;
    }
    if (phdr.p_filesz + phdr.p_vaddr > MEMSIZE) {
        return false;
    }
    //load segment if filesz is 0
    if (phdr.p_filesz == 0) {
        return true;
    }

    if (fseek(file, phdr.p_offset, SEEK_SET) != 0) {
        return false;
    }
    if (fread(&memory[phdr.p_vaddr], phdr.p_filesz, 1, file) != 1) {
        return false;
    }
    return true;
}

/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p2 (char **argv)
{
    printf("Usage: %s <option(s)> mini-elf-file\n", argv[0]);
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
    printf("  -a      Show all with brief memory\n");
    printf("  -f      Show all with full memory\n");
    printf("  -s      Show the program headers\n");
    printf("  -m      Show the memory contents (brief)\n");
    printf("  -M      Show the memory contents (full)\n");
}

bool parse_command_line_p2 (int argc, char **argv,
                            bool *print_header, bool *print_segments,
                            bool *print_membrief, bool *print_memfull,
                            char **filename)
{
    if (argv == NULL || print_header == NULL || print_segments == NULL || print_membrief == NULL
            || print_memfull == NULL || filename == NULL) {
        return false;
    }
    //parameter parsing with getopt()
    int c;
    while ((c = getopt(argc, argv, "hHafsmM")) != -1) {
        switch(c) {
            //display usage
            case 'h':
                usage_p2(argv);
                return true;
            //show the Mini-ELF header
            case 'H':
                *print_header = true;
                break;
            //show all with brief memory
            case 'a':
                *print_header = true;
                *print_segments = true;
                *print_membrief = true;
                break;
            case 'f':
                *print_header = true;
                *print_segments = true;
                *print_memfull = true;
                break;
            //show the program headers
            case 's':
                *print_segments = true;
                break;
            //show the memory contents brief
            case 'm':
                *print_membrief = true;
                break;
            //show the memory contents full
            case 'M':
                *print_memfull = true;
                break;
            default:
                usage_p2(argv);
                return false;
        }
    }

    if (optind != argc -1) {
        //no filename
        usage_p2(argv);
        return false;
    }
    //save filename
    *filename = argv[optind];
    //check if brief and full flags passed at the same time
    if (*print_membrief && *print_memfull) {
        usage_p2(argv);
        return false;
    }
    return true;

}

void dump_phdrs (uint16_t numphdrs, elf_phdr_t phdr[])
{

    printf(" Segment   Offset    VirtAddr  FileSize  Type      Flag\n");
    for (int i = 0; i < numphdrs; i++) {
        printf("  %02d", i);
        printf("       0x%04x", phdr[i].p_offset);
        printf("    0x%04x", phdr[i].p_vaddr);
        printf("    0x%04x", phdr[i].p_filesz);
        int type = phdr[i].p_type;
        switch(type) {
            //DATA
            case 0:
                printf("    DATA");
                break;
            //CODE
            case 1:
                printf("    CODE");
                break;
            //STACK
            case 2:
                printf("    STACK");
                break;
            //HEAP
            case 3:
                printf("    HEAP");
                break;
            //UNKNOWN
            case 4:
                printf("    UNKNOWN");
                break;
        }
        //print spacing
        if (type == 2) {
            printf("     ");
        } else {
            printf("      ");
        }
        int flag = phdr[i].p_flag;


        switch(flag) {
            //EXECUTE
            case 1:
                printf("  X");
                break;
            //WRITE
            case 2:
                printf(" W ");
                break;
            //WRITE EXECUTE
            case 3:
                printf(" WX");
                break;
            //READ
            case 4:
                printf("R  ");
                break;
            //READ EXECUTE
            case 5:
                printf("R X");
                break;
            //READ WRITE
            case 6:
                printf("RW ");
                break;
            //READ WRITE EXECUTE
            case 7:
                printf("RWX");
                break;
        }
        printf("\n");

    }
}

void dump_memory (byte_t *memory, uint16_t start, uint16_t end)
{

    printf("Contents of memory from %04x to %04x:", start, end);

    for (int i = start; i < end; i++) {
        if (i % 16 == 0) {
            printf("\n  %04x  ",i);
        } else if (i % 8 == 0) {
            printf(" ");
        }
        //fix spacing for end of line
        if (i % 16 == 0) {
            printf("%02x", memory[i]);
        } else {
            printf(" %02x", memory[i]);
        }
    }
    printf("\n");
}