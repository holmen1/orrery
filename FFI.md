# FFI — Manual Compilation Without Cabal

This is a detour: building the project by hand using only `gcc` and `ghc`.
Useful for understanding exactly what Cabal does under the hood.

## The Three Steps

### 1. Compile the C sources with gcc

```bash
gcc -std=c99 -Wall -O2 -fPIC -Icsrc -c csrc/ephemeris.c -o build/ephemeris.o
gcc -std=c99 -Wall -O2 -fPIC -Icsrc -c csrc/raytrace.c  -o build/raytrace.o
```

- `-std=c99` — C dialect matching our `cc-options`
- `-fPIC` — position-independent code, required when GHC links C objects into a
  Haskell binary
- `-Icsrc` — find `ephemeris.h` from `#include "ephemeris.h"`

### 2. Compile the Haskell sources with ghc (no link yet)

```bash
ghc -Wall -O2 -no-link \
    -isrc \
    -Icsrc \
    -outputdir build \
    src/Orrery/Types.hs   \
    src/Orrery/Time.hs    \
    src/Orrery/FFI.hs     \
    src/Orrery/Coords.hs  \
    src/Orrery/Display.hs \
    src/Main.hs
```

- `-isrc` — Haskell source root (so `Orrery.Types` resolves to `src/Orrery/Types.hs`)
- `-Icsrc` — C header search path (needed by `hsc2hs` / `CApiFFI` if ever used)
- `-outputdir build` — put all `.o` and `.hi` files in `build/`
- `-no-link` — compile only, no executable yet

GHC resolves the module dependency order automatically from the imports, so you
can list the files in any order.

### 3. Link everything together with ghc

```bash
ghc -Wall -O2 \
    -outputdir build \
    -isrc \
    build/Main.o          \
    build/Orrery/Types.o  \
    build/Orrery/Time.o   \
    build/Orrery/FFI.o    \
    build/Orrery/Coords.o \
    build/Orrery/Display.o \
    build/ephemeris.o     \
    build/raytrace.o      \
    -lm                   \
    -o orrery
```

- GHC drives the linker (`ld`) itself — you do **not** call `gcc` to link
- `build/ephemeris.o` and `build/raytrace.o` are the C objects from Step 1
- `-lm` — link the C math library (`sin`, `cos`, `fmod` etc.)
- GHC automatically links its own runtime system (RTS) and `base`

### Run it

```bash
./orrery
```

## Why GHC Links, Not GCC

When Haskell and C are mixed, GHC must drive the final link because it needs to
inject the Haskell **runtime system** (garbage collector, green-thread scheduler,
profiling hooks). If you linked with `gcc` directly you would get unresolved
symbols like `__stginit_Main`, `stg_gc_enter1`, etc.

## Why `-fPIC` on the C files

GHC's linker on Linux expects all foreign objects to be position-independent so
they can be loaded into any address (required for `-dynamic` builds and for GHCi
to load objects at runtime). Without `-fPIC`, static builds may still work but
GHCi will refuse to load them.

## One-liner (all steps combined)

```bash
mkdir -p build && \
gcc -std=c99 -Wall -O2 -fPIC -Icsrc -c csrc/ephemeris.c -o build/ephemeris.o && \
gcc -std=c99 -Wall -O2 -fPIC -Icsrc -c csrc/raytrace.c  -o build/raytrace.o  && \
ghc -Wall -O2 -isrc -Icsrc -outputdir build -no-link \
    src/Orrery/Types.hs src/Orrery/Time.hs src/Orrery/FFI.hs \
    src/Orrery/Coords.hs src/Orrery/Display.hs src/Main.hs   && \
ghc -Wall -O2 -outputdir build \
    build/Main.o build/Orrery/Types.o build/Orrery/Time.o \
    build/Orrery/FFI.o build/Orrery/Coords.o build/Orrery/Display.o \
    build/ephemeris.o build/raytrace.o -lm -o build/orrery
```

## Comparison With What Cabal Does

| Step | Manual | Cabal |
|------|--------|-------|
| Compile C | `gcc -fPIC -c ...` | same, via `-pgmc /usr/bin/gcc` |
| Compile Haskell | `ghc --make -no-link` | same |
| Link | `ghc ... ephemeris.o raytrace.o -lm -o orrery` | same |
| Dependency order | you figure it out | Cabal reads imports |
| Package DB | not needed (only `base`) | `--package-db=...` flags |
| Install paths | `./orrery` | `dist-newstyle/.../orrery` |

For a project with only `base` as a dependency, the manual and Cabal builds are
nearly identical. The value of Cabal grows with the number of external packages.


