/* src/par.c — the parallel schedule: map an opaque op over independent items. Slate-only; a walk over the
   spine, collecting into CSE-DSA cells. The items are independent, so a real backend may run them at once;
   the result is the same either way. SPDX-License-Identifier: MIT OR Apache-2.0 */
#include "cse/par.h"
#include "cse/dsa/cell.h"

slate_psda *cse_par_map(cse_op op, slate_psda *head, slate_psda **pool) {
  slate_psda *out = 0, *p = head, *cell;
  while (p) {
    slate_psda *r = op(pool, p);
    cell = cse_cell_take(pool);
    if (cell) out = cse_cell(cell, r, out, 0);
    p = p->next;
  }
  return cse_cell_reverse(out);
}
