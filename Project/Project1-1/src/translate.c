/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include "translate.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"
#include "tables.h"
#include "translate_utils.h"
#include "utils.h"

#define IMM_BUF_SIZE 32
#define LBL_BUF_SIZE 512

static const PseudoHandler pseudo_handlers[] = {
    {"beqz", transform_beqz}, {"bnez", transform_bnez}, {"li", transform_li},
    {"mv", transform_mv},     {"j", transform_j},       {"jr", transform_jr},
    {"jal", transform_jal},   {"jalr", transform_jalr}, {"lw", transform_lw},
};

static char labeled_symbol[LBL_BUF_SIZE];

/* You need to fill in this table before translating inst in pass #2.

Require fields per entry:
  - const char* name;         -- instr name
  - InstrType instr_type;     -- instr format, e.g. R_type
  - uint8_t opcode;
  - uint8_t funct3;
  - uint8_t funct7;           -- funct7 or partial imm
  - ImmType imm_type;         -- imm type (see translate_utils.h)
*/
static const InstrInfo instr_table[] = {
    // R-type instructions
    {"add", R_TYPE, 0x33, 0x0, 0x00, IMM_NONE},
    {"sub", R_TYPE, 0x33, 0x0, 0x20, IMM_NONE},
    {"xor", R_TYPE, 0x33, 0x4, 0x00, IMM_NONE},
    {"or", R_TYPE, 0x33, 0x6, 0x00, IMM_NONE},
    {"and", R_TYPE, 0x33, 0x7, 0x00, IMM_NONE},
    {"sll", R_TYPE, 0x33, 0x1, 0x00, IMM_NONE},
    {"srl", R_TYPE, 0x33, 0x5, 0x00, IMM_NONE},
    {"sra", R_TYPE, 0x33, 0x5, 0x20, IMM_NONE},
    {"slt", R_TYPE, 0x33, 0x2, 0x00, IMM_NONE},
    {"sltu", R_TYPE, 0x33, 0x3, 0x00, IMM_NONE},
    {"mul", R_TYPE, 0x33, 0x0, 0x01, IMM_NONE},
    {"mulh", R_TYPE, 0x33, 0x1, 0x01, IMM_NONE},
    {"div", R_TYPE, 0x33, 0x4, 0x01, IMM_NONE},
    {"rem", R_TYPE, 0x33, 0x6, 0x01, IMM_NONE},

    // I-type instructions
    {"addi", I_TYPE, 0x13, 0x0, 0x00, IMM_12_SIGNED},
    {"xori", I_TYPE, 0x13, 0x4, 0x00, IMM_12_SIGNED},
    {"ori", I_TYPE, 0x13, 0x6, 0x00, IMM_12_SIGNED},
    {"andi", I_TYPE, 0x13, 0x7, 0x00, IMM_12_SIGNED},
    {"slli", I_TYPE, 0x13, 0x1, 0x00, IMM_5_UNSIGNED},
    {"srli", I_TYPE, 0x13, 0x5, 0x00, IMM_5_UNSIGNED},
    {"srai", I_TYPE, 0x13, 0x5, 0x20, IMM_5_UNSIGNED},
    {"slti", I_TYPE, 0x13, 0x2, 0x00, IMM_12_SIGNED},
    {"sltiu", I_TYPE, 0x13, 0x3, 0x00, IMM_12_SIGNED},
    {"lb", I_TYPE, 0x03, 0x0, 0x00, IMM_12_SIGNED},
    {"lh", I_TYPE, 0x03, 0x1, 0x00, IMM_12_SIGNED},
    {"lw", I_TYPE, 0x03, 0x2, 0x00, IMM_12_SIGNED},
    {"lbu", I_TYPE, 0x03, 0x4, 0x00, IMM_12_SIGNED},
    {"lhu", I_TYPE, 0x03, 0x5, 0x00, IMM_12_SIGNED},
    {"jalr", I_TYPE, 0x67, 0x0, 0x00, IMM_12_SIGNED},
    {"ecall", I_TYPE, 0x73, 0x0, 0x00, IMM_NONE},
    {"ebreak", I_TYPE, 0x73, 0x0, 0x01, IMM_NONE},

    // S-type instructions
    {"sb", S_TYPE, 0x23, 0x0, 0x00, IMM_12_SIGNED},
    {"sh", S_TYPE, 0x23, 0x1, 0x00, IMM_12_SIGNED},
    {"sw", S_TYPE, 0x23, 0x2, 0x00, IMM_12_SIGNED},

    // SB-type instructions
    {"beq", SB_TYPE, 0x63, 0x0, 0x00, IMM_13_SIGNED},
    {"bne", SB_TYPE, 0x63, 0x1, 0x00, IMM_13_SIGNED},
    {"blt", SB_TYPE, 0x63, 0x4, 0x00, IMM_13_SIGNED},
    {"bge", SB_TYPE, 0x63, 0x5, 0x00, IMM_13_SIGNED},
    {"bltu", SB_TYPE, 0x63, 0x6, 0x00, IMM_13_SIGNED},
    {"bgeu", SB_TYPE, 0x63, 0x7, 0x00, IMM_13_SIGNED},

    // U-type instructions
    {"lui", U_TYPE, 0x37, 0x0, 0x00, IMM_20_UNSIGNED},
    {"auipc", U_TYPE, 0x17, 0x0, 0x00, IMM_20_UNSIGNED},

    // UJ-type instructions
    {"jal", UJ_TYPE, 0x6f, 0x0, 0x00, IMM_21_SIGNED},
};

