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
| `CPU_OCCUPY *generateCPUUsage(CPU_OCCUPY **CPU_array, int i)` | Return the `CPU_OCCUPY` type which contains all stat in new time point and print the `CPU usage` in specific format **tip: because the first time only has one information, the CPU usage is meaningless!** |
| `bool changeArgument(int argc, char**argv, int *sample, int *interval, bool *system_chose, bool *graph_chose, bool *user_chose, bool *sequential_chose` | Parses through command line arguments to determine which flags have been entered. Returns true iff arguments are entered in correct format. `argc` is the number of command line arguments entered and `argv` is an array of strings that holds the command line arguments entered. `sample` points to the number times statistics will be collected and `interval` points to the frequency of statistic collection. `system_chose`, `user_chose` and `graphics_chose` each point to a boolean variables that holds true iff the `system` chose, `user` chose, `graphics` chose, `sequential_chose` are inputted by the user, respectively. |
| `void getUserUsage()` | Print some relevant stat about user usage. |
| `void getUptime()` | Calculate the uptime parameter and print it to the screen in specific format. |
| `void printSystemInfo()` | Print all stat about the system information. |
| `void printSystemStats(int sample, int inerval, bool system_chose, bool graph_chose, bool user_chose, bool sequential_chose)` | Prints final output. `sample` is the number times statistics will be collected and `interval` is the frequency of statistic collection.`system_chose`, `user_chose`, `graph_chose` and `sequential_chose` hold true iff the `system` chose, `user` chose, `graphics` and 'sequential` chose are inputted by the user, respectively. |
| `void printSystemStatsSequence(int sample, int inerval, bool system_chose, bool graph_chose, bool user_chose, bool sequential_chose)` | Prints final output. `sample` is the number times statistics will be collected and `interval` is the frequency of statistic collection.`system_chose`, `user_chose`, `graph_chose` and `sequential_chose` hold true iff the `system` chose, `user` chose, `graphics` and 'sequential` chose are inputted by the user, respectively. |
| `MEMORY_OCCUPY *generateMemoryUsageGraphics(MEMORY_OCCUPY **MEMORY_array, int i, int sample)` | Displays memory usage with graphics. `sample` is the number times statistics will be collected, `i` indicates the number times statistics will have been collected by the end of the current cycle, `MEMORY_array` represents all memory information in this reading |
| `MEMORY_OCCUPY *generateMemoryUsageSequence(MEMORY_OCCUPY **MEMORY_array, int i, int sample)` | this is as same as `MEMORY_OCCUPY *generateMemoryUsageGraphics(MEMORY_OCCUPY **MEMORY_array, int i, int sample)` but under "--sequential" condition |
| `CPU_OCCUPY *generateCPUUsageGraphics(CPU_OCCUPY **CPU_array, int i)` | Displays CPU usage with graphics. `sample` is the number times statistics will be collected, `i` indicates the number times statistics will have been collected by the end of the current cycle, `CPU_array` represents all memory information in this reading |
| `CPU_OCCUPY *generateCPUUsageGraphicsSequence(CPU_OCCUPY **CPU_array, int i, int sample)` | Displays CPU usage with graphics. `sample` is the number times statistics will be collected, `i` indicates the number times statistics will have been collected by the end of the current cycle, `CPU_array` represents all memory information in this reading under `--sequential` |



### Running the Program
1. Navigate to the directory (i.e., `cd`) in which `MySystemStats.c` is saved on your machine.
2. In the terminal, enter `gcc -Wall -Werror mySystemStats.c -o MySystemStats` to compile the program.
3. To execute the program, enter `./MySystemStats` into the terminal. You may also use the following flags when executing:
     
| **Flag**                | **Description** |
| --- | --- |
| `--system` | to indicate that only the system usage should be generated |
| `--user` | to indicate that only the users usage should be generated |
| `--graphics` or `-g` |  to include graphical output in the cases where a graphical outcome is possible as indicated below |
| `--sequential` | to indicate that the information will be output sequentially without needing to "refresh" the screen (useful if you would like to redirect the output into a file) |
| `--samples=N` | if used the value N will indicate how many times the statistics are going to be collected and results will be average and reported based on the N number of repetitions.**If not value is indicated the default value will be 10** |
| `--tdelay=T` |  to indicate how frequently to sample in seconds. **If not value is indicated the default value will be 1 sec** |
| `samples tdelay` | The last two arguments can also be considered as positional arguments if not flag is indicated in the corresponding order |

