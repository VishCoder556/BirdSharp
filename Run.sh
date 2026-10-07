gcc code/main.c -o exes/bsh -Wall -Wextra -Werror -fsanitize=address -g3 -fno-omit-frame-pointer
./exes/bsh examples/temp/a.bsh
./main examples/temp/a.bsh
echo "Returned" $?
