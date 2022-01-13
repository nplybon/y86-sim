/*
 * CS 261 PA3: Mini-ELF disassembler
 *
 * Name: Nic Plybon
 */

#include "p3-disas.h"

/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

y86_inst_t fetch (y86_t *cpu, byte_t *memory)
{
    y86_inst_t ins;
	ins.icode = memory[cpu->pc] >> 4;
	ins.ifun.b = memory[cpu->pc] & 0xf;
	uint64_t *tenbyte = (uint64_t*)&memory[cpu->pc + 2];
	uint64_t *ninebyte = (uint64_t*)&memory[cpu->pc + 1];

	// based off of Y86 reference sheet
	switch(ins.icode) {
		case HALT:
		case NOP:
		case RET:
			if (ins.ifun.b != 0) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			ins.valP = cpu->pc + 1;
			break;
		case CMOV:
			if (ins.ifun.b > 6) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			ins.ra = memory[cpu->pc + 1] >> 4;
			ins.rb = memory[cpu->pc + 1] & 0xf;
			ins.valP = cpu->pc + 2;
			break;
		case OPQ:
			if (ins.ifun.b > 3) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			ins.ra = memory[cpu->pc + 1] >> 4;
			ins.rb = memory[cpu->pc + 1] & 0xf;
			ins.valP = cpu->pc + 2;
			break;
		case PUSHQ:
		case POPQ:	
			ins.ra = memory[cpu->pc + 1] >> 4;
			ins.rb = memory[cpu->pc + 1] & 0xf;
			ins.valP = cpu->pc + 2;
			if (ins.ifun.b != 0 || ins.rb != 0xf) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			break;
		case IRMOVQ:
			ins.ra = memory[cpu->pc + 1] >> 4;
			ins.rb = memory[cpu->pc + 1] & 0xf;
			ins.valP = cpu->pc + 10;
			ins.valC.v = *tenbyte;
			if (ins.ifun.b != 0 || ins.ra != 0xf) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			break;
		case RMMOVQ:
			if (ins.ifun.b != 0) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			ins.ra = memory[cpu->pc + 1] >> 4;
			ins.rb = memory[cpu->pc + 1] & 0xf;
			ins.valP = cpu->pc + 10;
			ins.valC.d = *tenbyte;

			
			break;
		case MRMOVQ:
			if (ins.ifun.b != 0) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			ins.ra = memory[cpu->pc + 1] >> 4;
			ins.rb = memory[cpu->pc + 1] & 0xf;
			ins.valP = cpu->pc + 10;
			ins.valC.d = *tenbyte;

			break;
		case JUMP:
			if (ins.ifun.b > 6) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			ins.valP = cpu->pc + 9;
			ins.valC.dest = *ninebyte;

			break;

		case CALL:
			if (ins.ifun.b != 0) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
			ins.valC.dest = *ninebyte;

			ins.valP = cpu->pc + 9;
			break;
		case IOTRAP:
			if (ins.ifun.b > 5) {
				ins.icode = INVALID;
				cpu->stat = INS;
			}
		
			ins.valP = cpu->pc + 1;
			break;
		default:
			ins.icode = INVALID;
			cpu->stat = INS;
			break;
	}
	


    return ins;
}
/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p3 (char **argv)
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
    printf("  -d      Disassemble code contents\n");
    printf("  -D      Disassemble data contents\n");
}

bool parse_command_line_p3 (int argc, char **argv,
        bool *print_header, bool *print_segments,
        bool *print_membrief, bool *print_memfull,
        bool *disas_code, bool *disas_data, char **filename)
{
	 if (argv == NULL || print_header == NULL || print_segments == NULL || print_membrief == NULL
            || print_memfull == NULL || filename == NULL || disas_code == NULL || disas_data == NULL) {
        return false;
    }
	 //parameter parsing with getopt()
    int c;
    while ((c = getopt(argc, argv, "hHafsmMdD")) != -1) {
        switch(c) {
            //display usage
            case 'h':
                usage_p3(argv);
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
			// show all with full memory
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
			// disassemble code
			case 'd':
				*disas_code = true;
				break;
			// disassemble data
			case 'D':
				*disas_data = true;
				break;
            default:
                usage_p3(argv);
                return false;
        }
    }

    if (optind != argc -1) {
        //no filename
        usage_p3(argv);
        return false;
    }
    //save filename
    *filename = argv[optind];
    //check if brief and full flags passed at the same time
    if (*print_membrief && *print_memfull) {
        usage_p3(argv);
        return false;
    }
    return true;
}

