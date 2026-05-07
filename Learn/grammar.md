# Mini Rust Compiler — Grammar Rules & Working (In Depth)

> **File:** `parser_standalone.y`  
> **Parser Type:** LALR(1) using GNU Bison  
> **Technique:** Syntax-Directed Translation (SDT) — all phases run during parsing

---

## Table of Contents

1. [Operator Precedence Table](#1-operator-precedence-table)
2. [Program Structure Rules](#2-program-structure-rules)
3. [Function Definition Rules](#3-function-definition-rules)
4. [Parameter Rules](#4-parameter-rules)
5. [Block & Statement List Rules](#5-block--statement-list-rules)
6. [Statement Rules](#6-statement-rules)
7. [Declaration Rules](#7-declaration-rules)
8. [Type Specification Rules](#8-type-specification-rules)
9. [Assignment Rules](#9-assignment-rules)
10. [Control Flow Rules](#10-control-flow-rules)
11. [I/O Rules](#11-io-rules)
12. [Return Rules](#12-return-rules)
13. [Expression Rules](#13-expression-rules)
14. [Term Rules (Multiplicative)](#14-term-rules-multiplicative)
15. [Factor Rules (Primaries)](#15-factor-rules-primaries)
16. [Argument List Rules](#16-argument-list-rules)
17. [Error Recovery](#17-error-recovery)
18. [Semantic Conditions Summary](#18-semantic-conditions-summary)

---

## 1. Operator Precedence Table

Bison resolves shift/reduce conflicts using declared precedence. Lower declarations = lower precedence.

```
%left  '<' '>' EQ NEQ LEQ GEQ    ← LOWEST precedence (comparison)
%left  '+' '-'                    ← Medium precedence (additive)
%left  '*' '/'                    ← Higher precedence (multiplicative)
%right UMINUS                     ← HIGHEST precedence (unary minus)
```

**How it works:**
- `%left` means the operator is **left-associative**: `a + b + c` → `(a + b) + c`
- `%right` means **right-associative**: unary minus `-(-x)` → `-((-x))`
- Operators declared **later** have **higher** precedence
- Example: `2 + 3 * 4` is parsed as `2 + (3 * 4)` because `*` has higher precedence than `+`

---

## 2. Program Structure Rules

### Grammar

```
program
    : item_list          → the whole file is a list of items
    | ε                  → or the file is empty
    ;

item_list
    : item               → single item
    | item_list item     → multiple items (left-recursive)
    ;

item
    : statement          → a standalone statement
    | function_def       → a function definition
    ;
```

### Working

| Rule | Action |
|------|--------|
| `program → item_list` | Sets `parse_tree_root = item_list node` |
| `program → ε` | Creates an `EMPTY_PROGRAM` node and sets it as root |
| `item_list → item_list item` | Creates a `SEQ` node connecting previous and new item |

**Parse Tree Node Created:**
```
SEQ
├── SEQ
│   ├── item1
│   └── item2
└── item3
```

The left-recursive structure of `item_list` builds the tree in this chained `SEQ` pattern.

---

## 3. Function Definition Rules

### Grammar

```
function_def
    : FN IDENTIFIER '(' ')' block
    | FN IDENTIFIER '(' ')' ARROW type_spec block
    | FN IDENTIFIER '(' param_list ')' ARROW type_spec block
    | FN IDENTIFIER '(' param_list ')' block
    ;
```

### All 4 Variants Explained

#### Variant 1 — No params, no return type
```rust
fn greet() { ... }
```
- Bison parses: `FN IDENTIFIER '(' ')' block`
- Creates: `FUNC` node with children `[name_leaf, block_node]`

#### Variant 2 — No params, with return type
```rust
fn get_val() -> i32 { ... }
```
- Bison parses: `FN IDENTIFIER '(' ')' ARROW type_spec block`
- The `ARROW` token (`->`) and `type_spec` are consumed but the return type is **not stored** in this implementation (parse tree only)
- Creates: `FUNC` node with children `[name_leaf, block_node]`

#### Variant 3 — With params and return type
```rust
fn add(x: i32, y: i32) -> i32 { ... }
```
- Bison parses: `FN IDENTIFIER '(' param_list ')' ARROW type_spec block`
- **Semantic Action:** Calls `sym_exit_scope()` to exit the parameter scope
- Creates: `FUNC_PARAMS` node → then `FUNC` node

#### Variant 4 — With params, no return type
```rust
fn print_val(x: i32) { ... }
```
- Same as Variant 3 but without `ARROW type_spec`

### Scope Lifecycle for Functions with Parameters

```
sym_enter_scope()   ← triggered inside param_list rule
  param declared
  param declared
sym_exit_scope()    ← triggered in function_def rule AFTER parsing
```

---

## 4. Parameter Rules

### Grammar

```
param_list
    : { sym_enter_scope(); } param_list_items
    ;

param_list_items
    : param
    | param_list_items ',' param
    ;

param
    : IDENTIFIER ':' type_spec
    ;
```

### Working

**`param_list`** uses a **mid-rule action** `{ sym_enter_scope(); }` — this runs **before** parsing param_list_items, ensuring the parameter scope is opened before any parameters are registered.

**`param_list_items`** is left-recursive, so `a, b, c` becomes:
```
PARAM_SEQ
├── PARAM_SEQ
│   ├── PARAM(a)
│   └── PARAM(b)
└── PARAM(c)
```

**`param`** semantic action:
1. Calls `sym_insert(name, type)` to add the parameter to the symbol table
2. **Condition:** If `sym_insert` returns `-1`, the parameter was already declared → calls `sem_error("parameter already declared")`
3. Creates a `PARAM` node in the parse tree

---

## 5. Block & Statement List Rules

### Grammar

```
block
    : '{' { sym_enter_scope(); } stmt_list '}' { sym_exit_scope(); }
    ;

stmt_list
    : ε
    | stmt_list statement
    ;
```

### Working

**`block`** has **two mid-rule actions**:
- `{ sym_enter_scope(); }` fires right after `{` is seen — opens a new variable scope
- `{ sym_exit_scope(); }` fires right before `}` is reduced — closes the scope (variables go out of scope)

This is what enables **block-scoped variables** in Rust:
```rust
{
    let x = 5;  // x enters scope here
}
// x is no longer accessible here
```

**`stmt_list`** builds a left-leaning `SEQ` chain. An empty block produces `NULL` for the stmt_list node.

---

## 6. Statement Rules

### Grammar

```
statement
    : declaration
    | assignment
    | if_stmt
    | while_stmt
    | println_stmt
    | return_stmt
    | expr_stmt
    | error ';'
    ;
```

### Working

Each alternative simply passes up (`$$ = $1`) the node from the matched sub-rule.

The `error ';'` alternative is the **error recovery rule**:
- When a syntax error is encountered, Bison discards tokens until it finds `;`
- Calls `yyerrok` to reset the error state
- Creates an `ERROR` node so parsing can continue

---

## 7. Declaration Rules

### Grammar

```
declaration
    : LET IDENTIFIER '=' expression ';'
    | LET IDENTIFIER ':' type_spec '=' expression ';'
    | LET MUT IDENTIFIER '=' expression ';'
    | LET MUT IDENTIFIER ':' type_spec '=' expression ';'
    ;
```

### All 4 Variants Explained

#### Variant 1 — Immutable, type inferred
```rust
let x = 42;
```
- Type is inferred from the expression's `.type` field
- **Conditions checked:**
  1. `sym_insert($2, expr.type)` — Insert variable; if returns `-1` → duplicate error
  2. `expr.is_const` — If expression is a literal, store the value in symbol table
- **TAC emitted:** `x = 42`
- **Node created:** `LET(x, expr_node)`

#### Variant 2 — Immutable, explicit type
```rust
let x: i32 = 42;
```
- **Extra Condition:** Calls `types_compatible(declared_type, expr_type)` to verify match
- If incompatible → `sem_error("type mismatch")`
- **Node created:** `LET(x, expr_node)`

#### Variant 3 — Mutable, type inferred
```rust
let mut x = 42;
```
- Same as Variant 1 but node label becomes `LET_MUT`
- Variable CAN be reassigned (symbol table stores it; actual mutability enforcement is partial)

#### Variant 4 — Mutable, explicit type
```rust
let mut x: i32 = 42;
```
- Combines type annotation and mutability
- Checks `types_compatible`, emits TAC, creates `LET_MUT` node

### Constant Folding in Declarations

```c
if ($4.is_const) {
    if (is_integer_type($4.type))
        sym_set_int(idx, $4.int_val);      // store actual int value
    else
        sym_set_float(idx, $4.float_val);  // store actual float value
}
```

When `let x = 3 + 4;` is parsed, the `expression` rule folds `3+4` into `7` before the declaration action runs. So `x` is stored in the symbol table with value `7` and `is_const = 1`. Later uses of `x` can be further folded.

---

## 8. Type Specification Rules

### Grammar

```
type_spec
    : TYPE_I8   → I8_TYPE
    | TYPE_I16  → I16_TYPE
    | TYPE_I32  → I32_TYPE
    | TYPE_I64  → I64_TYPE
    | TYPE_I128 → I128_TYPE
    | TYPE_U8   → U8_TYPE
    | TYPE_U16  → U16_TYPE
    | TYPE_U32  → U32_TYPE
    | TYPE_U64  → U64_TYPE
    | TYPE_U128 → U128_TYPE
    | TYPE_F32  → F32_TYPE
    | TYPE_F64  → F64_TYPE
    | TYPE_BOOL → BOOL_TYPE
    ;
```

### Working

`type_spec` returns an integer enum value (`$$` is `%type <ival>`).

**Type Compatibility Rules (`types_compatible` function):**

| Condition | Allowed? |
|-----------|----------|
| Exact same type | ✅ Always |
| Expression type is `UNKNOWN_TYPE` | ✅ (error already reported) |
| `I32_TYPE` literal → any integer type | ✅ e.g., `let x: u64 = 5;` |
| `F64_TYPE` literal → any float type | ✅ e.g., `let x: f32 = 3.14;` |
| Integer → Float or vice versa | ❌ Type mismatch error |

---

## 9. Assignment Rules

### Grammar

```
assignment
    : IDENTIFIER '=' expression ';'
    ;
```

### Working

**Semantic Actions (in order):**
1. `sym_lookup($1)` — Search symbol table for the variable
2. **Condition:** If not found → `sem_error("undeclared variable")`
3. `sym_get_type(idx)` — Get the declared type of the variable
4. **Condition:** `types_compatible(declared, expr.type)` — If mismatch → `sem_error("type mismatch")`
5. `tac_emit($1, expr.name, NULL, NULL)` — Emit `x = t2` (simple copy TAC)
6. Create `ASSIGN` parse tree node

**Example:**
```rust
x = a + b;
// TAC:
//   t0 = a + b
//   x = t0
```

**Note:** This rule does NOT allow re-declaration. The variable must already exist from a prior `let` declaration.

---

## 10. Control Flow Rules

### Grammar

```
if_stmt
    : IF expression block
    | IF expression block ELSE block
    ;

while_stmt
    : WHILE expression block
    ;
```

### If Statement — Working

#### Simple `if`
```rust
if x > 0 { ... }
```
- Creates `IF(condition_node, block_node)`
- The condition is the full `expression` result (may itself be a complex sub-tree)
- No condition type checking (boolean is not enforced in this implementation)

#### If-Else
```rust
if x > 0 { ... } else { ... }
```
- First creates a `BRANCHES(then_block, else_block)` node
- Then wraps it: `IF_ELSE(condition_node, BRANCHES_node)`

**Parse Tree:**
```
IF_ELSE
├── condition (e.g., ">")
│   ├── x
│   └── 0
└── BRANCHES
    ├── BLOCK (then)
    └── BLOCK (else)
```

### While Loop — Working

```rust
while i < 10 { ... }
```
- Creates `WHILE(condition_node, block_node)`
- The block automatically handles scope via the `block` rule

---

## 11. I/O Rules

### Grammar

```
println_stmt
    : PRINTLN '(' STRING_LIT ')' ';'
    | PRINTLN '(' STRING_LIT ',' expression ')' ';'
    ;
```

### Working

#### Variant 1 — String only
```rust
println!("Hello World");
```
- Creates a leaf node for the string literal
- Creates `PRINTLN(string_leaf, NULL)`

#### Variant 2 — String with expression
```rust
println!("Value: {}", x);
```
- Creates `PRINTLN(string_leaf, expr_node)`
- The expression is parsed normally (triggers TAC, type-checking, etc.)

**Note:** The format string is not validated for `{}` placeholders — it's treated as a raw string literal.

---

## 12. Return Rules

### Grammar

```
return_stmt
    : RETURN expression ';'
    | RETURN ';'
    ;
```

### Working

#### With value
```rust
return x + 1;
```
- Parses `expression`, which may emit TAC
- Creates `RETURN(expr_node, NULL)`

#### Without value (void return)
```rust
return;
```
- Creates `RETURN(NULL, NULL)` — both children are null

**Note:** The return type is not validated against the declared function return type in this implementation.

---

## 13. Expression Rules

### Grammar

```
expression
    : expression '+' term
    | expression '-' term
    | expression '<' term
    | expression '>' term
    | expression EQ term       (==)
    | expression NEQ term      (!=)
    | expression LEQ term      (<=)
    | expression GEQ term      (>=)
    | term
    ;
```

### Working — Arithmetic (`+`, `-`)

These two operators support **Constant Folding**.

**Constant Folding Condition:**
```c
if ($1.is_const && $3.is_const &&
    is_integer_type($1.type) && is_integer_type($3.type))
```

If both sides are known integer constants:
- Evaluate the result immediately at compile time
- Emit TAC with the actual numbers: `t0 = 3 + 4`
- After optimization, this becomes: `t0 = 7` (shown in optimized TAC output)
- Set `$$.is_const = 1` so parent rules can fold further

If either side is NOT constant (e.g., a variable):
- Emit TAC with variable names: `t0 = x + y`
- Set `$$.is_const = 0`

**Type Promotion:**
```c
$$.type = (is_float_type($1.type) || is_float_type($3.type)) ? F64_TYPE : I32_TYPE;
```
- If EITHER operand is a float → result is `F64_TYPE`
- Otherwise → result is `I32_TYPE`

### Working — Comparisons (`<`, `>`, `==`, `!=`, `<=`, `>=`)

- These do NOT support constant folding
- Always emit TAC: `t0 = a < b`
- Always return type `I32_TYPE` (boolean result represented as int)
- `$$.is_const = 0` always

---

## 14. Term Rules (Multiplicative)

### Grammar

```
term
    : term '*' factor
    | term '/' factor
    | factor
    ;
```

### Working

#### Multiplication (`*`)
Identical constant-folding logic as `+`:
- If both constants → fold: `t0 = 3 * 4` → optimized to `t0 = 12`
- Otherwise → `t0 = x * y`

#### Division (`/`)
Same but with **division-by-zero guard:**
```c
$$.int_val = ($3.int_val != 0) ? $1.int_val / $3.int_val : 0;
```
- If dividing by constant `0`, result stored as `0` (no crash, but no error emitted either)

#### Why Separate `expression` and `term`?

This is **classic grammar stratification** to enforce operator precedence:
- `expression` handles low-precedence ops (`+`, `-`, comparisons)
- `term` handles high-precedence ops (`*`, `/`)
- This ensures `2 + 3 * 4` parses as `2 + (3*4)` not `(2+3) * 4`

---

## 15. Factor Rules (Primaries)

### Grammar

```
factor
    : INT_LIT
    | FLOAT_LIT
    | TRUE_LIT
    | FALSE_LIT
    | IDENTIFIER
    | IDENTIFIER '(' ')'
    | IDENTIFIER '(' arg_list ')'
    | '(' expression ')'
    | '-' factor  %prec UMINUS
    ;
```

### Working — Each Variant

#### `INT_LIT` (e.g., `42`)
- Sets `type = I32_TYPE`, `is_const = 1`, `int_val = 42`
- Creates a leaf node with the number as a string

#### `FLOAT_LIT` (e.g., `3.14`)
- Sets `type = F64_TYPE`, `is_const = 1`, `float_val = 3.14`
- Creates a leaf node

#### `TRUE_LIT` / `FALSE_LIT`
- Sets `type = BOOL_TYPE`, `is_const = 1`
- `TRUE_LIT` → `int_val = 1`, `FALSE_LIT` → `int_val = 0`

#### `IDENTIFIER` (variable use)
**Conditions:**
1. `sym_lookup($1)` — search the symbol table
2. If **not found** → `sem_error("undeclared variable")`, set `type = UNKNOWN_TYPE`, `is_const = 0`
3. If **found** → get type from symbol table via `sym_get_type(idx)`
4. If the variable was stored as a constant → copy `int_val`/`float_val` and set `is_const = 1` for further folding

#### `IDENTIFIER '(' ')'` — Function Call (no args)
```rust
get_value()
```
- Allocates a new temporary: `t5`
- Emits TAC: `t5 = call get_value`
- Creates `CALL(name_leaf, NULL)`
- **Note:** No check if function is declared (function lookup not implemented)

#### `IDENTIFIER '(' arg_list ')'` — Function Call (with args)
```rust
add(x, y)
```
- Same as above but includes arg_list subtree
- Creates `CALL(name_leaf, arg_list_node)`

#### `'(' expression ')'` — Grouped expression
- Simply returns the inner expression unchanged (`$$ = $2`)
- Parentheses are NOT stored in the parse tree

#### `'-' factor %prec UMINUS` — Unary Minus
```rust
-x    or    -5
```
- `%prec UMINUS` forces this to use the `UMINUS` precedence level (highest)
- **Constant folding:** If the operand is a constant integer, negates `int_val` immediately
- **TAC emitted:** `t0 = 0 - x` (subtraction from zero represents negation)
- Creates `NEG(factor_node, NULL)`

---

## 16. Argument List Rules

### Grammar

```
arg_list
    : expression
    | arg_list ',' expression
    ;
```

### Working

- `arg_list` returns a **Node*** (parse tree node), not an expr struct
- Single argument: returns `expr.node` directly
- Multiple arguments: creates `ARG_SEQ(prev_args, new_expr_node)`

**Example:** `f(a, b, c)` → `ARG_SEQ(ARG_SEQ(a_node, b_node), c_node)`

---

## 17. Error Recovery

### Grammar

```
statement
    : ...
    | error ';'
    ;
```

### Working

Bison's `error` token is a special pseudo-token.

**Recovery Process:**
1. A syntax error is detected (e.g., `let 123 = x;`)
2. Bison calls `yyerror("syntax error message")` which prints the error
3. Bison tries to recover: it pops the stack and skips input tokens until `;` is found
4. When `;` is found, the `error ';'` rule matches
5. `yyerrok` resets the error state so further errors can be reported
6. An `ERROR` node is inserted in the parse tree
7. Parsing **continues** from the next statement

This means **one bad line doesn't kill the entire compilation.**

---

## 18. Semantic Conditions Summary

| Rule | Condition Checked | Error Thrown |
|------|-------------------|--------------|
| `declaration` | Variable already in scope? | "already declared" |
| `declaration` | Type of expr matches declared type? | "type mismatch" |
| `assignment` | Variable was declared? | "undeclared variable" |
| `assignment` | Assigned type matches declared type? | "type mismatch" |
| `factor (IDENTIFIER)` | Variable was declared? | "undeclared variable" |
| `param` | Parameter name already used? | "parameter already declared" |
| `expression (+,-)` | Both operands constants and integers? | → **Constant Fold** |
| `term (*,/)` | Both operands constants and integers? | → **Constant Fold** |
| `term (/)` | Divisor is zero? | → Result = 0 (silent) |
| `factor (UMINUS)` | Operand is constant integer? | → **Constant Fold** |

---

## Quick Reference — Grammar Production Summary

```
program         → item_list | ε
item_list       → item | item_list item
item            → statement | function_def

function_def    → fn ID() block
                | fn ID() -> type block
                | fn ID(params) block
                | fn ID(params) -> type block

param_list      → [scope_enter] param_list_items
param_items     → param | param_items , param
param           → ID : type

block           → { [scope_enter] stmt_list } [scope_exit]
stmt_list       → ε | stmt_list statement

statement       → declaration | assignment | if | while
                | println | return | expr_stmt | error ;

declaration     → let ID = expr ;
                | let ID : type = expr ;
                | let mut ID = expr ;
                | let mut ID : type = expr ;

type_spec       → i8|i16|i32|i64|i128|u8|u16|u32|u64|u128|f32|f64|bool

assignment      → ID = expr ;
if_stmt         → if expr block | if expr block else block
while_stmt      → while expr block
println_stmt    → println!(str) ; | println!(str, expr) ;
return_stmt     → return expr ; | return ;
expr_stmt       → expr ;

expression      → expr + term | expr - term
                | expr < term | expr > term
                | expr == term | expr != term
                | expr <= term | expr >= term
                | term

term            → term * factor | term / factor | factor

factor          → INT | FLOAT | true | false | ID
                | ID() | ID(args) | (expr) | -factor

arg_list        → expr | arg_list , expr
```

---

*Generated from `parser_standalone.y` — Mini Rust Compiler (Lex & Bison Implementation)*
