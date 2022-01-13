/*
 * CS 261 PA4: Mini-ELF interpreter
 *
 * Name: Nic Plybon
 */

#include "p4-interp.h"

/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

y86_reg_t decode_execute (y86_t *cpu, y86_inst_t inst, bool *cnd, y86_reg_t *valA)
{
    y86_reg_t valE = 0;
    y86_reg_t valB = 0;
    if (cnd == NULL) {
        cpu->stat = INS;
    }

    //decode
    switch(inst.icode) {
        case OPQ:
            if (inst.ifun.b > 3) {
                cpu->stat = INS;
            }
            valA = (y86_reg_t *) cpu->reg[inst.ra];
            valB = cpu->reg[inst.rb];
            break;
        case RMMOVQ:
            *valA = cpu->reg[inst.ra];
            valB = cpu->reg[inst.rb];
            break;
        case MRMOVQ:
            valB = cpu->reg[inst.rb];
            break;
        case PUSHQ:
            *valA = cpu->reg[inst.ra];
            valB = cpu->reg[RSP];
            break;
        case POPQ:
            *valA = cpu->reg[RSP];
            valB = cpu->reg[RSP];
            break;
        case CALL:
            valB = cpu->reg[RSP];
            break;
        case RET:
            *valA = cpu->reg[RSP];
            valB = cpu->reg[RSP];
            break;
		case CMOV:
			*valA = cpu->reg[inst.ra];
			break;
        default:
            break;
    }

    //execute
    switch(inst.icode) {
        case HALT:
            cpu->stat = HLT;
            break;
        case IRMOVQ:
            valE = (y86_reg_t)inst.valC.v;
            break;
        case OPQ:
            switch (inst.ifun.b) {
                case ADD:
                    valE = (int64_t)valB + (int64_t)valA;
                    break;
                case SUB:
                    valE = (int64_t)valB - (int64_t)valA;
                    break;
                case AND:
                    valE = (int64_t)valB & (int64_t)valA;
                    break;
                case XOR:
                    valE = (int64_t)valB ^ (int64_t)valA;
                    break;
                case BADOP:
                    cpu->stat = INS;
                    break;
                default:
                    cpu->stat = INS;
                    break;
            }
            break;
        case RMMOVQ:
            valE = valB + inst.valC.d;
            break;
        case MRMOVQ:
            valE = valB + inst.valC.d;
            break;
        case PUSHQ:
            valE = valB - 8;
            break;
        case POPQ:
            valE = valB + 8;
            break;
        case CALL:
            valE = valB - 8;
            break;
        case RET:
            valE = valB + 8;
            break;
		case CMOV:
			valE = valA;
			break;
        default:
            break;
    }

    return valE;
}

void memory_wb_pc (y86_t *cpu, y86_inst_t inst, byte_t *memory,
                   bool cnd, y86_reg_t valA, y86_reg_t valE)
{
    uint64_t *ptrmem;
    y86_reg_t valM = 0;

    //memory
    switch(inst.icode) {
        case PUSHQ:
            ptrmem = (uint64_t*)&memory[valE];
            *ptrmem = valA;
            break;
        case POPQ:
            ptrmem = (uint64_t*)&memory[valA];
            valM = *ptrmem;
            break;
        case RMMOVQ:
            ptrmem = (uint64_t*)&memory[valE];
            *ptrmem = valA;
            break;
        case MRMOVQ:
            ptrmem = (uint64_t*)&memory[valE];
            valM = *ptrmem;
            break;
        case CALL:
            ptrmem = (uint64_t*)&memory[valE];
            *ptrmem = inst.valP;
            break;
        case RET:
            ptrmem = (uint64_t*)&memory[valA];
            valM = *ptrmem;
            break;
        default:
            break;



    }
    //write back
    switch(inst.icode) {
        case IRMOVQ:
            cpu->reg[inst.rb] = valE;
        case OPQ:
            cpu->reg[inst.rb] = valE;
            break;
        case PUSHQ:
            cpu->reg[RSP] = valE;
            break;
        case POPQ:
            cpu->reg[RSP] = valE;
            cpu->reg[inst.ra] = valM;
            break;
        case MRMOVQ:
            if (valE >= MEMSIZE) {
                cpu->stat = ADR;
                break;
            }
            cpu->reg[inst.ra] = valM;
            break;
        case CALL:
            cpu->reg[RSP] = valE;
            break;
        case RET:
            cpu->reg[RSP] = valE;
            break;
        default:
            break;

    }

    //pc update
    switch(inst.icode) {
        case CALL:
            cpu->pc = inst.valC.dest;
            break;
        case RET:
            cpu->pc = valM;
            break;
        default:
            cpu->pc = inst.valP;
            break;
    }
}

/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p4 (char **argv)
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
    printf("  -e      Execute program\n");
    printf("  -E      Execute program (trace mode)\n");
}

