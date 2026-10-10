#!/bin/zsh

GREEN='\033[0;032m'
RED='\033[031m'
CLEAR='\033[0m'

for file in tests/inputs/*; do
    base_name=$(basename "$file" .bsh)
    echo "[INFO] Testing Example \""$base_name"\""

    ./exes/bsh $file
    if [ $? -ne 0 ]; then
        echo -e "${RED}[ERROR] Compiler crashed${CLEAR}"
        continue
    fi
    output=$(./main 2>&1)

    expected=$(cat tests/expected/"$base_name".txt)
    if [[ "$output" == "$expected" ]]; then
        echo "\t"$GREEN"Success"$CLEAR
    else
        echo $RED"[ERROR] Unexpected output"$CLEAR
        echo "\tExpected: "$expected
        echo "\tRecieved: "$output
    fi
    rm main
done
