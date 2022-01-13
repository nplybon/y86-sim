/*
 * CS 261: Main driver
 *
 * Name: Nic Plybon
 */

#include "p1-check.h"
#include "p2-load.h"

int main (int argc, char **argv)
{

    // parse command-line options
    bool print_header = false;
    bool print_segments = false;
    bool print_membrief = false;
    bool print_memfull = false;
    char *fn = NULL;
    if (!parse_command_line_p2(argc, argv, &print_header, &print_segments, &print_membrief,
                               &print_memfull, &fn)) {
        return EXIT_FAILURE;
    }
    if (fn != NULL) {
        // open Mini-ELF binary
        FILE *f = fopen(fn, "r");
        if (!f) {
            printf("Failed to read file\n");

            return EXIT_FAILURE;
        }

        // P1: load and check Mini-ELF header
        elf_hdr_t hdr;
        if (!read_header(f, &hdr)) {
            printf("Failed to read file\n");

            return EXIT_FAILURE;
        }

        // P1 output
        if (print_header) {
            dump_header(hdr);
        }

        // P2 read phdrs
        byte_t* mem = (byte_t*)calloc(MEMSIZE, 1);

        struct elf_phdr phdrs[hdr.e_num_phdr];
        int offset = hdr.e_phdr_start;
        for (int i = 0; i < hdr.e_num_phdr; i++) {
            if (!read_phdr(f, offset, &phdrs[i])) {
                printf("Failed to read file\n");
                free(mem);

                return EXIT_FAILURE;
            }
            //phdrs are 20 bytes
            offset += 20;
        }

        // P2 print segments
        if (print_segments) {
            dump_phdrs(hdr.e_num_phdr, phdrs);
        }


        // P2 print membrief
        if (print_membrief) {
            for (int i = 0; i < hdr.e_num_phdr; i++) {
                if (!load_segment(f, mem, phdrs[i])) {
                    printf("Failed to read file\n");
                    free(mem);
                    return EXIT_FAILURE;

                }
                dump_memory(mem, phdrs[i].p_vaddr, phdrs[i].p_vaddr + phdrs[i].p_filesz);
            }
        }
        // P2 print membrief
        if (print_memfull) {
            for (int i = 0; i < hdr.e_num_phdr; i++) {
                if (!load_segment(f, mem, phdrs[i])) {
                    //printf("Failed to read file\n");
                    free(mem);
                    return EXIT_FAILURE;
                }
            }
            dump_memory(mem, 0, MEMSIZE);
        }

        // cleanup
        fclose(f);
        free(mem);


    }

    return EXIT_SUCCESS;
}

