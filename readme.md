# Folyamatok modellezése szimulátor

![C](https://img.shields.io/badge/-00599C?style=for-the-badge&logo=c&logoColor=white) ![makefile](https://img.shields.io/badge/-makefile-ff6600?style=for-the-badge&logo=makefile&logoColor=white) ![Version](https://img.shields.io/badge/Version-0.1.0-blue?style=for-the-badge&logo=Version&logoColor=white) ![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge&logo=License&logoColor=white)

> Készítette: Kiss Konrád Soma (CNY8MP)

## Feladat

> Forrás: [Link](https://mandarin.iit.uni-miskolc.hu/iitkgy/do/FM_2025/Feladat/FM_feladat_2025.pdf)

Célszerűen választott fejlesztői környezetben készítsen számítógépi programot egy tetszőlegesen választott diszkrét folyamat hatékony szimulációjának megvalósítására.

Értékelési szintek és a kapcsolódó elvárások:

- Elégséges: A rendszerben megjelenő minden egyes munkához egynél több elvégzendő művelet (operáció) tartozik. Minden egyes művelet elvégzéséhez dedikált erőforrás áll rendelkezésre. A műveletek végrehajtási sorrendje munkánként egyedileg meghatározott.
- Közepes: Az előzőn túl további megvalósítandó funkció: A szimulált rendszerben korlátozott kapacitású tárolók/várótermek helyezkednek el.
- Jó: Az előzőn túl további megvalósítandó funkció: A műveleteket végrehajtó erőforrások (személyek/munkahelyek) nem folyamatosan állnak rendelkezésre, hanem előre meghatározott időintervallumokban dolgozhatnak.
- Jeles: Az előzőn túl további megvalósítandó funkció: Dolgozzon ki többcélú ütemezési/irányítási módszert, amely alkalmas a szimulált rendszer hatékony irányítására a következő optimalizálási célok szerint:
  - az utolsó munka befejezési időpontja legyen minimális.
  - a legnagyobb csúszás (határidő-túllépés) legyen minimális.
  - a csúszások összege legyen minimális.
  - a határidőt túllépő munkák száma legyen minimális.

A feladat megoldásához felhasználhatók a tantárgy előadásain és gyakorlatain bemutatott modellek, megoldási módszerek és algoritmusok. Mintapéldákon keresztül mutassa be az implementált szoftver működését!

## Megvalósítás

...

A projekt elvileg képes windows és linux rendszereken is futni, de a tesztelés többségét linux alatt végeztem.

## Klónozás

```bash
git clone https://github.com/KissKonradUni/folyamatok_modellezese_beadando.git
```

## Fordítás

```bash
make
```

## Futtatás

```bash
./bin/simulator[.exe]
```

## Licensz

A projekt az [MIT Licensz](./license.txt) alatt érhető el.
