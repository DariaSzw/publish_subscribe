<div align="center">
  <h1>Publish_Subscribe</h1>
  <h3>Programowanie systemowe i współbieżne</h3>
  <h3>Daria Szwałek 159666</h3>
  <h3>daria.szwalek@student.put.poznan.pl</h3>
  <h3>2024-11-07</h3>
</div> 


Projekt jest dostępny w repozytorium pod adresem:  
<https://github.com/DariaSzw/publish_subscribe>.

# 1. Struktury danych
W celu rozwiązania zadania, potrzebne jest zadeklarowanie struktury kolejki `TQueue`:
```C
struct TQueue {
  pthread_t subscribers[2000]; //liczba możliwych subskrybentów
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
```
# 2. Funkcje

Funkcje potrzebne w rozwiązaniu zadania:

1. `TQueue* createQueue(int size)`
    * inicjuje strukturę `TQueue` reprezentującą nową kolejkę o początkowym, maksymalnym
rozmiarze `size`,
2. `void destroyQueue(TQueue *queue)`
    * usuwa kolejkę queue i zwalnia pamięć przez nią zajmowaną,
    * próba dostarczania lub odbioru nowych wiadomości z kolejki skończy się błędem,
3. `void subscribe(TQueue *queue, pthread_t thread)`
    * rejestruje wątek `thread` jako kolejnego odbiorcę wiadomości z kolejki `queue`,
4. `void unsubscribe(TQueue *queue, pthread_t thread)`
    * wyrejestrowuje wątek `thread` z kolejki `queue`,
    * nieodebrane przez wątek wiadomości są traktowane jako odebrane,
5. `void addMsg(TQueue *queue, void *msg)`
    * wstawia do kolejki `queue` nową wiadomość reprezentowaną wskaźnikiem `msg`.
6. `void* getMsg(TQueue *queue, pthread_t thread)`
    * odbiera pojedynczą wiadomość z kolejki `queue` dla wątku `thread`,
    * jeżeli nie ma nowych wiadomości, funkcja jest blokująca,
    * jeżeli wątek `thread` nie jest zasubskrybowany – zwracany jest pusty wskaźnik `NULL`,
7. `int getAvailable(TQueue *queue, pthread_t thread)`
    * zwraca liczbę wiadomości z kolejki queue dostępnych dla wątku `thread`,
8. `void removeMsg(TQueue *queue, void *msg)`
    * usuwa wiadomość `msg` z kolejki.
9. `void setSize(TQueue *queue, int size)`
    * ustala nowy, maksymalny rozmiar kolejki,
    * jeżeli nowy rozmiar jest mniejszy od aktualnej liczby wiadomości w kolejce, to nadmiarowe wiadomości są usuwane
  z kolejki, począwszy od najstarszych,

# 3. Algorytm

Wizualizacja wyniku z pliku `test.c`:
```python
─────────────────────────────────────────────────────────────────────────
Faza      Wątek T₁       Wątek T₂         Wątek T₃         Wątek T₄
─────────────────────────────────────────────────────────────────────────
 1:       put(m₁)
 2:                     subscribe()
 3:       put(m₂)
 4:                    1←getAvail()     subscribe()
 5:                                       get()
 6:                      m₂←get()           │
 7:                       get()             │
 8:       put(m₃)          │                │
 9:                       m₃←              m₃←
 10:     setSize(2)
 11:      put(m₄)
 12:                   1←getAvail()     1←getAvail()      subscribe()
 13:      put(m₅)
 14:      put(m₆)      2←getAvail()     2←getAvail()     1←getAvail()
 15:        │                                            unsubscribe()
 16:        │                            m₄←get()
 17:        ┴            m₄←get()
 18:                                   unsubscribe()
 19:      put(m₇)
 20:        │          2←getAvail()
 21:        ┴            m₅←get()
 22:                     m₆←get()
 23:                     m₇←get()
─────────────────────────────────────────────────────────────────────────
```


# 4. Przykład użycia

Dla wiadomości:
```
          m1: 1      m2: 2      m3: 3     m4: 4     m5: 5     m6: 6     m7: 7
```
Oraz początkowym rozmiarze kolejki 2, gdzie -X- jest oczekiwaną odpowiedzią, wynik pliku `test.c` wynosi:
```c
1. Dodawanie wiadomości -1-: 1
2. Wątek T2 zasubskrybował
3. Dodawanie wiadomości -2-: 2
4. Wątek T3 zasubskrybował
4. Ile wiadomości można odczytać przez T2 -1-: 1
6. Pobrana wiadomość przez T2 -2-: 2
8. Dodawanie wiadomości -3-: 3
7. Pobrana wiadomość przez T2 -3-: 3
5. Pobrana wiadomość przez T3 -3-: 3
10. Zmiana rozmiaru kolejki na -2-: 2
11. Dodawanie wiadomości -4-: 4
12. Wątek T4 zasubskrybował
12. Ile wiadomości można odczytać przez T2 -1-: 1
12. Ile wiadomości można odczytać przez T3 -1-: 1
13. Dodawanie wiadomości -5-: 5
14. Ile wiadomości można odczytać przez T4 -1-: 1
14. Ile wiadomości można odczytać przez T2 -2-: 2
14. Ile wiadomości można odczytać przez T3 -2-: 2
14. Dodawanie wiadomości -6-: 6
15. Wątek T4 odsubskrybował
16. Pobrana wiadomość przez T3 -4-: 4
17. Pobrana wiadomość przez T2 -4-: 4
18. Wątek T3 odsubskrybował
19. Dodawanie wiadomości -7-: 7
20. Ile wiadomości można odczytać przez T2 -2-: 2
21. Pobrana wiadomość przez T2 -5-: 5
22. Pobrana wiadomość przez T2 -6-: 6
23. Pobrana wiadomość przez T2 -7-: 7
Koniec
```
