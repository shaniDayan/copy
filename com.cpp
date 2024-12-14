#include "Commands.h"
#include <cctype>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <fcntl.h>
#include <sys/types.h>


struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen;
    char d_name[];
};

const std::string WHITESPACE = " \n\r\t\f\v";
#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

//HELP FUNCTIONS
string _ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string &s) {
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char *cmd_line, char **args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for (std::string s; iss >> s;) {
        args[i] = (char *) malloc(s.length() + 1);
        memset(args[i], 0, s.length() + 1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundComamnd(const char *cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char *cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    if (idx == string::npos) {
        return;
    }
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

//CHPROMPT
void Chprompt::execute() {
    SmallShell &smash = SmallShell::getInstance();
    if(newPrompt.empty()){
        smash.setPrompt("smash");
    }
    else{
        smash.setPrompt(newPrompt);
    }
}

Chprompt::Chprompt(const char *cmdLine) : BuiltInCommand(cmdLine) {
    string cmd = cmdLine;
    if (cmd.length() > 9){
        char *args[COMMAND_MAX_ARGS];
        _parseCommandLine(cmdLine,args);
        newPrompt=args[1];
    } else {
        newPrompt = ""; // No new name provided
    }
}

//SHOWPID
void ShowPidCommand::execute(){
    SmallShell &smash = SmallShell::getInstance();
    cout << "smash pid is " << smash.getPID() <<endl;
}

//PWD
void GetCurrDirCommand::execute() {
    char cwd[COMMAND_MAX_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        string path = string(cwd);
        std::cout << path << std::endl;
    } else {
        perror("smash error: getcwd failed");
    }
}

//CD
void ChangeDirCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    char *cmd = const_cast<char *>(getCmd_line());
    char *args[COMMAND_MAX_ARGS];
    int numArgs = _parseCommandLine(cmd, args);

    if (numArgs > 2) {
        cerr << "smash error: cd: too many arguments" << endl;
        return;
    }
    char currentDir[COMMAND_MAX_LENGTH];
    if (getcwd(currentDir, COMMAND_MAX_LENGTH) == nullptr) {
        perror( "smash error: getcwd failed" );
        return;
    }
    if (numArgs == 1) {
        return;
    }
    if (strcmp(args[1], "-") == 0) {
        const char* prevDir = smash.getDir();
        if (prevDir == nullptr) {
            cerr << "smash error: cd: OLDPWD not set" << endl;
            return;
        }
        if (chdir(prevDir) == -1) {
            perror("smash error: chdir failed" );
            return;
        }
        char currentDir[COMMAND_MAX_LENGTH];
        if (getcwd(currentDir, COMMAND_MAX_LENGTH) == nullptr) {
            perror( "smash error: getcwd failed" );
            return;
        }
        smash.setlastDir(currentDir);

    }
    else if (strcmp(args[1], "..") == 0) {
        if (chdir("..") == -1) {
            perror( "smash error: chdir failed" );
            return;
        }
        smash.setlastDir(currentDir);
    }
    else {
        const char* path = args[1];
        smash.setlastDir(currentDir);
        if (chdir(path) == -1) {
            perror( "smash error: chdir failed" );
            return;
        }

    }
}

//SMALL_SHELL
Command *SmallShell::CreateCommand(const char *cmd_line, bool toFork) {
    if (cmd_line == nullptr || strlen(cmd_line) == 0) {
        return nullptr;
    }
    char *cmd = const_cast<char *>(cmd_line);

    char *cmdCopy = strdup(cmd_line);
    _removeBackgroundSign(cmd);
    string cmd_s = _trim(string(cmd));


    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    if (jobs) {
        jobs->removeFinishedJobs();
    }
    if (checkIfIoCommand(cmd_line)){
        return new RedirectionCommand(cmd_line);
    }
    else if(strchr(cmdCopy, '|')){
        return new PipeCommand(cmd_line);
    }
    else if (firstWord == aliases["chprompt"]){
        return new Chprompt(cmd_line);
    }
    else if (firstWord == aliases["pwd"]) {
        return new GetCurrDirCommand(cmd_line);
    }
    else if (firstWord == aliases["showpid"]) {
        return new ShowPidCommand(cmd_line);
    }
    else if (firstWord == aliases["cd"]) {
        return new ChangeDirCommand( cmd_line, getlastDir());
    }
    else if (firstWord == aliases["alias"]) {
        return new aliasCommand( cmd_line);
    }
    else if (firstWord == aliases["unalias"]) {
        return new unaliasCommand( cmd_line);
    }
    else if(firstWord == aliases["jobs"]){
        return new JobsCommand(cmd_line,jobs);
    }
    else if(firstWord == aliases["fg"]){
        return new ForegroundCommand(cmd_line,jobs);
    }
    else if(firstWord == aliases["quit"]){
        return new QuitCommand(cmd_line,jobs);
    }
    else if(firstWord == aliases["kill"]){
        return new KillCommand(cmd_line,jobs);
    }
    else if(firstWord== aliases["listdir"]){
        return new ListDirCommand(cmd_line);
    }
    else if(firstWord== aliases["whoami"]){
        return new WhoAmICommand(cmd_line);
    }
    else {
        return new ExternalCommand(cmdCopy, toFork);
    }
}

void SmallShell::setNewCommandName(const string& oldCommand, const string& newName){
    aliases[oldCommand] = newName ;
}

bool SmallShell::findReservedCommand(const string& command) const{
    if(find(reservedCommands.begin(), reservedCommands.end(),command)==reservedCommands.end()){
        return false;
    }
    return true;
}

void SmallShell::executeCommand(const char *cmd_line, bool toFork) {
    Command *cmd = CreateCommand(cmd_line , toFork);
    if(cmd == nullptr){
        return;
    }
    cmd->execute();
    delete cmd;
}

void SmallShell::printAllCurrCommand() const{
    for(const auto& command:aliases){
        cout<<command.second<<"='"<<command.first<<"'"<<endl;
    }
}

//ALIAS
void aliasCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    char *cmd = const_cast<char *>(getCmd_line());
    char *args[COMMAND_MAX_ARGS];
    int numArgs=_parseCommandLine(cmd,args);
    if (numArgs == 1) {
        smash.printAllCurrCommand();
        return;
    }
    if(!fixInput(numArgs, args)){
        return;
    }

    if(!smash.findReservedCommand(alias_name)){
        smash.aliases[alias_name]=alias_cmd;
    }
    else{
        cerr << "smash error: alias: "<<alias_cmd<< " already exists or is a reserved command" << endl;
        return;
    }
    smash.setNewCommandName(alias_cmd,alias_name);
}

bool aliasCommand::fixInput(int numArgs, char **args){
    std::string full_alias;
    for (int i = 1; i < numArgs; i++) {
        if (i > 1) full_alias += " ";
        full_alias += args[i];
    }

    size_t equal_pos = full_alias.find('=');
    if (equal_pos == std::string::npos || equal_pos == 0 || equal_pos == full_alias.length() - 1) {
        cerr << "smash error: alias: invalid alias format" << endl;
        return false;
    }

    alias_name = full_alias.substr(0, equal_pos);
    alias_cmd = full_alias.substr(equal_pos + 1);

    // Remove surrounding quotes if present
    if (!alias_cmd.empty() && alias_cmd.front() == '\'' && alias_cmd.back() == '\'') {
        alias_cmd = alias_cmd.substr(1, alias_cmd.length() - 2);
    }

    // Validate alias name (consists of alphanumeric and underscores)
    if (!regex_match(alias_name, std::regex("^[a-zA-Z0-9_]+$"))) {
        cerr << "smash error: alias: invalid alias format" << endl;
        return false;
    }

    return true;
}

//UNALIAS
void unaliasCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    char *cmd = const_cast<char *>(getCmd_line());
    char *args[COMMAND_MAX_ARGS];
    int num = _parseCommandLine(cmd, args);
    if (num < 2) {
        cerr << "smash error: unalias: not enough arguments" << endl;
        return;
    }
    for (int i = 1; i < num; ++i) {
        auto it = smash.findValueInMap(args[i]);
        if (it != smash.aliases.end()) {
            it->second = it->first;
        } else {
            cerr << "smash error: unalias: " << args[i] << " alias does not exist" << endl;
            return;
        }
    }
}

//EXTERNAL
void ExternalCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    const char *cmd_line = getCmd_line();
    char *cmdCopy = strdup(cmd_line);
    char *args[COMMAND_MAX_ARGS];
    bool isBackground = _isBackgroundComamnd(cmdCopy);
    _removeBackgroundSign(const_cast<char *>(cmdCopy));
    int numArgs=_parseCommandLine(cmdCopy, args);
    string commandName(args[0]);  // Get the first argument (command name)
    if (smash.findValueInMap(commandName) != smash.aliases.end()) {
        string aliasCommand = smash.returnValueInMap(commandName);
        string expandedCommand = aliasCommand;
        for (int i = 1; i < numArgs; ++i) {
            expandedCommand += " ";
            expandedCommand += args[i];
        }
        strcpy(cmdCopy, expandedCommand.c_str());
        numArgs = _parseCommandLine(cmdCopy, args);
    }
    if (shouldForkFlag) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            setpgrp();
            if (isComplexCommand(args)) {
                char *bashArgs[] = { (char *)"/bin/bash", (char *)"-c", (char *)cmdCopy, nullptr };
               if(execv("/bin/bash", bashArgs)==-1) {
                   perror("smash error: execv failed");
                   exit(1);
               }

            } else {
                if (strcmp(args[0], "rm") == 0) {
                    // Add the "-f" flag to the rm command
                    char *rmArgs[COMMAND_MAX_ARGS];
                    rmArgs[0] = args[0]; // "rm"
                    rmArgs[1] = (char *)"-f"; // "-f" flag
                    int i = 1;
                    for (; args[i] != nullptr; i++) {
                        rmArgs[i + 1] = args[i]; // Shift other arguments
                    }
                    rmArgs[i + 1] = nullptr; // Null-terminate the new args array
                    if(execvp(rmArgs[0], rmArgs)==-1){
                        // If execvp fails
                        perror("smash error: execvp failed");
                        exit(1);
                    }
                } else {
                    cout<<"hii 6"<<endl;
                    if(execvp(args[0], args)==-1){
                        // If execvp fails
                        perror("smash error: execvp failed");
                        exit(1);
                    }

                }
            }
        } else if (pid > 0) { // Parent process
            smash.setForegroundPid(pid);
            if (isBackground) {
                smash.getJobList()->addJob(cmd_line, pid, true);
            } else {
                smash.getJobList()->removeFinishedJobs();
                cout<<"hii 7"<<endl;
                if(waitpid(pid, nullptr, 0)==-1){

                     perror("smash error: execvp failed");
                    return;
                }
            }
        } else {
            perror("smash error: fork failed");
        }
    } else { // Direct Execution without forking
        // In the context of PipeCommand, this block runs in a child process already
        setpgrp();
        if (isComplexCommand(args)) {
            char *bashArgs[] = { (char *)"/bin/bash", (char *)"-c", (char *)cmdCopy, nullptr };
            cout<<"hii 8"<<endl;
            if(execv("/bin/bash", bashArgs)==-1){
                // If execv fails
                perror("smash error: execv failed");
                exit(1);
            }

        } else {
            if (strcmp(args[0], "rm") == 0) {
                // Add the "-f" flag to the rm command
                char *rmArgs[COMMAND_MAX_ARGS];
                rmArgs[0] = args[0]; // "rm"
                rmArgs[1] = (char *)"-f"; // "-f" flag
                int i = 1;
                for (; args[i] != nullptr; i++) {
                    rmArgs[i + 1] = args[i]; // Shift other arguments
                }
                rmArgs[i + 1] = nullptr; // Null-terminate the new args array
                cout<<"hii 9"<<endl;
                if(execvp(rmArgs[0], rmArgs)==-1){
                    // If execvp fails

                    perror("smash error: execvp failed");
                    exit(1);
                }

            } else {
                cout<<"hii 10"<<endl;
                if(execvp(args[0], args)==-1){
                    // If execvp fails

                    perror("smash error: execvp failed");
                    exit(1);
                }
            }
        }
    }


