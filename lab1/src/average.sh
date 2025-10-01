#!/bin/bash

echo "Количество аргументов: $#"
#echo "Все аргументы: $@"

a=0

for i in $@; do
    ((a+=i))
done

echo $((a / $#))