# MySystemStats
CSCB09 A1
### APPROACH
#### Step 1 : Gather the information:
Reading through the documentation for the auxiliray C libraries listed in the assignment handout, I determined which functions I can use to gather all the required **stat** (_running parameters, user usage, system usage, system informtaion_).
###### SYSTEM INFORMATION:
- To get the system information, I created the `utsname` struct defined in `<sys/utsname.h>` and got the required information using the `usname()` function. The following fields in the struct were useful for this assignment:
     - `sysname` : system name
     - `nodename` : machine name
     - `verson` : OS version
     - `release` : OS release
     - `machine` : machine architecture
 - To get the system informtaion about the `System running since last reboot`, I read through the `/proc/uptime` file, got the first parameter in this file and performed the calculation in Step 2.
###### RUNNING PARAMETERS:
- There are some default value for this assignment like`sample = 10` and `time delay = 1`:
     - `sample` : the default value is 10, can be changed by command
     - `time delay` : the default value is 1, can be changed by command
     - `memory self-utilization` : how much memory the monitoring tool is actually using
- To get the value of `memory self-utilization` (in KB), I used the `ru_maxrss` field from the `rusage` struct defined in `sys/resource.h` and got this informtaion required using the `getusage()` function.
###### USER USAGE:
- To get the user usage information, I used the `utmp` struct defined in `utmp.h` and got this required **stat** using the `getutent` function. The following fields in the struct were necessary for this assignment:
     - `ut_user` : the user log-in name
     - `ut_line` : the device name
     - `host` : the host name
###### SYSTEM USAGE:
- To get the number of CPU cores, I called the `sysconf(_SC_NPROCESSORS_ONLN)`function from `unistd.h`.
- To get the total CPU usage, I read through the `/proc/stat` file and performed the calculations outlined in Step 2.
- To get the amount of memory used (physical and virtual), I used the `sysinfo` struct defined in `sys/sysinfo.h` and got this required **stat** using the `sysinfo()` function. The following fields from the `sysinfo` struct were necessary for the purposes of this assignment:
    - `totalram`: the total usable main memory size
    - `freeram`: the available memory space
    - `totalswap`: the total swap space size
    - `freeswap`: the size of the swap space still available
    - `mem_unit`: the memory unit size in bytes
 
#### Step 2 : Calculation:
- To get the **CPU Usage**, it required two time points:
<p align="center">`CPU Usage (%) = 100 - (idle_2 - idle_1) * 100.0 / (totalTime_2 - totalTime_1)`</p>

  Note: the subscript 1 denotes the value at the first time point and the subscript 2 denotes the value at the second time point.
The total time can be calculated be summing all the values in the first line of the `/proc/stat` file and the idle time is the fourth number on the first 

- To get the **Memory Usage**, it required many parameters and formulas:
  - `total physical memory` = (total ram)
  - `total virtual memory` = (total ram) + (total swap space)
  - `physical memory used` = (total ram) - (free ram)
  - `swap space used` = (total swap space) - (free swap space)
  - `virtual memory used` = (physical memory used) + (swap space used)

#### Step 3 : Data Storage:
- I created two complex struct type called `CPU_OCCUPY` and `MEMORY_OCCUPY` to store each `Memory usage` data and `CPU usage` data.
- For each type of node, I also created two Arrays to store each nodes represented different value in different time points.
- Each node contained all the relevant **stat**.

#### Step 4 : Parsing Through User Input:
I iterated through `argv` to get access to the command line arguments entered by the user. Since some `choses` have a `=` sign in the middle, I used `strtok()` from `string.h` to split each argument at `=`. This way, we can read the `chose` name and the value inputted (if applicable separately). If the string after the `=` cannot be converted to an integer, an error message will appear and the program will terminate. To decide which `chose` have been inputted by the user, I used `strcmp()` from `string.h` and used boolean variables to store whether or not each `chose` has been inputted. Depending on which combination of `chose` have been inputted, the program prints the relevant **stat** (through a series of `if`/`else` statements.)

