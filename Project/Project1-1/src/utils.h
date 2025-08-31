/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#ifndef UTILS_H
#define UTILS_H

#define WORD_SIZE 4
#define INSN_SIZE 4

int is_log_file_set(void);

void set_log_file(const char *filename);

void write_to_log(char *fmt, ...);

void log_inst(const char *name, char **args, int num_args);

int read_imm(const char *arg, long *imm);

int read_imm_ll(const char *arg, long long *imm);

#endif
