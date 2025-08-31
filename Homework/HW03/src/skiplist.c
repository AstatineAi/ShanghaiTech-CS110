#include "skiplist.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//  DO NOT CHANGE THESE DEFINES
#define MAX_LEVEL 16
#define CAPACITY (2 << MAX_LEVEL)
#define P 0.5

#define IMPLEMENT_ME()                                                         \
  do {                                                                         \
    (void)__func__;                                                            \
    printf("TODO: Implement skiplist function %s at %s:%d!\n", __func__,       \
           __FILE__, __LINE__);                                                \
  } while (0)

static int rand_level(void);

/**
 * You can traverse to the corresponding implementations for detailed
 * documentation. Feel free to add helper functions.
 */
static SkipListNode *sl_node_create(int level, const char *member, int score);
static void sl_node_free(SkipListNode *node);

/**
 * Create a new empty skiplist with:
 * - initialized header node (whose level is MAX_LEVEL)
 * - level set to 1
 * - length set to 0
 *
 * The `header` serves as the entry point to the skiplist and
 * contains forward pointers to the actual elements, thus is not
 * counted by `length`.
 *
 * @return pointer to the created skiplist, or NULL if failed
 */
SkipList *sl_create(void) {
  SkipList *sl = malloc_arena(sizeof(SkipList));
  if (!sl)
    return NULL;
  sl->header = sl_node_create(MAX_LEVEL, "", INT_MIN);
  if (!sl->header) {
    free_arena(sl);
    return NULL;
  }
  sl->level = 1;
  sl->length = 0;
  return sl;
}

/**
 * Free a skiplist `sl` by deallocating:
 * - all nodes
 * - the header node
 * - `sl` itself
 *
 * @param sl skiplist to free
 */
void sl_free(SkipList *sl) {
  if (!sl)
    return;
  SkipListNode *node = sl->header->forward[1];
  while (node) {
    SkipListNode *next = node->forward[1];
    sl_node_free(node);
    node = next;
  }
  sl_node_free(sl->header);
  free_arena(sl);
}

/**
 * @param sl skilist
 * @return   The number of elements that `sl` has. -1 if `sl` is NULL
 */
int sl_get_length(SkipList *sl) {
  if (!sl)
    return -1;
  return sl->length;
}

/**
 * Insert a new node with given `member` and `score` into the skiplist `sl`.
 *
 * Insertion fails when:
 * - `sl` or `member` string is NULL
 * - beyond CAPACITY
 * - a node with the same score already exists (should be checked at
 *   `_sl_insert()`)
 *
 * Note that uplicate member checking is time consuming here and should be
 * handled at a higher level using a dictionary, so you don't have to take care
 * of this case.
 *
 * @param sl     the skiplist to insert into
 * @param member string identifier for the new node
 * @param score  integer score for ordering (must be unique)
 * @return       1 for successful insertion, 0 otherwise
 */
int sl_insert(SkipList *sl, const char *member, int score) {
  if (!sl || !member || sl->length >= CAPACITY)
    return 0;
  return _sl_insert(sl, member, score, rand_level());
}

/**
 * Helper function for sl_insert that `performs` the actual insertion with a
 * predetermined level.
 *
 * This function handles the core insertion logic once the random level has been
 * determined by sl_insert. It performs the following operations:
 * 1. creates a new node with the given level, member and score
 * 2. searches for the insertion position at each level of the skiplist
 * 3. updates the forward pointers at each level to maintain skiplist structure
 * 4. updates Skip List's level if new node's level is higher than that of Skip
 * List
 *
 * The function assumes all preliminary validations (NULL `sl`, capacity) have
 * already been performed by `sl_insert()`.
 *
 * Its in-header-decleration is only for testing. For normal usage, call
 * `sl_insert()` instead.
 *
 * @param sl     the skiplist to insert into
 * @param member string identifier for the new node
 * @param score  integer score for ordering (must be unique)
 * @param level  the pre-determined level for the new node
 * @return       1 for successful insertion, 0 if a node with the same score
 * already exists
 */
int _sl_insert(SkipList *sl, const char *member, int score, int level) {
  SkipListNode *update[MAX_LEVEL + 1];
  SkipListNode *node = sl_node_create(level, member, score);

  if (!node)
    return 0;
  SkipListNode *current = sl->header;
  for (int i = sl->level; i >= 1; i--) {
    while (current->forward[i] && current->forward[i]->score < score)
      current = current->forward[i];
    update[i] = current;
  }

  if (current->forward[1] && current->forward[1]->score == score) {
    sl_node_free(node);
    return 0;
  }

  if (level > sl->level) {
    for (int i = sl->level + 1; i <= level; i++)
      update[i] = sl->header;
    sl->level = level;
  }

  for (int i = 1; i <= level; i++) {
    node->forward[i] = update[i]->forward[i];
    update[i]->forward[i] = node;
  }

  sl->length++;

  return 1;
}