#### Step 5 : Printing the Report:
To ensure that the output is refreshed at every time point, before taking each sample, I saved the position of the cursor using the `\x1b[s` (which used to save the current cursor position in a terminal emulator) escape code. After printing out the relevant information using the functions described below, I used the `\x1b[u` (restores the cursor to the previously saved position) escape code to allow for the cursor to return to previously saved position. In the next iteration, the previous output is overwritten. In this way, the output refreshes at every time point.


### Methods Overview
| **Function** | **Description** | 
| --- | --- |
| `MEMORY_OCCUPY* newMemory()` | Creats a new Memory Node containd all relevant **stat** which record the data in different time points. |
| `CPU_OCCUPY *newCpu()` | Creats a new Memory Node containd all relevant **stat** which record the data in different time points. |
| `float calculateCPUUsage(CPU_OCCUPY *cpuinfo_1, CPU_OCCUPY *cpuinfo_2)` |  Returns CPU usage as a percentage. `cpuinfo_1` and `cpuinfo_2` represent _the first time points' CPU usage_ and _the second time points' CPU usage_. |
| `bool isInteger(char *string)` | Returns true iff the string s is an integer. |
| `void printMemoryUsage()` | Print the relevant **stat** about Memory Usage in specific format. |
| `MEMORY_OCCUPY *generateMemoryUsage(MEMORY_OCCUPY **MEMORY_array, int i, int sample)` | Return the `MEMORY_OCCUPY` type which contains all stat in new time point and print the `total memory usage` in specific format. |
| `MEMORY_OCCUPY *generateMemoryUsageSequence(MEMORY_OCCUPY **MEMORY_array, int i, int sample)` | Return the `MEMORY_OCCUPY` type which contains all stat in new time point and print the `total memory usage` in specific format under the command `--sequential` |
| `CPU_OCCUPY *generateCPUUsage(CPU_OCCUPY **CPU_array, int i)` | Return the `CPU_OCCUPY` type which contains all stat in new time point and print the `CPU usage` in specific format. |
| `bool changeArgument(int argc, char**argv, int *sample, int *interval, bool *system_chose, bool *graph_chose, bool *user_chose, bool *sequential_chose` ｜ Parses through command line arguments to determine which flags have been entered. Returns true iff arguments are entered in correct format. `argc` is the number of command line arguments entered and `argv` is an array of strings that holds the command line arguments entered. `sample` points to the number times statistics will be collected and `interval` points to the frequency of statistic collection. `system_chose`, `user_chose` and `graphics_chose` each point to a boolean variables that holds true iff the `system` chose, `user` chose, `graphics` chose, `sequential_chose` are inputted by the user, respectively. |
| `void getUserUsage()` | Print some relevant stat about user usage. |
| `void getUptime()` | Calculate the uptime parameter and print it to the screen in specific format. |
| `void printSystemInfo()` | Print all stat about the system information. |
| `void printSystemStats(int sample, int inerval, bool system_chose, bool graph_chose, bool user_chose, bool sequential_chose)` | Prints final output. `sample` is the number times statistics will be collected and `interval` is the frequency of statistic collection.`system_chose`, `user_chose`, `graph_chose` and `sequential_chose` hold true iff the `system` chose, `user` chose, `graphics` and 'sequential` chose are inputted by the user, respectively. |
| `void printSystemStatsSequence(int sample, int inerval, bool system_chose, bool graph_chose, bool user_chose, bool sequential_chose)` | Prints final output. `sample` is the number times statistics will be collected and `interval` is the frequency of statistic collection.`system_chose`, `user_chose`, `graph_chose` and `sequential_chose` hold true iff the `system` chose, `user` chose, `graphics` and 'sequential` chose are inputted by the user, respectively. |






















