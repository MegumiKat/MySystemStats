#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>


/*Define a cpu_occupy struct*/
typedef struct CPU_PACKAGE{
    unsigned int total_usage;//store the total usage
    unsigned int idle;//store the idle 
}CPU_OCCUPY;

typedef struct MEMORY_PACKAGE{
    char str[1024];//store the official format of all data
    float phys_mem_used;//store the physical used
    float total_ram;// store the total ram
    float virtual_mem_used;// store the virtual used
    float total_memory;// store the total memory
}MEMORY_OCCUPY;

/*Create a newMemory strcut to save relevant data about sysinfo*/
MEMORY_OCCUPY* newMemory(){
    MEMORY_OCCUPY *memory = malloc(sizeof(MEMORY_OCCUPY));// create a new memory space

    struct sysinfo s;

    float total_ram; // represents the total ram
    float free_ram; // represents the free ram
    float total_swap; // represents the total swap
    float free_swap; // represents the free swap
    float swap_used; // reprsents the swap used
    float phys_mem_used;// represents the physical used
    float virtual_mem_used; // represents the virtual used
    float total_memory; // represents the total memory

    strcpy(memory->str," ");//intialized the data tp space
    sysinfo(&s); // initialize the relevant date to struct type s

    total_ram = (float)s.totalram/s.mem_unit/1000000000;//calculate the total ram
    free_ram = (float)s.freeram/s.mem_unit/1000000000;//calculate the free ram
    total_swap =(float)s.totalswap/s.mem_unit/1000000000;//calculate the total swap
    free_swap =(float)s.freeswap/s.mem_unit/1000000000;//calculate the free swap
    
    total_memory = total_ram + total_swap;//calculate the total memory
    phys_mem_used = total_ram - free_ram;//calculate the physical used
    swap_used = total_swap - free_swap;//calculate the swap used
    virtual_mem_used = phys_mem_used + swap_used;//calculate the virtual used

    memory->total_memory = total_memory; //set the memory value
    memory->phys_mem_used = phys_mem_used;//set the memory value
    memory->virtual_mem_used = virtual_mem_used;//set the memory value
    memory->total_ram = total_ram;//set the memory value

    sprintf(memory->str, "%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n", memory->phys_mem_used, memory->total_ram, memory->virtual_mem_used, memory->total_memory);//set the memory value

    return memory;
}

/*Creat a new CPU data type*/
CPU_OCCUPY *newCpu(){
    CPU_OCCUPY *cpu = malloc(sizeof(CPU_OCCUPY));//create a memory space
    FILE *stat = fopen("/proc/stat","r");//open the /proc/stat and read relevant data
    char cpuinfo[60];//creat a char data to store the data read by fopen
    if(stat == NULL){
        printf("Error: Opening /proc/stat");// if read failed
    }else{
        fgets(cpuinfo, 60, stat);//if succeed, store the data
    }
    fclose(stat);// close the file
    
    char *token = strtok(cpuinfo, " ");//read the first data and split them
    token = strtok(NULL, " "); //read the first
    int user = atoi(token);//store it
    token = strtok(NULL, " ");// move to next token
    int nice = atoi(token);//store it
    token = strtok(NULL, " "); // move to next token
    int system = atoi(token);//store it
    token = strtok(NULL, " "); // move to next token
    int idle = atoi(token);//store it
    token = strtok(NULL, " "); // move to next token
    int iowait = atoi(token);//store it
    token = strtok(NULL, " "); // move to next token
    int irq = atoi(token);//store it
    token = strtok(NULL, " "); // move to next token
    int softirq = atoi(token);//store it
    token = strtok(NULL, " "); // move to next token
    int steal = atoi(token);//store it
    token = strtok(NULL, " "); // move to next token
    int guest = atoi(token);//store it

    cpu->total_usage = user + nice + system + idle + iowait + irq + softirq + steal + guest;//calculate the total usage
    cpu->idle = idle;//set the idle

    return cpu;
}

/*Calculating cpu usage*/
float calculateCPUUsage(CPU_OCCUPY *cpuinfo_1, CPU_OCCUPY *cpuinfo_2){
    float cpu_usage;
    if(cpuinfo_1 == NULL){//if it is the first time to get data
        cpu_usage = 0;
    }else{//calcualte the cpu usage in percentage
         cpu_usage = 100 - (cpuinfo_2->idle - cpuinfo_1->idle) * 100.0 / (cpuinfo_2->total_usage - cpuinfo_1->total_usage);
    }
    return cpu_usage;
}

