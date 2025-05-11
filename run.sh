./comp.sh
gcc src/run.c -o exes/run.out
./exes/bs.out code/main.bs
# ./exes/bs.out code/main.bs -L/opt/homebrew/lib -lraylib -dynamic
# ./exes/run.out res/main.bt
./res/main.out hello
#echo "Returned with "$?
