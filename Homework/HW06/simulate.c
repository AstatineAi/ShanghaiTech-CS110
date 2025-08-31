#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "cache.h"
#include "simulate.h"

#define REGISTER_NAME_TO_NUMBER(name1, name2, number)                          \
  if (strcmp(register_name, name1) == 0 || strcmp(register_name, name2) == 0)  \
  return number

#define PARSE_LW(inst_name)                                                    \
  else if (strcmp(inst, #inst_name) == 0) {                                    \
    instruction->type = INSTRUCTION_##inst_name;                               \
    instruction->rd = register_name_to_label(strtok(NULL, ", "));              \
    instruction->imm = strtol(strtok(NULL, " ("), NULL, 10);                   \
    instruction->rs1 = register_name_to_label(strtok(NULL, ")"));              \
    fprintf(stderr, "rs1: %u, rd: %u, imm: %ld\n", instruction->rs1,           \
            instruction->rd, instruction->imm);                                \
  }

#define PARSE_SW(inst_name)                                                    \
  else if (strcmp(inst, #inst_name) == 0) {                                    \
    instruction->type = INSTRUCTION_##inst_name;                               \
    instruction->rs2 = register_name_to_label(strtok(NULL, ", "));             \
    instruction->imm = strtol(strtok(NULL, " ("), NULL, 10);                   \
    instruction->rs1 = register_name_to_label(strtok(NULL, ")"));              \
    fprintf(stderr, "rs1: %u, rs2: %u, imm: %ld\n", instruction->rs1,          \
            instruction->rs2, instruction->imm);                               \
  }

#define PARSE_RTYPE(inst_name)                                                 \
  else if (strcmp(inst, #inst_name) == 0) {                                    \
    instruction->type = INSTRUCTION_##inst_name;                               \
    instruction->rd = register_name_to_label(strtok(NULL, ", "));              \
    instruction->rs1 = register_name_to_label(strtok(NULL, ", "));             \
    instruction->rs2 = register_name_to_label(strtok(NULL, " \n"));            \
    fprintf(stderr, "rd: %u, rs1: %u, rs2: %u\n", instruction->rd,             \
            instruction->rs1, instruction->rs2);                               \
  }

#define PARSE_ITYPE(inst_name)                                                 \
  else if (strcmp(inst, #inst_name) == 0) {                                    \
    instruction->type = INSTRUCTION_##inst_name;                               \
    instruction->rd = register_name_to_label(strtok(NULL, ", "));              \
    instruction->rs1 = register_name_to_label(strtok(NULL, ", "));             \
    instruction->imm = strtol(strtok(NULL, " \n"), NULL, 0);                   \
    fprintf(stderr, "rd: %u, rs1: %u, imm: %ld\n", instruction->rd,            \
            instruction->rs1, instruction->imm);                               \
  }

#define PARSE_SBTYPE(inst_name)                                                \
  else if (strcmp(inst, #inst_name) == 0) {                                    \
    instruction->type = INSTRUCTION_##inst_name;                               \
    instruction->rs1 = register_name_to_label(strtok(NULL, ", "));             \
    instruction->rs2 = register_name_to_label(strtok(NULL, ", "));             \
    instruction->imm = strtol(strtok(NULL, " \n"), NULL, 0);                   \
    fprintf(stderr, "rs1: %u, rs2: %u, imm: %ld\n", instruction->rs1,          \
            instruction->rs2, instruction->imm);                               \
  }

/* 32 integer registers. */
static uint32_t registers[32];

/* The program counter. */
static uint32_t program_counter;

static unsigned register_name_to_label(const char *register_name) {
  REGISTER_NAME_TO_NUMBER("zero", "x0", 0);
  REGISTER_NAME_TO_NUMBER("ra", "x1", 1);
  REGISTER_NAME_TO_NUMBER("sp", "x2", 2);
  REGISTER_NAME_TO_NUMBER("gp", "x3", 3);
  REGISTER_NAME_TO_NUMBER("tp", "x4", 4);
  REGISTER_NAME_TO_NUMBER("t0", "x5", 5);
  REGISTER_NAME_TO_NUMBER("t1", "x6", 6);
  REGISTER_NAME_TO_NUMBER("t2", "x7", 7);
  REGISTER_NAME_TO_NUMBER("s0", "x8", 8);
  REGISTER_NAME_TO_NUMBER("s1", "x9", 9);
  REGISTER_NAME_TO_NUMBER("a0", "x10", 10);
  REGISTER_NAME_TO_NUMBER("a1", "x11", 11);
  REGISTER_NAME_TO_NUMBER("a2", "x12", 12);
  REGISTER_NAME_TO_NUMBER("a3", "x13", 13);
  REGISTER_NAME_TO_NUMBER("a4", "x14", 14);
  REGISTER_NAME_TO_NUMBER("a5", "x15", 15);
  REGISTER_NAME_TO_NUMBER("a6", "x16", 16);
  REGISTER_NAME_TO_NUMBER("a7", "x17", 17);
  REGISTER_NAME_TO_NUMBER("s2", "x18", 18);
  REGISTER_NAME_TO_NUMBER("s3", "x19", 19);
  REGISTER_NAME_TO_NUMBER("s4", "x20", 20);
  REGISTER_NAME_TO_NUMBER("s5", "x21", 21);
  REGISTER_NAME_TO_NUMBER("s6", "x22", 22);
  REGISTER_NAME_TO_NUMBER("s7", "x23", 23);
  REGISTER_NAME_TO_NUMBER("s8", "x24", 24);
  REGISTER_NAME_TO_NUMBER("s9", "x25", 25);
  REGISTER_NAME_TO_NUMBER("s10", "x26", 26);
  REGISTER_NAME_TO_NUMBER("s11", "x27", 27);
  REGISTER_NAME_TO_NUMBER("t3", "x28", 28);
  REGISTER_NAME_TO_NUMBER("t4", "x29", 29);
  REGISTER_NAME_TO_NUMBER("t5", "x30", 30);
  REGISTER_NAME_TO_NUMBER("t6", "x31", 31);

  /* s0 is also called fp. */
  if (strcmp(register_name, "fp") == 0)
    return 8;

  /* Return 32 on error. */
  return 32;
}

static void parse_inst(char *buf, struct Instruction *instruction) {
  char *inst = strtok(buf, " ");
  fprintf(stderr, "Instruction: %s, ", inst);

  if (strcmp(inst, "li") == 0) {
    instruction->type = INSTRUCTION_li;
    instruction->rd = register_name_to_label(strtok(NULL, ", "));
    instruction->imm = strtol(strtok(NULL, " \n"), NULL, 0);
    fprintf(stderr, "rd: %u, imm: %ld\n", instruction->rd, instruction->imm);
  }
  PARSE_LW(lw)
  PARSE_SW(sw)
  PARSE_RTYPE(add)
  PARSE_RTYPE(and)
  PARSE_RTYPE(div)
  PARSE_RTYPE(mul)
  PARSE_RTYPE(rem)
  PARSE_RTYPE(or)
  PARSE_RTYPE(sll)
  PARSE_RTYPE(srl)
  PARSE_RTYPE(sub)
  PARSE_RTYPE(xor)
  PARSE_ITYPE(addi)
  PARSE_ITYPE(andi)
  PARSE_ITYPE(ori)
  PARSE_ITYPE(slli)
  PARSE_ITYPE(srli)
  PARSE_ITYPE(xori)
  PARSE_SBTYPE(beq)
  PARSE_SBTYPE(bne)
  else {
    fprintf(stderr, "Invalid instruction!\n");
  }
}

struct Instruction *parse_asm(FILE *file, size_t num_lines) {
  struct Instruction *instructions =
      malloc(sizeof(struct Instruction) * num_lines);
  size_t i;
  char buf[100];
  for (i = 0; i < num_lines; ++i) {
    fgets(buf, sizeof(buf), file);
    parse_inst(buf, instructions + i);
  }
  return instructions;
}

void start_simulation(struct Instruction *instructions, size_t num) {
  // Initialize registers and program counter
  memset(registers, 0, sizeof(registers));
  program_counter = 0;

  // Initialize cache system
  init_cache_system();

  while (program_counter < num) {
    struct Instruction *inst = &instructions[program_counter];

    // Add instruction fetch to cache
    add_inst_access(program_counter * 4);

    fprintf(stderr, "Executing instruction at PC=0x%x\n", program_counter * 4);

    // Execute instruction
    switch (inst->type) {
    case INSTRUCTION_li:
      registers[inst->rd] = inst->imm;
      fprintf(stderr, "li: x%d = %ld\n", inst->rd, inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_lw:
      add_data_access(registers[inst->rs1] + inst->imm);
      fprintf(stderr, "lw: x%d = [x%d + %ld]\n", inst->rd, inst->rs1,
              inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_sw:
      add_data_access(registers[inst->rs1] + inst->imm);
      fprintf(stderr, "sw: [x%d + %ld] = x%d\n", inst->rs1, inst->imm,
              inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_add:
      registers[inst->rd] = registers[inst->rs1] + registers[inst->rs2];
      fprintf(stderr, "add: x%d = x%d + x%d\n", inst->rd, inst->rs1, inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_and:
      registers[inst->rd] = registers[inst->rs1] & registers[inst->rs2];
      fprintf(stderr, "and: x%d = x%d & x%d\n", inst->rd, inst->rs1, inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_div:
      if (registers[inst->rs2] != 0) {
        registers[inst->rd] = registers[inst->rs1] / registers[inst->rs2];
      }
      fprintf(stderr, "div: x%d = x%d / x%d\n", inst->rd, inst->rs1, inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_mul:
      registers[inst->rd] = registers[inst->rs1] * registers[inst->rs2];
      fprintf(stderr, "mul: x%d = x%d * x%d\n", inst->rd, inst->rs1, inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_rem:
      if (registers[inst->rs2] != 0) {
        registers[inst->rd] = registers[inst->rs1] % registers[inst->rs2];
      }
      fprintf(stderr, "rem: x%d = x%d %% x%d\n", inst->rd, inst->rs1,
              inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_or:
      registers[inst->rd] = registers[inst->rs1] | registers[inst->rs2];
      fprintf(stderr, "or: x%d = x%d | x%d\n", inst->rd, inst->rs1, inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_sll:
      registers[inst->rd] = registers[inst->rs1]
                            << (registers[inst->rs2] & 0x1F);
      fprintf(stderr, "sll: x%d = x%d << x%d\n", inst->rd, inst->rs1,
              inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_srl:
      registers[inst->rd] =
          (uint32_t)registers[inst->rs1] >> (registers[inst->rs2] & 0x1F);
      fprintf(stderr, "srl: x%d = x%d >> x%d\n", inst->rd, inst->rs1,
              inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_sub:
      registers[inst->rd] = registers[inst->rs1] - registers[inst->rs2];
      fprintf(stderr, "sub: x%d = x%d - x%d\n", inst->rd, inst->rs1, inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_xor:
      registers[inst->rd] = registers[inst->rs1] ^ registers[inst->rs2];
      fprintf(stderr, "xor: x%d = x%d ^ x%d\n", inst->rd, inst->rs1, inst->rs2);
      program_counter++;
      break;

    case INSTRUCTION_addi:
      registers[inst->rd] = registers[inst->rs1] + inst->imm;
      fprintf(stderr, "addi: x%d = x%d + %ld\n", inst->rd, inst->rs1,
              inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_andi:
      registers[inst->rd] = registers[inst->rs1] & inst->imm;
      fprintf(stderr, "andi: x%d = x%d & %ld\n", inst->rd, inst->rs1,
              inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_ori:
      registers[inst->rd] = registers[inst->rs1] | inst->imm;
      fprintf(stderr, "ori: x%d = x%d | %ld\n", inst->rd, inst->rs1, inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_slli:
      registers[inst->rd] = registers[inst->rs1] << (inst->imm & 0x1F);
      fprintf(stderr, "slli: x%d = x%d << %ld\n", inst->rd, inst->rs1,
              inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_srli:
      registers[inst->rd] =
          (uint32_t)registers[inst->rs1] >> (inst->imm & 0x1F);
      fprintf(stderr, "srli: x%d = x%d >> %ld\n", inst->rd, inst->rs1,
              inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_xori:
      registers[inst->rd] = registers[inst->rs1] ^ inst->imm;
      fprintf(stderr, "xori: x%d = x%d ^ %ld\n", inst->rd, inst->rs1,
              inst->imm);
      program_counter++;
      break;

    case INSTRUCTION_beq:
      fprintf(stderr, "beq: x%d == x%d ? %ld\n", inst->rs1, inst->rs2,
              inst->imm);
      if (registers[inst->rs1] == registers[inst->rs2]) {
        program_counter += inst->imm;
      } else {
        program_counter++;
      }
      break;

    case INSTRUCTION_bne:
      fprintf(stderr, "bne: x%d != x%d ? %ld\n", inst->rs1, inst->rs2,
              inst->imm);
      if (registers[inst->rs1] != registers[inst->rs2]) {
        program_counter += (inst->imm) / 4;
      } else {
        program_counter++;
      }
      break;
    }
  }
}
