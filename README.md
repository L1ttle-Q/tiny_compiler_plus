# tiny_compiler_plus

[中文说明](README_zh.md)

An educational C++ extension of the TINY compiler and Tiny Machine from Kenneth C. Louden's *Compiler Construction: Principles and Practice*. The compiler turns `.tny` source into `.tm` instructions; a separate interactive virtual machine executes those instructions.

## Features and scope

- Integer and floating-point expressions, scientific-notation literals, XOR, and `>=` / `<=` comparisons.
- Declarations, initial values, duplicate-definition and undefined-variable diagnostics.
- Integer expressions in conditions, `if` / `else`, `repeat` / `until`, `read`, and `write`.
- Expression DAG merging, temporary variables, and constant folding.
- DOT syntax-tree output for optional Graphviz visualization.

Type tracking remains incomplete: declaration checks can reject some constant type mismatches, but variable types are not fully preserved in the symbol table or machine memory. These features do not constitute a complete strongly typed language implementation.

## Requirements and build

Use Git, CMake 3.16+, and a C++11 compiler. Automated tests additionally need Python 3.8+ (standard library only). GCC/MinGW may emit warnings about legacy string-literal conversions; those have not all been modernized.

Windows PowerShell with MinGW-w64 (`g++` and `mingw32-make` on PATH):

```powershell
git clone https://github.com/L1ttle-Q/tiny_compiler_plus.git
cd tiny_compiler_plus
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Linux/macOS: omit `-G "MinGW Makefiles"` from the configure command. Set `-DBUILD_TESTING=OFF` if Python is unavailable and only the executables are needed. A separate Visual Studio build directory produces executables under its configuration subdirectory, such as `build-vs/Release/`.

The existing `TC/Makefile` remains available: run `mingw32-make` in `TC/` on Windows, or `make` there on Unix. To build the machine directly from the repository root, run `g++ -std=c++11 TM/tm.cpp -o TM/tm.exe` on Windows (use `-o TM/tm` on Unix). The file is **`tm.cpp`**, not `tm.c`.

## Compile and run the supplied minimal example

`examples/hello.tny` contains:

```text
int answer := 6 * 7;
write answer
```

From the repository root on Windows:

```powershell
cd examples
..\build\tiny.exe hello.tny
..\build\tm.exe hello.tm
```

On Linux/macOS, the last two commands are `../build/tiny hello.tny` and `../build/tm hello.tm`. Compile from the example directory using a short basename: the legacy filename handling uses the first `.` when choosing the generated `.tm` path.

At the TM `Enter command:` prompt, enter `g` and press Enter. Expected output includes `OUT instruction prints: 42` and `HALT:`. Enter `q` to leave the simulator. Afterward, run `cd ..` to return to the repository root before using the test commands below.

Starting `tm` only opens the simulator; **`g` is needed to execute the program**. Compilation produces `.tm` beside the source and may also overwrite `syntax_tree.dot` in the working directory. Output is generated code for this VM, not a native executable.

| TM command | Action |
| --- | --- |
| `g` | Run until halt or a machine error |
| `s 1` | Execute one instruction |
| `r` | Inspect registers |
| `i` / `d` | Inspect instruction / data memory; `h` describes optional ranges |
| `t` / `p` | Toggle tracing / instruction-count output |
| `c` | Clear machine state |
| `h` / `q` | Help / quit |

For a source program containing `read`, supply its numeric input when the VM's `IN` instruction requests it.

## Tests

From the repository root:

```powershell
ctest --test-dir build --output-on-failure
python tests/run_tests.py --compiler build/tiny.exe --machine build/tm.exe
```

On Unix, omit `.exe`; with Visual Studio, add `-C Release` to CTest and use the corresponding `Release/` executable paths for the Python runner.

Eight CTest cases cover integer XOR, floating-point arithmetic, conditions, undefined variables, a missing initializer, the existing `triangle.tny`, and expected rejection of the two other legacy examples. Tests compile in disposable directories, execute the newly generated instructions, check numeric output and halt, and enforce subprocess timeouts. Negative cases require a nonzero compiler exit, a failure diagnostic, and no generated machine program.

`TC/sample.tny` currently misses a semicolon between two `write` statements; `TC/calc.tny` uses variables without declarations. They are preserved and tested as **expected compile failures**, not successful usage examples. `TC/triangle.tny` currently has its output statements commented out, so a successful run halts without printing an `OUT` value. Existing `TM/*.tm` files are historical artifacts and need not match current source.

## Optional syntax-tree images

Install Graphviz separately and make `dot` available, then run this command in the directory containing the generated DOT file:

```sh
dot -Tjpg syntax_tree.dot -o syntax_tree.jpg
```

Graphviz is not required for building, compiling, VM execution, or tests.

Existing optimization illustrations:

![Expression DAG](TC/syntax_tree.jpg)
![Temporary variables](TC/syntax_tree_1.jpg)
![Constant folding](TC/syntax_tree_2.jpg)

## Source layout, limitations, and validation

- `TC/`: scanning, parsing, symbols/types, optimization, code generation, and legacy examples.
- `TM/tm.cpp`: VM instruction loader and interactive execution.
- `examples/hello.tny`: minimal working example; `tests/run_tests.py`: regression runner.
- The VM has 1024 instruction slots, 1024 data slots, eight registers, and fixed-size input buffers. This is a teaching implementation; malformed or large input is not comprehensively validated.
- This update makes compiler errors return a failure status, handles a missing assignment expression without dereferencing null, and replaces unbounded VM `gets` calls with bounded input that stops on EOF. It does not make every parser error or arithmetic edge case safe.
- Verified on 2026-10-26 with Windows, MinGW-w64 GCC 8.1, CMake 3.31, and Python 3.14: build and all 8 CTest cases passed. Linux/macOS and Visual Studio execution were not verified in this run.

Licensed under [Apache License 2.0](LICENSE). Original source files retain their TINY/TM attribution comments.
