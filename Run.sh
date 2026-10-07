gcc code/main.c -o exes/main -Wall -Wextra -Werror -fsanitize=address -g3 -fno-omit-frame-pointer
./exes/main examples/temp/a.lang
./main examples/temp/a.lang
echo "Returned" $?
