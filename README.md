# BirdSharp

BirdSharp is a low-level programming language inspired by C, but unique in many aspects. It is based upon a design philosophy which focuses on a few things:

* **Customizability:** BirdSharp was made so that its compilers can be interchangeable.  
* **Low Level:** BirdSharp takes comfort in being low-level, although some safety features may be added for ease-of-use in the future.  
* **Easy Syntax:** Despite being low-level, BirdSharp's syntax is quite simple to learn, as seen in basic expressions like `deref(ptr)` and `cast(type, value)`.

---

## Core Language Features

### Types
- `int` – 32-bit signed integer  
- `long` – 64-bit signed integer  
- `char` – single byte character  
- `char*` – pointer to a character  
- `int*` – pointer to an integer  

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`  
- Comparisons: `==`, `!=`, `<`, `>`, `<=`, `>=`  
- Bitwise: `&`, `|`, `^`, `<<`, `>>`  
- Casts: `cast(type, expr)`  
- Pointer dereference: `deref(ptr)`  

### Control Flow
- `if (condition) { ... }`  
- `while (condition) { ... }`  
- `return value`  

### Functions
Declared similarly to C:

```c
int string_len(char *a){
    int i = 0
    while (deref(a + i) != cast(char, 0)) {
        i = i + 1
    }
    return i
}
```
---
## Examples
### Printing Command-line Arguments

```c
int main(long argc, char **argv){
    int i = 0
    while i < cast(int, argc) {
        print(deref(argv + i * 8))
        if ((i + 1) != cast(int, argc)) {
            print(" ")
        }
        i = i + 1
    }
    print("\n")
    return 0
}
```

### Reading a File

```c
int main(){
    int fd = syscall(0x2000005, "code/main.bsh", 0, 0);
    if (fd == -1){
        print("Error\n")
    }
    char *str = cast(char*, malloc(cast(long, 2000)));
    syscall(0x2000003, fd, str, 2000)
    print(str);
    syscall(0x2000006, fd);
    return 0
}
```
---

## Known Issues

* Portability: BirdSharp is only made for macOS, and although I want my programming language to reach a larger audience, I would need another computer or emulator for that.
* Safety: BirdSharp will have some safety features, but I haven't implemented them yet, as I have focused mainly on what makes it unique rather than fine polishing.
* Finishing Features: BirdSharp's interpreter is quite behind, and I would appreciate help on that.

## BirdSharp File Structure
This is a typical BirdSharp program structure
```
project/
│
├─ code/             # Source files
│   ├─ main.bsh
│
├─ build/            # Compiled outputs
│   └─ bs.out
│
└─ README.md         # Project documentation
```

## Running BirdSharp
1. Write your program in the code directory.
2. Run it (compile or interpret) with the BirdSharp compiler:
```sh
./build/bs.out code/main.bsh
```
