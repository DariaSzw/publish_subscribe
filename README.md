# Publish_Subscribe
Projekt na przedmiot Programowanie Systemowe i Współbieżne.

Jest to zaimplementowany system dystrybucji wiadomości o ograniczonej pojemności.
Dowolny wątek może zasubskrybować, od tego momentu ma możliwość pobierania wiadomości z kolejki. Każda wiadomość, zanim zostanie
usunięta, musi być odebrana przez każdego zasubskrybowanego. W systemie może być przechowywanych
maksymalnie N wiadomości. Próba dostarczenia wiadomości N+1 skutkuje zablokowaniem
wątku wysyłającego. Odbiór kolejnej wiadomości jest być blokujący,
jeżeli wątek odczytał już wszystkie wiadomości z kolejki. Dostarczanie wiadomości
do kolejki bez subskrybentów, powoduje natychmiastowe usuwanie tych wiadomości.

Przykład działania programu:
```python
─────────────────────────────────────────────────────────────────────
Faza Wątek T₁ Wątek T₂ Wątek T₃ Wątek T₄
─────────────────────────────────────────────────────────────────────
1:   put(m₁)
2:             subscribe()
3:   put(m₂)
4:             1←getAvail()    subscribe()
5:                              get()
6:               m₂←get()         │
7:                get()           │
8:   put(m₃)        │             │
9:                 m₃←           m₃←
10:  setSize(2)
11: put(m₄)
12: 1←getAvail() 1←getAvail() subscribe()
13: put(m₅)
14: put(m₆) 2←getAvail() 2←getAvail() 1←getAvail()
15: │ unsubscribe()
16: │ m₄←get()
17: ┴ m₄←get()
18: unsubscribe()
19: put(m₇)
20: │ 2←getAvail()
21: ┴ m₅←get()
22: m₆←get()
23: m₇←get()
─────────────────────────────────────────────────────────────────────
```
