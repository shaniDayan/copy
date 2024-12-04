g++ --std=c++11 -Wall -c smash.cpp
g++ --std=c++11 -Wall Commands.o signals.o smash.o -o smash
./smash < test_input1.txt > test_output1.txt
diff test_output1.txt test_expected_output1.txt
1c1
< smash> smash> hello> hello> hello> hello> smash> smash> smash> smash: sending SIGKILL signal to 0 jobs: 
---
> smash> smash> hello> hello> hello> hello> smash> smash> smash> smash: sending SIGKILL signal to 0 jobs:
Makefile:16: recipe for target 'test_output1.txt' failed
make: *** [test_output1.txt] Error 
