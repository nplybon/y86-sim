/*
 * CS 261: Main driver
 *
 * Name: Nic Plybon
 */

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"

int main (int argc, char **argv)
{
	//parse command-line options
    bool print_header = false;
    bool print_segments = false;
    bool print_membrief = false;
    bool print_memfull = false;
	bool disas_code = false;
	bool disas_data = false;
    char *fn = NULL;
    if (!parse_command_line_p3(argc, argv, &print_header, &print_segments, &print_membrief,
                               &print_memfull, &disas_code, &disas_data, &fn)) {
        return EXIT_FAILURE;
    }
	if (fn != NULL) {
        //open Mini-ELF binary
        FILE *f = fopen(fn, "r");
        if (!f) {
            printf("Failed to read file\n");

            return EXIT_FAILURE;
        }

        //P1 load and check Mini-ELF header
        elf_hdr_t hdr;
        if (!read_header(f, &hdr)) {
            printf("Failed to read file\n");
            return EXIT_FAILURE;
        }

        //P1 output
        if (print_header) {
            dump_header(hdr);
        }
		
		//P2 read phdrs
		struct elf_phdr phdrs[hdr.e_num_phdr];
        for (int i = 0, offset = hdr.e_phdr_start; i < hdr.e_num_phdr; i++) {
            if (!read_phdr(f, offset, &phdrs[i])) {
                printf("Failed to read file\n");
                return EXIT_FAILURE;
            }
			offset += 20;
        }

        //P2 print segment
        if (print_segments) {
            dump_phdrs(hdr.e_num_phdr, phdrs);
        }

        //P2 load segment
        byte_t* mem = (byte_t*)calloc(MEMSIZE, 1);
        for (int i = 0; i < hdr.e_num_phdr; i++) {
            if (!load_segment(f, mem, phdrs[i])) {
                printf("Failed to read file\n");
                return EXIT_FAILURE;
            }

			//P2 print memory brief
            if (print_membrief) {
                dump_memory(mem, phdrs[i].p_vaddr, phdrs[i].p_vaddr + phdrs[i].p_filesz);
            }
        }

		//P2 print memmory full
        if (print_memfull) {
            dump_memory(mem, 0, MEMSIZE);
        }

		
		//P3 disassemble code
		if (disas_code) {
			printf("Disassembly of executable contents:\n");
			for (int i = 0; i < hdr.e_num_phdr; i++) {
				disassemble_code(mem, &phdrs[i], &hdr);		

			}


			

	
		}
		//P3 disassemble data
		if (disas_data) {
			// not implemented
		}
		

      

		

        // cleanup
		free(mem);
        fclose(f);
		
	}
    return EXIT_SUCCESS;
}