void disassemble (y86_inst_t inst)
{
	//prints the name for a particular inst
	switch(inst.icode) {
		
		case HALT: printf("halt"); break;
		case NOP:  printf("nop");  break;
		case CMOV:
			switch(inst.ifun.b) {
				case RRMOVQ: printf("rrmovq"); break;
				case CMOVLE: printf("cmovle"); break;
				case CMOVL: printf("cmovl"); break;
				case CMOVE: printf("cmove"); break;
				case CMOVNE: printf("cmovne"); break;
				case CMOVGE: printf("cmovge"); break;
				case CMOVG: printf("cmovg"); break;
			}
			break;
		case IRMOVQ: printf("irmovq"); break;
		case RMMOVQ: printf("rmmovq"); break;
		case MRMOVQ: printf("mrmovq"); break;
		case OPQ:
			switch(inst.ifun.b) {
				case ADD: printf("addq"); break;
				case SUB: printf("subq"); break;
				case AND: printf("andq"); break;
				case XOR: printf("xorq"); break;	
			}
			break;
		case JUMP:
			switch(inst.ifun.b) {
				case JMP: printf("jmp"); break;
				case JLE: printf("jle"); break;
				case JL:  printf("jl"); break;
				case JE:  printf("je"); break;
				case JNE: printf("jne"); break;
				case JGE: printf("jge"); break;
				case JG:  printf("jg"); break;			
			}
			break;
		case CALL:  printf("call"); break;
		case RET:   printf("ret");  break;
		case PUSHQ: printf("pushq");  break;
		case POPQ:  printf("popq");  break;	
		case IOTRAP:
			switch(inst.ifun.b) {
				case CHAROUT: printf("iotrap 0"); break;
				case CHARIN:  printf("iotrap 1"); break;
				case DECOUT:  printf("iotrap 2"); break;
				case DECIN:   printf("iotrap 3"); break;
				case STROUT:  printf("iotrap 4"); break;
				case FLUSH:   printf("iotrap 5"); break;
			}
			break;
		case INVALID: break;
		
	}
	
	
}

void disassemble_code (byte_t *memory, elf_phdr_t *phdr, elf_hdr_t *hdr)
{
	y86_t cpu;			// CPU struct to store "fake" PC
    y86_inst_t ins;		// struct to hold fetched instruction

    // start at beginning of the segment
    cpu.pc = phdr->p_vaddr;

	printf("  0x%03lx:                      | .pos 0x%03lx code\n", cpu.pc, cpu.pc);
    // iterate through the segment one instruction at a time
    while (cpu.pc < phdr->p_vaddr + phdr->p_filesz) {
		
        ins = fetch (&cpu, memory);         // stage 1: fetch instruction
		
		if (cpu.pc == hdr->e_entry) {		
			printf("  0x%03lx:                      | _start:\n", cpu.pc);
		}
		
	
        // print current address and raw bytes of instruction
		
		if (ins.icode == HALT || ins.icode == NOP || ins.icode == RET || ins.icode == IOTRAP) {
			printf("  0x%03lx: %02x                   |   ", cpu.pc, memory[cpu.pc]);

		} else if (ins.icode == CMOV || ins.icode == OPQ || ins.icode == PUSHQ || ins.icode == POPQ) {
			printf("  0x%03lx: %02x%02x                 |   ", cpu.pc, memory[cpu.pc], memory[cpu.pc + 1]);

		} else if (ins.icode == JUMP || ins.icode == CALL) {
			printf("  0x%03lx: %02x%02x%02x%02x%02x%02x%02x%02x%02x   |   ", cpu.pc, memory[cpu.pc], memory[cpu.pc + 1], memory[cpu.pc + 2],memory[cpu.pc + 3],memory[cpu.pc + 4],memory[cpu.pc + 5],memory[cpu.pc + 6],memory[cpu.pc + 7],memory[cpu.pc + 8]);

		} else if (ins.icode == IRMOVQ || ins.icode == RMMOVQ || ins.icode == MRMOVQ) {
			printf("  0x%03lx: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x |   ", cpu.pc, memory[cpu.pc], memory[cpu.pc + 1], memory[cpu.pc + 2],memory[cpu.pc + 3],memory[cpu.pc + 4],memory[cpu.pc + 5],memory[cpu.pc + 6],memory[cpu.pc + 7],memory[cpu.pc + 8], memory[cpu.pc + 9]);

		}
			
        disassemble (ins);                  // stage 2: print disassembly
		//inst that dont have registers
		if (ins.icode == HALT || ins.icode == IOTRAP || ins.icode == NOP || ins.icode == RET) {
			//do nothing
		} else {
			//helper method that prints registers for inst that have them
			print_reg(ins);

		}
		
		

		
		cpu.pc = ins.valP;                  // stage 3: update PC (go to next instruction)
		printf("\n");
	}
	printf("\n");
}

