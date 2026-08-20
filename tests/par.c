/* tests/par.c — parallelize a three-part whole: each part is stepped once, in order, then recomposed.
   SPDX-License-Identifier: MIT OR Apache-2.0 */
#include <stdio.h>
#include "cse/par.h"
#include "cse/dsa/cell.h"
#include "slate/psda.h"

static int fails = 0;
#define CHECK(c, m) do { if (!(c)) { printf("  FAIL %s\n", (m)); fails++; } } while (0)

/* a strong scheme: the whole is already a trajectory of parts; step records each in order; recompose
   records that it was handed a non-empty joined list. */
static slate_psda *stepped[8];
static int n_step = 0;
static slate_psda *joined = 0;

slate_psda *cse_par_step(slate_psda *part)       { stepped[n_step++] = part; return part; }
slate_psda *cse_par_recompose(slate_psda *parts) { joined = parts; return parts; }

int main(void) {
  slate_psda a = {0,0,0}, b = {0,0,0}, c = {0,0,0};   /* the three parts */
  slate_step(&a, &b);
  slate_step(&b, &c);

  slate_psda spare[4] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0}};   /* a pool of blank cells */
  slate_step(&spare[0], &spare[1]);
  slate_step(&spare[1], &spare[2]);
  slate_step(&spare[2], &spare[3]);
  slate_psda *pool = &spare[0];

  slate_psda *whole = cse_par_run(&a, &pool);

  CHECK(n_step == 3, "every part was stepped");
  CHECK(stepped[0] == &a && stepped[1] == &b && stepped[2] == &c, "parts stepped in order");
  CHECK(whole != 0 && joined == whole, "the stepped parts were recomposed");
  CHECK(cse_cell_payload(whole) == &a, "recomposed order restored (first part first)");

  printf(fails ? "par: FAIL\n" : "par: ok\n");
  return fails ? 1 : 0;
}
