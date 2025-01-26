<div align="center">
  <h1>Publish_Subscribe</h1>
  <p>Projekt na przedmiot Programowanie Systemowe i Współbieżne.</p>
</div> 

Jest to zaimplementowany system dystrybucji wiadomości o ograniczonej pojemności.
Dowolny wątek może zasubskrybować, od tego momentu ma możliwość pobierania wiadomości z kolejki. Każda wiadomość, zanim zostanie
usunięta, musi być odebrana przez każdego zasubskrybowanego. W systemie może być przechowywanych
maksymalnie N wiadomości. Próba dostarczenia wiadomości N+1 skutkuje zablokowaniem
wątku wysyłającego. Odbiór kolejnej wiadomości jest być blokujący,
jeżeli wątek odczytał już wszystkie wiadomości z kolejki. Dostarczanie wiadomości
do kolejki bez subskrybentów, powoduje natychmiastowe usuwanie tych wiadomości.

Pliki w tym projekcie zawierają:
* queue.h - plik nagłówkowy, zawierający deklaracje funkcji oraz zainmplementowaną strukturę
* queue.c - plik zawierający główny kod programu
* test.c - plik testujący rozwiązanie

Dla wiadomości:
```
          m1: 1      m2: 2      m3: 3     m4: 4     m5: 5     m6: 6     m7: 7
```
Wynik pliku test.c wynosi: (-X- jest oczekiwaną odpowiedzią)
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
