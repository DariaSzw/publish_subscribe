#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "queue.h" 

void* w1(void* arg) {
    TQueue *queue = (TQueue *)arg;
    int *m1, *m2, *m3, *m4, *m5, *m6, *m7;
    int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7;
    m1 = malloc(sizeof(int)); m2 = malloc(sizeof(int)); m3 = malloc(sizeof(int)); m4 = malloc(sizeof(int)); 
    m5 = malloc(sizeof(int)); m6 = malloc(sizeof(int)); m7 = malloc(sizeof(int));
    *m1 = a; *m2 = b; *m3 = c; *m4 = d; *m5 = e; *m6 = f; *m7 = g;
    int new_size = 2;

    //Faza 1
    sleep(1);
    printf("1. Dodawanie wiadomości -1-: %d\n", *(int *)m1);
    addMsg(queue, m1);
    //Faza 3
    sleep(2);
    printf("3. Dodawanie wiadomości -2-: %d\n", *(int *)m2);
    addMsg(queue, m2);
    //Faza 8
    sleep(5);
    printf("8. Dodawanie wiadomości -3-: %d\n", *(int *)m3);
    addMsg(queue, m3);
    //Faza 10
    sleep(2);
    printf("10. Zmiana rozmiaru kolejki na -2-: %d\n", new_size);
    setSize(queue, new_size);
    //Faza 11
    sleep(1);
    printf("11. Dodawanie wiadomości -4-: %d\n", *(int *)m4);
    addMsg(queue, m4);
    //Faza 13
    sleep(2);
    printf("13. Dodawanie wiadomości -5-: %d\n", *(int *)m5);
    addMsg(queue, m5);
    //Faza 14
    sleep(1);
    printf("14. Dodawanie wiadomości -6-: %d\n", *(int *)m6);
    addMsg(queue, m6);
    //Faza 19
    sleep(2); //17-19
    printf("19. Dodawanie wiadomości -7-: %d\n", *(int *)m7);
    addMsg(queue, m7);

    return NULL;
}

void* w2(void* arg) {
    TQueue *queue = (TQueue *)arg;
    pthread_t T2 = pthread_self();

    //Faza 2
    sleep(2);
    printf("2. Wątek T2 zasubskrybował\n");
    subscribe(queue, T2);
    //Faza 4
    sleep(2);
    printf("4. Ile wiadomości można odczytać przez T2 -1-: %d\n", getAvailable(queue, T2));
    //Faza 6
    sleep(2);
    printf("6. Pobrana wiadomość przez T2 -2-: %d\n", *(int *)getMsg(queue, T2));
    //Faza 7
    sleep(1);
    printf("7. Pobrana wiadomość przez T2 -3-: %d\n", *(int *)getMsg(queue, T2));
    //Faza 12 
    sleep(4); //9-12
    printf("12. Ile wiadomości można odczytać przez T2 -1-: %d\n", getAvailable(queue, T2));
    //Faza 14
    sleep(2);
    printf("14. Ile wiadomości można odczytać przez T2 -2-: %d\n", getAvailable(queue, T2));
    //Faza 17
    sleep(3);
    printf("17. Pobrana wiadomość przez T2 -4-: %d\n", *(int *)getMsg(queue, T2));
    //Faza 20
    sleep(3);
    printf("20. Ile wiadomości można odczytać przez T2 -2-: %d\n", getAvailable(queue, T2));
    //Faza 21
    sleep(1);
    printf("21. Pobrana wiadomość przez T2 -5-: %d\n", *(int *)getMsg(queue, T2));
    //Faza 22
    sleep(1);
    printf("22. Pobrana wiadomość przez T2 -6-: %d\n", *(int *)getMsg(queue, T2));
    //Faza 23
    sleep(1);
    printf("23. Pobrana wiadomość przez T2 -7-: %d\n", *(int *)getMsg(queue, T2));

    return NULL;
}

void* w3(void* arg) {
    TQueue *queue = (TQueue *)arg;
    pthread_t T3 = pthread_self();

    //Faza 4
    sleep(4);
    printf("4. Wątek T3 zasubskrybował\n");
    subscribe(queue, T3);
    //Faza 5
    sleep(1);
    printf("5. Pobrana wiadomość przez T3 -3-: %d\n", *(int *)getMsg(queue, T3));
    //Faza 12
    sleep(4); //9-12
    printf("12. Ile wiadomości można odczytać przez T3 -1-: %d\n", getAvailable(queue, T3));
    //Faza 14
    sleep(2);
    printf("14. Ile wiadomości można odczytać przez T3 -2-: %d\n", getAvailable(queue, T3));
    //Faza 16
    sleep(2);
    printf("16. Pobrana wiadomość przez T3 -4-: %d\n", *(int *)getMsg(queue, T3));
    //Faza 18
    sleep(2);
    printf("18. Wątek T3 odsubskrybował\n");
    unsubscribe(queue, T3);

    return NULL;
}

void* w4(void* arg) {
    TQueue *queue = (TQueue *)arg;
    pthread_t T4 = pthread_self();
    
    //Faza 12
    sleep(12);
    printf("12. Wątek T4 zasubskrybował\n");
    subscribe(queue, T4);
    //Faza 14
    sleep(2);
    printf("14. Ile wiadomości można odczytać przez T4 -1-: %d\n", getAvailable(queue, T4));
    //Faza 15
    sleep(1);
    printf("15. Wątek T4 odsubskrybował\n");
    unsubscribe(queue, T4);

    return NULL;
}

int main() {
    pthread_t T1, T2, T3, T4;
    int size = 2;

    // Tworzenie kolejki
    TQueue *queue = createQueue(size);

    // Tworzenie wątków
    if (pthread_create(&T1, NULL, w1, queue) != 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&T2, NULL, w2, queue) != 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&T3, NULL, w3, queue) != 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&T4, NULL, w4, queue) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // Oczekiwanie na zakończenie wątków
    pthread_join(T1, NULL);
    pthread_join(T2, NULL);
    pthread_join(T3, NULL);
    pthread_join(T4, NULL);

    // Zniszczenie kolejki
    destroyQueue(queue);

    printf("Koniec\n");
    
    return 0;
}