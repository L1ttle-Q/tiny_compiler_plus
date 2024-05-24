# tiny_compiler_plus
**Based on tiny, several improvements have been applied**

## Highlights

### 1. Addition of floating point operations
### 2. Addition of XOR operation (useful for replacing boolean operations with integer operations in logical operations, which makes it logically complete)
### 3. Implementation of '>=' and '<=' in analysis and assembly code generation (already implemented in the virtual machine)
### 4. Direct use of integers in if condition judgments (actually, no extra implementation is needed, just remove the check for boolean types in syntax checking)
### 5. Addition of type declarations, which can only be used after declaration. (But it is not actually implemented at the bottom layer, because strong typing requires additional type annotation for each address, which is a large workload)
### 6. Assign initial values when defining, and report errors for repeated definitions
### 7. Incorporate DAG optimization, merging the same variable components within the same expression.