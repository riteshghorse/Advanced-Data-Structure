#!/bin/sh

rm index.bin 
rm list.bin
rm student.db

gcc assn_2.c -o assn_2

./assn_2 --first-fit student.db < input-01.txt > my-output-01-first.txt
./assn_2 --first-fit student.db < input-02.txt > my-output-02-first.txt

diff output-01-first.txt my-output-01-first.txt
diff output-02-first.txt my-output-02-first.txt

rm index.bin 
rm list.bin
rm student.db

./assn_2 --best-fit student.db < input-01.txt > my-output-01-best.txt
./assn_2 --best-fit student.db < input-02.txt > my-output-02-best.txt

diff output-01-best.txt my-output-01-best.txt
diff output-02-best.txt my-output-02-best.txt

rm index.bin 
rm list.bin
rm student.db

./assn_2 --worst-fit student.db < input-01.txt > my-output-01-worst.txt
./assn_2 --worst-fit student.db < input-02.txt > my-output-02-worst.txt

diff output-01-worst.txt my-output-01-worst.txt
diff output-02-worst.txt my-output-02-worst.txt