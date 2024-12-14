#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <unordered_map>
#include <regex>
#include <set>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
#define COMMAND_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
     const char *cmd_line;
public:

    explicit Command(const char *cmd_line) {
        this->cmd_line = strdup(cmd_line);
    }


    virtual void execute() = 0;
    virtual ~Command() {
        free((void*) this->cmd_line);
    }


    const char * getCmd_line() const{
        return cmd_line;
    }
    //virtual void prepare();
    //virtual void cleanup();
};

class BuiltInCommand : public Command {
public:
    explicit BuiltInCommand(const char *cmd_line): Command(cmd_line){}

    virtual ~BuiltInCommand() =default;
};

class ExternalCommand : public Command {
   bool shouldForkFlag;
public:

    ExternalCommand(const char *cmd_line, bool shouldForkFlag = true): Command(cmd_line),shouldForkFlag(shouldForkFlag){}
    static bool isComplexCommand(char **args);
    virtual ~ExternalCommand() {
    }

    void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char *cmd_line): Command(cmd_line) {}

    virtual ~PipeCommand() {
    }

    void execute() override;
};

class RedirectionCommand : public Command {

public:
    explicit RedirectionCommand(const char *cmd_line): Command(cmd_line){}

    virtual ~RedirectionCommand() {
    }
    void execute() override;


};

class Chprompt : public BuiltInCommand {
    string newPrompt;
public:
    explicit Chprompt(const char *cmdLine);

    virtual ~Chprompt(){
    }
    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
    public:
    ChangeDirCommand(const char *cmd_line, char **plastPwd) : BuiltInCommand(cmd_line) {
    }

    virtual ~ChangeDirCommand() {
    }

    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {

public:
    explicit GetCurrDirCommand(const char *cmd_line): BuiltInCommand(cmd_line){}
    virtual ~GetCurrDirCommand() {}

    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    explicit ShowPidCommand(const char *cmd_line): BuiltInCommand(cmd_line){}

    virtual ~ShowPidCommand() {
    }

    void execute() override;
};

class JobsList;

class QuitCommand : public BuiltInCommand {
    JobsList *jobs;
public:
    QuitCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line),jobs(jobs){}

    virtual ~QuitCommand() {
    }

    void execute() override;
};

class JobsList {
public:
    class JobEntry {
    public:
        int jobId;
        pid_t pid;
        string command;
        bool isStopped;

        JobEntry(int id, pid_t p, const string cmd, bool stopped)
                : jobId(id), pid(p), command(cmd), isStopped(stopped) {}


        string getCmdInJob()const {
            return command;
        }

         ~JobEntry();
    };

    map<int, JobEntry*> jobList;
    int nextJobId;
public:
    JobsList() : nextJobId(1) {}

    ~JobsList() {
        for (auto &job: jobList) {
            delete job.second;
        }
        jobList.clear();
    }

    void addJob(const string cmd, pid_t p,bool isStopped = false) {
        jobList[nextJobId] = new JobEntry(nextJobId, p, cmd, isStopped);
        nextJobId++;
    }

    void printJobsList() {
        for (const auto &job: jobList) {
            cout << "[" << job.first << "] " << job.second->getCmdInJob()<< endl;
        }
    }

    void killAllJobs() {
        for (auto &it: jobList) {
            //int jobId = it.first;
            pid_t pid = it.second->pid;
            if (kill(pid, SIGKILL) == -1) {
                cerr<<"kill failed" << endl;
            }
        }
        for (auto &it: jobList) {
            delete it.second;
        }
        jobList.clear();
    }

    void removeFinishedJobs(){
        vector<int> finishedJobs;

        pid_t result;
        if(jobList.empty()) return;
        for (auto &it: jobList) {
            int jobId = it.first;
            int status;
            pid_t pid = it.second->pid;
            result = waitpid(pid,&status,WNOHANG);
            if (result !=0){
                finishedJobs.push_back(jobId);
               
            }
            if (result == -1) {
                cerr<<"smash error: waitpid failed"<<endl;
            }

        }
        for (auto jobId:finishedJobs){
            auto it = jobList.find(jobId);
            if (it != jobList.end()) {

                jobList.erase(it);
            }
        }
        if (!jobList.empty()) {
            int maxJobId = 0;
            for (const auto &it : jobList) {
                if (it.first > maxJobId) {
                    maxJobId = it.first;
                }
            }
            nextJobId = maxJobId + 1;
        } else {
            nextJobId = 1;
        }

    }

    JobEntry *getJobById(int jobId){
        if (jobList.find(jobId) != jobList.end()) {
            return jobList[jobId];
        }
        return nullptr;
    }

    void removeJobById(int jobId){
        auto it = jobList.find(jobId);
        if (it != jobList.end()) {
            jobList.erase(it);
        }
    }

