# Standalone Compiler - LEX and BISON ONLY

## Overview

This is a **standalone version** of the Mini Rust Compiler that contains **ALL compiler logic in just 2 files**:

1. `lexer.l` - Flex lexer specification
2. `parser_standalone.y` - Bison parser with ALL helper code embedded

**No external .c or .h files are needed!** (except the minimal main.c driver)

## What's Included in parser_standalone.y

The `parser_standalone.y` file contains everything:

### 1. Symbol Table Implementation
- `sym_init()` - Initialize symbol table
- `sym_lookup()` - Find variables
- `sym_insert()` - Add new variables
- `sym_set_int()` / `sym_set_float()` - Store constant values
- `sym_print()` - Display symbol table

### 2. Intermediate Code (TAC) Implementation
- `tac_init()` - Initialize TAC generator
- `tac_new_temp()` - Generate temporary variables (t0, t1, ...)
- `tac_emit()` - Emit three-address code instructions
- `tac_print()` - Display TAC
- `tac_print_optimised()` - Display optimized TAC with constant folding

### 3. Parse Tree Implementation
- `createNode()` - Create internal tree nodes
- `createLeaf()` - Create leaf nodes
- `printTree()` - Display tree structure
- `saveTree()` - Save tree to file
- `freeTree()` - Clean up memory

### 4. All Grammar Rules
- Complete Rust subset grammar
- Semantic actions for each rule
- Parse tree construction
- TAC generation
- Type checking

## Files Structure

```
STANDALONE VERSION (Professor-approved):
├── lexer.l                    ← Lexer (tokenization)
├── parser_standalone.y        ← Parser + ALL helper code
├── main_standalone.c          ← Minimal driver (just calls yyparse)
└── build_standalone.bat       ← Build script

ORIGINAL VERSION (with separate helpers):
├── lexer.l
├── parser.y
├── symbol_table.c/.h          ← Moved into parser_standalone.y
├── intermediate_code.c/.h     ← Moved into parser_standalone.y
├── parse_tree.c/.h            ← Moved into parser_standalone.y
├── main.c
└── build.bat
```

## How to Build

### Windows:
```bash
build_standalone.bat
```

### Manual Build:
```bash
# Step 1: Generate lexer
win_flex lexer.l

# Step 2: Generate parser
win_bison -d parser_standalone.y

# Step 3: Compile
gcc -o compiler_standalone.exe lex.yy.c parser_standalone.tab.c main_standalone.c -lm
```

## How to Run

```bash
# Compile a Rust file
compiler_standalone.exe test.rs

# Or use stdin
compiler_standalone.exe < input.rs
```

## Output

The compiler produces:

1. **Token Stream** - All tokens recognized by the lexer
2. **Parse Result** - Success or error messages
3. **Parse Tree** - Hierarchical syntax tree
4. **Symbol Table** - All declared variables with types
5. **TAC (Intermediate Code)** - Three-address code
6. **Optimized TAC** - After constant folding

## Why This Meets Professor's Requirements

✅ **All compiler logic in .l and .y files**
- Lexical analysis: `lexer.l`
- Syntax analysis: `parser_standalone.y`
- Semantic analysis: `parser_standalone.y` (in semantic actions)
- Parse tree construction: `parser_standalone.y` (embedded functions)
- TAC generation: `parser_standalone.y` (embedded functions)
- Optimization: `parser_standalone.y` (constant folding)

✅ **No external helper files**
- Symbol table code is in `parser_standalone.y`
- TAC code is in `parser_standalone.y`
- Parse tree code is in `parser_standalone.y`

✅ **main.c is just a driver**
- Only calls `yyparse()` and prints results
- Contains NO compiler logic
- Just orchestrates the phases

## Compiler Design Theory Covered

1. **Lexical Analysis** (DFA-based tokenization in Flex)
2. **Syntax Analysis** (LALR(1) parsing in Bison)
3. **Syntax-Directed Translation** (SDT in grammar actions)
4. **Semantic Analysis** (symbol table, type checking)
5. **Intermediate Code Generation** (TAC)
6. **Code Optimization** (constant folding)

## Example Test File

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

## Comparison

| Feature | Original | Standalone |
|---------|----------|------------|
| Lexer | lexer.l | lexer.l (same) |
| Parser | parser.y | parser_standalone.y |
| Symbol Table | symbol_table.c/.h | Embedded in .y |
| TAC Generator | intermediate_code.c/.h | Embedded in .y |
| Parse Tree | parse_tree.c/.h | Embedded in .y |
| Main Driver | main.c | main_standalone.c |
| **Total Files** | 10+ files | **3 files** |

## Conclusion

This standalone version proves that **all compiler functionality is implemented in Lex and Bison**, with helper functions embedded directly in the `.y` file's C code section. The main.c file is purely a driver with no compiler logic.

Show this to your professor to demonstrate compliance with the "lex and bison only" requirement! 🎓
