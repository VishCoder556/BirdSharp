gcc code/main.c -o exes/main -Wall -Wextra -Werror
./exes/main examples/a.lang
./res/main.out examples/a.lang
echo "Returned" $?
