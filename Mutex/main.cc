#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

int ticket = 0;
pthread_mutex_t mutex;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *route(void *arg)
{
    char *id = (char *)arg;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (ticket > 0)
        {
            usleep(1000);
            printf("%s sells ticket:%d\n", id, ticket);
            ticket--;
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            printf("%s wait on cond!\n", id);
            pthread_cond_wait(&cond, &mutex); //醒来的时候，会重新申请锁！！
            printf("%s 被叫醒了\n", id);
        }
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main(void)
{
    pthread_t t1, t2, t3, t4;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&t1, NULL, route, (void *)"thread 1");
    pthread_create(&t2, NULL, route, (void *)"thread 2");
    pthread_create(&t3, NULL, route, (void *)"thread 3");
    pthread_create(&t4, NULL, route, (void *)"thread 4");

    int cnt = 10;

    while(true)
    {
        sleep(5);
        ticket += cnt;
        printf("主线程放票喽, ticket: %d\n", ticket);
        pthread_cond_signal(&cond);
    }


    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_mutex_destroy(&mutex);
}