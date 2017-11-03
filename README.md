# ifj-proj
Z Readme se dočasně stává interní dokumentace :D

## Obecné
- Tasky do Issues - labely task -> ready to test -> completed
- Bugy do Issues - label bug
- Branche - každý větší kus nového kódu implementovat ve vlastní branchi
- Master musí být vždy přeložitelný - mergovat jen funkční kód

## Scrum 1 - 18.10.2017
Zavedení obecných věcí (viz výše).
Review napsaného kódu.

Rozdělení úkolů na další týden:
- Petr - scanner - doplnění stavů, přepsat na pointery, přepsat key words
- Petr - upravit funkce pro práci s pamětí
- Adam - testy, CMake, kostra - hotovo   
- Vojta - symtable - změna velikosti při zaplnění, přidat is_defined k funkci, typ void
- Ondra + Adam - rozvržení projektu do jednotlivých tasků (co nejnižší úroveň), časový plán

## Scrum 2 - 25.10.2017
Overview:
  - Symtable ready to test, symtable větev zrušena, další hotfixy v aktivních větvích
  - Scanner ready to test, nedodělky pořád ve větvi scanner-test

Rozdělení úkolů na další týden:
- Petr, Adam - scanner - doladění scanneru
- Adam - testy
- Vojta - parser - nová větev, lexikální analýza formou konečného automatu
- Ondra - zásobník - implementovat buď do nové větve, nebo do větve k parseru

## Scrum 3 - 1.11.2017
Overview:
  - Petr dokončil scanner
  - Adam udělal testy pro symtable, odhalil bugy
  - Ondra došel k závěru, že zásobník nejspíš nepotřebujeme, takže dělal kachničku
  - Vojta napsal základní syntaktickou analýzu bez výrazů

Rozdělení úkolů na další týden:
- Petr - generování kódu
- Adam - pokračování testů
- Vojta - syntaktická analýza výrazů - precedenční tabulka, napsat zásobník, vyhodnotit výrazy
        - opravit bugy v symtable
- Ondra - sémantická analýza
