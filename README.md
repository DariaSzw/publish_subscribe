<div align="center">
  <h1>Publish_Subscribe</h1>
  <h3>Programowanie systemowe i współbieżne</h3>
  <h3>Daria Szwałek 159666 daria.szwalek@student.put.poznan.pl </h3>
  <h3>2024-11-07</h3>
</div> 


Projekt jest dostępny w repozytorium pod adresem:  
<https://git.cs.put.poznan.pl/voytek/project1>.


# Struktury danych

W tym punkcie przedstaw i krótko opisz struktury danych zaprojektowane i użyte
w implementacji.  Opis powinien uzględniać prezentację istotnych zmiennych i
-- jeśli to istotne -- możliwych wartości składowanych w nich.  Przygotowując
sprawozdanie warto mieć na względzie jego cel: umożliwienie zrozumienia
algorytmu i implementacji prezentowanego rozwiązania.

Nazwy funkcji oraz nazwy zmiennych powinny być formatowane jako `kod źródłowy`.

Przykład:

1. Elementy listy definiowane są strukturą `Node`:

   ```C
   struct Node {
       void *data;
       Node *next;
   };
   ```

1. Zmienne `Head` i `Tail` wskazują odpowiednio na: początek i koniec listy:

   ```C
   Node *Head, *Tail;
   ```

# Funkcje

Punkt ten powinien zawierać zwięzłe opisy funkcji zawartych w implementacji.
Jeżeli specyfikacja zadania opisuje szczegółowo interfejs, to należy pominąć tą
część i zamieścić jedynie opisy pozostałych, uzupełniających funkcji.

Przykład:

1. `resizeList(List *lst, int nsize)` -- ustalenie nowego rozmiaru listy `lst`
   na `nsize`.

1. `addItem(List *lst, void *ptr)` -- dodanie nowego elementu wskazywanego
   wskaźnikiem `ptr` do listy `lst`.


# Algorytm / dodatkowy opis

Wizualizacja wyniku z pliku test.c:
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


# Przykład użycia

Dla wiadomości:
```
          m1: 1      m2: 2      m3: 3     m4: 4     m5: 5     m6: 6     m7: 7
```
Oraz początkowym rozmiarze kolejki 2, gdzie -X- jest oczekiwaną odpowiedzią, wynik pliku test.c wynosi:
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
