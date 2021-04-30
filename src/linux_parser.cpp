#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Read and return system pids
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {   
  string line, key, value;
  float mem_total = 1.0, mem_free = 1.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);  
  if (filestream.is_open()) {    
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          mem_total = stof(value);
        } else if (key == "MemFree:") {
          mem_free = stof(value);
          //Memory Utilized = (Total-Free) / Total          
          return (mem_total-mem_free) / mem_total;
        }
      }
    }
  }
  return 0.0;  
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, time_up;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> time_up;
    return stoi(time_up);
  }
  return 0;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies();}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  vector <string> utilization_stats = CpuUtilization(pid);
  return std::stol(utilization_stats[13]) + std::stol(utilization_stats[14]) + std::stol(utilization_stats[15]) + std::stol(utilization_stats[16]);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector <string> utilization_stats = CpuUtilization();
  return std::stol(utilization_stats[kUser_]) + std::stol(utilization_stats[kNice_]) + std::stol(utilization_stats[kSystem_]) +
         std::stol(utilization_stats[kIRQ_]) + std::stol(utilization_stats[kSoftIRQ_]) + std::stol(utilization_stats[kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector <string> utilization_stats = CpuUtilization();
  return std::stol(utilization_stats[kIdle_]) + std::stol(utilization_stats[kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(int pid) { 
  string line, t;
  vector<string> return_vec;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {  
    std::getline(filestream, line);
    std::istringstream ssin(line);
    while (ssin.good()) {
      ssin >> t;
      return_vec.push_back(t);
    }
  }
  return return_vec;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, processes, processes_value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> processes >> processes_value) {
        if (processes == "processes") {
          return stoi(processes_value);
        }
      }
    }
  }
  return 0; 
}

// Read cpu utilization and return vector for processing jiffy counts
vector<string> LinuxParser::CpuUtilization() { 
  vector <string> cpu_utilization;
  string line, str;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream ssin(line);
    while (ssin.good()) {
      ssin >> str;
      if (str != "cpu") cpu_utilization.push_back(str);      
    }
    filestream.close();
  }
  return cpu_utilization;
}


// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, procs_running, procs_running_value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> procs_running >> procs_running_value) {
        if (procs_running == "procs_running") {
          return stoi(procs_running_value);
        }
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    stream.close();
	  return line;    
  }
  return string(); 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return to_string(stoi(value)/1000);
        }
      }
    }
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  while (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key >> value) {
          if (key == "Uid:") {
            return value;
      }
    }
  }
  return string();  
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
	string uid = Uid(pid);
	if (!uid.empty()) {
		string line, user, a, value;
        std::ifstream filestream(kPasswordPath);
        if (filestream.is_open()) {
          while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            while (linestream >> user >> a >> value) {
              if (value == uid) {
                return user;
              }
            }
          }
        }
    }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {   
  string line, time;
  int desired_idx = 21, i = 0;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream ssin(line);    
    while (ssin.good() && i < desired_idx+1) {
      ssin >> time;
      i++;
    }
    return UpTime() - (std::stoi(time) / sysconf(_SC_CLK_TCK));
  }
  return 0;
}