// Cleanup memory after execution
    for (int i = 0; i < numArgs; ++i) {
        free(args[i]);
    }
    free(cmdCopy);


}

bool ExternalCommand::isComplexCommand(char **args) {
    for (int i = 0; args[i] != nullptr; ++i) {
        string arg(args[i]);
        if (arg.find('*') != string::npos || arg.find('?') != string::npos) {
            return true;
        }
    }
    return false;
}

//JOBS
void JobsCommand::execute() {
    jobs->removeFinishedJobs();
    jobs->printJobsList();
}
void ForegroundCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    const char *cmd_line = getCmd_line();
    char *args[COMMAND_MAX_ARGS];
    int n = _parseCommandLine(cmd_line, args);

    // Handle "fg" with no arguments and an empty jobs list
    if (n == 1 && smash.getJobList()->jobList.empty()) {
        cerr << "smash error: fg: jobs list is empty" << endl;
        // Free args before returning
        for (int i = 0; i < n; i++) {
            free(args[i]);
        }
        return;
    }

    // Validate arguments: "fg" must not have more than one optional argument
    // and if an argument is given, it must be a positive integer.
    if (n > 2 || strcmp(args[0], "fg") != 0 ||
        (n > 1 && !std::all_of(args[1], args[1] + strlen(args[1]), ::isdigit))) {
        cerr << "smash error: fg: invalid arguments" << endl;
        // Free args before returning
        for (int i = 0; i < n; i++) {
            free(args[i]);
        }
        return;
    }
    JobsList::JobEntry* job = nullptr;
    int jobId = 0;

    if (n > 1) {
        // If a job-id argument is provided, try to find that specific job
        jobId = atoi(args[1]);
        job = smash.getJobList()->getJobById(jobId);
        if (!job) {
            cerr << "smash error: fg: job-id " << args[1] << " does not exist" << endl;
            // Free args before returning
            for (int i = 0; i < n; i++) {
                free(args[i]);
            }
            return;
        }
    } else {
        // No job-id provided, bring the job with the maximal job id to the foreground
        job = smash.getJobList()->getLastJob(&jobId);
        if (!job) {
            // If no jobs are available
            cerr << "smash error: fg: jobs list is empty" << endl;
            // Free args before returning
            for (int i = 0; i < n; i++) {
                free(args[i]);
            }
            return;
        }
    }

    // Print the command line and pid before waiting, as required
    cout << job->getCmdInJob() <<" "<< job->pid << endl;

    smash.setForegroundPid(job->pid);

    // Wait for the job to finish
    if (waitpid(job->pid, nullptr, WUNTRACED) == -1) {
        perror("smash error: waitpid failed");
        smash.setForegroundPid(-1);
        // Free args before returning
        for (int i = 0; i < n; i++) {
            free(args[i]);
        }
        return;
    }

    smash.setForegroundPid(-1);

    // Remove the job from the jobs list after it finishes
    smash.getJobList()->removeJobById(jobId);

    // Free args array memory
    for (int i = 0; i < n; i++) {
        free(args[i]);
    }
}


