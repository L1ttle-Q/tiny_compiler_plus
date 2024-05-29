# tiny_compiler_plus
**Based on tiny, several improvements have been applied**

## Highlights

### 1. Addition of floating point operations
### 2. Addition of XOR operation (useful for replacing boolean operations with integer operations in logical operations, which makes it logically complete)
### 3. Implementation of '>=' and '<=' in analysis and assembly code generation (already implemented in the virtual machine)
### 4. Direct use of integers in if condition judgments
### 5. Addition of type declarations, which can only be used after declaration. (But it is not actually implemented at the bottom layer, because strong typing requires additional type annotation for each address, which is a large workload)
### 6. Assign initial values when defining, and report errors for repeated definitions
### 7. Incorporate DAG optimization, merging the same variable components within the same expression.
![DAG](TC/syntax_tree.jpg)
### 8. Based on step 7, use temporary variables for storage to optimize the underlying code generation.
![DAG_tmpvar](TC/syntax_tree_1.jpg)
### 9. Stronger type checking at declaration (However, type checking can only be done on constants, not on variables in expressions, because types are not marked in the symbol table)
### 10. Add constant merging
![DAG_constmerge](TC/syntax_tree_2.jpg)

## Usage

**In the 'TC' folder**

```
make
```
windows
```
tiny x.tny
copy x.tm ../TM
```
mac/linux
```
./tiny x.tny
cp x.tm ../TM
```
This will generate the executable file for tiny, compile the .tny file to generate a .tm file and copy it to the /TM directory.

Also, you can use
```
dot -Tjpg syntax_tree.dot -o syntax_tree.jpg
```
to generate a visualization of the syntax tree.

**In the 'TM' folder**

```
g++ tm.c -o tm -w
```
windows
```
tm x.tm
```
mac/linux
```
./tm x.tm
```