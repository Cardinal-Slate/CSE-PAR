# CSE-PAR

Parallelize anything: decompose a structure into independent parts, run the parts, recompose. Opaque,
slate-only — **usable on its own**, not an engine part.

## Role

**Depends on [CSE-IO](https://github.com/Cardinal-Slate/CSE-IO)** (which brings CSE-DSA + the spine).

Compose and decompose are already the spine — the `D` in PSDA, `slate_compose`. The one new thing PAR
adds is the **schedule**: because the parts do not interfere, they run independently.

- `cse_par_run` owns that schedule — decompose, step each part, recompose. It is the only non-hole.
- the channel scheme is three **holes**, weak defaults that degrade to no parallelism:
  - `cse_par_decompose` — how to cut the whole into independent parts;
  - `cse_par_step` — how to compute one part (independent, so a strong impl may run them concurrently);
  - `cse_par_recompose` — how to join the parts back.

**RNS is one implementation** of that scheme: residues mod pᵢ (no carry, so the parts are independent),
joined by CRT. Lanes (SIMD) or device tiles (GPU) fill the same three holes differently. This is the
engine's floor seam (`cse_floor`), with a scheme linked in as the floor — but it stands alone.

Only slate types cross.

## Build

```
make            gate + standalone headers + test, then libcse-par.a
make check      the gate and the test only
```

Build the deps first: `make -C ../CardinalSlate lib && make -C ../CSE-DSA lib && make -C ../CSE-IO lib`.

## License

MIT OR Apache-2.0.
