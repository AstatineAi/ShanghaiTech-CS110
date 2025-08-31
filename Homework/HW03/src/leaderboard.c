#include "leaderboard.h"
#include "dict.h"
#include "skiplist.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DO NOT CHANGE THIS DEFINE
#define DICT_BUCKETS 1024
#define IMPLEMENT_ME()                                                         \
  do {                                                                         \
    (void)__func__;                                                            \
    printf("TODO: Implement leaderboard function %s at %s:%d!\n", __func__,    \
           __FILE__, __LINE__);                                                \
  } while (0)

static void print_array(SkipListNode **nodes, int node_count, int reverse);
static void print_int(int r) { printf("(integer) %d\n", r); }
static void print_nil(void) { printf("(nil)\n"); }
static int conv_rank(int rank, int length) {
  return rank < 0 ? rank + length + 1 : rank;
}

/**
 * Create a new empty leaderboard.
 *
 * @return pointer to the created leaderboard, or NULL if failed
 */
Leaderboard *lb_create(void) {
  Leaderboard *lb = (Leaderboard *)malloc(sizeof(Leaderboard));
  if (!lb)
    return NULL;

  lb->sl = sl_create();
  if (!lb->sl) {
    free(lb);
    return NULL;
  }
  lb->dict = dict_create(DICT_BUCKETS);
  if (!lb->dict) {
    sl_free(lb->sl);
    free(lb);
    return NULL;
  }

  return lb;
}

/**
 * Free all resources associated with a leaderboard.
 *
 * @param lb to be freed
 */
void lb_free(Leaderboard *lb) {
  if (!lb)
    return;

  if (lb->sl)
    sl_free(lb->sl);
  if (lb->dict)
    dict_free(lb->dict);

  free(lb);
}

/**
 * Add or update a `member` in the leaderboard `lb` with the given `score`.
 *
 * This function:
 * - adds `member` with `score` into `lb` if `member` doesn't exist
 * - updates `member`'s score if `member` exists and `score` is different to the
 * old score.
 * - does not add or update if a `member` with the same `score` already exists
 *
 * @param lb     The leaderboard to modify
 * @param member The member name to add or update
 * @param score  The score to assign to the member
 *
 * Output:
 * - Print the number of members added or updated (0 or 1) via `print_int()`.
 * - Print 0 if any parameter is invalid via `print_int()`.
 */
void zadd(Leaderboard *lb, const char *member, int score) {
  if (!lb || !member) {
    print_int(0);
    return;
  }
  /* Check if member exists */
  DictEntry *entry = dict_get(lb->dict, member);
  SkipListNode *node = sl_get_by_score(lb->sl, score);

  if (!entry) {
    /* Add new member */
    int result = dict_insert(lb->dict, member, score);
    if (!result) {
      print_int(0);
      return;
    }
    result = sl_insert(lb->sl, member, score);
    if (!result) {
      print_int(0);
      dict_remove(lb->dict, member);
      return;
    }
    print_int(1);
  } else {
    /* Update existing member */
    if (node) {
      /* Member with the same score already exists */
      print_int(0);
    } else {
      /* Update member's score */
      int result = sl_remove(lb->sl, entry->val);
      if (!result) {
        print_int(0);
        return;
      }
      entry->val = score;
      result = sl_insert(lb->sl, member, score);
      if (!result) {
        print_int(0);
        return;
      }
      print_int(1);
    }
  }
}

/**
 * Remove `member` from the leaderboard `lb`.
 *
 * @param lb     the leaderboard to modify
 * @param member the member name to remove
 *
 * Output:
 * - Print the number of members removed (0 or 1) via `print_int()`
 * - Print 0 if any parameter is invalid via `print_int()`
 */
void zrem(Leaderboard *lb, const char *member) {
  if (!lb || !member) {
    print_int(0);
    return;
  }

  DictEntry *entry = dict_get(lb->dict, member);
  if (!entry) {
    print_int(0);
    return;
  }

  int result = sl_remove(lb->sl, entry->val);

  if (!result) {
    print_int(0);
    return;
  }

  result = dict_remove(lb->dict, member);

  if (!result) {
    print_int(0);
    return;
  }

  print_int(1);
}

