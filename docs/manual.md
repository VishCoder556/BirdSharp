# BirdSharp Language Manual

BirdSharp is a relatively simple programming language. Its main design philosophy is to let the programmer code how they want. Unlike other programming languages, like Rust or Python, BirdSharp can be modified and changed to fit whatever style the programmer likes.

This is a manual on the features of BirdSharp.

## 1. Basic Syntax

A lot of BirdSharp's syntax is inherited from C.

```c
int main(){
    int x = 10
    int y = 20
    return x + y
}
```

Notice how in this example, there are no semicolons at the ends of a line. Yes, this is on purpose. If you want, you can add semicolons as well; the tokenizer will just ignore them.

### Comments

The only comment type supported so far is //, which, just like C, is a single-line comment.

### Variables and Literals

For the most part, this is basically the same as C. I mean, there's variables, functions, structs, and I guess types. Strings can just be "like this", and there are integer literals like 5 and 10. There's also floating point literals like 50.0, but MAKE SURE there is no f at the end, because BirdSharp, unlike C, determines floats just from the floating point, not from any identifiers at the end.

## 2. The Type System

The type system isn't exactly the same as C, and is more minimal.

There's int (which is a 64-BIT integer). This is not the same as a C int.

The real equivalent to a C int is actually i32, a 32-bit integer. There's also i64, which is the same as int really, but is used if you want to emphasize the integer size really.

Char, float, and void are all there as well. Unsigned is there, but it doesn't really do anything.

BirdSharp also comes with a string type, that represents a char* in C.

If you don't want to use the string type, you can also define a string as a pointer to a character, like the C syntax. Unlike C, pointers are ptr<type>, so an int* would be ptr<int>.
You can nest pointers inside of pointers, like ptr<ptr<int>> which is an int**. Pointers can be dereferenced with deref(a) and referenced with ref(a)

Arrays are a little different than in C.

```c
int[5] numbers
```

Notice how the length specifier ([5]) is actually after the type? Compare this to the C equivalent:

```c
int numbers[5]
```

BirdSharp's unique array syntax makes it much easier for the parser.

### Structures

Structures are similar to C as well, except for typedef struct, which isn't allowed;

```c
struct Point{
    int x
    int y
}
```
You can access structure instances as follows:

```c
struct Point p
p.x = 10
p.y = 20
```
