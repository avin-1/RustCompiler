// ============================================================
// test.rs  —  Sample input for the Mini Rust Compiler
// ============================================================
// This file demonstrates all supported features:
//   - Variable declarations (with and without type annotations)
//   - Arithmetic expressions and assignments
//   - If / if-else statements
//   - While loops
//   - println! macro
//   - Constant folding (e.g. 10 + 20)
// ============================================================

fn main() {
    // --- Variable declarations ---
    let x: i32 = 10;
    let y: i32 = 20;
    let mut z: i32 = 0;

    // --- Arithmetic + assignment ---
    z = x + y;

    // --- Constant folding demo ---
    let a: i32 = 3 + 4 * 2;

    // --- If-else statement ---
    if z > 25 {
        let result_a: i32 = z - 5;
    } else {
        let result_b: i32 = z + 5;
    }

    // --- While loop ---
    let mut counter: i32 = 0;
    while counter < 5 {
        counter = counter + 1;
    }

    // --- Float variable ---
    let pi: f64 = 3.1415;

    // --- println! ---
    println!("done");
}