/**
 * Remove a node with given `score` from the skiplist `sl` by following steps:
 * 1. search for the exact node with given `score`
 * 2. update forward pointers to "skip over" the node to deleted
 * 3. free the node's allocated resources
 * 4. adjust `sl`'s level after removing if needed
 *    - when there are levels has no nodes (note that skip list's level >= 1)
 *
 * @param sl    the skiplist to remove from
 * @param score score of the node to remove
 * @return      1 if a node was removed, 0 if not found or error
 */
int sl_remove(SkipList *sl, int score) {
  if (!sl)
    return 0;

  SkipListNode *update[MAX_LEVEL + 1];
  SkipListNode *current = sl->header;

  for (int i = sl->level; i >= 1; i--) {
    while (current->forward[i] && current->forward[i]->score < score)
      current = current->forward[i];
    update[i] = current;
  }

  if (!current->forward[1] || current->forward[1]->score != score)
    return 0;

  SkipListNode *node = current->forward[1];
  for (int i = 1; i <= sl->level; i++) {
    if (update[i]->forward[i] != node)
      break;
    update[i]->forward[i] = node->forward[i];
  }

  sl_node_free(node);

  while (sl->level > 1 && !sl->header->forward[sl->level])
    sl->level--;

  sl->length--;

  return 1;
}

/*
 * DO NOT CHANGE THIS FUNCTION
 * Print skip list structures.
 */
void sl_print(SkipList *sl) {
  if (!sl)
    return;

  printf("\nSkip List (level %d):\n", sl->level);

  for (int i = sl->level; i >= 1; i--) {
    SkipListNode *node = sl->header;
    printf("Level %d: header", i);
    while (node->forward[i]) {
      printf(" --> (%s,%d)", node->forward[i]->member, node->forward[i]->score);
      node = node->forward[i];
    }
    printf("\n");
  }
}

/**
 * Find a node with given `score` in the skiplist `sl`. You should traverse the
 * skiplist efficiently using the multi-level structure, starting from the
 * highest level and working downwards. At each level, advance as far as
 * possible without overshooting the target score.
 *
 * @param sl    the skiplist to search in
 * @param score the score to search for
 * @return      pointer to the node if found, NULL otherwise
 */
SkipListNode *sl_get_by_score(SkipList *sl, int score) {
  if (!sl)
    return NULL;
  SkipListNode *node = sl->header;
  for (int i = sl->level; i >= 1; i--) {
    while (node->forward[i] && node->forward[i]->score < score)
      node = node->forward[i];
  }
  node = node->forward[1];
  return node && node->score == score ? node : NULL;
}

/**
 * Get the node with given `rank` in the skiplist `sl`.
 *
 * Rank -- the position of a node when nodes are ordered by score:
 * - Rank 1 is the node with the lowest score
 * - Rank N (where N is the length of the skiplist) is the node with the highest
 * score
 *
 * You may find it is inefficient to traverse to the `rank`th element in `sl`.
 * You can add another array -- `span` to leverage the multi-level structure of
 * skiplist for faster traversing. `span[i]` represents "rank distance"
 * from a node to its next node, `forward[i]`, at level `i`. Thus `span[1]`
 * always equals to 1. You can implement a "faster" version of this interface if
 * interested.
 *
 * @param sl   the skiplist to search in
 * @param rank the rank to search for, within [1, length]
 * @return     pointer to the node with the specified rank, NULL if invalid rank
 * or error
 */
SkipListNode *sl_get_by_rank(SkipList *sl, int rank) {
  if (!sl)
    return NULL;
  SkipListNode *node = sl->header;
  if (!node || rank < 1 || rank > sl->length)
    return NULL;
  for (int i = 0; i < rank; i++)
    node = node->forward[1];
  return node;
}

/**
 * Get the rank of a node with given `score` from the skiplist `sl`.
 *
 * @param sl    the skiplist to search in
 * @param score the score to find the rank for
 * @return      rank (1-based) if found, 0 if not found or error
 */
int sl_get_rank_by_score(SkipList *sl, int score) {
  if (!sl)
    return 0;
  SkipListNode *node = sl->header;
  int rank = 0;
  while (node->forward[1] && node->forward[1]->score < score) {
    rank++;
    node = node->forward[1];
  }
  if (!node->forward[1] || node->forward[1]->score != score) {
    return 0;
  }
  return rank + 1;
}

/**
 * Get all nodes whose ranks are within [start, end] (inclusive). Returned nodes
 * should be sorted by their ranks in ascending order (matches start, start+1,
 * ..., end). You need to allocate spaces to store the pointers to nodes found.
 *
 * @param sl    the skiplist to search in
 * @param start starting rank (must be within [1, length])
 * @param end   ending rank (must be within [start, length])
 * @param count if valid, should be set to the number of nodes found. 0 if error
 *              or none found
 * @return      pointers to the found nodes, NULL if invalid range, invalid
 * parameter or none found
 */
