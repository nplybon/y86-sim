/*
 * CS 261: Main driver
 *
 * Name: Nic Plybon
 */

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"
#include "p4-interp.h"

int main (int argc, char **argv)
{
    //parse command-line options
    bool print_header = false;
    bool print_segments = false;
    bool print_membrief = false;
    bool print_memfull = false;
    bool disas_code = false;
    bool disas_data = false;
    bool exec_normal = false;
    bool exec_trace = false;
    char *fn = NULL;
    if (!parse_command_line_p4(argc, argv, &print_header, &print_segments, &print_membrief,
                               &print_memfull, &disas_code, &disas_data, &exec_normal, &exec_trace, &fn)) {
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

        //P2 print memory full
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
            disassemble_data(mem, phdrs);
        }

        y86_t cpu;
        bool cnd;
        y86_reg_t valA, valE;

        //initialize cpu
        cpu.stat = AOK;
        cpu.pc = hdr.e_entry;
        cpu.zf = 0;
        cpu.sf = 0;
        cpu.of = 0;
        //initialize registers to 0
        for (int i = 0; i < 15; i++) {
            cpu.reg[i] = 0;
        }
        int count = 0;
        //formatting for beginning...
        bool start = true;
        //formatting for final state of cpu
        bool trace = false;
        //main cpu cycle
        while(cpu.stat == AOK) {
            //p3
            y86_inst_t ins = fetch(&cpu, mem);
            //p4
            if (exec_trace) {
                if (start) {
                    printf("Beginning execution at 0x%04x\n", hdr.e_entry);
                    start = false;
                    trace = true;

                }
                printf("Y86 CPU state:\n");
                dump_cpu_state(cpu);
                printf("\n");
                printf("Executing: ");
                switch(ins.icode) {
                    case HALT:
                        printf("halt");
                        break;
                    case NOP:
                        printf("nop");
                        break;
                    case IRMOVQ:
                        printf("irmovq 0x%lx, ", ins.valC.v);
                        print_regb(ins);
                        break;
                    case OPQ:
                        switch(ins.ifun.b) {
                            case ADD:
                                printf("addq ");
                                break;
                            case SUB:
                                printf("subq ");
                                break;
                            case AND:
                                printf("andq ");
                                break;
                            case XOR:
                                printf("xorq ");
                                break;
                        }
                        print_rega(ins);
                        printf(", ");
                        print_regb(ins);
                        break;
                    case PUSHQ:
                        printf("pushq ");
                        print_rega(ins);
                        break;
                    case POPQ:
                        printf("popq ");
                        print_rega(ins);
                        break;
                    case RMMOVQ:
                        printf("rmmovq ");
                        print_rega(ins);
                        printf(", ");
                        printf("0x%lx", ins.valC.d);
                        if (ins.rb == NOREG) {
                            print_regb(ins);
                        } else {
                            printf("(");
                            print_regb(ins);
                            printf(")");
                        }
                        break;
                    case MRMOVQ:
                        printf("mrmovq ");
                        printf("0x%lx", ins.valC.d);
                        if (ins.rb == NOREG) {
                            print_regb(ins);
                            printf(", ");
                        } else {
                            printf("(");
                            print_regb(ins);
                            printf("), ");
                        }

                        print_rega(ins);
                        break;
                    case CALL:
                        printf("call ");
                        printf("0x%lx", ins.valC.dest);
                        break;
                    case RET:
                        printf("ret");
                        break;
					case CMOV:
						switch(ins.ifun.b){
							case RRMOVQ: printf("rrmovq "); break;
							case CMOVLE: printf("cmovle "); break;
							case CMOVL: printf("cmovl "); break;
							case CMOVE: printf("cmove "); break;
							case CMOVNE: printf("cmovne "); break;
							case CMOVGE: printf("cmovge "); break;
							case CMOVG: printf("cmovg "); break;
						}
						print_rega(ins);
						printf(", ");
						print_regb(ins);
						break;
					default:
						break;


                }
                printf("\n");
            }
            valE = decode_execute(&cpu, ins, &cnd, &valA);
            memory_wb_pc(&cpu, ins, mem, cnd, valA, valE);
            count++;


        }

        if (exec_normal || trace) {
            if (!trace) {
                printf("Beginning execution at 0x%04x\n", hdr.e_entry);
            }
            printf("Y86 CPU state:\n");

            dump_cpu_state(cpu);
        }
        printf("Total execution count: %d\n", count);

        //mem dump for trace mode
        if (exec_trace) {
            printf("\n");
            dump_memory(mem, 0, MEMSIZE);

        }

        // cleanup
        free(mem);
        fclose(f);

    }
    return EXIT_SUCCESS;
}

