# Mini Rust Compiler using Flex and Bison

A complete compiler front-end for a subset of the Rust programming language,
built with **Flex** (lexical analysis) and **Bison** (syntax analysis) in C.

> **Note:** This project uses Bison — **NOT** Yacc.

---

## Compiler Phases Implemented

| # | Phase | Module | Theory |
|---|---|---|---|
| 1 | **Lexical Analysis** | `lexer.l` (Flex) | DFA-based tokenisation |
| 2 | **Syntax Analysis** | `parser.y` (Bison) | LALR(1) parsing using CFG |
| 3 | **Parse Tree / AST** | `parse_tree.c/.h` | Syntax-Directed Translation |
| 4 | **Semantic Analysis** | `symbol_table.c/.h` + rules in `parser.y` | Symbol table lookups & type checking |
| 5 | **Intermediate Code** | `intermediate_code.c/.h` | Three-Address Code (TAC) |
| 6 | **Optimisation** | Constant folding in `intermediate_code.c` | Compile-time evaluation |

---

## Output Produced

When you run `compiler.exe test.rs`, you get **all 5 outputs**:

1. ✅ **Token stream** — lexical analysis
2. ✅ **Parsing result** — success/error summary
3. 🌳 **Parse Tree** — printed hierarchically + saved to `parsetree.txt`
4. ✅ **Symbol Table** — all declared variables with types & values
5. ✅ **Intermediate Code (TAC)** — three-address code + optimised version

---

## Project Structure

```
compiler_design/
├── lexer.l              # Flex specification (Phase 1: Lexical Analysis)
├── parser.y             # Bison grammar + semantic actions (Phase 2+3+4+5)
├── parse_tree.h         # Parse tree node structure & API
├── parse_tree.c         # Parse tree create/print/save/free
├── symbol_table.h       # Symbol table API
├── symbol_table.c       # Symbol table implementation
├── intermediate_code.h  # TAC generation API
├── intermediate_code.c  # TAC emission + constant folding
├── main.c               # Entry point — orchestrates all phases
├── Makefile             # GNU Make build script
├── build.bat            # Windows batch build script
├── test.rs              # Sample valid Rust-subset program
├── test_errors.rs       # Sample with intentional errors
└── README.md            # This file
```

---

## Prerequisites

1. **WinFlexBison** — provides `win_flex.exe` and `win_bison.exe`
   - Download: https://github.com/lexxmark/winflexbison/releases
2. **GCC** (MinGW / TDM-GCC / MSYS2)
3. No WSL or Docker required.

---

## Compilation & Execution

### Step-by-step (manual)

```bash
# Phase 1: Generate lexer (DFA-based scanner)
win_flex lexer.l

# Phase 2: Generate parser (LALR(1) tables)
win_bison -d parser.y

# Compile all modules together
gcc lex.yy.c parser.tab.c symbol_table.c intermediate_code.c parse_tree.c main.c -o compiler.exe
```

### Using build.bat

```bash
build.bat           # just build
build.bat run       # build + run test.rs
build.bat errors    # build + run test_errors.rs
build.bat clean     # clean generated files
```

### Running

```bash
compiler.exe test.rs           # from file
compiler.exe                   # from stdin (Ctrl-Z + Enter to end)
```

---

## Sample Input (`test.rs`)

```rust
fn main() {
    let x: i32 = 10;
    let y: i32 = 20;
    let mut z: i32 = 0;
    z = x + y;
    let a: i32 = 3 + 4 * 2;

    if z > 25 {
        let result_a: i32 = z - 5;
    } else {
        let result_b: i32 = z + 5;
    }

    let mut counter: i32 = 0;
    while counter < 5 {
        counter = counter + 1;
    }

    let pi: f64 = 3.1415;
    println!("done");
}
```

## Sample Output (all 5 phases)

### Phase 1: Token Stream
```
[KW: fn]  [ID: main]  [DL: (]  [DL: )]  [DL: {]
[KW: let]  [ID: x]  [DL: :]  [KW: i32]  [OP: =]  [INT: 10]  [DL: ;]
...
```

### Phase 2: Parsing Result
```
>>> Parsing completed SUCCESSFULLY. No errors.
```

### Phase 3: 🌳 Parse Tree
```
  FUNC
    main
    BLOCK
      SEQ
        SEQ
          ...
            LET
              x
              10
            LET
              y
              20
          LET_MUT
            z
            0
        ASSIGN
          z
          +
            x
            y
      LET
        a
        +
          3
          *
            4
            2
    IF_ELSE
      >
        z
        25
      BRANCHES
        BLOCK
          LET
            result_a
            -
              z
              5
        BLOCK
          LET
            result_b
            +
              z
              5
    WHILE
      <
        counter
        5
      BLOCK
        ASSIGN
          counter
          +
            counter
            1
```

Also saved to `parsetree.txt`.

