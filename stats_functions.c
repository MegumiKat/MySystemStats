#include "header.h"

// Function: package
// Description: Creates and initializes a COMMAND structure and returns a pointer to it.
// Parameters:
//   - sample: The number of samples.
//   - t_delay: The delay between samples.
// Return value: A pointer to the newly created COMMAND structure.
COMMAND *package(int sample, int t_delay)
{
    COMMAND *command = malloc(sizeof(COMMAND));
    command->flag_num = 0;
    command->gra_flag = false;
    command->pos_flag = false;
    command->pos_num = 0;
    command->samples = sample;
    command->seq_flag = false;
    command->sys_flag = false;
    command->t_delay = t_delay;
    command->use_flag = false;
    return command;
}
// Function: getUptime
// Description: Reads system uptime information from /proc/uptime and prints it.
void getUptime()
{
    int a, b;
    int second, minute, hour, day, complex_hour;
    FILE *fp;
    fp = fopen("/proc/uptime", "r");
    fscanf(fp, "%d%d", &a, &b);
    second = a % 60;
    minute = (a / 60) % 60;
    hour = (a / 3600) % 24;
    complex_hour = a / 3600;
    day = a / 86400;
    printf("System running since last reboot: ");
    printf("%d days %d:%d:%d (%d:%d:%d)\n", day, hour, minute, second, complex_hour, minute, second);
    fclose(fp);
}
// Function: printSystemInfo
// Description: Prints system information such as system name, machine name, version, release, architecture, and uptime.
void printSystemInfo()
{
    struct utsname uts;
    uname(&uts);
    printf("-------------------------------------------------------\n");
    printf("### System Information ###\n");
    printf("System Name = %s\n", uts.sysname);   // print system name
    printf("Machine Name = %s\n", uts.nodename); // print machine name
    printf("Version = %s\n", uts.version);       // print current version
    printf("Release = %s\n", uts.release);       // print recent release
    printf("Architecture = %s\n", uts.machine);  // print the architecture
    getUptime();                                 // print uptime data
    printf("-------------------------------------------------------\n");
}
// Function: generateCPUUsage
// Description: Reads CPU usage information from /proc/stat, calculates CPU usage percentage,
//              and generates formatted strings for CPU usage data.
// Parameters:
//   - i: Index for the current sample.
//   - cpu: Pointer to store CPU usage percentage.
//   - TIME_pre: Pointer to store previous total CPU time.
//   - UT_pre: Pointer to store previous user CPU time.
//   - command: Pointer to COMMAND structure containing configuration flags.
//   - CPU_arr_3: 2D array to store graphical representation of CPU usage.
//   - CPU_arr_4: 2D array to store numerical CPU usage data.
//   - CPU_str: String to store formatted CPU usage information.
void generateCPUUsage(int i, double *cpu, double *TIME_pre, double *UT_pre, COMMAND *command, char CPU_arr_3[][1024], char CPU_arr_4[][1024], char CPU_str[1024])
{
    FILE *fp;
    long int user, nice, system, i2, iowait, irq, softirq;
    fp = fopen("/proc/stat", "r");
    if (fp != NULL)
    {
        fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &i2, &iowait, &irq, &softirq);

        fclose(fp);
    }
    double total = user + nice + system + iowait + irq + softirq + i2;
    double Ut = total - i2;
    *cpu = fabs(((double)(Ut - *UT_pre) / (total - *TIME_pre)) * 100);
    *UT_pre = Ut;
    *TIME_pre = total;

    if (i == 0)
    {
        sprintf(CPU_str, "total cpu use:%f%c\n", 0.00, '%');
        if (command->gra_flag)
        {
            strcpy(CPU_arr_4[0], "@0.00");
        }
    }
    else
    {
        sprintf(CPU_str + strlen(CPU_str), "total cpu use:%.2f%c\n", *cpu, '%');
        sprintf(CPU_arr_4[i], "%f", *cpu);

        if (command->gra_flag)
        {
            int numB = (int)(((*cpu * 200) / 200));
            char b[numB+1];
            b[0] = '|';
            for (int i = 1; i < numB+1; i++)
            {
                b[i] = '|';
            }
            b[numB + 1] = '\0';
            strcpy(CPU_arr_3[i], b);

            for (int p = 0; p < i; p++)
            {
                sprintf(CPU_str + strlen(CPU_str), "          %s", CPU_arr_3[p]);
                sprintf(CPU_str + strlen(CPU_str), "%s\n", CPU_arr_4[p]);

                if (i == (command->samples - 1) && p == i - 1)
                {
                    sleep(0.001);
                    sprintf(CPU_str + strlen(CPU_str), "          %s", CPU_arr_3[i]);
                    sprintf(CPU_str + strlen(CPU_str), "%s\n", CPU_arr_4[i]);
                }
            }
        }
    }
}
// Function: printMemoryStats
// Description: Retrieves memory usage statistics using sysinfo() and formats the data.
// Parameters:
//   - command: Pointer to COMMAND structure containing configuration flags.
//   - i: Index for the current sample.
//   - pre: Pointer to previous virtual memory usage.
//   - arr: String to store formatted memory usage information.
void printMemoryStats(COMMAND *command, int i, double *pre, char arr[1024])
{
    struct sysinfo s;
    float total_ram;        // represents the total ram
    float free_ram;         // represents the free ram
    float total_swap;       // represents the total swap
    float free_swap;        // represents the free swap
    float swap_used;        // reprsents the swap used
    float phys_mem_used;    // represents the physical used
    float virtual_mem_used; // represents the virtual used
    float total_memory;     // represents the total memory

    sysinfo(&s); // initialize the relevant date to struct type s

    total_ram = (float)s.totalram / s.mem_unit / 1000000000;   // calculate the total ram
    free_ram = (float)s.freeram / s.mem_unit / 1000000000;     // calculate the free ram
    total_swap = (float)s.totalswap / s.mem_unit / 1000000000; // calculate the total swap
    free_swap = (float)s.freeswap / s.mem_unit / 1000000000;   // calculate the free swap

    total_memory = total_ram + total_swap;        // calculate the total memory
    phys_mem_used = total_ram - free_ram;         // calculate the physical used
    swap_used = total_swap - free_swap;           // calculate the swap used
    virtual_mem_used = phys_mem_used + swap_used; // calculate the virtual used
    if (i == 0)
    {
        *pre = virtual_mem_used;
    }
    int delta = (int)((virtual_mem_used - *pre) * 1000);
    bool sign = true;

    if (delta < 0)
    {
        sign = false;
    }

    delta = abs(delta);

    if (command->gra_flag)
    {
        if (delta == 0)
        {
            sprintf(arr, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB       |*%.2f(%.2f)", phys_mem_used, total_ram, virtual_mem_used, total_memory, virtual_mem_used - *pre, virtual_mem_used);
        }
        else
        {
            if (sign)
            {
                char bar[delta];
                memset(bar, '#', delta);
                bar[delta] = '\0';
                sprintf(arr, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB       |%s*%.2f(%.2f)", phys_mem_used, total_ram, virtual_mem_used, total_memory, bar, virtual_mem_used - *pre, virtual_mem_used);
            }
            if (!sign)
            {
                char bar[delta];
                memset(bar, ':', delta);
                bar[delta] = '\0';
                sprintf(arr, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB       |%s@%.2f(%.2f)", phys_mem_used, total_ram, virtual_mem_used, total_memory, bar, virtual_mem_used - *pre, virtual_mem_used);
            }
        }
    }
    else
    {
        sprintf(arr, " %.2f GB /%.2f GB  --  %.2f GB /%.2f GB", phys_mem_used, total_ram, virtual_mem_used, total_memory);
    }
    *pre = virtual_mem_used;
}
// Function: printMemoryTitle
// Description: Prints title and additional memory-related information.
// Parameters:
//   - i: Index for the current sample.
//   - command: Pointer to COMMAND structure containing configuration flags.
void printMemoryTitle(int i, COMMAND *command)
{
    struct rusage ump;
    if (!command->seq_flag)
    {
        system("clear");
    }

    if (command->seq_flag)
    {
        printf(">>> iteration %d\n", i);
    }
    printf("Nbr of samples: %d -- every %d secs\n", command->samples, command->t_delay);
    getrusage(RUSAGE_SELF, &ump);
    printf("Memory usage: %ld kilobytes\n", ump.ru_maxrss);
    printf("-------------------------------------------------------\n");
}
// Function: printUserTitle
// Description: Prints a title for user session information.
void printUserTitle()
{
    printf("-------------------------------------------------------\n");
    printf("### Sessions/users ###\n");
}
// Function: printMemoryInfo
// Description: Prints memory usage information.
// Parameters:
//   - i: Index for the current sample.
//   - command: Pointer to COMMAND structure containing configuration flags.
//   - memory_arr: 2D array storing memory usage information for each sample.
void printMemoryInfo(int i, COMMAND *command, char memory_arr[][1024])
{
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    if (command->seq_flag)
    {
        for (int j = 0; j < i; j++)
        {
            printf("\n");
        }
        printf("%s\n", memory_arr[i]);
    }
    else
    {
        for (int j = 0; j <= i; j++)
            printf("%s\n", memory_arr[j]);
    }
    for (int j = command->samples - 1; j > i; j--)
        printf("\n");
}
// Function: printCPUInfo
// Description: Prints CPU usage information.
// Parameters:
//   - i: Index for the current sample.
//   - command: Pointer to COMMAND structure containing configuration flags.
//   - CPU_arr_0: 2D array storing CPU usage information for each sample.
void printCPUInfo(int i, COMMAND *command, char CPU_arr_0[][1024])
{
    printCores();
    if (command->seq_flag)
    {
        for (int j = 0; j < i; j++)
        {
            printf("\n");
        }
        printf("%s\n", CPU_arr_0[i]);
    }
    else
    {
        for (int j = 0; j <= i; j++)
            printf("%s\n", CPU_arr_0[j]);
    }
    for (int j = command->samples - 1; j > i; j--)
    {
        printf("\n");
    }
}
// Function: printCores
// Description: Prints the number of CPU cores.
void printCores()
{
    printf("-------------------------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
}
// Function: getUserUsage
// Description: Retrieves user session information and formats the data.
// Parameters:
//   - USER_arr: String to store formatted user session information.
void getUserUsage(char USER_arr[1024])
{
    struct utmp *ut;
    setutent();      // set the environment
    ut = getutent(); // get the relavant value

    while (ut != NULL)
    {
        if (ut->ut_type == USER_PROCESS)
        { // check the type
            sprintf(USER_arr + strlen(USER_arr), "%s       %s(%s)\n", ut->ut_user, ut->ut_line, ut->ut_host);
        }
        ut = getutent();
    }
}
// Function: isInt
// Description: Checks if a string represents an integer.
// Parameters:
//   - string: String to be checked.
// Return value: true if the string represents an integer, false otherwise.
bool isInt(char *string)
{
    int i = 0;
    while (string[i] != '\0')
    {
        if (!isdigit(string[i]))
        {
            return false;
        }
        i++;
    }
    return true;
}
