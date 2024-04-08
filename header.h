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
#include <sys/utsname.h>

typedef struct COMMAND_PACKAGE{
    int samples;
    int tdelay;
    int pos_num;
    int flag_num;
    bool pos_flag;
    bool sys_flag;
    bool seq_flag;
    bool use_flag;
    bool gra_flag;
}COMMAND;

typedef struct CPU_PACKAGE{
    double CPU_cur;
    double TIME_pre;
    double UT_pre;
            
    char CPU_str[1024];
    char CPU_arr_2[1024];
    char CPU_arr_1[1024];
}CPU_OCCUPY;

typedef struct MEMORY_PACKAGE{
    double VIR_pre;
    char memory_store[1024]; 
}MEMORY_OCCUPY; 

COMMAND* package(int sample, int tdelay);
void changeFlags(int argc, char**argv, COMMAND* command);
void Pressed_ctr_c(int signal);
void generateCPUUsage(int i, double *cpu, double *t1, double *c1, COMMAND*command, char arrForCpu[][1024], char arrForStorCpu[][1024], char storeCPUs[1024]);
void printMemoryStats(COMMAND* command, int i, double *preVirMem, char arr[1024]);
void printTitle(int i,  COMMAND* command);
void printMemorySession(int i, COMMAND*command, char memoryArray[][1024]);
void printCores();
void printCPUUsage(int i, bool graph, double cpu, int samples, int tdelay, char arrForCpu[][1024], char arrForStorCpu[][1024]);
void getUserUsage(char USER_arr[1024]);
bool isInt(char *string);
void printSystemStats(int argc, char** argv, COMMAND* command);
void printSysOnly();
void getUptime();
void printSystemInfo();
