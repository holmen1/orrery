# Minimal Haskell FFI Example

Calls a C function from Haskell using `capi` FFI — no Cabal, just `gcc` and `ghc`.

## Files

| File | Description |
|------|-------------|
| `adder.h` | C header declaring `int add(int, int)` |
| `adder.c` | C implementation returning `x + y` |
| `main.hs` | Haskell entry point importing and calling `add` |

## Build & Run

```sh
gcc -c adder.c -o adder.o
ghc main.hs adder.o -o main
./main
# 4
```

## How it works

The `capi` FFI import references the header and function name:

```haskell
foreign import capi "adder.h add"
  c_add :: Int -> Int -> Int
```

GHC uses the header to verify the C signature at compile time. The compiled
object file `adder.o` is passed directly to `ghc` for linking — no build system
required.
