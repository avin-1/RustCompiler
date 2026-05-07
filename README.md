# Mini Rust Compiler

A fully functional compiler for a subset of Rust, built with Flex and Bison. Features both a terminal-based standalone version and a GTK3-based GUI version.

![Status](https://img.shields.io/badge/status-production%20ready-brightgreen)
![Tests](https://img.shields.io/badge/tests-passing-brightgreen)
![Types](https://img.shields.io/badge/types-13%20supported-blue)

## 🚀 Quick Start

### Prerequisites
- **Flex** (Lexical Analyzer)
- **Bison** (Parser Generator)
- **GCC** (C Compiler)
- **GTK3** (GUI version only)

### Build & Run (Standalone)
```bash
cd compiler_design
build_standalone.bat
compiler_standalone.exe test.rs
```

### Build & Run (GUI)
```bash
cd compiler_design
build_gui.bat
compiler_gui.exe
```

## 📖 Documentation

**👉 See [GETTING_STARTED.txt](GETTING_STARTED.txt) for complete setup instructions**

Additional documentation:
- [ARCHITECTURE.txt](compiler_design/ARCHITECTURE.txt) - System architecture
- [FIXES_APPLIED.md](compiler_design/FIXES_APPLIED.md) - Recent fixes
- [TEST_RESULTS.md](compiler_design/TEST_RESULTS.md) - Test coverage
- [VERIFICATION_COMPLETE.txt](compiler_design/VERIFICATION_COMPLETE.txt) - Verification summary

## ✨ Features

### Supported Data Types (13 total)
- **Signed integers:** `i8`, `i16`, `i32`, `i64`, `i128`
- **Unsigned integers:** `u8`, `u16`, `u32`, `u64`, `u128`
- **Floating point:** `f32`, `f64`
- **Boolean:** `bool`

### Language Features
- ✅ Variable declarations (immutable & mutable)
- ✅ Type inference and explicit typing
- ✅ Arithmetic operators (`+`, `-`, `*`, `/`)
- ✅ Comparison operators (`<`, `>`, `==`, `!=`, `<=`, `>=`)
- ✅ Control flow (`if`, `if-else`, `while`)
- ✅ Functions with parameters and return types
- ✅ Recursive functions
- ✅ `println!` macro

### Compilation Phases
1. **Lexical Analysis** - Token stream generation
2. **Syntax Analysis** - Grammar validation
3. **Parse Tree** - AST construction
4. **Symbol Table** - Variable tracking
5. **Intermediate Code** - Three-Address Code (TAC)
6. **Optimization** - Constant folding

## 📝 Example Code

```rust
fn main() {
    let x: i32 = 10;
    let y: i32 = 20;
    let mut z: i32 = 0;
    
    z = x + y;
    
    if z > 25 {
        let result: i32 = z * 2;
    }
    
    println!("Done");
}

fn calculate(n: i32) -> i32 {
    let mut sum: i32 = 0;
    let mut i: i32 = 0;
    
    while i < n {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}
```

## 🧪 Testing

Run all tests:
```bash
cd compiler_design
run_all_tests.bat
```

Test individual files:
```bash
compiler_standalone.exe test.rs
compiler_standalone.exe test_all_types.rs
compiler_standalone.exe test_complex.rs
```

### Test Results
| Test File | Status | Symbols | Errors |
|-----------|--------|---------|--------|
| test_all_types.rs | ✅ PASS | 61 | 0 |
| test.rs | ✅ PASS | 8 | 0 |
| test_complex.rs | ✅ PASS | 16 | 0 |

## 🏗️ Project Structure

```
RustCompiler_Mine/
├── GETTING_STARTED.txt          # Complete setup guide
├── README.md                     # This file
└── compiler_design/
    ├── lexer.l                   # Flex lexer specification
    ├── parser_standalone.y       # Bison parser
    ├── main_standalone.c         # Standalone entry point
    ├── ui.c / ui.h              # GUI implementation
    ├── callbacks.c              # GUI event handlers
    ├── compiler_interface.c     # Compiler-GUI bridge
    ├── build_standalone.bat     # Build script (terminal)
    ├── build_gui.bat           # Build script (GUI)
    ├── run_all_tests.bat       # Test runner
    ├── test.rs                 # Basic test
    ├── test_all_types.rs       # Type system test
    ├── test_complex.rs         # Complex expressions test
    └── [documentation files]
```

## 🔧 Installation

### Windows (Recommended)

1. **Install MSYS2** (easiest method)
   ```bash
   # Download from https://www.msys2.org/
   # Then install required packages:
   pacman -S flex bison mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-gtk3  # For GUI version
   ```

2. **Or install individually:**
   - Flex & Bison: https://github.com/lexxmark/winflexbison/releases
   - MinGW-w64: https://www.mingw-w64.org/
   - GTK3: Via MSYS2 (for GUI)

3. **Add to PATH:**
   - Add Flex, Bison, and GCC bin directories to system PATH

### Verify Installation
```bash
win_flex --version
win_bison --version
gcc --version
```

## 🎯 Usage

### Standalone (Terminal) Version

**Compile a file:**
```bash
compiler_standalone.exe mycode.rs
```

**View output:**
- Token stream
- Parse tree
- Symbol table
- Intermediate code (TAC)
- Optimized code

### GUI Version

**Launch:**
```bash
compiler_gui.exe
```

**Features:**
- Syntax highlighting
- Real-time compilation
- Interactive output
- File save/load
- Error highlighting

## 🐛 Troubleshooting

### Build Errors

**"flex/win_flex not found"**
- Install Flex and add to PATH
- Or use MSYS2: `pacman -S flex`

**"bison/win_bison not found"**
- Install Bison and add to PATH
- Or use MSYS2: `pacman -S bison`

**"gcc not found"**
- Install MinGW-w64 or MSYS2
- Add GCC to PATH

**GUI build fails**
- Install GTK3: `pacman -S mingw-w64-x86_64-gtk3`
- Use standalone version if GTK3 unavailable

### Runtime Errors

**"Parsing completed with ERRORS"**
- Check Rust syntax
- Ensure semicolons after statements
- Verify matching braces
- See example files for reference

## 📊 Performance

- **Build Time:** < 5 seconds
- **Compilation Speed:** Instant for test files
- **Memory Usage:** Efficient (256 symbols, 1024 TAC instructions)
- **Test Coverage:** 100% passing

## 🎓 Educational Value

This compiler demonstrates:
- **Lexical Analysis** with Flex
- **Syntax Analysis** with Bison
- **Symbol Table** management
- **Type Checking** and semantic analysis
- **Intermediate Code** generation (TAC)
- **Code Optimization** (constant folding)
- **GUI Development** with GTK3

Perfect for:
- Compiler design courses
- Understanding compilation phases
- Learning Flex and Bison
- Studying type systems

## 📈 Recent Improvements

### Version 1.0 (May 2026)
- ✅ Fixed all 58 semantic errors
- ✅ Implemented type compatibility system
- ✅ Added boolean literal support
- ✅ Fixed function parameter scoping
- ✅ Preserved symbol table after compilation
- ✅ 100% test pass rate

See [FIXES_APPLIED.md](compiler_design/FIXES_APPLIED.md) for details.

## 🤝 Contributing

This is an educational project. Feel free to:
- Report issues
- Suggest improvements
- Add new features
- Improve documentation

## 📄 License

Educational project for learning compiler design.

## 🙏 Acknowledgments

Built with:
- **Flex** - Fast Lexical Analyzer
- **Bison** - GNU Parser Generator
- **GCC** - GNU Compiler Collection
- **GTK3** - GIMP Toolkit

## 📞 Support

For detailed setup instructions, see [GETTING_STARTED.txt](GETTING_STARTED.txt)

For architecture details, see [ARCHITECTURE.txt](compiler_design/ARCHITECTURE.txt)

---

**Status:** Production Ready ✅  
**Version:** 1.0  
**Last Updated:** May 7, 2026

Happy Compiling! 🚀
