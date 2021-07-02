#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


struct mypara
{
    int pthread;
    int start;
    int end;
};

void* thread_function(void* arg) {
    struct mypara* tmp = (struct mypara*)arg;
    int i;
    printf("Thread%d working...!\n", tmp->pthread);
    for (i = tmp->start; i < tmp->end; i++) {
        printf("Now the time is %d\n", i);
        sleep(10);
    }
    getchar();
    return NULL;
}


int main(void) {
    pid_t pid;
    pthread_t mythread1;
    pthread_t mythread2;
    pthread_t mythread3;
    struct mypara p1 = { 1, 0, 10 };
    struct mypara p2 = { 2, 10, 20 };
    struct mypara p3 = { 3, 20, 30 };
    int i = 0, j = 10, k = 20;
    pid = fork();
    if (pid < 0)
    {
        perror("fork failed!\n");
    }
    if (pid == 0)
    {
        printf("This is the child process.\n");
        int result = execl("/bin/echo", "echo", __FILE__, __func__, (char*)NULL);
        getchar();
    }
    else
    {
        if (pthread_create(&mythread1, NULL, thread_function, &p1)) {
            printf("error creating thread1.");
            abort();
        }
        if (pthread_create(&mythread2, NULL, thread_function, &p2)) {
            printf("error creating thread2.");
            abort();
        }
        if (pthread_create(&mythread3, NULL, thread_function, &p3)) {
            printf("error creating thread3.");
            abort();
        }
        if (pthread_join(mythread1, NULL)) {
            printf("error join thread.");
            abort();
        }
        if (pthread_join(mythread2, NULL)) {
            printf("error join thread.");
            abort();
        }
        if (pthread_join(mythread3, NULL)) {
            printf("error join thread.");
            abort();
        }
    }
    printf("thread done! \n");
    exit(0);
}