#!/usr/bin/bash
for i in 1 2 4 5 6 7 11 12 13 14 15 16 17 18 19 20 21  24 25 26 27 28 29 30 36 40 41 43 44 45 47 48 51 52 53 54 55 56 57 58 59 60 61 62 64 65 66 67 68 69 71 72 74
do
  echo "Test $i"
  ./hw5 < tests/tests_auto/test$i.in | lli > out 2> /dev/null
  diff out tests/tests_auto/test$i.myOut
done