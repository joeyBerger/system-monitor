#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() { 
  	long active_jiffies_new = LinuxParser::ActiveJiffies();
  	long total_jiffies_new = LinuxParser::Jiffies();
    if (total_jiffies_new == total_jiffies) return previousUtilization;
    previousUtilization = float(active_jiffies_new - active_jiffies) / float(total_jiffies_new - total_jiffies);
    active_jiffies = active_jiffies_new;
    total_jiffies = total_jiffies_new;
    return previousUtilization;
}