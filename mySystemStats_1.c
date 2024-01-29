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




/*Calculating total Memory usage*/
void calculateMemoryUsage(){
    struct sysinfo sys_info;
    char into_buff[100];

    if
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
            char *flag = __strtok_r(argv[i], "=", &p);
            if(strcmp(flag, "--sample") == 0){
                *sample = atoi(__strtok_r(NULL, "", &p));
                sample_chose = true;
            }else if(strcmp(flag, "--tdelay") == 0){
                *interval = atoi(__strtok_r(NULL, "", &p));
                interval_chose = true;
            }else if (strcmp(flag, "--system") == 0)
            {
                *system_chose = true;
            }else if (strcmp(flag, "--user") == 0){
                *user_chose = true;
            }else if (strcmp(flag, "--graphics") == 0 || strcmp(flag, "--g") == 0 ){
                *graph_chose = true;
            }else if (strcmp(flag, "--sequential") == 0){
                sequential_chose = true;
            }else if (isInt(argv[i]) && i+1 < argc && isInt(argv[i+1]) && !sample_chose && !interval_chose){
                *sample = atoi(argv[i]);
                *interval = atoi(argv[i+1]);

                sample_chose = true;
                interval_chose = true;
                i = i + 1;
            }else{
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
    printf("### Sessions/users ###");

    setutent();
    ut = getutent();

    while(ut != NULL){
        if(ut->ut_type == USER_PROCESS){
            printf("%s         %s (%s)", ut->ut_user, ut->ut_line, ut->ut_host);
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

    printf("Nbr of samples: %d -- every %d secs\n", sample, inerval); // print number of samples and time delay

    int i = 0;
    while(i <  sample){








        i++;
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

    printSystemStats(sample, interval, system_chose, graph_chose, user_chose, sequential_chose);

    return 0;

}
