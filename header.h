#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>
#include <utmp.h>
#include <sys/resource.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/utsname.h>

typedef struct COMMAND_PACKAGE
{
    int samples;
    int t_delay;
    int pos_num;
    int flag_num;
    bool pos_flag;
    bool sys_flag;
    bool seq_flag;
    bool use_flag;
    bool gra_flag;
} COMMAND;

typedef struct CPU_PACKAGE
{
    double CPU_cur;
    double TIME_pre;
    double UT_pre;
    int samples;

    char CPU_str[1024];
    char CPU_arr_2[1024];
    char CPU_arr_1[1024];
} CPU_OCCUPY;

typedef struct MEMORY_PACKAGE
{
    double VIR_pre;
    char memory_store[1024];
} MEMORY_OCCUPY;

COMMAND *package(int sample, int t_delay);
void changeArgument(int argc, char **argv, COMMAND *command);
void Pressed_ctr_c();
void generateCPUUsage(int i, double *cpu, double *TIME_pre, double *UT_pre, COMMAND *command, char CPU_arr_3[][1024], char CPU_arr_4[][1024], char CPU_str[1024]);
void printMemoryStats(COMMAND *command, int i, double *preVirMem, char arr[1024]);
void printMemoryTitle(int i, COMMAND *command);
void printMemoryInfo(int i, COMMAND *command, char memory_arr[][1024]);
void printCores();
void getUserUsage(char USER_arr[1024]);
bool isInt(char *string);
void printSystemStats(COMMAND *command);
void getUptime();
void printSystemInfo();
void printCPUInfo(int i, COMMAND *command, char CPU_arr_0[][1024]);
void printUserTitle();