- For example, `./MySystemStats --system --g --samples=5 --tdelay=2` will print the system usage report with graphics and will collect statistics every 2 seconds for a total and of 5 time points.
- The number of samples and frequency can also be inputted as positional arguments (i.e., as two adjacent numbers separated by a space, where the first number is the number of samples and the second number is the frequency).
     - These positional arguments can be located anywhere along the input as long as the two numbers are adjacent (e.g., `./MySystemStats 5 --system 2` is invalid).
     - For example, `./MySystemStats --user 5 2`  and `./MySystemStats 5 2 --system` will both print the system information report and will collect statistics every 2 seconds for a total of 5 time points.
     - `./MySystemStats` is equivalent to `./mySystemStats --system --user --samples=10 --tdelay=1`.
4. If `Invalid argument entered!` is printed on the screen after executing, refer back to flags outline in _Step 3_ and repeat the above steps.


### Understanding Graphics
This section applies if the user inputs the graphics flag (i.e., `--graphics` or `--g`).

**Memory Usage Graphics**: The graphics are a display of `delta_memory_usage`, the relative change in physical memory usage between the previous and current samples. If `delta_memory_usage` is negative, there is a : symbol for every change of 0.01 with a @ at the end. If deltaMemoryUsage is positive, there is a # symbol for every change of 0.01 with a * at the end. If deltaMemoryUsage is 0.00, a o is printed. Beside this graphic is an expression of the form `delta_memory_usage` , where totalMemoryUsed is expressed in GB.

**CPU Usage Graphics:**
If the CPU usage is 0%, the `|||` symbol is printed. If the CPU usage is positive, a `|` is printed for every percent of usage (rounded up to the nearest percent). 


_Sample Graphics_
```
Nbr of samples: 10 -- every 1 secs
 Memory usage: 4052 kilobytes
---------------------------------------
### Memory ### (Phys.Used/Tot -- Virtual Used/Tot) 
9.75 GB / 15.37 GB  -- 9.75 GB / 16.33 GB   |o 0.00 (9.75) 
9.75 GB / 15.37 GB  -- 9.75 GB / 16.33 GB   |* 0.00 (9.75) 
9.75 GB / 15.37 GB  -- 9.75 GB / 16.33 GB   |* 0.00 (9.75) 
9.76 GB / 15.37 GB  -- 9.76 GB / 16.33 GB   |* 0.00 (9.76) 
9.85 GB / 15.37 GB  -- 9.85 GB / 16.33 GB   |#########* 0.09 (9.85) 
10.06 GB / 15.37 GB  -- 10.06 GB / 16.33 GB   |####################* 0.20 (10.06) 
10.13 GB / 15.37 GB  -- 10.13 GB / 16.33 GB   |#######* 0.07 (10.13) 
10.16 GB / 15.37 GB  -- 10.16 GB / 16.33 GB   |##* 0.03 (10.16) 
10.28 GB / 15.37 GB  -- 10.28 GB / 16.33 GB   |###########* 0.12 (10.28) 
10.38 GB / 15.37 GB  -- 10.38 GB / 16.33 GB   |##########* 0.11 (10.38) 
---------------------------------------
### Sessions/users ### 
 marcelo       pts/0 (138.51.12.217)
 marcelo       pts/1 (tmux(277015).%0)
 alberto        tty7 (:0)
 marcelo       pts/2 (tmux(277015).%1)
 marcelo       pts/5 (138.51.12.217)
---------------------------------------
Number of cores: 4 
 total cpu use = 15.57%
         ||| 0.25 
         ||||||||| 6.93 
         ||||||||||||||| 12.08 
         |||||||||||||||| 13.83 
         ||||||||| 6.41 
         |||||||||||||||| 13.97 
         |||||||||||||||||| 15.37 
         ||||||||||||||||| 14.91 
         ||||||||||||||||||| 16.34 
         |||||||||||||||||| 15.57 
---------------------------------------
### System Information ### 
 System Name = Linux
 Machine Name = iits-b473-01
 Version = #99-Ubuntu SMP Thu Sep 23 17:29:00 UTC 2021
 Release = 5.4.0-88-generic
 Architecture = x86_64
 System running since last reboot: 3 days 19:27:30 (91:27:30)
---------------------------------------

```


 
**Potential Technical Difficulties:**
If there are too many information output in terminal, the formatting for the graphics may not be as desired. If this occurs, kindly using the terminal command `clear` and rerun the program.
 
 



















