fn main() {
    let a: i8 = 10;
    let b: i16 = 100;
    let c: i32 = 1000;
    let d: i64 = 10000;
    let e: i128 = 100000;
    
    let f: u8 = 5;
    let g: u16 = 50;
    let h: u32 = 500;
    let i: u64 = 5000;
    let j: u128 = 50000;
    
    let x: f32 = 3.14;
    let y: f64 = 2.718;
    
    let flag: bool = true;
    let check: bool = false;
    
    let sum_int: i32 = a + b + c;
    let sum_uint: u32 = f + g + h;
    let sum_float: f64 = x + y;
    
    let mut counter: i64 = 0;
    while counter < 5 {
        counter = counter + 1;
    }
    
    if sum_int > 100 {
        let big: i32 = sum_int * 2;
    }
    
    let result: i32 = sum_int - sum_uint;
}

fn calculate_i8(n: i8) -> i8 {
    let mut sum: i8 = 0;
    let mut i: i8 = 0;
    
    while i < n {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}

fn calculate_i16(n: i16) -> i16 {
    let mut sum: i16 = 0;
    let mut i: i16 = 0;
    
    while i < n {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}

fn calculate_i64(n: i64) -> i64 {
    let mut sum: i64 = 0;
    let mut i: i64 = 0;
    
    while i < n {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}

fn calculate_u32(n: u32) -> u32 {
    let mut sum: u32 = 0;
    let mut i: u32 = 0;
    
    while i < n {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}

fn calculate_u64(n: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    while i < n {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}

fn calculate_f32(x: f32) -> f32 {
    let result: f32 = x * 2.0;
    return result;
}

fn calculate_f64(x: f64) -> f64 {
    let result: f64 = x * 3.0;
    return result;
}

fn test_bool(flag: bool) -> bool {
    if flag == true {
        return false;
    } else {
        return true;
    }
}

fn mixed_types() {
    let small: i8 = 10;
    let medium: i32 = 100;
    let large: i64 = 1000;
    let huge: i128 = 10000;
    
    let tiny: u8 = 5;
    let normal: u32 = 50;
    let big: u64 = 500;
    let massive: u128 = 5000;
    
    let pi: f32 = 3.14;
    let e: f64 = 2.718;
    
    let yes: bool = true;
    let no: bool = false;
    
    let calc1: i32 = small + medium;
    let calc2: i64 = medium + large;
    let calc3: u32 = tiny + normal;
    let calc4: u64 = normal + big;
    
    if calc1 > 50 {
        let temp: i32 = calc1 * 2;
    } else {
        let temp: i32 = calc1 + 10;
    }
    
    let mut loop_i8: i8 = 0;
    while loop_i8 < 3 {
        loop_i8 = loop_i8 + 1;
    }
    
    let mut loop_u16: u16 = 0;
    while loop_u16 < 5 {
        loop_u16 = loop_u16 + 1;
    }
    
    let final_result: i32 = calc1 + calc3;
}
