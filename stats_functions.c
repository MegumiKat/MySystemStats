#include "header.h"

COMMAND* package(int sample, int tdelay){
    COMMAND* command = malloc(sizeof(COMMAND));
    command->flag_num = 0;
    command->gra_flag = false;
    command->pos_flag = false;
    command->pos_num = 0;
    command->samples = sample;
    command->seq_flag = false;
    command->sys_flag = false;
    command->tdelay = tdelay;
    command->use_flag = false;
    return command;
}


void getUptime(){
    int a,b;
    int second, minute, hour, day, complex_hour;
    FILE *fp; 
    fp=fopen("/proc/uptime","r");
    fscanf(fp,"%d%d",&a,&b);
    second = a % 60;
    minute = (a / 60) % 60;
    hour = (a / 3600) % 24;
    complex_hour = a /3600;
    day = a / 86400;
    printf("System running since last reboot: ");
    printf("%d days %d:%d:%d (%d:%d:%d)\n", day, hour, minute, second, complex_hour, minute, second); 
    fclose(fp);
}

void printSystemInfo(){
    struct utsname uts;
    uname(&uts);
    printf("---------------------------------------------------\n");
    printf("### System Information ###\n");
    printf("System Name = %s\n", uts.sysname); // print system name
    printf("Machine Name = %s\n", uts.nodename); // print machine name
    printf("Version = %s\n", uts.version); // print current version 
    printf("Release = %s\n", uts.release); // print recent release
    printf("Architecture = %s\n", uts.machine); // print the architecture
    getUptime();// print uptime data
    printf("----------------------------------------------------\n");

}


void generateCPUUsage(int i, double *cpu, double *t1, double *c1, COMMAND*command, char arrForCpu[][1024], char arrForStorCpu[][1024], char storeCPUs[1024])
{
    FILE *fp;
    long int user, nice, system, i2, iowait, irq, softirq;
    double curr=0;
    //char buff[255];
    fp = fopen("/proc/stat", "r");
    if (fp != NULL){
        fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &i2, &iowait, &irq, &softirq);

        fclose(fp);
    }
    
    //calculation of the CPU usage which was given by Marcelo
    curr = user + nice + system + iowait + irq + softirq + i2;
    double Ut = curr - i2;
    *cpu = fabs(((double)(Ut - *c1) / (curr - *t1)) * 100);
    *c1 = Ut;
    // Update the values for the next iteration
    *t1 = curr;
    



    char per = '%';
    if (i == 0) {
        sprintf(storeCPUs, "total cpu use:%f%c\n", 0.00, per);
        if (command->gra_flag) {
            strcpy(arrForStorCpu[0], "@0.00");
        }
    } else {
        sprintf(storeCPUs + strlen(storeCPUs), "total cpu use:%.2f%c\n", *cpu, per);
        sprintf(arrForStorCpu[i], "%f", *cpu);

        if (command->gra_flag) {
            int numB = (int)(((*cpu * 10000) / 200));
            char b[numB];
            memset(b, '|', numB);
            b[numB] = '\0';
            strcpy(arrForCpu[i], b);

            for (int p = 0; p < i; p++) {
                sprintf(storeCPUs + strlen(storeCPUs), "          %s", arrForCpu[p]);
                sprintf(storeCPUs + strlen(storeCPUs), "%s\n", arrForStorCpu[p]);

                if (i == (command->samples - 1) && p == (command->samples - 2)) {
                    sleep(0.001);
                    sprintf(storeCPUs + strlen(storeCPUs), "          %s", arrForCpu[command->samples - 1]);
                    sprintf(storeCPUs + strlen(storeCPUs), "%s\n", arrForStorCpu[command->samples - 1]);
                }
            }
        }
    }

    
    
}


