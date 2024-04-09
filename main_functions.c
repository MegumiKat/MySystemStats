#include "header.h"
// Function: Pressed_ctr_c
// Description: Signal handler for SIGINT (Ctrl+C) signal.
//              Prompts the user to confirm program termination.
// Parameters:
//   - signal: The signal number.
void Pressed_ctr_c()
{
    printf("\n Quit the program? [Press 'Y' to ensure] ");
    char answer;
    scanf("%c", &answer);
    if (answer == 'Y')
    {
        printf("Waiting...\n");
        exit(EXIT_SUCCESS);
    }
}
// Function: changeArgument
// Description: Parses command-line arguments and updates command structure flags accordingly.
// Parameters:
//   - argc: Number of command-line arguments.
//   - argv: Array of command-line arguments.
//   - command: Pointer to COMMAND structure to update flags.
void changeArgument(int argc, char **argv, COMMAND *command)
{
    for (int i = 0; i < argc; i++)
    {
        char *flag = strtok(argv[i], "=");
        if (strcmp(argv[i], "--system") == 0)
        {
            command->sys_flag = true;
            command->flag_num++;
        }
        if (strcmp(argv[i], "--sequential") == 0)
        {
            command->seq_flag = true;
            command->flag_num++;
        }
        if (strcmp(argv[i], "--graphics") == 0 || strcmp(argv[i], "--g") == 0)
        {
            command->gra_flag = true;
            command->flag_num++;
        }
        if (strcmp(argv[i], "--user") == 0)
        {
            command->use_flag = true;
            command->flag_num++;
        }
        if (isInt(argv[i]))
        {
            command->pos_flag = true;
            if (command->pos_num == 0)
            {
                command->samples = atoi(argv[i]);
                command->pos_num++;
                command->flag_num++;
            }
            else if (command->pos_num == 1)
            {
                command->t_delay = atoi(argv[i]);
                command->flag_num++;
                command->pos_num++;
            }
        }
        if (strcmp(flag, "--sample") == 0)
        {
            command->samples = atoi(strtok(NULL, ""));
            command->flag_num++;
        }
        if (strcmp(flag, "--t_delay") == 0)
        {
            command->t_delay = atoi(strtok(NULL, ""));
            command->flag_num++;
        }
    }
}
// Function: printSystemStats
// Description: Prints system statistics based on command flags and arguments.
// Parameters:
//   - argc: Number of command-line arguments.
//   - argv: Array of command-line arguments.
//   - command: Pointer to COMMAND structure containing configuration flags.
void printSystemStats(COMMAND *command)
{

    int samples = command->samples;

    char CPU_arr_0[samples][1024];
    char CPU_arr_3[samples][1024];
    char CPU_arr_4[samples][1024];
    char Memory_arr[samples][1024];
    char USER_arr[1024];
    typedef struct
    {
        double CPU_cur;
        double TIME_pre;
        double UT_pre;
        char CPU_str[1024];
        char CPU_arr_1[samples][1024];
        char CPU_arr_2[samples][1024];
    } CPU_OCCUPY;
    MEMORY_OCCUPY memory;
    CPU_OCCUPY cpu;
    for (int i = 0; i < samples; i++)
    {
        strcpy(USER_arr, "");
        strcpy(cpu.CPU_str, "");
        strcpy(cpu.CPU_arr_1[i], "");
        strcpy(cpu.CPU_arr_2[i], "");
        strcpy(memory.memory_store, "");
        int buffer;
        int memoryPipe[2], userPipe[2], cpuPipe[2];
        if (pipe(memoryPipe) != 0 || pipe(userPipe) != 0 || pipe(cpuPipe) != 0)
        {
            fprintf(stderr, "Failed to create pipe");
            exit(EXIT_FAILURE);
        }
        pid_t stat_func_child[3];
        for (int j = 0; j < 3; j++)
        {
            if (j == 1)
            {
                if ((stat_func_child[j] = fork()) == -1)
                {
                    fprintf(stderr, "Fork error");
                }
                else if (stat_func_child[j] == 0)
                {
                    close(memoryPipe[0]);
                    printMemoryStats(command, i, &memory.VIR_pre, memory.memory_store);

                    if ((buffer = write(memoryPipe[1], &memory, sizeof(MEMORY_OCCUPY))) >= 0)
                    {
                        fprintf(stderr, "Writing erro");
                        exit(EXIT_FAILURE);
                    }
                    close(memoryPipe[1]);
                }
                else
                {
                    while (wait(NULL) > 0)
                        ;
                    close(memoryPipe[1]);
                    if ((buffer = read(memoryPipe[0], &memory, sizeof(MEMORY_OCCUPY))) >= 0)
                    {
                        strcpy(Memory_arr[i], memory.memory_store);
                        close(memoryPipe[0]);
                    }
                    else
                    {
                        fprintf(stderr, "Writing erro");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            if (j == 0)
            {
                if ((stat_func_child[j] = fork()) == -1)
                {
                    fprintf(stderr, "Fork error");
                }
                else if (stat_func_child[j] == 0)
                {
                    close(userPipe[0]);
                    getUserUsage(USER_arr);
                    if ((buffer = write(userPipe[1], USER_arr, strlen(USER_arr) + 1)) >= 0)
                    {
                        close(userPipe[1]);
                    }
                    else
                    {
                        fprintf(stderr, "Writing erro");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    while (wait(NULL) > 0)
                        ;
                    close(userPipe[1]);
                    if ((buffer = read(userPipe[0], USER_arr, sizeof(USER_arr))) >= 0)
                    {
                        close(userPipe[0]);
                    }
                    else
                    {
                        fprintf(stderr, "Reading erro");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else if (j == 2)
            {
                if ((stat_func_child[j] = fork()) == -1)
                {
                    fprintf(stderr, "Fork error");
                }
                else if (stat_func_child[j] == 0)
                {
                    close(cpuPipe[0]);
                    generateCPUUsage(i, &cpu.CPU_cur, &cpu.TIME_pre, &cpu.UT_pre, command, CPU_arr_3, CPU_arr_4, cpu.CPU_str);
                    strcpy(cpu.CPU_arr_2[i], CPU_arr_3[i]);
                    strcpy(cpu.CPU_arr_1[i], CPU_arr_4[i]);
                    if ((buffer = write(cpuPipe[1], &cpu, sizeof(CPU_OCCUPY))) >= 0)
                    {
                        close(cpuPipe[0]);
                    }
                    else
                    {
                        fprintf(stderr, "Writing erro");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    while (wait(NULL) > 0)
                        ;
                    close(cpuPipe[1]);
                    if ((buffer = read(cpuPipe[0], &cpu, sizeof(CPU_OCCUPY))) >= 0)
                    {
                        strcpy(CPU_arr_0[i], cpu.CPU_str);
                        strcpy(CPU_arr_3[i], cpu.CPU_arr_2[i]);
                        strcpy(CPU_arr_4[i], cpu.CPU_arr_1[i]);
                        close(cpuPipe[0]);
                    }
                    else
                    {
                        fprintf(stderr, "Reading erro");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        if (stat_func_child[0] == 0 || stat_func_child[1] == 0 || stat_func_child[2] == 0)
        {
            exit(EXIT_SUCCESS);
        }
        printMemoryTitle(i, command);
        if (command->sys_flag || command->gra_flag || command->pos_flag || command->seq_flag)
        {
            printMemoryInfo(i, command, Memory_arr);
            printCores();
            printf("%s", CPU_arr_0[i]);
        }
        if (command->flag_num == 0)
        {
            printMemoryInfo(i, command, Memory_arr);
            printUserTitle();
            printf("%s", USER_arr);
            printCores();
            printf("%s", CPU_arr_0[i]);
        }
        if (command->use_flag)
        {
            printUserTitle();
            printf("%s", USER_arr);
        }
        sleep(command->t_delay);
    }
    printSystemInfo();
}

int main(int argc, char **argv)
{
    COMMAND *command = package(10, 1);

    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, Pressed_ctr_c);

    changeArgument(argc, argv, command);

    printSystemStats(command);

    free(command);
}