/*Print the Memory usage*/
void printMemoryUsage(){
    struct rusage r;
    getrusage(RUSAGE_SELF, &r); //initialize the r
    printf(" Memory usage: %ld kilobytes\n", r.ru_maxrss);//get the value of memorage usage
    printf("----------------------------------------------------\n");
}

/*Generating the Memory Usage */
MEMORY_OCCUPY *generateMemoryUsage(MEMORY_OCCUPY **MEMORY_array, int i, int sample){
    printMemoryUsage();// print the memory usage
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    MEMORY_OCCUPY *memory = malloc(sizeof(MEMORY_OCCUPY));// create a new memory data type
    memory = newMemory();
    for(int j = 0; j < i; j++){//print all elements in the MEMORY_array
        printf("%s",MEMORY_array[j]->str);
    }
    printf("%s", memory->str);//print the data read in this time
    for (int j = 0; j < sample - i - 1; j++){// change the column
        printf("\n");
    }
    return memory;

}

/*Generating the Memory Usage if "--sequence" command*/
MEMORY_OCCUPY *generateMemoryUsageSequence(MEMORY_OCCUPY **MEMORY_array, int i, int sample){
    printMemoryUsage();
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    MEMORY_OCCUPY *memory = malloc(sizeof(MEMORY_OCCUPY));
    memory = newMemory();
    for(int j = 0; j < i; j++){
        strcpy(MEMORY_array[j]->str, "\n");
        printf("%s",MEMORY_array[j]->str);
    }
    printf("%s", memory->str);
    for (int j = 0; j < sample - i - 1; j++){
        printf("\n");
    }
    return memory;

}