void printMemoryStats(COMMAND*command, int i, double *pre, char arr[1024])
{
    
   
    struct sysinfo s;

    float total_ram; // represents the total ram
    float free_ram; // represents the free ram
    float total_swap; // represents the total swap
    float free_swap; // represents the free swap
    float swap_used; // reprsents the swap used
    float phys_mem_used;// represents the physical used
    float virtual_mem_used; // represents the virtual used
    float total_memory; // represents the total memory

    sysinfo(&s); // initialize the relevant date to struct type s

    total_ram = (float)s.totalram/s.mem_unit/1000000000;//calculate the total ram
    free_ram = (float)s.freeram/s.mem_unit/1000000000;//calculate the free ram
    total_swap =(float)s.totalswap/s.mem_unit/1000000000;//calculate the total swap
    free_swap =(float)s.freeswap/s.mem_unit/1000000000;//calculate the free swap
    
    total_memory = total_ram + total_swap;//calculate the total memory
    phys_mem_used = total_ram - free_ram;//calculate the physical used
    swap_used = total_swap - free_swap;//calculate the swap used
    virtual_mem_used = phys_mem_used + swap_used;//calculate the virtual used

    // Update the previous virtual memory value
    if(i == 0)
    {
        *pre = virtual_mem_used;
    }
    
    int delta = (int)((virtual_mem_used - *pre) * 1000);
    int sign = 0;

    if (delta < 0)
    {
        sign = -1;
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
            if (sign == 0)
            {
                char bar[delta];
                memset(bar, '#', delta);
                bar[delta] = '\0';
                sprintf(arr, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB       |%s*%.2f(%.2f)", phys_mem_used, total_ram, virtual_mem_used, total_memory, bar, virtual_mem_used - *pre, virtual_mem_used);
            }
            if (sign == -1)
            {
                char bar[delta];
                memset(bar, ':', delta);
                bar[delta] = '\0';
                sprintf(arr, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB       |%s@%.2f(%.2f)", phys_mem_used, total_ram, virtual_mem_used, total_memory, bar, virtual_mem_used - *pre, virtual_mem_used);
            }
        }
    }else{
        sprintf(arr, " %.2f GB /%.2f GB  --  %.2f GB /%.2f GB", phys_mem_used, total_ram, virtual_mem_used, total_memory);
    }
        //print out all the sotored information in array called arr, and reserve samples space for furture iteration 

    *pre = virtual_mem_used;
        //print out all the sotored information in array called arr, and reserve samples space for furture iteration 

}


void printTitle(int i, COMMAND* command)
{
    sleep(command->tdelay);
    if (!command->seq_flag)
    {
        system("clear");
    }
   
    if (command->seq_flag)
    {
        printf("Iteration----->%d\n", i + 1);
    }
    printf("Nbr of samples: %d -- every %d seconds\n", command->samples, command->tdelay);
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Memory usage: %ld kilobytes\n", usage.ru_maxrss);
    printf("---------------------------------\n");
}


void printMemorySession(int i, COMMAND* command, char memoryArray[][1024]) { // PRINTS MEMORY INFORMATION
   
    if (command->seq_flag) { // if sequential flag is called, only print current iteration
        for (int j=0; j<i; j++) printf("\n");
        printf("%s\n", memoryArray[i]);
    }
    else { // otherwise, print previous + current iterations


        for (int j=0; j<=i; j++) printf("%s\n", memoryArray[j]);
    }
    for (int j=command->samples-1; j>i; j--) printf("\n");
}

void printCores() {
    printf("---------------------------------\n");
    int numOfCores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of cores: %d\n", numOfCores);
}



void printCPUUsage(int i, bool gf, double cpu, int samples, int tdelay, char arrForCpu[][1024], char arrForStorCpu[][1024]) {
    char per = '%';
    if (i == 0) {
        printf("total cpu use:%f%c\n", 0.00, per);
        if (gf == 1) {
            strcpy(arrForStorCpu[0], "@0.00");
        }
    } else {
        printf("total cpu use:%f%c\n", cpu, per);
        sprintf(arrForStorCpu[i], "%f", cpu);

        if (gf == 1) {
            int numB = (int)(((cpu * 10000) / 80));
            char b[numB];
            memset(b, '|', numB);
            b[numB] = '\0';
            strcpy(arrForCpu[i], b);

            for (int p = 0; p < i; p++) {
                printf("          %s", arrForCpu[p]);
                printf("%s\n", arrForStorCpu[p]);

                if (i == (samples - 1) && p == (samples - 2)) {
                    sleep(tdelay);
                    printf("          %s", arrForCpu[samples - 1]);
                    printf("%s\n", arrForStorCpu[samples - 1]);
                }
            }
        }
    }
}



void getUserUsage(char USER_arr[1024])
{
    //int num = 0;
    //get the pointer to first user in file using struct utmp and loop it till end of the file, print out all the information needed
    struct utmp *users;
    //rewinds the file pointer to the beginning of the utmp file. It is generally a good idea to call it before any of the other functions
    setutent();
    //reads a line from the current file position in the utmp file. It returns a pointer to a structure containing the fields of the line.
    users = getutent();
    while(users!=NULL)
    {
        if(users->ut_type == USER_PROCESS)
            // printf("%s       %s(%s)\n",users->ut_user, users->ut_line, users->ut_host);
            sprintf(USER_arr + strlen(USER_arr), "%s       %s(%s)\n", users->ut_user, users->ut_line, users->ut_host);
        users = getutent();
    }
}

bool isInt(char *string){
    int i = 0;
    while(string[i] != '\0'){
        if(!isdigit(string[i])){
            return false;
        }
        i++;
    }
    return true;
}