//QUIT
void QuitCommand::execute(){
    SmallShell &smash = SmallShell::getInstance();
    const char *cmd_line = getCmd_line();
    char *args[COMMAND_MAX_ARGS];
    int n = _parseCommandLine(cmd_line, args);
    // only quit
    if(n==1){
        exit(0);
    } else{
        JobsList* jobs = smash.getJobList();
        jobs->removeFinishedJobs();
        int num= jobs->jobList.size();
        cout<<"smash: sending SIGKILL signal to "<< num << " jobs:"<<endl;
        for(auto &job: jobs->jobList){
            if (kill(job.second->pid, SIGKILL) == -1) {
                perror("smash error: kill failed");
                return;
            }
            cout<< job.second->pid << ": "<< job.second->getCmdInJob() << endl;
        }
        exit(0);
    }
}

//KILL
void KillCommand::execute(){
    SmallShell &smash = SmallShell::getInstance();
    const char *cmd_line = getCmd_line();
    char *args[COMMAND_MAX_ARGS];
    int n = _parseCommandLine(cmd_line, args);

    if (n != 3 || strcmp(args[0], "kill") != 0) {
        cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }
    string signalStr = args[1];
    if (signalStr[0] != '-' || !std::all_of(signalStr.begin() + 1, signalStr.end(), ::isdigit)) {
        cerr << "smash error: kill: invalid arguments"  << std::endl;
        return;
    }

    string pidStr = args[2];
    if (!std::all_of(pidStr.begin(), pidStr.end(), ::isdigit)) {
        cerr << "smash error: kill: invalid arguments"  << std::endl;
        return;
    }

    if(smash.getJobList()->getJobById(atoi(args[2])) == nullptr){
        cerr<<"smash error: kill: job-id "<< args[2] <<" does not exist"<<endl;
        return;
    }
    pid_t pid =smash.getJobList()->getJobById(atoi(args[2]))->pid;
    int signalNum=abs(atoi(args[1]));
    if(signalNum<1||signalNum>31){
        cerr << "smash error: kill failed: Invalid argument"  << std::endl;
        return;
    }

    if (kill(pid, signalNum) == -1) {
        perror("smash error: kill failed");
        return;
    } else {
        cout << "signal number " << signalNum << " was sent to pid " << pid << endl;
        // If the signal is a terminating signal (like SIGKILL), remove the job from the list:
        if(signalNum == SIGKILL || signalNum == 9) {
            jobs->removeJobById(atoi(args[2]));
        }
    }



}

