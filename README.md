student@pc:~/Desktop/OS_WET1-main/skeleton_smash$ make
make: Warning: File 'Makefile' has modification time 130035 s in the future
make: Nothing to be done for 'test'.
make: warning:  Clock skew detected.  Your build may be incomplete.
student@pc:~/Desktop/OS_WET1-main/skeleton_smash$ make
make: Warning: File 'Makefile' has modification time 130031 s in the future
make: Nothing to be done for 'test'.
make: warning:  Clock skew detected.  Your build may be incomplete.
student@pc:~/Desktop/OS_WET1-main/skeleton_smash$ ls
cmake-build-debug  Commands.o   signals.h  smash.o
CMakeLists.txt     Makefile     signals.o  test_expected_output1.txt
Commands.cpp       README.txt   smash      test_input1.txt
Commands.h         signals.cpp  smash.cpp  test_output1.txt
student@pc:~/Desktop/OS_WET1-main/skeleton_smash$ ./smash
bash: ./smash: Permission denied
student@pc:~/Desktop/OS_WET1-main/skeleton_smash$ 
