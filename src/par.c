/* src/par.c — the schedule: decompose, step each independent part, recompose.
   SPDX-License-Identifier: MIT OR Apache-2.0

   Walking the parts is a CSE-IO walk; collecting them is a DSA cons. The three channel operations —
   how to cut, compute, and join — are holes. The weak defaults degrade to no parallelism (one part,
   computed as-is, joined as-is), so the schedule runs correctly before any scheme is linked. */
#include "cse/par/run.h"
#include "cse/io.h"
#include "cse/dsa/cell.h"

__attribute__((weak)) slate_psda *cse_par_decompose(slate_psda *whole) { return whole; }
__attribute__((weak)) slate_psda *cse_par_step(slate_psda *part)       { return part; }
__attribute__((weak)) slate_psda *cse_par_recompose(slate_psda *parts) { return parts; }

slate_psda *cse_par_run(slate_psda *whole, slate_psda **pool) {
  slate_psda *out = 0, *p = cse_par_decompose(whole), *done, *cell;   /* cut into independent parts */
  while (p) {
    done = cse_par_step(p);              /* compute one part (independent → parallelizable) */
    cell = cse_cell_take(pool);
    if (cell) out = cse_cell(cell, done, out, 0);   /* collect the computed part */
    p = cse_io_next(p);
  }
  return cse_par_recompose(cse_cell_reverse(out));  /* restore order, then join the parts */
}
