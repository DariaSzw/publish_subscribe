#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

struct TQueue {
	pthread_t *subscribers; 
	int max_sub; //początkowa liczba możliwych subskrybentów
	int sub; //liczba subskrybentów
    void **mess; //tablica wskaźników - wiadomości
	int count_mess; //liczba wiadomości
	int max_size; //maksymalna wielkość kolejki wiadomości
	pthread_t **who_can_read; //tablica dwuwymiarowa z informacjami kto może odczytać daną wiadomość
	int *read_counters; //tablica informująca ile osób może odczytać daną wiadomość

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