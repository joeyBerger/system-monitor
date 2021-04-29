#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();
  
 private:
 	long active_jiffies{0};
 	long total_jiffies{0};
  	float previousUtilization{0.0};
};

#endif