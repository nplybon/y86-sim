/*
 * CS 261: Main driver
 *
 * Name: Nic Plybon
 */

#include "p1-check.h"

int main (int argc, char **argv)
{

    bool help = false;
    bool show = false;
    char* filename;
    if (argc == 3) {
        filename = argv[2];
    } else {
        filename = argv[1];
    }
    int c;
    while ((c = getopt(argc, argv, "hH")) != -1) {
        //checking for multiple args
        if (optind != argc - 1) {
            usage_p1(argv);
            return EXIT_FAILURE;
        }
        switch(c) {
            //diplay usage
            case 'h':
                help = true;
                show = false;
                break;
            //show the Mini-ELF header
            case 'H':
                show = true;
                break;
            default:
                usage_p1(argv);
                return EXIT_FAILURE;
        }

    }
    //check if file is valid
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to read file\n");
        return EXIT_FAILURE;
    }

    elf_hdr_t* header = malloc(sizeof(elf_hdr_t));
    read_header(file, header);
    unsigned char* str = header;
    //check if header is valid
    if (header->magic != 0x00464c45) {
        printf("Failed to read file\n");
        free(header);
        return EXIT_FAILURE;
    }
    //fixes mem leak when -H not passed
    if (!show) {
        free(header);
    }

    if (help) {
        //print standard help option
        usage_p1(argv);
        return EXIT_SUCCESS;
    }

    if (show) {
        for (int i = 0; i < 15; i++) {
            //for "double" space between bytes
            if (i == 8) {
                printf(" ");
            }
            printf("%02x ", str[i]);
        }
        printf("%02x", str[16]);
        printf("\nMini-ELF version %d\n", header->e_version);
        printf("Entry point 0x%x\n", header->e_entry);
        printf("There are %d program headers, starting at offset %d (0x%x)\n", header->e_num_phdr,
               header->e_phdr_start, header->e_phdr_start);
        if (header->e_symtab == 0) {
            printf("There is no symbol table present\n");
        } else {
            printf("There is a symbol table starting at offset %d (0x%x)\n", header->e_symtab,
                   header->e_symtab);
        }

        if (header->e_strtab == 0) {
            printf("There is no string table present\n");
        } else {
            printf("There is a string table starting at offset %d (0x%x)\n", header->e_strtab,
                   header->e_strtab);
        }
        free(header);

    }
    return EXIT_SUCCESS;
}

