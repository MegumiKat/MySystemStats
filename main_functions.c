#include "header.h"


void Pressed_ctr_c(int signal){
    printf("\n Quit the program? [Press 'Y' to ensure] ");
    char answer;
    scanf("%c", &answer);
    if (answer == 'Y') {
        exit(0); 
    }
}

void changeFlags(int argc, char**argv, COMMAND* command){
    for(int i = 0; i < argc; i++){
        char *flag = strtok(argv[i], "=");
        if(strcmp(argv[i], "--system")==0){
            command->sys_flag = true;
            command->flag_num++;
        }
        if(strcmp(argv[i], "--sequential")==0){
            command->seq_flag = true;
            command->flag_num++;
        }
        if(strcmp(argv[i], "--graphics") ==0 || strcmp(argv[i], "--g") ==0){
            command->gra_flag = true;
            command->flag_num++;
        }
        if(strcmp(argv[i], "--user")==0){
            command->use_flag = true;
            command->flag_num++;
        }
        if(isInt(argv[i])){
            command->pos_flag = true;
            if(command->pos_num == 0){
                command->samples = atoi(argv[i]);
                command->pos_num++;
                command->flag_num++;
            }else if (command->pos_num == 1)
            {
                command->tdelay= atoi(argv[i]);
                command->flag_num++;
                command->pos_num++;
            }
        }
        if(strcmp(flag, "--sample") == 0){
                command->samples = atoi(strtok(NULL, ""));
                command->flag_num++;
        }
        if(strcmp(flag, "--tdelay") == 0){
                command->tdelay = atoi(strtok(NULL, ""));
                command->flag_num++;
            }
    }
}

void printSystemStats(int argc, char** argv, COMMAND* command){

    int samples = command->samples;
    int tdelay = command->tdelay;

    if(command->flag_num > 0 || argc == 1){
        char CPU_arr_0[samples][1024];
        char CPU_arr_3[samples][1024];
        char CPU_arr_4[samples][1024];
        char Memory_arr[samples][1024];
        char USER_arr[1024];
        typedef struct {
            char CPU_str[1024];
            double CPU_cur;
            double TIME_pre;
            double UT_pre;
            char CPU_arr_1[samples][1024];
            char CPU_arr_2[samples][1024];
        } CPU_OCCUPY;
        MEMORY_OCCUPY memory;
        CPU_OCCUPY cpu;
        for(int i=0;i < samples;i++){
            strcpy(cpu.CPU_arr_2[i],"");
        }
        //loop thorugh the samples
        for(int i=0; i<samples;i++)
        {
            strcpy(USER_arr, "");
            strcpy(cpu.CPU_str, "");
            strcpy(cpu.CPU_arr_1[i], "");
            strcpy(cpu.CPU_arr_2[i], "");
            strcpy(memory.memory_store, "");
            int retval; 
            int memoryPipe[2], userPipe[2], cpuPipe[2];
        if (pipe(memoryPipe) != 0 || pipe(userPipe) != 0 || pipe(cpuPipe) != 0) {
            fprintf(stderr, "Failed to create pipe");
            exit(EXIT_FAILURE);
        }
        pid_t child_pids[3]; 
        for (int j=0; j<3; j++) {
              if (j == 0) {
                if ((child_pids[j] = fork()) == -1) {
                    fprintf(stderr, "Failed to fork");
                } 
                else if (child_pids[j] == 0) {
                    close(memoryPipe[0]);
                    printMemoryStats(command, i, &memory.VIR_pre, memory.memory_store);

                    if ((retval = write(memoryPipe[1], &memory, sizeof(MEMORY_OCCUPY))) < 0) {
                        fprintf(stderr, "Error writing to pipe");
                        exit(EXIT_FAILURE);
                    }
                    close(memoryPipe[1]);
                }
                else {
                    while(wait(NULL) > 0);
                    close(memoryPipe[1]);
                    if ((retval = read(memoryPipe[0], &memory, sizeof(MEMORY_OCCUPY))) < 0) {
                        fprintf(stderr, "Error writing to pipe");
            
                        exit(EXIT_FAILURE);
                    }
                    
                    strcpy(Memory_arr[i], memory.memory_store);

                    close(memoryPipe[0]);
                }
            }
               if (j == 1) {
                if ((child_pids[j] = fork()) == -1) {
                    fprintf(stderr, "Failed to fork");
                } 
                else if (child_pids[j] == 0) {
                    close(userPipe[0]);
                    getUserUsage(USER_arr);
                    if ((retval = write(userPipe[1], USER_arr, strlen(USER_arr) + 1)) < 0) {
                        fprintf(stderr, "Error writing to pipe");
                        exit(EXIT_FAILURE);
                    }
                    close(userPipe[1]);
                }
                else {
                    while(wait(NULL) > 0);
                    close(userPipe[1]);
                    if ((retval = read(userPipe[0], USER_arr, sizeof(USER_arr))) < 0) {
                        fprintf(stderr, "Error reading from pipe");
                        exit(EXIT_FAILURE);
                    }
                    close(userPipe[0]);
                }
            }
                else if (j == 2) {
                if ((child_pids[j] = fork()) == -1) {
                    fprintf(stderr, "Failed to fork");
                } 
                else if (child_pids[j] == 0) {
                    close(cpuPipe[0]);
                    generateCPUUsage(i, &cpu.CPU_cur, &cpu.TIME_pre, &cpu.UT_pre, command, CPU_arr_3, CPU_arr_4, cpu.CPU_str);
                    strcpy(cpu.CPU_arr_2[i], CPU_arr_3[i]);
                    strcpy(cpu.CPU_arr_1[i], CPU_arr_4[i]);
                    if ((retval = write(cpuPipe[1], &cpu, sizeof(CPU_OCCUPY))) < 0) {
                        fprintf(stderr, "Error writing to pipe");
                        exit(EXIT_FAILURE);
                    }
                    close(cpuPipe[0]);
                }
                else {
                    while(wait(NULL) > 0);
                    close(cpuPipe[1]);
                    if ((retval = read(cpuPipe[0], &cpu, sizeof(CPU_OCCUPY))) < 0) {
                        fprintf(stderr, "Error reading from pipe");
                        exit(EXIT_FAILURE);
                    }
                    strcpy(CPU_arr_0[i], cpu.CPU_str);
                    strcpy(CPU_arr_3[i], cpu.CPU_arr_2[i]);
                    strcpy(CPU_arr_4[i], cpu.CPU_arr_1[i]);
                    close(cpuPipe[0]);
                }
            }

        }

            if (child_pids[0] == 0) {
                exit(EXIT_SUCCESS);
            }

            if (child_pids[1] == 0) {
                exit(EXIT_SUCCESS);
            }
            if (child_pids[2] == 0) {
                exit(EXIT_SUCCESS);
            }
            printTitle(i, command);
            if(command->sys_flag || command->gra_flag || command->seq_flag || command->pos_flag || argc==1){
                printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
                printMemorySession(i, command, Memory_arr);
            }
            printCores();
            if(command->sys_flag || command->gra_flag || command->seq_flag || command->pos_flag || argc==1) printf("%s", CPU_arr_0[i]);
            if(command->use_flag||argc==1){
                printf("-----------------------------------------------------------\n");
                printf("### Sessions/users ###\n");
                printf("%s", USER_arr);
            }
        }
        printSystemInfo();
    }
}


int main(int argc, char**argv){
    COMMAND* command = package(10, 1);

    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, Pressed_ctr_c);
    
    changeFlags(argc, argv, command);

    printSystemStats(argc, argv, command);

    free(command);
}