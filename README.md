# BirdSharp
** Keep in mind that BirdSharp isn't complete yet, and it still may have random Assembly errors that are unfixed. Please approach with caution.

*(Technically, I began working on a bytecode mode, but it can only support basic arithmetic & stuff, so you shouldn't use the bytecode mode)*

*(This expects a specific file structure, so you should probably just clone the repo.)*

## BirdSharp Examples

Hello World:

```birdsharp
int main(){
  print("Hello, World!\n")
  return 0
}
```

Writing to a File
```birdsharp
int main(){
    int fd = syscall(33554437, "epicfile.txt", 1537, 438)
    syscall(33554436, fd, "Hello, World\n", 13)
    syscall(33554438, fd)
}
```

## How to Compile
To compile, simply type in:
```console
$ ./exes/bs.out code/main.bs
$ ./res/main.out
```
