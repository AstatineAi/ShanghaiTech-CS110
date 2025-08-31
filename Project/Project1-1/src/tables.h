/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#ifndef TABLES_H
#define TABLES_H

#include <stdint.h>
#include <stdio.h>

/* increment of capacity */
#define INCREMENT_OF_CAP 32

/* Indicates whether unique labels are supported. */
extern const int SYMBOLTBL_NON_UNIQUE;  /* allows duplicate names in table */
extern const int SYMBOLTBL_UNIQUE_NAME; /* duplicate names not allowed */

/* Entry of the symbol table. */
typedef struct {
  char *name;
  uint32_t addr;
} Symbol;

typedef struct {
  /* Pointer to the array of entries in the table. */
  Symbol *entries;
  /* The current length of the table. */
  uint32_t len;
  /* The current capacity of the table. */
  uint32_t cap;
  /* The mode of the table - whether it allows duplicate names. */
  int mode;
} SymbolTable;

/* Helper functions: */
void allocation_failed(void);
void addr_alignment_incorrect(void);
void name_already_exists(const char *name);
void write_sym(FILE *output, uint32_t addr, const char *name);

SymbolTable *create_table(int mode);

void free_table(SymbolTable *table);

void resize_table(SymbolTable *table);

int add_to_table(SymbolTable *table, const char *name, uint32_t addr);

int64_t get_addr_for_symbol(SymbolTable *table, const char *name);

void write_table(SymbolTable *table, FILE *output);

#endif
