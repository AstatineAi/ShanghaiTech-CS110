#include "cache.h"
#include <stdio.h>
#include <stdlib.h>


#define BLOCK_SIZE 16
#define NUM_BLOCKS 4
#define TAG_MASK 0xFFFFFFF0 // Mask for 16-byte alignment

typedef struct {
  uint32_t tag;
  int valid;
  int lru_counter;
} CacheLine;

static CacheLine cache[NUM_BLOCKS];
static int total_accesses = 0;
static int inst_hits = 0;
static int data_hits = 0;
static int current_time = 0;

void init_cache_system() {
  for (int i = 0; i < NUM_BLOCKS; i++) {
    cache[i].valid = 0;
    cache[i].lru_counter = 0;
  }
  total_accesses = 0;
  inst_hits = 0;
  data_hits = 0;
  current_time = 0;
}

static int find_lru_block() {
  int lru_idx = 0;
  int min_time = cache[0].lru_counter;

  for (int i = 1; i < NUM_BLOCKS; i++) {
    if (!cache[i].valid || cache[i].lru_counter < min_time) {
      lru_idx = i;
      min_time = cache[i].lru_counter;
    }
  }
  return lru_idx;
}

static void access_cache(uint32_t addr, int is_instruction) {
  total_accesses++;
  uint32_t tag = addr & TAG_MASK;

  fprintf(stderr, "%s access at address 0x%x (tag: 0x%x)\n",
          is_instruction ? "Instruction" : "Data", addr, tag);

  // Check for hit
  for (int i = 0; i < NUM_BLOCKS; i++) {
    if (cache[i].valid && cache[i].tag == tag) {
      cache[i].lru_counter = current_time++;
      if (is_instruction) {
        inst_hits++;
        fprintf(stderr, "Instruction cache hit at block %d\n", i);
      } else {
        data_hits++;
        fprintf(stderr, "Data cache hit at block %d\n", i);
      }
      return;
    }
  }

  // Cache miss - replace LRU block
  int lru_idx = find_lru_block();
  cache[lru_idx].tag = tag;
  cache[lru_idx].valid = 1;
  cache[lru_idx].lru_counter = current_time++;

  fprintf(stderr, "Cache miss, replacing block %d\n", lru_idx);
}

void add_inst_access(uint32_t addr) { access_cache(addr, 1); }

void add_data_access(uint32_t addr) { access_cache(addr, 0); }

void print_cache_statistics() {
  printf("Total memory accesses: %d.\n", total_accesses);
  printf("Instruction cache hit: %d.\n", inst_hits);
  printf("Data cache hit: %d.\n", data_hits);
  printf("Total cache misses: %d.\n", total_accesses - inst_hits - data_hits);
}
