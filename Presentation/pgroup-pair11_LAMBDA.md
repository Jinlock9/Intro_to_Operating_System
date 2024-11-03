# pgroup-pair 11 : Lambda Expression
## Authors
Jinlock Choi  520370990012  
Jaeheung Park 518370990004 

## Lamda Expressions
A **lambda expression** in C++ is a convenient way to define an anonymous function, i.e., a function without a name, which can be used on the spot.  
Lambda expressions are primarily used for short, throwaway functions, often passed as arguments to higher-order functions.

### Basic Syntax
```js
[capture](parameters) -> return_type {
    // function body
}
```
- `capture`: Specifies which variables from the surrounding scope are captured and can be used in the lambda.
- `parameters`: Parameters the lambda takes, similar to a function's parameter list.
- `return_type` (optional): Specifies the return type of the lambda. If omitted, the return type is inferred.
- `{}`: The body of the lambda, where the logic is defined.

### Example 1: Basic Lambda
Simple lambda expression that adds two integers:
```js
auto add = [](int a, int b) -> int {
    return a + b;
};

int result = add(5, 3);  // result is 8
```
- The `[]` indicates the lambda does not capture any variables.
- The lambda is assigned to the add variable.

### Example 2: Capturing Variables
Lambda expressions can capture variables from their surrounding scope. There are different ways to capture variables:

1. By value ([`x`]): Captures variables by value, i.e., a copy of the variable.
2. By reference ([&]): Captures variables by reference, allowing modification.
3. Explicit capture: You can specify exactly which variables to capture and how.

```js
int x = 5;
int y = 3;

// Capture by value
auto lambda1 = [x, y]() {
    return x + y;
};

// Capture by reference
auto lambda2 = [&x, &y]() {
    x = 10;
    return x + y;
};

std::cout << lambda1() << std::endl;  // Output: 8
std::cout << lambda2() << std::endl;  // Output: 13
std::cout << x << std::endl;          // Output: 10 (x was modified in lambda2)
```
- If you see lambda2, you can see the value of x is updated.

### Example 3: Capturing All Variables
You can capture all variables either by value or by reference:
```js
int a = 10;
int b = 20;

// Capture all by value
auto lambda1 = [=]() {
    return a + b;
};

// Capture all by reference
auto lambda2 = [&]() {
    a += b;
};
```

### Example 4: Lambdas with Standard Algorithms
Lambdas are often used with standard algorithms like `std::sort` or `std::for_each`:
```js
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> vec = {4, 2, 3, 1, 5};

    // Sort the vector in descending order using a lambda
    std::sort(vec.begin(), vec.end(), [](int a, int b) {
        return a > b;
    });

    // Print the sorted vector
    for (int v : vec) {
        std::cout << v << " ";  // Output: 5 4 3 2 1
    }
}
```