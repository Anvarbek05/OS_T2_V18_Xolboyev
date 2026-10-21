# OS_T2_V18 — Katta sonlar yig'indisi (Variant 18)

## Kompilyatsiya va ishga tushirish
    gcc -Wall -Wextra -g -o main main.c
    ./main

## Valgrind
    valgrind --leak-check=full ./main 2> valgrind.txt
