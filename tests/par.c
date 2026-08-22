/* tests/par.c — map an opaque op over a list. The op is supplied here; the schedule is generic.
   SPDX-License-Identifier: MIT OR Apache-2.0 */
#include <stdio.h>
#include "cse/par.h"
#include "cse/dsa/cell.h"
#include "slate/psda.h"
#include "slate/encode.h"

static int fails = 0;
#define CHECK(c, m) do { if (!(c)) { printf("  FAIL %s\n", (m)); fails++; } } while (0)

static slate_psda arena[64]; static slate_psda *pool;
static void pool_init(void) { int i; for (i = 0; i < 63; i++) arena[i].next = &arena[i+1]; arena[63].next = 0; pool = &arena[0]; }
static unsigned char vb[64][4]; static slate_psda vn[64]; static int vk = 0;
static slate_psda *val(unsigned x) { unsigned char c = (unsigned char)x; slate_encode(&c, 1, vb[vk]); vn[vk].potential = vb[vk]; vn[vk].prev = 0; vn[vk].next = 0; return &vn[vk++]; }
static unsigned num(slate_psda *p) { unsigned char c[4]; slate_decode(p->potential, c, 4); return c[0]; }

/* the op supplied to the schedule: double the item */
static slate_psda *dbl(slate_psda **p, slate_psda *x) { (void)p; return val(num(x) * 2); }

int main(void) {
  pool_init();
  slate_psda *a = val(1), *b = val(2), *c = val(3);
  slate_step(a, b); slate_step(b, c);                 /* the list 1 → 2 → 3 */

  slate_psda *out = cse_par_map(dbl, a, &pool);        /* → 2 → 4 → 6 (results are cell payloads) */
  slate_psda *o0 = out, *o1 = out ? out->next : 0, *o2 = o1 ? o1->next : 0;
  CHECK(o0 && num(cse_cell_payload(o0)) == 2, "first = 2");
  CHECK(o1 && num(cse_cell_payload(o1)) == 4, "second = 4");
  CHECK(o2 && num(cse_cell_payload(o2)) == 6, "third = 6");

  printf(fails ? "par: FAIL\n" : "par: ok\n");
  return fails ? 1 : 0;
}
