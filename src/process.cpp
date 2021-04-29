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
  //https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  
  vector<string> times = LinuxParser::CpuUtilization(Pid());
  float uptime = (float)UpTime() * sysconf(_SC_CLK_TCK);
  float utime = stof(times[0]);
  float stime = stof(times[1]);
  float cutime = stof(times[2]);
  float cstime = stof(times[3]);
  float starttime = stof(times[4]);
  float hertz = sysconf(_SC_CLK_TCK);
  
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / hertz);

  return (total_time / hertz) / seconds;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process& a) {
	return (this->CpuUtilization() > a.CpuUtilization());
}