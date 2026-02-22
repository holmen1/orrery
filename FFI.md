# FFI — Foreign Function Interface

## What is FFI?

FFI lets Haskell call functions written in C (and vice versa).
The two languages compile separately, then the linker stitches them into
one binary.  At runtime there is no interpreter, no socket, no subprocess —
just a direct function call across a language boundary.

```
  Haskell world                              │  C world
  ─────────────────────────────────────────  │  ──────────────────────────────────────
  sunPosition :: JulianDay -> EclipticCoord  │
                                             │  foreign import ccall "sun_position"
                                             │    c_sun_position :: Double -> Ptr Double -> IO ()
                                             │
                                             ▼
                          [ linker joins the two .o files ]
                                             │
                                             ▼
                          one binary — direct CPU call, zero overhead
```

---

## A Concrete Example

### The C side

In `ephemeris.c` we define:

```c
void sun_position(double jd, double *out) {
    /* ... Meeus Ch. 25 maths ... */
    out[0] = longitude;   /* degrees */
    out[1] = latitude;    /* degrees */
    out[2] = distance;    /* km      */
}
```

- Takes a Julian Day (`double`) and a pointer to a 3-element array.
- Writes results into that array — no return value, no struct.
- The caller owns the memory.

### The Haskell side

```haskell
-- Tell GHC that a C function called "sun_position" exists with this type.
foreign import ccall "sun_position"
  c_sun_position :: Double -> Ptr Double -> IO ()
```

This single line is the **entire FFI declaration**.  
GHC trusts you that the C signature matches. The linker will catch it if it doesn't.

### The wrapper

```haskell
sunPosition :: JulianDay -> EclipticCoord
sunPosition (JulianDay jd) = unsafePerformIO $
  allocaArray 3 $ \ptr -> do   -- allocate double[3] on the stack
    c_sun_position jd ptr      -- C fills it
    [lon, lat, dist] <- peekArray 3 ptr  -- Haskell reads it back
    return $ EclipticCoord (Degrees lon) (Degrees lat) dist
```

Step by step:

| Line | What happens |
|------|-------------|
| `allocaArray 3` | Allocates 3 × 8 = 24 bytes on the **stack**. No malloc, no free — Haskell handles lifetime. Stack allocation is a single pointer decrement — effectively free. Heap allocation (`mallocArray`) requires the OS allocator, bookkeeping, and a later `free`. For short-lived scratch buffers like this, stack is always the right choice. |
| `c_sun_position jd ptr` | Crosses the FFI boundary. C runs, writes 3 doubles. |
| `peekArray 3 ptr` | Reads the 3 doubles back into a Haskell list. |
| `EclipticCoord ...` | Wraps them in type-safe Haskell values. |
| `unsafePerformIO` | Presents a pure interface — safe because C is a pure computation here (same input → same output, no side effects). |

---

## The Memory Picture

```
  Haskell stack
  ┌─────────────────────┐
  │  ptr → [ 0.0, 0.0, 0.0 ]  ← before call
  └─────────────────────┘
            │
            │  c_sun_position(jd, ptr)
            ▼
  ┌─────────────────────┐
  │  ptr → [ 333.28, 0.0, 147965004.0 ]  ← after call
  └─────────────────────┘
            │
            │  peekArray 3 ptr
            ▼
  [333.28, 0.0, 147965004.0]  — back in Haskell
```

The pointer crosses the boundary, not the data.  
C writes into Haskell's memory.  Haskell reads it back.

---

## Why Not Return a Struct?

We could define a C struct and return it by value:

```c
typedef struct { double lon, lat, dist; } EclipticCoord;
EclipticCoord sun_position(double jd);
```

But GHC's FFI does not support structs returned by value.
The `double *out` pattern is the standard workaround:
**C writes into Haskell-allocated memory.**

If you have a complex struct used in many places, `hsc2hs` can generate
the `Storable` instance for you — but for 3 doubles, the pointer pattern
is simpler and equally efficient.

---

## Key Types from `Foreign.*`

| Type / Function | What it does |
|-----------------|--------------|
| `Ptr a` | A raw C pointer to values of type `a` |
| `allocaArray n` | Allocate `n` elements on the stack (freed on scope exit) |
| `peekArray n ptr` | Read `n` values from a pointer into a Haskell list |
| `pokeArray ptr xs` | Write a Haskell list into memory at a pointer |
| `foreign import ccall` | Declare a C function visible to Haskell |
| `IO ()` | The FFI call lives in IO — C may have side effects |

---

## Manual Compilation Without Cabal

This is how to build the project by hand using only `gcc` and `ghc`.
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