/**
 * Print the number of members in the leaderboard `lb`.
 *
 * @param lb the leaderboard to query
 *
 * Output:
 * - Prints the number of entries in `lb` via `print_int()`
 * - Print 0 if invalid `lb` via `print_int()`
 */
void zcard(Leaderboard *lb) {
  if (!lb) {
    print_int(0);
    return;
  }

  int result = sl_get_length(lb->sl);
  print_int(result == -1 ? 0 : result);
}

/**
 * Print the score of `member`.
 *
 * @param lb     the leaderboard to query
 * @param member the member to look up
 *
 * Output:
 * - Print `member`'s score if found via `print_int()`
 * - Call `print_nil()` if member is not found or any parameter is invalid
 */
void zscore(Leaderboard *lb, const char *member) {
  if (!lb || !member) {
    print_nil();
    return;
  }

  DictEntry *entry = dict_get(lb->dict, member);

  if (!entry) {
    print_nil();
  } else {
    print_int(entry->val);
  }
}

/**
 * Print the rank of `member`. Rank starts from 1 for the member
 * with the lowest score. If reverse is true, prints the reverse rank
 * (where the member with highest score has rank 1).
 *
 * @param lb      the leaderboard to query
 * @param member  the member look up
 * @param reverse if true, print reverse rank
 *
 * Output:
 * - Prints the member's rank if found via `print_int()`
 * - Call `print_nil()` if not found or any parameter is invalid
 */
void zrank(Leaderboard *lb, const char *member, int reverse) {
  if (!lb || !member) {
    print_nil();
    return;
  }

  DictEntry *entry = dict_get(lb->dict, member);

  if (!entry) {
    print_nil();
    return;
  }

  int rank = sl_get_rank_by_score(lb->sl, entry->val);

  if (!rank) {
    print_nil();
    return;
  }

  if (!reverse) {
    print_int(rank);
  } else {
    int tot = sl_get_length(lb->sl);
    print_int(tot - rank + 1);
  }
}

/**
 * Print the reverse rank of `member`
 *
 * @param lb     the leaderboard to query
 * @param member the member name to look up
 *
 * Output:
 * - Prints the member's reverse rank if found via `print_int()`
 * - Call `print_nil()` if not found or any parameter is invalid
 */
void zrevrank(Leaderboard *lb, const char *member) { zrank(lb, member, 1); }

/**
 * IMPLEMENT ME
 *
 * Print all members within rank range [start, end].
 *
 * The `start` and `end` parameters can be negative to indicate positions
 * relative to the end of the list
 *  - rank `-1` refers to the last element in the leaderboard's skip list
 *  - rank `-2` refers to the second-to-last element
 *  - And so on...
 *
 * To convert negative rank to posiive ones:
 *   if (rank < 0) rank = length + rank + 1;
 *
 * A range query is considered valid if:
 *  - After conversion, `start` ≤ `end`
 *  - After conversion, , 1 <= `start` <= length, `start` <= `end` <= length
 * For example, with a leaderboard of 10 elements:
 *  - `zrange 1 3`: Valid, returns elements at positions 1, 2, and 3
 *  - `zrange -3 -1`: Valid, returns the last three elements
 *  (positions 8, 9, 10)
 *  - `zrange 3 2`: Invalid, start > end
 * - `zrange -15 -13`: Invalid if converted indices are out of bounds (neither
 *    converted `start`, -4, nor converted `end`, -2, in [1, 10])
 *
 * @param lb    the leaderboard to query
 * @param start starting rank (inclusive, 1-based, can be negative)
 * @param end   ending rank (inclusive, 1-based, can be negative)
 *
 * Output:
 * - Call `print_array()` to print all members and their scores in the specified
 * rank range
 * - Also call `print_array()` with proper parameters if `lb` is NULL, range
 * is invalid or no members found
 */