unsigned transform_beqz(Block *blk, char **args, int num_args) {
  /* beqz rs, offset -> beq rs, x0, offset */
  if (num_args != 2) {
    return 0;
  }
  char *new_args[3] = {args[0], "x0", args[1]};
  int result = add_to_block(blk, "beq", new_args, 3);
  return result == -1 ? 0 : 1;
}

unsigned transform_bnez(Block *blk, char **args, int num_args) {
  /* bnez rs, offset -> bne rs, x0, offset */
  if (num_args != 2) {
    return 0;
  }
  char *new_args[3] = {args[0], "x0", args[1]};
  int result = add_to_block(blk, "bne", new_args, 3);
  return result == -1 ? 0 : 1;
}

/* Hint:
  - make sure that the number is representable by 32 bits. (Hint: the number
      can be both signed or unsigned).
  - if the immediate can fit in the imm field of an addiu instruction, then
      expand li into a single addi instruction. Otherwise, expand it into
      a lui-addi pair.

  venus has slightly different translation rules for li, and it allows numbers
  larger than the largest 32 bit number to be loaded with li. You should follow
  the above rules if venus behaves differently.
*/
unsigned transform_li(Block *blk, char **args, int num_args) {
  /* li rd, imm -> (lui rd, imm[31:12]) addi rd, rd, imm[11:0] */
  if (num_args != 2) {
    return 0;
  }
  long long imm_ll = 0;
  if (read_imm_ll(args[1], &imm_ll) == -1) {
    return 0;
  }

  if (imm_ll >= (1LL << 32) || imm_ll < -(1LL << 31)) {
    return 0;
  }

  if (imm_ll < 0) {
    /* move the sign bit to 32-bit integer format */
    imm_ll &= (1LL << 32) - 1;
    imm_ll |= (1LL << 31);
  }

  int32_t imm = (int32_t)imm_ll;

  if (imm >= -(1 << 11) && imm < (1 << 11)) {
    char addi_imm[IMM_BUF_SIZE];
    snprintf(addi_imm, IMM_BUF_SIZE, "%d", imm);
    char *new_args[3] = {args[0], "x0", addi_imm};
    int result = add_to_block(blk, "addi", new_args, 3);
    return result == -1 ? 0 : 1;
  } else {
    int high = (imm + 0x800) >> 12;
    int low = imm - (high << 12);

    if (high < 0) {
      high &= (1 << 20) - 1;
      high |= (1 << 19);
    }

    if (low >= (1 << 11)) {
      low -= 1 << 12;
    }

    char addi_imm[IMM_BUF_SIZE];
    snprintf(addi_imm, IMM_BUF_SIZE, "%d", low);

    char lui_imm[IMM_BUF_SIZE];
    snprintf(lui_imm, IMM_BUF_SIZE, "%d", high);

    char *new_args[3] = {args[0], lui_imm, addi_imm};
    int result = add_to_block(blk, "lui", new_args, 2);
    if (result == -1) {
      return 0;
    }

    new_args[1] = args[0];
    result = add_to_block(blk, "addi", new_args, 3);
    return result == -1 ? 0 : 2;
  }
  return 0;
}