//listdir command
void ListDirCommand::execute(){
    const char *cmd_line = getCmd_line();
    char *args[COMMAND_MAX_ARGS];
    int n = _parseCommandLine(cmd_line, args);
    if(n>2){
        cerr<<"smash error: listdir: too many arguments "<<endl;
        return;
    }
    string dirTraverse;
    if(n==1){
        char currentDir[COMMAND_MAX_LENGTH];
        if (getcwd(currentDir, COMMAND_MAX_LENGTH) == nullptr) {
            perror( "smash error: getcwd failed" );
            return;
        }
        dirTraverse=currentDir;
    } else{
        dirTraverse= args[1];
    }
    if(!isdir(dirTraverse)){
        perror("smash error: S_ISDIR failed");
        return;
    }
    set<string> visitedDirs;
    Traversing(dirTraverse,0, visitedDirs);
}

bool ListDirCommand::isdir(const string& path) {
    struct stat pathStat;
    return (stat(path.c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode));
}

void ListDirCommand::Traversing(const string& path, int indent, set<string>& visitedDirs) {
    if (visitedDirs.find(path) != visitedDirs.end()) {
        return;
    }
    visitedDirs.insert(path);

    int dirFd = open(path.c_str(), O_RDONLY | O_DIRECTORY);
    if (dirFd == -1) {
        perror("smash error: open failed");
        return;
    }
    struct linux_dirent* entry;
    char buffer[1024];
    ssize_t bytesRead;

    vector<string> Directory;
    vector<string> file;

    while ((bytesRead = syscall(SYS_getdents, dirFd, buffer, sizeof(buffer))) > 0) {
        ssize_t offset = 0;

        while (offset < bytesRead) {
            entry = (struct linux_dirent*)(buffer + offset);  // Cast buffer to linux_dirent*
            string entryName = entry->d_name;

            if (entryName == "." || entryName == "..") {
                offset += entry->d_reclen;
                continue;
            }
            string fullPath = path + "/" + entryName;
            if (isdir(fullPath)) {
                Directory.push_back(entryName);
            } else {
                file.push_back(entryName);
            }
            offset += entry->d_reclen;
        }
    }

    if (bytesRead == -1) {
        perror("smash error: read failed");
    }

    close(dirFd);

    sort(Directory.begin(), Directory.end());
    sort(file.begin(), file.end());

    for (const auto& dirname : Directory) {
        cout << string(indent, '\t') << dirname << endl;
        Traversing(path + '/' + dirname, indent + 1, visitedDirs);
    }
    for (const auto& filename : file) {
        cout << string(indent, '\t') << filename << endl;
    }
}


void RedirectionCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    const char *cmd_line = getCmd_line();

    const char *redirection = nullptr;
    bool appendMode = false;
    size_t redirectionPos = string::npos;

    if ((redirection = strstr(cmd_line, ">>")) != nullptr) {
        appendMode = true;
        redirectionPos = redirection - cmd_line;
    } else if ((redirection = strstr(cmd_line, ">")) != nullptr) {
        redirectionPos = redirection - cmd_line;
    }

    if (redirectionPos != string::npos) {
        string cmdStr(cmd_line, redirectionPos);
        string trimmedCmdStr = _trim(cmdStr);

        string fileName;
        if (appendMode) {
            fileName = _trim(string(redirection + 2));
        } else {
            fileName = _trim(string(redirection + 1));
        }

        if (fileName.empty()) {
            cerr << "smash error: missing file for redirection" << endl;
            return;
        }

        int fd;
        if (appendMode) {
            fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        if (fd <0) {
            perror( "smash error: open failed");
            return;
        }

        int stdoutCopy = dup(STDOUT_FILENO);
        if (stdoutCopy == -1 || dup2(fd, STDOUT_FILENO) == -1) {
            perror("smash error: dup2 failed" );
            close(fd);
            return;
        }

        close(fd);

        smash.executeCommand(trimmedCmdStr.c_str(),true);

        if (dup2(stdoutCopy, STDOUT_FILENO) == -1) {
            perror( "smash error: dup2 failed" );
        }
        close(stdoutCopy);
    } else {
        SmallShell::getInstance().executeCommand(cmd_line,true);
    }
}

bool SmallShell::checkIfIoCommand(const char* cmd_line) {
    if (strstr(cmd_line, ">") != nullptr || strstr(cmd_line, ">>") != nullptr ||
        strstr(cmd_line, "<") != nullptr || strstr(cmd_line, "<<") != nullptr) {
        return true;
    }
    return false;

}