bool parse_command_line_p4 (int argc, char **argv,
                            bool *header, bool *segments, bool *membrief, bool *memfull,
                            bool *disas_code, bool *disas_data,
                            bool *exec_normal, bool *exec_trace, char **filename)
{
    if (argv == NULL || header == NULL || segments == NULL || membrief == NULL
            || memfull == NULL || filename == NULL || disas_code == NULL || disas_data == NULL) {
        return false;
    }
    //parameter parsing with getopt()
    int c;
    while ((c = getopt(argc, argv, "hHafsmMdDeE")) != -1) {
        switch(c) {
            //display usage
            case 'h':
                usage_p4(argv);
                return true;
            //show the Mini-ELF header
            case 'H':
                *header = true;
                break;
            //show all with brief memory
            case 'a':
                *header = true;
                *segments = true;
                *membrief = true;
                break;
            // show all with full memory
            case 'f':
                *header = true;
                *segments = true;
                *memfull = true;
                break;
            //show the program headers
            case 's':
                *segments = true;
                break;
            //show the memory contents brief
            case 'm':
                *membrief = true;
                break;
            //show the memory contents full
            case 'M':
                *memfull = true;
                break;
            // disassemble code
            case 'd':
                *disas_code = true;
                break;
            // disassemble data
            case 'D':
                *disas_data = true;
                break;
            case 'e':
                *exec_normal = true;
                break;
            case 'E':
                *exec_trace = true;
                break;
            default:
                usage_p4(argv);
                return false;
        }
    }

    if (optind != argc -1) {
        //no filename
        usage_p4(argv);
        return false;
    }
    //save filename
    *filename = argv[optind];
    //check if brief and full flags passed at the same time
    if (*membrief && *memfull) {
        usage_p4(argv);
        return false;
    }
    if (*exec_normal && *exec_trace) {
        usage_p4(argv);
        return false;
    }
    return true;
}

void dump_cpu_state (y86_t cpu)
{
    printf("  %%rip: %016lx", cpu.pc);
    printf("   flags: Z%d S%d O%d     ", cpu.zf, cpu.sf, cpu.of);
    switch(cpu.stat) {
        case AOK:
            printf("AOK");
            break;
        case HLT:
            printf("HLT");
            break;
        case ADR:
            printf("ADR");
            break;
        case INS:
            printf("INS");
            break;
    }
    printf("\n");
    printf("  %%rax: %016lx", cpu.reg[0]);
    printf("    %%rcx: %016lx\n", cpu.reg[1]);
    printf("  %%rdx: %016lx", cpu.reg[2]);
    printf("    %%rbx: %016lx\n", cpu.reg[3]);
    printf("  %%rsp: %016lx", cpu.reg[4]);
    printf("    %%rbp: %016lx\n", cpu.reg[5]);
    printf("  %%rsi: %016lx", cpu.reg[6]);
    printf("    %%rdi: %016lx\n", cpu.reg[7]);
    printf("   %%r8: %016lx", cpu.reg[8]);
    printf("     %%r9: %016lx\n", cpu.reg[9]);
    printf("  %%r10: %016lx", cpu.reg[10]);
    printf("    %%r11: %016lx\n", cpu.reg[11]);
    printf("  %%r12: %016lx", cpu.reg[12]);
    printf("    %%r13: %016lx\n", cpu.reg[13]);
    printf("  %%r14: %016lx\n", cpu.reg[14]);
}

void print_rega (y86_inst_t inst)
{
    switch(inst.ra) {
        case RAX:
            printf("%%rax");
            break;
        case RCX:
            printf("%%rcx");
            break;
        case RDX:
            printf("%%rdx");
            break;
        case RBX:
            printf("%%rbx");
            break;
        case RSP:
            printf("%%rsp");
            break;
        case RBP:
            printf("%%rbp");
            break;
        case RSI:
            printf("%%rsi");
            break;
        case RDI:
            printf("%%rdi");
            break;
        case R8:
            printf("%%r8");
            break;
        case R9:
            printf("%%r9");
            break;
        case R10:
            printf("%%r10");
            break;
        case R11:
            printf("%%r11");
            break;
        case R12:
            printf("%%r12");
            break;
        case R13:
            printf("%%r13");
            break;
        case R14:
            printf("%%r14");
            break;
        case NOREG:
            break;

    }
}

void print_regb (y86_inst_t inst)
{
    switch(inst.rb) {
        case RAX:
            printf("%%rax");
            break;
        case RCX:
            printf("%%rcx");
            break;
        case RDX:
            printf("%%rdx");
            break;
        case RBX:
            printf("%%rbx");
            break;
        case RSP:
            printf("%%rsp");
            break;
        case RBP:
            printf("%%rbp");
            break;
        case RSI:
            printf("%%rsi");
            break;
        case RDI:
            printf("%%rdi");
            break;
        case R8:
            printf("%%r8");
            break;
        case R9:
            printf("%%r9");
            break;
        case R10:
            printf("%%r10");
            break;
        case R11:
            printf("%%r11");
            break;
        case R12:
            printf("%%r12");
            break;
        case R13:
            printf("%%r13");
            break;
        case R14:
            printf("%%r14");
            break;
        case NOREG:
            break;

    }
}