unsigned transform_mv(Block *blk, char **args, int num_args) {
  /* mv rd, rs -> addi rd, rs, 0 */
  if (num_args != 2) {
    return 0;
  }
  char *new_args[3] = {args[0], args[1], "0"};
  int result = add_to_block(blk, "addi", new_args, 3);
  return result == -1 ? 0 : 1;
}

unsigned transform_j(Block *blk, char **args, int num_args) {
  /* j offset -> jal x0, offset */
  if (num_args != 1) {
    return 0;
  }
  char *new_args[2] = {"x0", args[0]};
  int result = add_to_block(blk, "jal", new_args, 2);
  return result == -1 ? 0 : 1;
}

unsigned transform_jr(Block *blk, char **args, int num_args) {
  /* jr rs -> jalr x0, rs, 0 */
  if (num_args != 1) {
    return 0;
  }
  char *new_args[3] = {"x0", args[0], "0"};
  int result = add_to_block(blk, "jalr", new_args, 3);
  return result == -1 ? 0 : 1;
}

unsigned transform_jal(Block *blk, char **args, int num_args) {
  char *new_args[2];
  uint32_t arg_num;
  if (num_args == 1) {
    /* jal offset -> jal x1, offset */
    arg_num = 2;
    new_args[0] = "x1";
    new_args[1] = args[0];
  } else if (num_args == 2) {
    /* jal rd, offset */
    arg_num = 2;
    new_args[0] = args[0];
    new_args[1] = args[1];
  } else {
    /* Invalid number of args */
    return 0;
  }
  int result = add_to_block(blk, "jal", new_args, arg_num);
  return result == -1 ? 0 : 1;
}

unsigned transform_jalr(Block *blk, char **args, int num_args) {
  char *new_args[3];
  uint32_t arg_num;
  if (num_args == 1) {
    /* jalr rs -> jalr x1, rs, 0 */
    arg_num = 3;
    new_args[0] = "x1";
    new_args[1] = args[0];
    new_args[2] = "0";
  } else if (num_args == 3) {
    /* jalr rd, rs, offset */
    arg_num = 3;
    new_args[0] = args[0];
    new_args[1] = args[1];
    new_args[2] = args[2];
  } else {
    return 0;
  }
  int result = add_to_block(blk, "jalr", new_args, arg_num);
  return result == -1 ? 0 : 1;
}

unsigned transform_lw(Block *blk, char **args, int num_args) {
  if (num_args == 3) {
    /* lw rd, (offset)rs  */
    int result = add_to_block(blk, "lw", args, num_args);
    return result == -1 ? 0 : 1;
  } else if (num_args == 2) {
    /* lw rd, symbol -> auipc rd, symbol[31:12]; lw rd, (symbol[11:0])rd */
    /* This should be distinguished from the case of lw rd, symbol(rs) or auipc
     * rd, symbol */
    /* %pcrel_hi(symbol) for the high 20 bits of the address of symbol */
    /* %pcrel_lo(symbol) for the low 12 bits of the address of symbol */

    snprintf(labeled_symbol, LBL_BUF_SIZE, "%%pcrel_hi(%s)", args[1]);
    char *new_args[3] = {args[0], labeled_symbol, NULL};

    int result = add_to_block(blk, "auipc", new_args, 2);
    if (result == -1) {
      return 0;
    }

    snprintf(labeled_symbol, LBL_BUF_SIZE, "%%pcrel_lo(%s)", args[1]);
    new_args[2] = args[0];

    result = add_to_block(blk, "lw", new_args, 3);

    return result == -1 ? 0 : 2;
  }
  return 0;
}

/* Traverse pseudo_handlers table to select corresponding handler by NAME */
const PseudoHandler *find_pseudo_handler(const char *name) {
  for (size_t i = 0; i < sizeof(pseudo_handlers) / sizeof(pseudo_handlers[0]);
       i++) {
    if (strcmp(name, pseudo_handlers[i].name) == 0) {
      return &pseudo_handlers[i];
    }
  }
  return NULL;
}

