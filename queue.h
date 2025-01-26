#ifndef QUEUE_H
#define QUEUE_H

// ==============================================
//
//  Version 1.1, 2025-01-16
//
// ==============================================

#include <pthread.h>

struct TQueue {
	pthread_t subscribers[16]; //liczba możliwych subskrybentów
	int sub;
    void **mess;
	int count_mess;
	int max_size;
	pthread_t **who_can_read;
	int *read_counters;

	pthread_mutex_t mutex; 
    pthread_cond_t delete_mess; //gdy wątek chce dodać wiadomość, ale kolejka jest pełna, czeka na sygnał usunięcia wiadomości
    pthread_cond_t new_mess; //gdy wątek chce pobrać wiadomość, ale kolejka jest pusta, czeka na sygnał dodania nowej wiadomości
};
typedef struct TQueue TQueue;

TQueue* createQueue(int size);

void destroyQueue(TQueue *queue);

void subscribe(TQueue *queue, pthread_t thread);

void unsubscribe(TQueue *queue, pthread_t thread);

void addMsg(TQueue *queue, void *msg);

void* getMsg(TQueue *queue, pthread_t thread);

int getAvailable(TQueue *queue, pthread_t thread);

void removeMsg(TQueue *queue, void *msg);

void setSize(TQueue *queue, int size);

#endif