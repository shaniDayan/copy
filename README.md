this is the output: smash> smash> smash> smash> smash> smash> smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
smash> smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
[10] sleep 1000&
smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
[10] sleep 1000&
smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
[10] sleep 1000&
[11] sleep 1001&
smash> smash: got ctrl-C
smash: process 2 was killed
smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
[10] sleep 1000&
smash> 
and this is the expexted output:
smash> smash> smash> smash> smash> smash> smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
smash> smash> smash> [1] sleep 100&
[2] sleep 200&
[3] sleep 300&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
smash> signal number 9 was sent to pid 2
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
smash> smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
[10] sleep 1000&
smash> signal number 9 was sent to pid 3
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
smash> smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
[10] sleep 1001&
smash> sleep 1001& 4
smash: got ctrl-C
smash: process 4 was killed
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
[9] sleep 900&
smash> sleep 900& 5
smash: got ctrl-C
smash: process 5 was killed
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
[8] sleep 800&
smash> sleep 800& 6
smash: got ctrl-C
smash: process 6 was killed
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
[7] sleep 700&
smash> sleep 700& 7
smash: got ctrl-C
smash: process 7 was killed
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> smash: got ctrl-C
[1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> smash: got ctrl-C
smash: process 8 was killed
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> smash: got ctrl-C
smash: process 9 was killed
smash> smash> smash: got ctrl-C
smash: got ctrl-C
smash: got ctrl-C
smash: got ctrl-C
[1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> smash: got ctrl-C
smash: process 10 was killed
smash> smash: got ctrl-C
smash: process 11 was killed
smash> smash: got ctrl-C
smash: process 12 was killed
smash> smash> smash: got ctrl-C
smash> smash: got ctrl-C
smash> smash: got ctrl-C
[1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> smash> smash: got ctrl-C
smash> smash: got ctrl-C
smash> smash: got ctrl-C
smash> smash: got ctrl-C
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> [1] sleep 100&
[2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> signal number 9 was sent to pid 13
smash> smash> [2] sleep 200&
[4] sleep 400&
[5] sleep 500&
[6] sleep 600&
smash> signal number 9 was sent to pid 14
smash> signal number 9 was sent to pid 15
smash> smash> smash: got ctrl-C
smash> smash: got ctrl-C
[5] sleep 500&
[6] sleep 600&
smash> smash> [5] sleep 500&
[6] sleep 600&
[7] sleep 3&
smash> [5] sleep 500&
[6] sleep 600&
smash> [5] sleep 500&
[6] sleep 600&
smash> [5] sleep 500&
[6] sleep 600&
smash> [5] sleep 500&
[6] sleep 600&
smash> sleep 600& 16
smash: got ctrl-C
smash: process 16 was killed
smash> [5] sleep 500&
smash> sleep 500& 17
smash: got ctrl-C
smash: process 17 was killed
smash> smash> smash> smash: got ctrl-C
smash> smash> [1] sleep 777&
smash> smash: got ctrl-C
smash: process 18 was killed
smash> smash: got ctrl-C
smash: process 19 was killed
smash> smash> [1] sleep 777&
[2] sleep 100&
smash> sleep 100& 20
smash: got ctrl-C
smash: process 20 was killed
smash> smash: sending SIGKILL signal to 1 jobs:
21: sleep 777&
and the error that is showed:
smash error: kill failed: Success
smash error: kill failed: Invalid argument
runner: Timeout, killing child