void print_reg (y86_inst_t ins) {
	//prints the valC values for a specific inst
	if (ins.icode == IRMOVQ) {
		printf(" 0x%lx", ins.valC.v);
	} else if (ins.icode == MRMOVQ) {
		if (ins.rb != NOREG) {
			printf(" 0x%lx(", ins.valC.d);
		} else {
			printf(" 0x%lx", ins.valC.d);
		}
	}
	//if JUMP or CALL print only destination
	if (ins.icode == JUMP || ins.icode == CALL) {
		printf(" 0x%lx", ins.valC.dest);
	} else {
    // print rb then ra if MRMOVQ otherwise print ra
	if (ins.icode == MRMOVQ) {
		reg_b(ins);
	} else {
		reg_a(ins);
	}
	
	//formatting for CMOV, OPQ and IRMOVQ
	if (ins.icode == CMOV || ins.icode == OPQ || ins.icode == IRMOVQ) {
		printf(", ");
	} else if (ins.icode == RMMOVQ) {
		if (ins.rb != NOREG) {
			printf(", 0x%lx(", ins.valC.d);

		} else {
			printf(", 0x%lx", ins.valC.d);

		}
	// formatting for MRMOVQ
	} else if (ins.icode == MRMOVQ) {
		if (ins.rb != NOREG) {
			printf("),");
		} else {
			printf(",");

		}

	}
	// print ra then rb if MRMOVQ otherwise print rb

	if (ins.icode == MRMOVQ) {
		reg_a(ins);
	} else {
			reg_b(ins);

	}
	// formatting for RMMOVQ
	if (ins.icode == RMMOVQ) {
		if (ins.rb != NOREG) {
			printf(")");
		}	
	} 
		
		
	}

	
}
void reg_a (y86_inst_t ins) {
	switch(ins.ra) {
		case RAX: printf(" %%rax"); break;
		case RCX: printf(" %%rcx"); break;
		case RDX: printf(" %%rdx"); break;
		case RBX: printf(" %%rbx"); break;
		case RSP: printf(" %%rsp"); break;
		case RBP: printf(" %%rbp"); break;
		case RSI: printf(" %%rsi"); break;
		case RDI: printf(" %%rdi"); break;
		case R8: printf(" %%r8"); break;
		case R9: printf(" %%r9"); break;
		case R10: printf(" %%r10"); break;
		case R11: printf(" %%r11"); break;
		case R12: printf(" %%r12"); break;
		case R13: printf(" %%r13"); break;
		case R14: printf(" %%r14"); break;
		case NOREG: break;
	}
}

void reg_b (y86_inst_t ins) {
	switch(ins.rb) {
		case RAX: printf("%%rax"); break;
		case RCX: printf("%%rcx"); break;
		case RDX: printf("%%rdx"); break;
		case RBX: printf("%%rbx"); break;
		case RSP: printf("%%rsp"); break;
		case RBP: printf("%%rbp"); break;
		case RSI: printf("%%rsi"); break;
		case RDI: printf("%%rdi"); break;
		case R8: printf("%%r8"); break;
		case R9: printf("%%r9"); break;
		case R10: printf("%%r10"); break;
		case R11: printf("%%r11"); break;
		case R12: printf("%%r12"); break;
		case R13: printf("%%r13"); break;
		case R14: printf("%%r14"); break;
		case NOREG: break;

	}
	
	
}

void disassemble_data (byte_t *memory, elf_phdr_t *phdr)
{	
}

void disassemble_rodata (byte_t *memory, elf_phdr_t *phdr)
{
}

