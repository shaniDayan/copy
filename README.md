g++ --std=c++11 -Wall -c Commands.cpp
g++ --std=c++11 -Wall -c signals.cpp
g++ --std=c++11 -Wall -c smash.cpp
g++ --std=c++11 -Wall Commands.o signals.o smash.o -o smash
Commands.o: In function `SmallShell::CreateCommand(char const*)':
Commands.cpp:(.text+0xe5c): undefined reference to `ShowPidCommand::ShowPidCommand(char const*)'
Commands.o: In function `SmallShell::executeCommand(char const*)':
Commands.cpp:(.text+0xf88): undefined reference to `typeinfo for Command'
Commands.o: In function `Command::Command(char const*)':
Commands.cpp:(.text._ZN7CommandC2EPKc[_ZN7CommandC5EPKc]+0xf): undefined reference to `vtable for Command'
Commands.o: In function `BuiltInCommand::~BuiltInCommand()':
Commands.cpp:(.text._ZN14BuiltInCommandD2Ev[_ZN14BuiltInCommandD5Ev]+0x22): undefined reference to `Command::~Command()'
Commands.o: In function `SmallShell::getInstance()':
Commands.cpp:(.text._ZN10SmallShell11getInstanceEv[_ZN10SmallShell11getInstanceEv]+0x5b): undefined reference to `SmallShell::~SmallShell()'
Commands.o:(.data.rel.ro._ZTI14BuiltInCommand[_ZTI14BuiltInCommand]+0x10): undefined reference to `typeinfo for Command'
collect2: error: ld returned 1 exit status
Makefile:21: recipe for target 'smash' failed
make: *** [smash] Error 1
