#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

struct process {
    char filename[100];
    pid_t pid;
    char ExecutionState[100];
    double TimeOfEntry;
    double TimeOfExecution;
    double TimeStopped;
    double TimeInCPU;
    double TimeOfExit;
    struct process* prev;
    struct process* next;
};

double get_wtime(void){
struct timeval t;
gettimeofday(&t,NULL);
return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}


struct process* head = NULL;

void signal_handler(int sigid);

struct sigaction sact = {
    .sa_handler = signal_handler,
    .sa_flags = SA_NOCLDSTOP
};

struct process* AddToEmptyList(char* line) {
    struct process* temp = malloc(sizeof(struct process));
    temp->prev = NULL;
    strcpy(temp->filename, line);
    temp->pid = -1;
    strcpy(temp->ExecutionState, "NEW");
    temp->next = NULL;
    temp->TimeOfEntry = get_wtime();
    temp->TimeInCPU = 0;
    head = temp;
    return head;
}

struct process* AddProcess(char* line) {
    struct process* temp, * tp;
    temp = malloc(sizeof(struct process));
    temp->prev = NULL;
    strcpy(temp->filename, line);
    temp->pid = -1;
    strcpy(temp->ExecutionState, "NEW");
    temp->next = NULL;
    temp->TimeOfEntry = get_wtime();
    temp->TimeInCPU = 0;

    if (head == NULL) {
        head = temp;
    } else {
        tp = head;
        while (tp->next != NULL)
            tp = tp->next;
        tp->next = temp;
        temp->prev = tp;
    }

    return head;
}

struct process* CreateList(char* line, int n) {
    if (n == 1) {
        return AddToEmptyList(line);
    } else {
        return AddProcess(line);
    }
}

struct process* DequeueProcess(struct process* head, char* filename) {
    struct process* temp = head;

    head = head->next;
    free(temp);
    if (head != NULL) {
        head->prev = NULL;
    }

    return head;
}

void FCFS_policy(struct process* head) {
    struct process* cur = head;
    double TotalWorkload = 0;
    while (cur != NULL) {
        pid_t pid = fork();
        cur->TimeOfExecution = get_wtime();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            printf("Dequeue process with name %s\n", cur->filename);
            printf("Executing %s\n", cur->filename);
            strcpy(cur->ExecutionState, "RUNNING");
            execl(cur->filename, cur->filename, NULL);
            perror("Failure");
            exit(2);
        } else {
            cur->pid = pid;
            waitpid(pid, NULL, 0);
            cur->TimeOfExit = get_wtime();
            double ElapsedTime = cur->TimeOfExit - cur->TimeOfEntry;
            double WorkloadTime = cur->TimeOfExit - cur->TimeOfExecution; 
            printf("PID %d - CMD: %s\n", pid, cur->filename);
            printf("Elapsed time = %.2f secs\n", (float)ElapsedTime);
            printf("Workload time = %.2f secs\n\n", (float)WorkloadTime);
            strcpy(cur->ExecutionState, "EXITED");
            head = DequeueProcess(head, cur->filename);
            TotalWorkload += WorkloadTime;
            cur = cur->next;
        }
    }
    printf("Total workload time = %.2f\n\n", TotalWorkload);
}

int EmptyList(struct process* head) {
    if(head == NULL){
    return 1;
    }else {
    return 0;
    }
}

volatile sig_atomic_t ProcessTermination = 0;


void signal_handler(int sigid) {
 ProcessTermination = 1;
}

void RR_policy(struct process* head, int Quantum, int n) {
    struct timespec quantumTime;
    quantumTime.tv_sec = Quantum / 1000;
    quantumTime.tv_nsec = (Quantum % 1000) * 1000000;
    double TotalWorkload_RR = 0;
    struct process* cur = head;
    int i = 0;
        while (cur != NULL) {

            if (strcmp(cur->ExecutionState, "NEW") == 0) {
		pid_t pid = fork();
                cur->TimeOfExecution = get_wtime();
                if (pid < 0) {
                    perror("fork failed");
                    exit(1);
                } else if (pid == 0) {
                    printf("Dequeue process with name %s\n", cur->filename);
                    printf("Executing %s\n", cur->filename);
                    strcpy(cur->ExecutionState, "RUNNING");
                    execl(cur->filename, cur->filename, NULL);
                    perror("Failure");
                    exit(2);
                } else {
		   cur->pid = pid;
		   strcpy(cur->ExecutionState, "RUNNING");
                   }
            } else if (strcmp(cur->ExecutionState, "STOPPED") == 0) {
                printf("Process %d continues!\n", cur->pid);
                cur->TimeOfExecution = get_wtime();
                if (kill(cur->pid, SIGCONT) == -1) {
                perror("kill(SIGCONT) failed");
                }
                strcpy(cur->ExecutionState, "RUNNING");
		}

		nanosleep(&quantumTime, NULL);

              if(ProcessTermination == 1){
                pid_t pid;
                int stat;
   	        struct process* temp = head;

    		while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        		while (temp != NULL) {
                            if(temp->pid == pid){
            		      	temp->TimeOfExit = get_wtime();
				double ElapsedTime = temp->TimeOfExit - temp->TimeOfEntry;
                                double WorkloadTime = temp->TimeInCPU + (temp->TimeOfExit - temp->TimeOfExecution);
                                printf("PID %d - CMD: %s\n", temp->pid, temp->filename);
                                printf("Elapsed time = %.2f secs\n", (float)ElapsedTime);
                                printf("Workload time = %.2f secs\n\n", (float)WorkloadTime);
                                TotalWorkload_RR += WorkloadTime;
                                strcpy(temp->ExecutionState, "EXITED");
				i = i + 1;
                                if(i == n){
                                printf("Total workload time = %.2f\n\n", (float)TotalWorkload_RR);
                                return;
                                }
                             }
                          temp = temp->next;
        		}
    		    }
                 ProcessTermination = 0;

               }
               if(strcmp(cur->ExecutionState, "RUNNING") == 0) {
                printf("Process %d stopped!\n\n", cur->pid);
                cur->TimeStopped = get_wtime();
                cur->TimeInCPU += cur->TimeStopped - cur->TimeOfExecution;
                if (kill(cur->pid, SIGSTOP) == -1) {
                perror("kill(SIGSTOP) failed");
                }
                strcpy(cur->ExecutionState, "STOPPED");
               }
            cur = cur->next;

        if (cur == NULL) {
            cur = head;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 3 && strcmp(argv[1], "FCFS") == 0) {
        char* FileName = argv[2];
        FILE* file = fopen(FileName, "r");

        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }

        char line[100];
        int n = 0;

        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0';
            n = n + 1;
            head = CreateList(line, n);
        }

        FCFS_policy(head);

        printf("FCFS scheduler exits!!!\n");

    } else if (argc == 4 && strcmp(argv[1], "RR") == 0) {
        int Quantum = atoi(argv[2]);
        char* FileName = argv[3];
        FILE* file = fopen(FileName, "r");

        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }

        char line[100];
        int n = 0;

        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0';
            n = n + 1;
            head = CreateList(line, n);
        }

        if (sigaction(SIGCHLD, &sact, NULL) < 0) {
            perror("could not set action for SIGCHLD\n");
        }

        RR_policy(head, Quantum, n);

        printf("RR scheduler exits!!!\n");

        fclose(file);
    }

    return 0;
}