/* Writes instructions during the assembler's first pass to BLK. The case
   for pseudo-instructions will be handled by handlers, but you need to
   write code to complete these TRANSFORM functions, as well as dealing
   with general instructions. Your pseudoinstruction expansions should not
   have any side effects.

   BLK is the intermediate instruction block you should write to,
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are
   valid, since that will be checked in part two.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(Block *blk, const char *name, char **args,
                        int num_args) {
  /* Deal with pseudo-instructions */
  const PseudoHandler *handler = find_pseudo_handler(name);
  if (handler) {
    return handler->transform(blk, args, num_args);
  }
  int result = add_to_block(blk, name, args, num_args);
  return result == -1 ? 0 : 1;
}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step.

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write
   anything to OUTPUT but simply return -1. venus may be a useful resource for
   this step.

   You need to add instruction definitions to instr_table first.

   Note the use of helper functions. Consider writing your own! If the function
   definition comes afterwards, you must declare it first (see translate.h).

   Returns 0 on success and -1 on error.
 */
int translate_inst(FILE *output, const char *name, char **args, size_t num_args,
                   uint32_t addr, SymbolTable *symtbl) {
  for (size_t i = 0; i < sizeof(instr_table) / sizeof(instr_table[0]); i++) {
    const InstrInfo *info = &instr_table[i];
    if (strcmp(name, info->name) == 0) {
      switch (info->instr_type) {
      case R_TYPE:
        return write_rtype(output, info, args, num_args);
      case I_TYPE:
        return write_itype(output, info, args, num_args, addr, symtbl);
      case S_TYPE:
        return write_stype(output, info, args, num_args);
      case SB_TYPE:
        return write_sbtype(output, info, args, num_args, addr, symtbl);
      case U_TYPE:
        return write_utype(output, info, args, num_args, addr, symtbl);
      case UJ_TYPE:
        return write_ujtype(output, info, args, num_args, addr, symtbl);
      }
    }
  }
  return -1;
}

