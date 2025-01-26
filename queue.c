#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "queue.h"

TQueue* createQueue(int size) 
{
    TQueue *queue = malloc(sizeof(TQueue));

    queue->max_size = size;
    queue->sub = 0;
    queue->count_mess=0;
    
    queue->mess = malloc(sizeof(void *) * queue->max_size);
    queue->who_can_read = malloc(sizeof(pthread_t *) * queue->max_size);
    queue->read_counters = (int* )calloc(queue->max_size, sizeof(int));

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->delete_mess, NULL);
    pthread_cond_init(&queue->new_mess, NULL);

    return queue;
}

void destroyQueue(TQueue *queue)
{
    pthread_mutex_lock(&queue->mutex);

    free(queue->mess);
    free(queue->read_counters);

    for (int i=0; i<queue->max_size; i++)
        free(queue->who_can_read[i]);
    free(queue->who_can_read);

    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->delete_mess);
    pthread_cond_destroy(&queue->new_mess);
    
    free(queue);
}

void subscribe(TQueue *queue, pthread_t thread)
{
    pthread_mutex_lock(&queue->mutex);

    queue->subscribers[queue->sub] = thread;
    queue->sub++;

    pthread_mutex_unlock(&queue->mutex);
}

void unsubscribe(TQueue *queue, pthread_t thread)
{
    pthread_mutex_lock(&queue->mutex);

    /*
        usunięcie ze subskrybentów
    */
    for (int i=0; i<queue->sub; i++)
    {
        if (queue->subscribers[i] == thread)
        {
            for (int j=i; j<queue->sub-1; j++)
            {
                queue->subscribers[j]=queue->subscribers[j+1];
            }
            queue->sub--;
            break;
        }
    }
    /*
        usunięcie sub. ze wszystkich wiadomości, które mógł odczytać, (jeśli są)
        (nawet jeśli po usunięciu sub. nie będzie miał kto odczytać wiadomości to usunie się ona w momencie pobrania)
    */
    for (int i=0; i<queue->count_mess; i++)
    {
        for (int j=0; j<queue->read_counters[i]; j++) 
        {
            if (queue->who_can_read[i][j] == thread) //sprawdza czy mógł odczytać daną wiadomość
            {
                for (int k=j; k<queue->read_counters[i]-1; k++)
                {
                    queue->who_can_read[i][k]=queue->who_can_read[i][k+1];
                }
                queue->read_counters[i]--;
                break; //sub. może odczytać daną wiadomość tylko raz
            }

            if (queue->read_counters[i] == 0) //jeśli nie ma kto odczytać to usuwa wiadomość
            {
                for (int j=i; j<queue->count_mess-1; j++)
                {
                    queue->mess[j] = queue->mess[j+1];
                    queue->who_can_read[j] = queue->who_can_read[j+1];
                    queue->read_counters[j] = queue->read_counters[j+1];
                }
                queue->count_mess--;

                //sygnał, że wiadomość została usunięta
                pthread_cond_broadcast(&queue->delete_mess);
            }
        }
    }

    pthread_mutex_unlock(&queue->mutex);
}

void addMsg(TQueue *queue, void *msg)
{
    pthread_mutex_lock(&queue->mutex);

    while (queue->count_mess == queue->max_size) 
        pthread_cond_wait(&queue->delete_mess, &queue->mutex);

    //jeśli nie ma subskrybentów to nie dodaje wiadomości do kolejki
    if (queue->sub > 0)
    {
        /*
            nowo dodana wiadomość jest dodawana na początek tablicy
            count_mess-1 wskazuje na wiadomość która może być aktualnie pobierana
        */
        if (queue->count_mess > 1)
        {
            for (int i=queue->count_mess; i>0; i--) 
            {
                queue->mess[i] = queue->mess[i-1];
                queue->who_can_read[i] = queue->who_can_read[i-1];
                queue->read_counters[i] = queue->read_counters[i-1];
            }
        }
        else if (queue->count_mess == 1)
        {
            queue->mess[1] = queue->mess[0];
            queue->who_can_read[1] = queue->who_can_read[0];
            queue->read_counters[1] = queue->read_counters[0];
        }
        
        queue->mess[0] = msg;
        /*
            zapisywanie, którzy subskrybenci mogą odczytać dodawaną wiadomość
        */
        queue->read_counters[0]=queue->sub;
        queue->who_can_read[0] = malloc(sizeof(pthread_t *) * queue->sub);
        for (int i=0; i<queue->sub; i++) 
        {
            queue->who_can_read[0][i] = queue->subscribers[i];
        }

        queue->count_mess++;

        pthread_cond_broadcast(&queue->new_mess);
    }

    pthread_mutex_unlock(&queue->mutex);
}

