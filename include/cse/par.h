/// cse/par.h — parallelization: apply an opaque operation to each item of a list independently, then
/// collect the results. The schedule is generic — it knows nothing about the op or the items, only that
/// they are psda and independent (so the order of execution does not matter). Spine + CSE-DSA only.
/// SPDX-License-Identifier: MIT OR Apache-2.0
#pragma once
#include "slate/psda.h"

/// an operation on one item.
typedef slate_psda *(*cse_op)(slate_psda **pool, slate_psda *item);

/// apply `op` to every item of `head`, collecting the results into a new list drawn from `pool`.
slate_psda *cse_par_map(cse_op op, slate_psda *head, slate_psda **pool);