static inline uint32_t bit_range(uint32_t num, uint32_t start, uint32_t end) {
  return (num >> start) & (((uint32_t)1 << (end - start + 1)) - 1);
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(FILE *output, const InstrInfo *info, char **args,
                size_t num_args) {
  if (num_args != 3) {
    return -1;
  }
  int rd = translate_reg(args[0]);
  int rs1 = translate_reg(args[1]);
  int rs2 = translate_reg(args[2]);
  if (rd == -1 || rs1 == -1 || rs2 == -1) {
    return -1;
  }
  uint32_t inst = info->opcode;
  inst |= rd << 7;
  inst |= info->funct3 << 12;
  inst |= rs1 << 15;
  inst |= rs2 << 20;
  inst |= info->funct7 << 25;
  write_inst_hex(output, inst);
  return 0;
}

/* Hint:
  - Number of ARGS and immediate range of each I_type instruction may
  vary. Refer to RISC-V green card and design proper encoding rules.
  - Some instruction(s) expanded from pseudo ones may has(have) unresolved
  label(s). You need to take that special case into consideration. Refer to
  write_sbtype for detailed relative address calculation.
 */
/*
lb, lh, lw, lbu, lhu, jalr
rd, imm, rs (12 bits signed)
funct3 from 12 to 14

lw rd, %pcrel_lo, rd
special case, treat pc as pc-4
*/
int write_itype(FILE *output, const InstrInfo *info, char **args,
                size_t num_args, uint32_t addr, SymbolTable *symtbl) {
  uint64_t inst = info->opcode;
  if (num_args == 3) {
    if (strcmp(info->name, "addi") == 0 || strcmp(info->name, "xori") == 0 ||
        strcmp(info->name, "ori") == 0 || strcmp(info->name, "andi") == 0 ||
        strcmp(info->name, "slti") == 0 || strcmp(info->name, "sltiu") == 0) {
      int rd = translate_reg(args[0]);
      int rs = translate_reg(args[1]);
      if (rd == -1 || rs == -1) {
        return -1;
      }
      long imm = 0;
      int64_t sym_addr = get_addr_for_symbol(symtbl, args[2]);
      if (sym_addr != -1) {
        imm = sym_addr;
      } else if (translate_num(&imm, args[2], IMM_12_SIGNED) == -1) {
        return -1;
      }
      inst |= rd << 7;
      inst |= rs << 15;
      inst |= bit_range(imm, 0, 11) << 20;
      if (imm < 0) {
        inst |= 1 << 31;
      }
    } else if (strcmp(info->name, "slli") == 0 ||
               strcmp(info->name, "srli") == 0 ||
               strcmp(info->name, "srai") == 0) {
      int rd = translate_reg(args[0]);
      int rs = translate_reg(args[1]);
      if (rd == -1 || rs == -1) {
        return -1;
      }
      long imm = 0;
      if (translate_num(&imm, args[2], IMM_5_UNSIGNED) == -1) {
        return -1;
      }
      inst |= rd << 7;
      inst |= rs << 15;
      inst |= imm << 20;
    } else if (strcmp(info->name, "lb") == 0 || strcmp(info->name, "lh") == 0 ||
               strcmp(info->name, "lbu") == 0 ||
               strcmp(info->name, "lhu") == 0 ||
               strcmp(info->name, "jalr") == 0) {
      char *rd_arg = args[0];
      char *rs_arg = args[2];
      char *imm_arg = args[1];
      if (strcmp(info->name, "jalr") == 0) {
        rs_arg = args[1];
        imm_arg = args[2];
      }
      int rd = translate_reg(rd_arg);
      int rs = translate_reg(rs_arg);
      if (rd == -1 || rs == -1) {
        return -1;
      }
      long imm = 0;
      int64_t sym_addr = get_addr_for_symbol(symtbl, imm_arg);
      if (sym_addr != -1) {
        imm = sym_addr;
      } else if (translate_num(&imm, imm_arg, IMM_12_SIGNED) == -1) {
        return -1;
      }
      inst |= rd << 7;
      inst |= bit_range(imm, 0, 11) << 20;
      inst |= rs << 15;
      if (imm < 0) {
        inst |= 1 << 31;
      }
    } else if (strcmp(info->name, "lw") == 0) {
      int rd = translate_reg(args[0]);
      int rs = translate_reg(args[2]);
      if (rd == -1 || rs == -1) {
        return -1;
      }
      long imm = 0;
      if (args[1] && args[1][0] == '%') {
        /* %pcrel_lo(symbol) */
        size_t len = strlen(args[1]);
        memcpy(labeled_symbol, args[1] + 10, len - 11);
        labeled_symbol[len - 11] = '\0';
        int64_t sym_addr = get_addr_for_symbol(symtbl, labeled_symbol);
        if (sym_addr == -1) {
          return -1;
        }
        long offset = sym_addr - (addr - 4);
        long hi20 = (offset + 0x800) >> 12;
        imm = offset - (hi20 << 12);
      } else {
        int64_t sym_addr = get_addr_for_symbol(symtbl, args[1]);
        if (sym_addr != -1) {
          /* Use label address as imm */
          if (!is_valid_imm(sym_addr, IMM_12_SIGNED)) {
            return -1;
          }
          imm = sym_addr;
        } else if (translate_num(&imm, args[1], IMM_12_SIGNED) == -1) {
          /* Can't load by symbol or imm */
          return -1;
        }
      }
      inst |= rd << 7;
      inst |= bit_range(imm, 0, 11) << 20;
      inst |= rs << 15;
      if (imm < 0) {
        inst |= 1 << 31;
      }
    } else {
      /* NOT REACHED */
      return -1;
    }
  } else if (!(num_args == 0 && info->opcode == 0x73)) {
    return -1;
  }
  inst |= info->funct3 << 12;
  inst |= info->funct7 << 25;
  write_inst_hex(output, inst);
  return 0;
}

int write_stype(FILE *output, const InstrInfo *info, char **args,
                size_t num_args) {
  /* (To the TAs:) S-type instructions should have the ability to use symbols as
   * offsets, but you forgot to add this feature. The symbol table should be
   * passed to this function. */
  if (num_args != 3) {
    return -1;
  }
  int rs2 = translate_reg(args[0]);
  int rs1 = translate_reg(args[2]);
  if (rs1 == -1 || rs2 == -1) {
    return -1;
  }
  long imm = 0;
  if (translate_num(&imm, args[1], IMM_12_SIGNED) == -1) {
    return -1;
  }
  uint32_t inst = info->opcode;
  inst |= bit_range(imm, 0, 4) << 7;
  inst |= info->funct3 << 12;
  inst |= rs1 << 15;
  inst |= rs2 << 20;
  inst |= bit_range(imm, 5, 11) << 25;
  if (imm < 0) {
    inst |= 1 << 31;
  }
  write_inst_hex(output, inst);
  return 0;
}

int write_sbtype(FILE *output, const InstrInfo *info, char **args,
                 size_t num_args, uint32_t addr, SymbolTable *symtbl) {
  if (num_args != 3) {
    return -1;
  }
  int rs1 = translate_reg(args[0]);
  int rs2 = translate_reg(args[1]);
  if (rs1 == -1 || rs2 == -1) {
    return -1;
  }
  long imm = 0;
  int64_t sym_addr = get_addr_for_symbol(symtbl, args[2]);
  if (sym_addr != -1) {
    /* Jump to the address of the label */
    imm = sym_addr - addr;
    if (!is_valid_imm(imm, IMM_13_SIGNED)) {
      return -1;
    }
  } else if (translate_num(&imm, args[2], IMM_13_SIGNED) == -1) {
    /* Can't jump by symbol or imm */
    return -1;
  }

  uint32_t inst = info->opcode;
  inst |= bit_range(imm, 11, 11) << 7;
  inst |= bit_range(imm, 1, 4) << 8;
  inst |= info->funct3 << 12;
  inst |= rs1 << 15;
  inst |= rs2 << 20;
  inst |= bit_range(imm, 5, 10) << 25;
  if (imm < 0) {
    inst |= 1 << 31;
  }
  write_inst_hex(output, inst);
  return 0;
}

int write_utype(FILE *output, const InstrInfo *info, char **args,
                size_t num_args, uint32_t addr, SymbolTable *symtbl) {
  if (num_args != 2) {
    return -1;
  }
  int rd = translate_reg(args[0]);
  long imm = 0;
  if (args[1] && args[1][0] == '%') {
    /* %pcrel_hi(symbol) */
    size_t len = strlen(args[1]);
    memcpy(labeled_symbol, args[1] + 10, len - 11);
    labeled_symbol[len - 11] = '\0';
    int64_t sym_addr = get_addr_for_symbol(symtbl, labeled_symbol);

    if (sym_addr == -1) {
      /* Invalid symbol for lw rd, symbol */
      return -1;
    }
    imm = (sym_addr - addr + 0x800) >> 12;
  } else {
    int64_t sym_addr = get_addr_for_symbol(symtbl, args[1]);
    if (sym_addr != -1) {
      /* Use label address as imm */
      if (!is_valid_imm(sym_addr, IMM_20_UNSIGNED)) {
        return -1;
      }
      imm = sym_addr;
    } else if (translate_num(&imm, args[1], IMM_20_UNSIGNED) == -1) {
      /* Can't jump by symbol or imm */
      return -1;
    }
  }
  uint32_t inst = info->opcode;
  inst |= rd << 7;
  inst |= imm << 12;
  write_inst_hex(output, inst);
  return 0;
}

int write_ujtype(FILE *output, const InstrInfo *info, char **args,
                 size_t num_args, uint32_t addr, SymbolTable *symtbl) {
  if (num_args != 2) {
    return -1;
  }
  int rd = translate_reg(args[0]);
  if (rd == -1) {
    return -1;
  }
  long imm = 0;
  int64_t sym_addr = get_addr_for_symbol(symtbl, args[1]);
  if (sym_addr != -1) {
    /* Jump to the address of the label */
    imm = sym_addr - addr;
    if (!is_valid_imm(imm, IMM_21_SIGNED)) {
      return -1;
    }
  } else if (translate_num(&imm, args[1], IMM_21_SIGNED) == -1) {
    /* Can't jump by symbol or imm */
    return -1;
  }
  uint32_t inst = info->opcode;
  inst |= rd << 7;
  inst |= bit_range(imm, 12, 19) << 12;
  inst |= bit_range(imm, 11, 11) << 20;
  inst |= bit_range(imm, 1, 10) << 21;
  if (imm < 0) {
    inst |= 1 << 31;
  }
  write_inst_hex(output, inst);
  return 0;
}
