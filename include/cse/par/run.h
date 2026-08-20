/// cse/par/run.h — decompose a whole into independent parts, compute each, recompose.
/// SPDX-License-Identifier: MIT OR Apache-2.0
///
/// Compose and decompose are already the spine (the D in PSDA, slate_compose). The one new thing here
/// is the schedule: because the parts do not interfere, they run independently. `cse_par_run` owns that
/// schedule; how the parts are cut, computed, and joined is a channel scheme — three holes. The
/// reference scheme is residues (CSE-RNS: no carry, so the parts are independent); lanes or device
/// tiles fill the same holes differently. Standalone: parallelize any structure over CSE-IO + DSA.
#pragma once
#include "slate/psda.h"

/// Cut `whole` into a trajectory of independent parts — a channel scheme, a hole. Weak default is the
/// whole itself (one part: no parallelism). RNS cuts it into residues mod pᵢ.
slate_psda *cse_par_decompose(slate_psda *whole);

/// Compute one `part` — a channel scheme, a hole. Weak default is identity. The parts do not interfere,
/// so a strong impl may run these concurrently (lanes, threads, a device).
slate_psda *cse_par_step(slate_psda *part);

/// Join computed `parts` back into a whole — a channel scheme, a hole. Weak default is identity. RNS
/// recomposes by CRT.
slate_psda *cse_par_recompose(slate_psda *parts);

/// Map cse_par_step over the independent `parts`, collecting the results in order (cells from `pool`).
/// The schedule seam, a hole: the weak default is the serial walk, one part at a time. Because the
/// parts do not interfere, a concurrent floor (CSE-Accel) overrides this to run them at once — the
/// result is identical, only the waiting is gone.
slate_psda *cse_par_map(slate_psda *parts, slate_psda **pool);

/// The whole pass: decompose `whole` into independent parts, map the step over them, recompose. Returns
/// the recomposed whole; the original is untouched.
slate_psda *cse_par_run(slate_psda *whole, slate_psda **pool);
