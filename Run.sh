gcc code/main.c -o exes/bsh -Wall -Wextra -Werror -fsanitize=address -g3 -fno-omit-frame-pointer
./exes/bsh examples/a.bsh
./main examples/a.bsh
echo "Returned" $?