void zrange(Leaderboard *lb, int start, int end) {
  if (!lb || !lb->sl) {
    print_array(NULL, 0, 0);
    return;
  }

  int length = sl_get_length(lb->sl);

  start = conv_rank(start, length);
  end = conv_rank(end, length);

  if (start > end || start < 1 || start > length || end < 1 || end > length) {
    print_array(NULL, 0, 0);
    return;
  }

  int count;

  SkipListNode **nodes = sl_get_range_by_rank(lb->sl, start, end, &count);

  print_array(nodes, count, 0);

  free(nodes);
}

/**
 * Print all members within "reverse rank" range [start, end].
 *
 * The `start` and `end` parameters can be negative to indicate positions
 * relative to the end of the list, but their interpretation is relative to the
 * reversed order of the list:
 *  - reverse rank `-1` refers to the lowest-scoring element in the leaderboard
 *  - reverse rank `-2` refers to the second-lowest-scoring element
 *  - And so on...
 *
 * To convert negative reverse rank to posiive ones:
 *   if (reverse_rank < 0) reverse_rank = length + reverse_rank + 1;
 *
 * Similarly, a reverse range query is considered valid if:
 *  - After conversion, `start` ≤ `end`
 *  - After conversion, , 1 <= `start` <= length, `start` <= `end` <= length
 * However, the result interpretation differs because `zrevrange` prints
 * elements in **descending score** order. For example, with the same
 * leaderboard of 10 elements:
 *  - `zrevrange 1 3`: Valid, returns elements with the 3 highest scores
 *    (reverse ranks 1, 2, 3, ranks 10, 9, 8)
 *  - `zrevrange -3 -1`: Valid, returns the 3 lowest-scoring elements
 *    (reverse ranks 8, 9, 10, ranks 3, 2, 1)
 *  - `zrevrange 3 2`: Invalid, start > end
 *  - `zrevrange -15 -13`: Invalid if converted indices are out of bounds
 *
 * @param lb    the leaderboard to query
 * @param start start reverse rank (inclusive, 1-based, can be negative)
 * @param end   end reverse rank (inclusive, 1-based, can be negative)
 *
 * Output:
 * - Call `print_array()` to print all members and their scores following the
 * specified reverse rank range
 * - Call `print_array()` if `lb` is invalid, range is invalid or no members
 * found
 */
void zrevrange(Leaderboard *lb, int start, int end) {
  if (!lb || !lb->sl) {
    print_array(NULL, 0, 0);
    return;
  }

  int length = sl_get_length(lb->sl);

  start = conv_rank(start, length);
  end = conv_rank(end, length);

  if (start > end || start < 1 || start > length || end < 1 || end > length) {
    print_array(NULL, 0, 0);
    return;
  }

  start = length - start + 1;
  end = length - end + 1;

  int count;

  SkipListNode **nodes = sl_get_range_by_rank(lb->sl, end, start, &count);

  print_array(nodes, count, 1);

  free(nodes);
}

/**
 * IMPLEMENT ME
 *
 * Print all members with scores within [min_score, max_score].
 *
 * @param lb        the leaderboard to query
 * @param min_score minimum score (inclusive)
 * @param max_score maximum score (inclusive)
 *
 * Output:
 * - Call `print_array()` to print all members and their scores in the specified
 * score range
 * - Call `print_array()` if `lb` is NULL, invalid score range or no members
 * found
 */
void zrangebyscore(Leaderboard *lb, int min_score, int max_score) {
  if (!lb) {
    print_array(NULL, 0, 0);
    return;
  }

  int count;

  SkipListNode **nodes =
      sl_get_range_by_score(lb->sl, min_score, max_score, &count);

  print_array(nodes, count, 0);

  free(nodes);
}

/* Set `reverse` to print nodes in reverse order (from last to first) */
static void print_array(SkipListNode **nodes, int node_count, int reverse) {
  if (node_count <= 0) {
    printf("(empty array)\n");
    return;
  }

  assert(nodes != NULL); /* shouldn't be triggered */
  printf("(array) %d item(s)\n", node_count);
  for (int i = 0; i < node_count; i++) {
    int j = reverse ? node_count - 1 - i : i;
    const SkipListNode *node = nodes[j];
    printf("  #%d: %s (%d)\n", i + 1, node->member, node->score);
  }
}
