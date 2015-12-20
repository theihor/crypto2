#!/bin/bash
echo Compiling...
g++ rng.cpp

rm stats.txt
touch stats.txt
rm zip_stats.txt
touch zip_stats.txt

for i in 0 7 33 45 63 
do
    echo "i = $i"
    ./a.out e "x$i" 10240 $i >> "stats.txt"
    ./stats.py "x$i" >> "stats.txt"
    
    zip "x$i.zip" "x$i"
    wc -c "x$i" >> zip_stats.txt
    wc -c "x$i.zip" >> zip_stats.txt
    
    rm "x$i.zip" "x$i"
done

echo