//who I am command
void WhoAmICommand::execute(){
    uid_t userId= getuid();
    int fd = open("/etc/passwd", O_RDONLY);
    if(fd==-1){
        perror( "smash error: open failed" );
        return;
    }
    char buff[1024];
    ssize_t numRead;
    string line;

    while((numRead =read(fd, buff, sizeof(buff)-1)) > 0){
        buff[numRead]= '\0';
        line += buff;
        size_t position=0;
        while((position=line.find('\n')) != string::npos){
            string userline = line.substr(0, position);
            line.erase(0,position+1);
            ssize_t first = userline.find(':');
            size_t second = userline.find(':', first + 1);
            size_t third = userline.find(':', second + 1);
            if(third != string::npos){
                string uid_to_cmp= userline.substr(second+1,third-second-1);
                uid_t uid = stoi(uid_to_cmp);
                if(uid==userId){
                    ssize_t last=userline.find_last_of(':');
                    string sub = userline.substr(third+1,last-(third+1));
                    ssize_t final_last= sub.find_last_of(':');
                    string homedir= sub.substr(final_last+1);
                    string username = userline.substr(0,first);
                    cout<<username<<' '<<homedir<<endl;
                    close(fd);
                    return;
                }
            }
        }
    }
}

void PipeCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    int my_pipe[2];
    const char *cmd_line = getCmd_line();
    char *cmd_line_copy = strdup(cmd_line);

    char *command1 = nullptr;
    char *command2 = nullptr;
    bool STDERR = false;

    // Parse commands and check for pipe type
    char *args[COMMAND_MAX_ARGS];
    int n = _parseCommandLine(cmd_line, args);

    for (int i = 0; i < n; i++) {
        if (strcmp(args[i], "|") == 0) {
            command1 = strtok(cmd_line_copy, "|");
            command2 = strtok(nullptr, "|");
        } else if (strcmp(args[i], "|&") == 0) {
            command1 = strtok(cmd_line_copy, "|&");
            command2 = strtok(nullptr, "|&");
            STDERR = true;
        }
    }

    if (!command1 || !command2) {
        cerr << "smash error: invalid command format" << endl;
        free(cmd_line_copy);
        return;
    }

    // Trim commands
    string command1Tmp = _trim(command1);
    string command2Tmp = _trim(command2);
    command1 = strdup(command1Tmp.c_str());
    command2 = strdup(command2Tmp.c_str());

    if (pipe(my_pipe) == -1) {
        perror("smash error: pipe failed");
        free(cmd_line_copy);
        free(command1);
        free(command2);
        return;
    }

    // Fork for the first command
    pid_t pid1 = fork();
    if(pid1 == -1){
        perror("smash error: fork failed");
        free(cmd_line_copy);
        free(command1);
        free(command2);
        return;
    }
    if (pid1 == 0) { // Child process 1
        close(my_pipe[0]); // Close unused read end
        if (dup2(my_pipe[1], STDERR ? STDERR_FILENO : STDOUT_FILENO) == -1) {
            perror("smash error: dup2 failed");
            _exit(1); // Proper termination
        }
        close(my_pipe[1]); // Close write end after duplication


        smash.executeCommand(cmd_line,false);
        _exit(0); // Ensure child process exits after execution
    }

    // Fork for the second command
    pid_t pid2 = fork();
    if(pid2 == -1){
        perror("smash error: fork failed");
        free(cmd_line_copy);
        free(command1);
        free(command2);
        return;
    }
    if (pid2 == 0) { // Child process 2
        close(my_pipe[1]); // Close unused write end
        if (dup2(my_pipe[0], STDIN_FILENO) == -1) {
            perror("smash error: dup2 failed");
            _exit(1); // Proper termination
        }
        close(my_pipe[0]); // Close read end after duplication

        smash.executeCommand(cmd_line,false);
        _exit(0); // Ensure child process exits after execution
    }

    // Parent process closes both ends of the pipe
    if(close(my_pipe[0]) == -1 || close(my_pipe[1]) == -1){
        perror("smash error: close failed");
        // Continue to wait for child processes even if close fails
    }

    // Wait for both child processes
    if (waitpid(pid1, nullptr, 0) == -1) {
        perror("smash error: waitpid failed for pid1");
    }
    if (waitpid(pid2, nullptr, 0) == -1) {
        perror("smash error: waitpid failed for pid2");
    }

    free(cmd_line_copy);
    free(command1);
    free(command2);
}