void* getMsg(TQueue *queue, pthread_t thread)
{
    pthread_mutex_lock(&queue->mutex);
    
    int temp=0;
    for (int i=0; i<queue->sub; i++) //sprawdza czy subskrybuje
    {
        if (queue->subscribers[i] == thread)
        {
            temp++;
            break;
        }
    }

    if (temp == 0) //jeśli nie subskrybuje to zwraca NULL
    {
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    }
    
    while (queue->count_mess == 0) //czeka jeśli nie ma żadnych wiadomości
        pthread_cond_wait(&queue->new_mess, &queue->mutex);

    for (int k=queue->count_mess - 1; k>=0; k--)
    {
        for (int i=0; i<queue->read_counters[k]; i++) 
        {
            if (queue->who_can_read[k][i] == thread) //sprawdza czy subskrybent może odczytać tą wiadomość
            {
                /*
                    jeśli może odczytać, to usuwa dany wątek z tych które mogą odczytać i zwraca wiadomość
                */
                if (queue->read_counters[k] > 1)
                {
                    for (int j=i; j<queue->read_counters[k]-1; j++)
                    {
                        queue->who_can_read[k][j]=queue->who_can_read[k][j+1];
                    }
                    queue->read_counters[k]--;
                }
                else 
                {
                    queue->read_counters[k] = 0; 
                    
                    //usunięta może zostać tylko jedna, najstarsza wiadomość więc nie trzeba zamieniać
                    queue->count_mess--;
                        
                    //wysyła sygnał, gdy zostaje usunięta wiadomość
                    pthread_cond_broadcast(&queue->delete_mess);
                    
                }

                pthread_mutex_unlock(&queue->mutex);
                return queue->mess[k];
            }
        }
    }
    //jeśli nie może odczytać, czeka za dodaniem nowej wiadomości i powtarza funkcję
    pthread_cond_wait(&queue->new_mess, &queue->mutex);
    pthread_mutex_unlock(&queue->mutex);
    return getMsg(queue, thread);
}

int getAvailable(TQueue *queue, pthread_t thread)
{
    pthread_mutex_lock(&queue->mutex);
    /*
        sprawdzenie czy subskrybuje, jeśli nie to zwraca -1
    */
    int temp=0;
    for (int i=0; i<queue->sub; i++)
    {
        if (queue->subscribers[i] == thread)
        {
            temp++;
            break;
        }
    }
    if (temp == 0)
    {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }
        
    /*
        sprawdza ile jest dostępnych wiadomości dla danego wątku
    */
    int result=0;
    for (int i=0; i<queue->count_mess; i++)
    {
        for (int j=0; j<queue->read_counters[i]; j++) 
        {
            if (queue->who_can_read[i][j] == thread) 
            {
                result++;
                break; //sub. może odczytać daną wiadomość tylko raz
            }
        }
    }

    pthread_mutex_unlock(&queue->mutex);
    return result;
}

void removeMsg(TQueue *queue, void *msg)
{
    pthread_mutex_lock(&queue->mutex);

    for (int i=0; i<queue->count_mess; i++)
    {
        if (queue->mess[i] == msg)
        {
            for (int j=i; j<queue->count_mess-1; j++)
            {
                queue->mess[j] = queue->mess[j+1];
                queue->who_can_read[j] = queue->who_can_read[j+1];
                queue->read_counters[j] = queue->read_counters[j+1];
            }
            queue->count_mess--;

            //sygnał, że wiadomość została usunięta
            pthread_cond_broadcast(&queue->delete_mess);
            break;
        }
    }

    pthread_mutex_unlock(&queue->mutex);
}

void setSize(TQueue *queue, int size)
{
    pthread_mutex_lock(&queue->mutex);

    queue->max_size = size;

    queue->mess = realloc(queue->mess, sizeof(void *) * queue->max_size);
    
    if (queue->count_mess > queue->max_size)
        queue->count_mess = queue->max_size;

    queue->who_can_read = realloc(queue->who_can_read, sizeof(pthread_t *) * queue->max_size);
    queue->read_counters = realloc(queue->read_counters, sizeof(void *) * queue->max_size);

    pthread_mutex_unlock(&queue->mutex);
}