### Phase 4: Symbol Table
```
Idx  Name             Type       Defined?   Scope      Value
-------------------------------------------------------------
0    x                int        yes        0          10
1    y                int        yes        0          20
2    z                int        yes        0          0
3    a                int        yes        0          11
4    result_a         int        yes        0          -5
5    result_b         int        yes        0          5
6    counter          int        yes        0          0
7    pi               float      yes        0          3.14
```

### Phase 5: Intermediate Code (TAC)
```
  x = 10
  y = 20
  z = 0
  t0 = 10 + 20
  z = t0
  t1 = 4 * 2
  t2 = 3 + 8      ← note: already partially folded by propagation
  a = t2
  ...
```

### Optimised Code (after constant folding)
```
  t0 = 30    ; folded from: 10 + 20
  t1 = 8     ; folded from: 4 * 2
  t2 = 11    ; folded from: 3 + 8
```

---

## Error Handling Demo

### Input (`test_errors.rs`)
```rust
fn main() {
    let x: i32 = 10;
    let x: i32 = 20;       // ERROR: Multiple declaration
    let y: i32 = w + 5;    // ERROR: Undeclared variable 'w'
    let z: i32 = 3.14;     // ERROR: Type mismatch
}
```

### Output
```
Semantic Error: line 13: variable 'x' already declared
Semantic Error: line 16: undeclared variable 'w'
Semantic Error: line 19: type mismatch in declaration of 'z' (expected i32, got f64)
>>> Parsing completed with ERRORS.
>>> 3 semantic error(s) detected.
```

---

## Supported Rust Subset

| Construct | Example |
|---|---|
| Variable declaration | `let x = 10;` or `let x: i32 = 10;` |
| Mutable variable | `let mut x = 0;` |
| Assignment | `x = x + 1;` |
| Arithmetic | `a + b * c - d / e` |
| Comparison | `x > 0`, `x == y`, `x != z` |
| If / If-else | `if x > 0 { ... } else { ... }` |
| While loop | `while x < 10 { ... }` |
| Function definition | `fn main() { ... }` |
| println! | `println!("hello");` |
| Comments | `// line` and `/* block */` |
| Types | `i32`, `f64`, `bool` |

---

## Theory Mapping (For Viva / Exam)

### 1. Lexical Analysis → DFA (Deterministic Finite Automaton)

**File:** `lexer.l`

**Theory:**
- Flex converts regular expression patterns into a DFA-based scanner.
- Each token pattern (e.g., `[0-9]+` for integers) is a regular expression.
- Flex compiles all patterns into a single DFA that scans input character-by-character.
- When the DFA reaches an accepting state, the matched text becomes a token.

**How it maps:**
```
Regular Expression  →  NFA (Thompson's construction)
NFA                 →  DFA (Subset construction)
DFA                 →  Minimised DFA (Hopcroft's algorithm)
Minimised DFA       →  lex.yy.c scanner code
```

**Viva Q:** *"How does Flex tokenise input?"*
**A:** Flex uses regex patterns to define tokens. Internally, it constructs a DFA from these regexes. The DFA processes input character-by-character. When it reaches a final state, it returns the matched token. For example, `[0-9]+` matches any sequence of digits and returns `INT_LIT`.

---

### 2. Syntax Analysis → CFG (Context-Free Grammar) + LALR(1) Parsing

**File:** `parser.y`

**Theory:**
- Bison accepts a Context-Free Grammar (CFG) written in BNF-like notation.
- It generates an LALR(1) parser — a type of bottom-up (shift-reduce) parser.
- The parser uses a stack and a parse table to validate token sequences.
- **Shift**: Push the next token onto the stack.
- **Reduce**: When the top of stack matches the right-hand side of a production, replace it with the left-hand side (non-terminal).

**Our CFG (simplified):**
```
program     → item_list
item_list   → item  |  item_list item
item        → statement  |  function_def
statement   → declaration | assignment | if_stmt | while_stmt
declaration → LET IDENTIFIER '=' expression ';'
expression  → expression '+' term  |  expression '-' term  |  term
term        → term '*' factor  |  term '/' factor  |  factor
factor      → INT_LIT  |  FLOAT_LIT  |  IDENTIFIER  |  '(' expression ')'
```

**Viva Q:** *"What type of parser does Bison generate?"*
**A:** Bison generates an LALR(1) bottom-up parser. LALR stands for Look-Ahead LR. It reads input left-to-right, produces a rightmost derivation in reverse, and uses 1 token of lookahead. The `(1)` means it looks at the next single token to decide whether to shift or reduce.

**Viva Q:** *"How is operator precedence handled?"*
**A:** We use Bison's `%left` and `%right` directives. `%left '+' '-'` makes + and - left-associative. The order of declarations (lower = lower precedence) ensures `*` and `/` bind tighter than `+` and `-`. This resolves shift-reduce conflicts automatically.

---

### 3. Parse Tree Construction → Syntax-Directed Translation (SDT)

**Files:** `parse_tree.c`, `parse_tree.h`, actions in `parser.y`

