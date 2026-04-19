# Mini Rust Compiler - LEX and BISON ONLY

## 🎓 Course Project - Compiler Design

This is a **standalone compiler** that implements ALL functionality in just **2 source files**:
- `lexer.l` (Flex lexer)
- `parser_standalone.y` (Bison parser with embedded helpers)

## ✅ Professor's Requirement: "Lex and Bison Only"

This project meets the strict requirement because:

1. **All compiler logic is in `.l` and `.y` files**
   - Lexical analysis → `lexer.l`
   - Syntax analysis → `parser_standalone.y`
   - Semantic analysis → `parser_standalone.y` (semantic actions)
   - Parse tree construction → `parser_standalone.y` (embedded functions)
   - TAC generation → `parser_standalone.y` (embedded functions)
   - Optimization → `parser_standalone.y` (constant folding)

2. **No external helper files**
   - Symbol table code is embedded in `parser_standalone.y`
   - TAC generator code is embedded in `parser_standalone.y`
   - Parse tree code is embedded in `parser_standalone.y`

3. **main.c is just a driver**
   - Only calls `yyparse()` and prints results
   - Contains NO compiler logic

## 📁 Project Structure

```
compiler_design/
├── lexer.l                      ← Lexer (tokenization)
├── parser_standalone.y          ← Parser + ALL helper code embedded
├── main_standalone.c            ← Minimal driver (just calls yyparse)
├── build_standalone.bat         ← Build script
├── compiler_standalone.exe      ← Compiled executable
├── test.rs                      ← Test input file
├── test_errors.rs               ← Error test cases
└── README_STANDALONE.md         ← Detailed documentation
```

## 🚀 Quick Start

### Build:
```bash
build_standalone.bat
```

### Run:
```bash
# Compile a Rust file
compiler_standalone.exe test.rs

# Or use stdin
compiler_standalone.exe < input.rs
```

## 📊 Compiler Phases

The compiler implements all 6 phases:

1. **Lexical Analysis** (DFA-based tokenization)
2. **Syntax Analysis** (LALR(1) parsing)
3. **Parse Tree Construction** (during parsing)
4. **Semantic Analysis** (symbol table, type checking)
5. **Intermediate Code Generation** (TAC)
6. **Code Optimization** (constant folding)

## 📝 Output

The compiler produces:

1. **Token Stream** - All recognized tokens
2. **Parse Result** - Success/error messages
3. **Parse Tree** - Hierarchical syntax tree
4. **Symbol Table** - All variables with types and values
5. **TAC** - Three-address intermediate code
6. **Optimized TAC** - After constant folding

## 🔧 Supported Language Features

Mini Rust subset:
- Variable declarations: `let x = 10;` `let mut y: i32 = 5;`
- Assignments: `x = x + 1;`
- Arithmetic: `+`, `-`, `*`, `/`
- Comparisons: `<`, `>`, `==`, `!=`, `<=`, `>=`
- Control flow: `if`/`else`, `while`
- Functions: `fn main() { ... }`
- Types: `i32`, `f64`, `bool`
- Output: `println!("text");`

## 📖 Example

Input (`test.rs`):
```rust
fn main() {
    let x: i32 = 10;
    let y: i32 = 20;
    let z = x + y * 2;
    
    if z > 40 {
        println!("Large value");
    }
}
```

Output includes:
- Tokens: `[KW: fn] [ID: main] [DL: (] ...`
- Parse tree showing program structure
- Symbol table with x=10, y=20, z=50
- TAC: `t0 = 20 * 2`, `t1 = 10 + t0`, etc.
- Optimized TAC with constant folding

## 🎯 Key Implementation Details

### In `parser_standalone.y`:

**Symbol Table** (lines 20-120):
- `sym_init()` - Initialize
- `sym_lookup()` - Find variables
- `sym_insert()` - Add variables
- `sym_print()` - Display table

**TAC Generator** (lines 122-220):
- `tac_init()` - Initialize
- `tac_new_temp()` - Generate t0, t1, ...
- `tac_emit()` - Emit instructions
- `tac_print_optimised()` - Constant folding

**Parse Tree** (lines 222-300):
- `createNode()` - Create nodes
- `printTree()` - Display tree
- `freeTree()` - Cleanup

**Grammar Rules** (lines 350+):
- All Rust grammar rules
- Semantic actions for each rule
- Type checking
- Code generation

## 🏆 Why This Approach Works

Bison allows embedding C code in three places:
1. **Prologue** (`%{ ... %}`) - Type definitions, helper functions
2. **Actions** (`{ ... }`) - Code executed when rules match
3. **Epilogue** (after `%%`) - Additional functions

We use the prologue to embed all helper code, making it part of the `.y` file itself!

## 📚 Compiler Theory Covered

- **Lexical Analysis**: Regular expressions → DFA
- **Syntax Analysis**: CFG → LALR(1) parser
- **Syntax-Directed Translation**: Grammar actions
- **Semantic Analysis**: Symbol tables, type checking
- **Intermediate Representation**: Three-address code
- **Optimization**: Constant folding

## 🔍 For Your Professor

To verify compliance:
1. Open `parser_standalone.y` - see all helper code embedded
2. Open `main_standalone.c` - see it only calls yyparse()
3. Run `build_standalone.bat` - compiles with just .l and .y
4. Run `compiler_standalone.exe test.rs` - see full output

All compiler logic is in the `.l` and `.y` files as required!

## 📄 License

Course project for Compiler Design class.

---

**Built with:** Flex (lexer generator) + Bison (parser generator) + GCC (C compiler)
