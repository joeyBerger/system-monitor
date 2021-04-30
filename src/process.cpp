#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

#include <unistd.h>
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {pid_ = pid;}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
  long total_jiffies = LinuxParser::ActiveJiffies(Pid());
  long time = total_jiffies / sysconf(_SC_CLK_TCK);
  return (float)time / float(UpTime());
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
// long int Process::UpTime() { return LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK); }
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process& a) {
	return (this->CpuUtilization() > a.CpuUtilization());
}