    JobEntry *getLastJob(int *lastJobId){
        if (jobList.empty()) {
            return nullptr;
        }
        auto it = jobList.rbegin();
        if (lastJobId) {
            *lastJobId = it->first;
        }
        return it->second;
    }

    JobEntry *getLastStoppedJob(int *jobId){
        for (auto it = jobList.rbegin(); it != jobList.rend(); ++it) {
            if (it->second->isStopped) {
                if (jobId) {
                    *jobId = it->first;
                }
                return it->second;
            }
        }
        return nullptr;
    }

};

class JobsCommand : public BuiltInCommand {
    JobsList *jobs;
public:
    JobsCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line),jobs(jobs){}

    virtual ~JobsCommand() {
    }

    void execute() override;
};

class KillCommand : public BuiltInCommand {
    JobsList *jobs;
public:
    KillCommand(const char *cmd_line, JobsList *jobs):BuiltInCommand(cmd_line),jobs(jobs){}
    virtual ~KillCommand() {
    }

    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    JobsList *jobs;
public:
    ForegroundCommand(const char *cmd_line, JobsList *jobs): BuiltInCommand(cmd_line),jobs(jobs){}

    virtual ~ForegroundCommand() {
    }

    void execute() override;
};

class ListDirCommand : public Command {
public:
    explicit ListDirCommand(const char *cmd_line): Command(cmd_line) {}

    virtual ~ListDirCommand() {
    }

    void execute() override;
    void Traversing(const string& path, int indent, set<string>& visitedDirs);
    bool isdir(const string& path);
};

class WhoAmICommand : public Command {
public:
    explicit WhoAmICommand(const char *cmd_line): Command(cmd_line){}

    virtual ~WhoAmICommand() {
    }

    void execute() override;
};

class NetInfo : public Command {
    // TODO: Add your data members
public:
    NetInfo(const char *cmd_line);

    virtual ~NetInfo() {
    }

    void execute() override;
};

class aliasCommand : public BuiltInCommand {
    string alias_name;
    string alias_cmd;
public:
    explicit aliasCommand(const char *cmd_line): BuiltInCommand(cmd_line){}
    bool fixInput(int numArgs, char **args);

    virtual ~aliasCommand() {
    }

    void execute() override;
};

class unaliasCommand : public BuiltInCommand {
public:
    explicit unaliasCommand(const char *cmdLine) : BuiltInCommand(cmdLine) {}

    virtual ~unaliasCommand() {
    }

    void execute() override;

};

class SmallShell {
private:
    string currentPrompt;
     char* lastDir;
    JobsList *jobs;
    pid_t foregroundPid = -1;

    SmallShell() : currentPrompt("smash"), lastDir(nullptr), jobs(new JobsList()) {
        for (const auto &command : reservedCommands) {
            aliases[command] = command;
        }
    }

public:
    unordered_map<string,string> aliases;
    vector<string> reservedCommands = {"chprompt", "pwd", "showpid", "cd", "alias","unalias","jobs","fg","quit","kill","listdir","whoami"};
    pid_t getForegroundPid()const{
        return foregroundPid;
    }
    void setForegroundPid(const pid_t pid){
        foregroundPid=pid;
    }
    JobsList * getJobList() const{
        return jobs;
    }
    void printAllCurrCommand() const;
    bool findReservedCommand(const string& command) const;
    void setNewCommandName(const string& newName,const string& oldCommand);
    void setPrompt(const string &newPrompt){
        currentPrompt = newPrompt;
    }
    const string &getPrompt() const{
        return currentPrompt;
    }
    int getPID ()const{
        return getpid();
    }
    const char* getDir() {
        return lastDir;
    }
     char** getlastDir(){
        return &lastDir;
    }
    bool checkIfIoCommand(const char* cmd_line);
    void setlastDir(const char* path) {
        if (lastDir) {
            free(lastDir); // Free old memory to avoid memory leaks
        }
        if (path) {
            lastDir = strdup(path); // Allocate memory and copy path
        } else {
            lastDir = nullptr; // Handle case where path is null
        }
    }
    string returnValueInMap(const string& aliasValue){
        for (auto it = aliases.begin(); it != aliases.end(); ++it){
            if (it->second == aliasValue) {
                return it->first;
            }
        }
        return nullptr;
    }
    unordered_map<string, string>::iterator findValueInMap(const string& aliasValue) {
        for (auto it = aliases.begin(); it != aliases.end(); ++it) {
            if (it->second == aliasValue) {
                return it;
            }
        }
        return aliases.end();
    }
    Command *CreateCommand(const char *cmd_line,bool toFork);

    SmallShell(SmallShell const &) = delete; // disable copy ctor
    void operator=(SmallShell const &) = delete; // disable = operator
    static SmallShell &getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ~SmallShell()=default;

    void executeCommand(const char *cmd_line, bool isPipe);

};

#endif //SMASH_COMMAND_H_
