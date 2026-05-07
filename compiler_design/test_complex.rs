fn main() {
    let x: i32 = 10;
    let y: i32 = 20;
    let mut z: i32 = 0;
    
    z = x + y * 2;
    
    let a: i32 = z - x / 2;
    let b: i32 = a * 3 + y;
    
    if b > 50 {
        let c: i32 = b - 10;
        z = c + x;
    } else {
        z = b + y;
    }
    
    let mut counter: i32 = 0;
    while counter < 5 {
        let temp: i32 = counter * 2;
        counter = counter + 1;
    }
    
    let result: i32 = (x + y) * (z - a) / 2;
    
    let final_value: i32 = result + counter - b;
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

fn factorial(num: i32) -> i32 {
    if num == 0 {
        return 1;
    } else {
        let prev: i32 = num - 1;
        let fact_result: i32 = factorial(prev);
        return num * fact_result;
    }
}