SkipListNode **sl_get_range_by_rank(SkipList *sl, int start, int end,
                                    int *count) {
  if (!count)
    return NULL;

  *count = 0;

  if (!sl || start < 1 || end < start || end > sl->length)
    return NULL;

  SkipListNode **nodes = malloc(sizeof(SkipListNode *) * (end - start + 1));

  if (!nodes)
    return NULL;

  SkipListNode *node = sl->header;
  for (int i = 0; i < start; i++)
    node = node->forward[1];

  for (*count = 0; *count < end - start + 1; (*count)++) {
    nodes[*count] = node;
    node = node->forward[1];
  }

  return nodes;
}

/**
 * Get all nodes whose scores are within [min_score, max_score] (inclusive).
 * Returned nodes should be sorted by their scores in ascending order.
 *
 * @param sl        the skiplist to search in
 * @param min_score minimum score (inclusive)
 * @param max_score maximum score (inclusive) (must >= min_score)
 * @param count     if valid, should be set to the number of nodes found. 0 if
 *                  error or none found
 * @return          pointers to the found nodes, NULL if invalid range, invalid
 * paramter or none found
 */
SkipListNode **sl_get_range_by_score(SkipList *sl, int min_score, int max_score,
                                     int *count) {
  if (!count)
    return NULL;

  *count = 0;

  if (!sl || max_score < min_score)
    return NULL;

  SkipListNode *begin = sl->header;
  for (int i = sl->level; i >= 1; i--) {
    while (begin->forward[i] && begin->forward[i]->score < min_score)
      begin = begin->forward[i];
  }

  SkipListNode *p = begin;
  while (p->forward[1] && p->forward[1]->score <= max_score) {
    (*count)++;
    p = p->forward[1];
  }

  if (*count == 0)
    return NULL;

  SkipListNode **nodes = malloc(sizeof(SkipListNode *) * (*count));
  if (!nodes) {
    *count = 0;
    return NULL;
  }

  for (int i = 0; i < *count; i++) {
    begin = begin->forward[1];
    nodes[i] = begin;
  }

  return nodes;
}

/**
 * Create and return a new skiplist node. Each node in a skiplist maintains
 * multiple forward pointers to enable efficient "skipping" during traversal.
 * The number of forward pointers is determined by the node's level.
 *
 * @param level  number of levels
 * @param member string identifier
 * @param score  used for ordering
 * @return       Pointer to the newly created node, or NULL if failed
 */
static SkipListNode *sl_node_create(int level, const char *member, int score) {
  SkipListNode *node = malloc_arena(sizeof(SkipListNode));
  if (!node)
    return NULL;
  node->member = strdup_arena(member);
  if (!node->member) {
    free_arena(node);
    return NULL;
  }
  node->forward = malloc_arena(sizeof(SkipListNode *) * (level + 1));
  if (!node->forward) {
    free_arena(node->member);
    free_arena(node);
    return NULL;
  }
  for (int i = 0; i <= level; i++)
    node->forward[i] = NULL;
  node->score = score;
  node->level = level;
  return node;
}

/**
 * Free all resources allocated to a skiplist node.
 *
 * @param node to be freed
 */
static void sl_node_free(SkipListNode *node) {
  if (!node)
    return;
  free_arena(node->member);
  free_arena(node->forward);
  free_arena(node);
}

/**
 * DO NOT CHANGE THIS FUNCTION
 *
 * Generate a random level for a new skiplist node by "coin tossing".
 * - Every node has base level 1.
 * - Each node has a probability P (already defined) of "promoting" to
 * next level until reaches MAX_LEVEL
 *
 * This creates a distribution where approximately:
 * - All nodes have level 1
 * - P of nodes have level 2
 * - P^2 of nodes have level 3
 * - ...
 *
 * @return node's level -- an integer ranges [1, MAX_LEVEL]
 */
static int rand_level(void) {
  int level = 1;
  while ((double)rand() / RAND_MAX < P && level < MAX_LEVEL)
    level++;
  return level;
}

size_t arena_cap = ARENA_SIZE;
size_t arena_used = 0;
uint8_t arena[ARENA_SIZE];

void *malloc_arena(size_t size) {
  if (arena_used + size > arena_cap) {
    fprintf(stderr, "OK, you win.");
    return NULL;
  }
  void *ptr = arena + arena_used;
  arena_used += size;
  return ptr;
}

void free_arena(void *ptr) {
  (void)ptr;
  /**
   * Do nothing
   * This arena will be collected by OS
   * Choosing C for CS110 is a mistake
   * It's disrespect to CS100
   * Actually I get this idea from std::pmr::monotonic_buffer_resource
   */
}

char *strdup_arena(const char *str) {
  size_t len = strlen(str) + 1;
  char *dup = malloc_arena(len);
  if (dup)
    memcpy(dup, str, len);
  return dup;
}