/*Generating the CPU usage */
CPU_OCCUPY *generateCPUUsage(CPU_OCCUPY **CPU_array, int i){
    printf("----------------------------------------------------\n");
    printf("Number of cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN)); // get the number of core
    CPU_OCCUPY *cpu = malloc(sizeof(CPU_OCCUPY)); //creat a new CPU data type
    cpu = newCpu();
    float cpu_usage = calculateCPUUsage(CPU_array[i], cpu);//get the cpu usage value
    printf(" total cpu use = %.2f%%\n", cpu_usage); // display CPU usage
    return cpu;
}

/*Reading a string and checking if each element in this string is a decimal*/
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


/*Reading the arguments typed in terminal and desciding which commands should be implemented */
bool changeArgument(int argc, char**argv, int *sample, int *interval, bool *system_chose, bool *graph_chose, bool *user_chose, bool *sequential_chose){

    bool sample_chose = false; // the boolean reveal whether the N in "--sample=N" need to be change or not
    bool interval_chose = false; // the boolean reveal whether the T in "--tdelay=T" need to be change or not
    
    
    if(argc > 1){
        char *p;
        int i = 1;
        while(i < argc){
            char *flag =                                           (argv[i], "=", &p);//read the command line
            if(strcmp(flag, "--sample") == 0){
                *sample = atoi(__strtok_r(NULL, "", &p));//change the sample value
                sample_chose = true;
            }else if(strcmp(flag, "--tdelay") == 0){
                *interval = atoi(__strtok_r(NULL, "", &p));//change the time delay value
                interval_chose = true;
            }else if (strcmp(flag, "--system") == 0)
            {
                *system_chose = true;
            }else if (strcmp(flag, "--user") == 0){
                *user_chose = true;
            }else if (strcmp(flag, "--graphics") == 0 || strcmp(flag, "--g") == 0 ){
                *graph_chose = true;
            }else if (strcmp(flag, "--sequential") == 0){
                *sequential_chose = true;
            }else if (isInt(argv[i]) && i+1 < argc && isInt(argv[i+1]) && !sample_chose && !interval_chose){// the case has two integers
                *sample = atoi(argv[i]);
                *interval = atoi(argv[i+1]);

                sample_chose = true;
                interval_chose = true;
                i = i + 1;
            }else{//the command wasn't accepted
                fprintf(stderr, "Pleas give valid command!\n");
                return false;
            }
            i++;
        }
    }
    return true;
}


/*Getting the user information 
~ report how many users are connected in a given name
~ report how many sessions each user is connected to 
~hostname or IP address*/
void getUserUsage(){
    struct utmp *ut;

    printf("---------------------------------------------------\n");
    printf("### Sessions/users ###\n");

    setutent();// set the environment
    ut = getutent();//get the relavant value

    while(ut != NULL){
        if(ut->ut_type == USER_PROCESS){//check the type
            printf("%s         %s (%s)\n", ut->ut_user, ut->ut_line, ut->ut_host);// print out
        }
        
        ut = getutent();
    }

    
}





/*Getting the uptime data
~time that the system has been running since its last reboot*/
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



/*Through the systeminfo library to get relevant data
~system name
~machine name
~OS version
~OS release
~machine architecture
~time that the system has been running since its last reboot
*/
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


/*Finally outputing the information about the system according to the required commands*/
void printSystemStats(int sample, int inerval, bool system_chose, bool graph_chose, bool user_chose, bool sequential_chose){

    printf("Nbr of samples: %d -- every %d secs\n", sample, inerval); 
    // printMemoryUsage();

    CPU_OCCUPY *CPU_array[sample + 1];//store every CPU data
    MEMORY_OCCUPY *MEMORY_array[sample + 1];//store evert MEmory data

    CPU_array[0] = NULL;// set the first element to NULL

    int i = 0;
    while(i <  sample){
        
        printf("\x1b%d", 7); //save the position

        if (system_chose && !graph_chose) { // if system flag indicated without graphics
            MEMORY_array[i] = generateMemoryUsage(MEMORY_array, i, sample);
            CPU_array[i+1] = generateCPUUsage(CPU_array, i);
        }
        if (!system_chose && !user_chose && !graph_chose) { //if no command inputed
            MEMORY_array[i] = generateMemoryUsage(MEMORY_array, i, sample);
            getUserUsage();
            CPU_array[i+1] = generateCPUUsage(CPU_array, i);
            
        }
        if (user_chose) { 
            getUserUsage();
        }





        if (i+1 < sample) { 
            sleep(inerval); //the interval to read relavant data
            printf("\x1b%d", 8);//back to the position
        }
        i++;
    }

    //free the memory space
    for(int j = 0; j < sample + 1; j++){
        free(CPU_array[j]);
    }
    for(int j = 0; j < sample; j++){
        free(MEMORY_array[j]);
    }

    printSystemInfo();// print the system information

}



/*Finally outputing the information about the system according to the required commands withe command "--sequence"*/
void printSystemStatsSequence(int sample, int inerval, bool system_chose, bool graph_chose, bool user_chose, bool sequential_chose){

    printf("Nbr of samples: %d -- every %d secs\n", sample, inerval); 
    // printMemoryUsage();

    CPU_OCCUPY *CPU_array[sample + 1];
    MEMORY_OCCUPY *MEMORY_array[sample + 1];

    CPU_array[0] = NULL;

    int i = 0;
    while(i <  sample){
        
        printf(">>> iteration %d\n", i); 

        if (system_chose && !graph_chose) { // if system flag indicated without graphics
            MEMORY_array[i] = generateMemoryUsageSequence(MEMORY_array, i, sample);
            CPU_array[i+1] = generateCPUUsage(CPU_array, i);
        }
        if (!system_chose && !user_chose && !graph_chose) { 
            MEMORY_array[i] = generateMemoryUsageSequence(MEMORY_array, i, sample);
            getUserUsage();
            CPU_array[i+1] = generateCPUUsage(CPU_array, i);
            
        }
        if (user_chose) { 
            getUserUsage();
        }





        if (i+1 < sample) { 
            sleep(inerval); 
        }
        i++;
    }

    for(int j = 0; j < sample + 1; j++){
        free(CPU_array[j]);
    }
    for(int j = 0; j < sample; j++){
        free(MEMORY_array[j]);
    }

    printSystemInfo();
}






int main(int argc, char **argv){
    int sample = 10; // default number of sample
    int interval = 1; // default number of seconds
    
    bool system_chose = false; // the boolean reveal whether "--system" is inputed
    bool graph_chose = false; // the boolean reveal whether "--graph" is inputed
    bool user_chose = false; // the boolean reveal whether "--user" is inputed
    bool sequential_chose = false; // the boolean reveal whether "--sequential" is inputed
    

    bool change = changeArgument(argc, argv, &sample, &interval, &system_chose, &graph_chose, &user_chose, &sequential_chose);// giving the command and checking if it is valid
    if(!change){
        fprintf(stderr, "There is something wrong with your command!\n");// checking if the command is valid
        return 1;
    }

    if(sequential_chose){//check if want sequence
        printSystemStatsSequence(sample, interval, system_chose, graph_chose, user_chose, sequential_chose);

    }else{
        printSystemStats(sample, interval, system_chose, graph_chose, user_chose, sequential_chose);
    }
    

    return 0;

}
