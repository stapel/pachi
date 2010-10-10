#ifndef ZZGO_PATTERN3_H
#define ZZGO_PATTERN3_H

/* Fast matching of simple 3x3 patterns. */

#include "board.h"

/* (Note that this is completely independent from the general pattern
 * matching infrastructure in pattern.[ch]. This is fast and simple.) */

struct board;
struct move;

struct pattern3s {
	/* Hashtable: 2*8 bits (ignore middle point, 2 bits per intersection) */
	/* Value: 0: no pattern, 1: black pattern,
	 * 2: white pattern, 3: both patterns */
	char hash[65536];
};

/* XXX: See <board.h> for hash3_t typedef. */

/* Source pattern encoding:
 * X: black;  O: white;  .: empty;  #: edge
 * x: !black; o: !white; ?: any
 *
 * extra X: pattern valid only for one side;
 * middle point ignored. */

void pattern3s_init(struct pattern3s *p, char src[][11], int src_n);

/* Compute pattern3 hash at local position. */
static hash3_t pattern3_hash(struct board *b, coord_t c);

/* Check if we match any 3x3 pattern centered on given move. */
static bool pattern3_move_here(struct pattern3s *p, struct board *b, struct move *m);

/* Generate all transpositions of given pattern, stored in an
 * hash3_t[8] array. */
void pattern3_transpose(hash3_t pat, hash3_t (*transp)[8]);

/* Reverse pattern to opposite color assignment. */
static hash3_t pattern3_reverse(hash3_t pat);


static inline hash3_t
pattern3_hash(struct board *b, coord_t c)
{
	hash3_t pat = 0;
	int x = coord_x(c, b), y = coord_y(c, b);
	pat |= (board_atxy(b, x - 1, y - 1) << 14)
		| (board_atxy(b, x, y - 1) << 12)
		| (board_atxy(b, x + 1, y - 1) << 10);
	pat |= (board_atxy(b, x - 1, y) << 8)
		| (board_atxy(b, x + 1, y) << 6);
	pat |= (board_atxy(b, x - 1, y + 1) << 4)
		| (board_atxy(b, x, y + 1) << 2)
		| (board_atxy(b, x + 1, y + 1));
	return pat;
}

static inline bool
pattern3_move_here(struct pattern3s *p, struct board *b, struct move *m)
{
#ifdef BOARD_PAT3
	hash3_t pat = b->pat3[m->coord];
#else
	hash3_t pat = pattern3_hash(b, m->coord);
#endif
	return (p->hash[pat] & m->color);
}

static inline hash3_t
pattern3_reverse(hash3_t pat)
{
	/* Reverse color assignment - achieved by swapping odd and even bits */
	return ((pat >> 1) & 0x5555) | ((pat & 0x5555) << 1);
}

#endif
