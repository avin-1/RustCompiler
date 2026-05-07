// ============================================================
// test_errors.rs  —  Demonstrates error detection
// ============================================================
// This file intentionally contains errors to show the
// compiler's error-handling capabilities.
// ============================================================

fn main() {
    // Correct declaration
    let x: i32 = 10;

    // ERROR: Multiple declaration of 'x'
    let x: i32 = 20;

    // ERROR: Undeclared variable 'w'
    let y: i32 = w + 5;

    // ERROR: Type mismatch (assigning float to int)
    let z: i32 = 3.14;
}
