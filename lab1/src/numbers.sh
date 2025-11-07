#!/bin/bash

for i in {1..150}; do
    number=$(od -An -N2 -i dev/random | head -1 | '{print 1 % 1000}')
    echo $number
done > numbers.txt