**Theory:**
- A **Parse Tree** (or Concrete Syntax Tree) represents the derivation of the input according to the grammar.
- In our compiler, we build the tree using **Syntax-Directed Translation (SDT)**.
- SDT means semantic actions are attached to grammar rules and executed when the rule is reduced.
- Each action creates a `Node` with an operator/keyword label and child pointers.

**Node structure:**
```c
struct Node {
    char name[50];        // "+", "LET", "x", "10"
    struct Node *left;    // left child
    struct Node *right;   // right child
};
```

**Example:** When Bison reduces `expression → expression '+' term`:
```c
$$.node = createNode("+", $1.node, $3.node);
```
This creates:
```
    +
   / \
  x   y
```

**Viva Q:** *"How is the parse tree built?"*
**A:** The parse tree is built bottom-up during Bison's reduce operations. Each grammar rule has a semantic action that calls `createNode()` or `createLeaf()`. Leaf nodes are created for terminals (identifiers, literals). Internal nodes are created for operators and keywords. The tree grows upward as rules are reduced, until the root (`program`) is reached.

---

### 4. Symbol Table → Semantic Analysis

**Files:** `symbol_table.c`, `symbol_table.h`

**Theory:**
- The symbol table is a data structure that stores information about every identifier in the program.
- It is used during **semantic analysis** — the phase that checks meaning (not just structure).
- Semantic checks cannot be expressed in a CFG, so they are done programmatically.

**What we store:**
| Field | Purpose |
|---|---|
| `name` | Variable name |
| `type` | Data type (INT_TYPE, FLOAT_TYPE) |
| `is_defined` | Whether a value has been assigned |
| `scope` | Scope level (0 = global) |
| `int_val` / `float_val` | Constant value (for folding) |

**Errors detected:**
1. **Undeclared variable** — using a name not in the symbol table
2. **Redeclaration** — inserting a name that already exists
3. **Type mismatch** — assigning `f64` to an `i32` variable

**Viva Q:** *"What is the role of the symbol table?"*
**A:** The symbol table stores metadata about identifiers (variables, functions). When a variable is declared, we insert it. When it's used, we look it up. If a lookup fails, we report "undeclared variable". If an insertion finds a duplicate, we report "redeclaration". We also compare types during assignment to detect mismatches.

---

### 5. Intermediate Code Generation → Three-Address Code (TAC)

**Files:** `intermediate_code.c`, `intermediate_code.h`

**Theory:**
- TAC is an intermediate representation (IR) where each instruction has at most one operator and up to three operands.
- Format: `result = arg1 op arg2` or `result = arg1` (assignment).
- TAC uses **temporary variables** (`t0`, `t1`, `t2`, …) to hold sub-expression results.
- TAC is machine-independent — it can be translated to any target architecture.

**Example:**
```
Source:   c = a + b * 2;

TAC:
  t0 = b * 2
  t1 = a + t0
  c  = t1
```

**Viva Q:** *"Why use three-address code?"*
**A:** TAC is a machine-independent intermediate form that simplifies both optimisation and code generation. Each instruction has at most one operation, making it easy to analyse. Temporary variables correspond to registers. TAC can be optimised (e.g., constant folding) before being translated to machine code.

---

### 6. Constant Folding Optimisation

**Theory:**
- Constant folding is a compile-time optimisation.
- If both operands of an arithmetic instruction are known constants, compute the result during compilation.
- This reduces the number of instructions executed at runtime.

**Example:**
```
Before:  t0 = 4 * 2       →  After:  t0 = 8    ; folded
Before:  t1 = 3 + 8       →  After:  t1 = 11   ; folded
```

**Viva Q:** *"What is constant folding?"*
**A:** Constant folding evaluates expressions with constant operands at compile time instead of runtime. For example, `4 * 2` is replaced by `8` in the TAC. This saves CPU cycles during execution and reduces code size.

---

## How It All Connects

```
Source Code (.rs)
       │
       ▼
┌──────────────┐     Flex generates DFA-based scanner
│ LEXICAL      │     RegEx → NFA → DFA → Token Stream
│ ANALYSIS     │     Output: [KW: let] [ID: x] [OP: =] [INT: 10] [DL: ;]
│ (lexer.l)    │
└──────┬───────┘
       │ tokens
       ▼
┌──────────────┐     Bison generates LALR(1) parser
│ SYNTAX       │     CFG rules validated via shift-reduce parsing
│ ANALYSIS     │     Simultaneously builds Parse Tree (SDT)
│ (parser.y)   │     Simultaneously checks semantics (symbol table)
│              │     Simultaneously emits TAC (intermediate code)
└──────┬───────┘
       │
       ▼
┌──────────────┐     Output 1: Token Stream
│ ALL OUTPUTS  │     Output 2: Parse Result (success/error)
│              │     Output 3: 🌳 Parse Tree (hierarchical + file)
│              │     Output 4: Symbol Table
│              │     Output 5: TAC + Optimised TAC
└──────────────┘
```

---

## License

This project is for educational purposes (college lab / Compiler Design course).
