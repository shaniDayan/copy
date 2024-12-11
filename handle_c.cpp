#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlCHandler(int sig_num) {
    SmallShell &smash = SmallShell::getInstance();
    cout << "smash: got ctrl-C" << endl;  // Always print
    pid_t fgPid = smash.getForegroundPid();
    if (fgPid != -1) {
        if(kill(fgPid, SIGKILL) == -1) {
            perror("smash error: kill failed");
            return;
        }
        cout << "smash: process " << fgPid << " was killed" << endl;
        smash.setForegroundPid(-1);
    }
}

