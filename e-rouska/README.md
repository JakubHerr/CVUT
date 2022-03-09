## Shrnutí zadání
- úkolem programu je vyhledávat kontakty mezi lidmi, podobně jako e-rouška
- program obsahuje třídy **CFaceMask**, **CTimeStamp** a **CContact**
- jako unikátní identifikátor člověka slouží tel. číslo

## CFaceMask
- hlavní třída programu, obsahuje DB kontaktů a dále s nimi pracuje
- **addContact** přidá nový kontakt do databáze
  - kontakty jsou seřazeny podle pořadí, ve kterém byly přidány
- **listContacts(id člověka)**
  - vrací vektor id lidí, kteří byli s hledaným člověkem v kontaktu
  - vektor id je seřazen podle pořadí přidání kontaktu do DB a neobsahuje duplicity
- **listContacts(id člověka, začátek, konec)**
  - vrací vektor id lidí, kteří byli s hledaným člověkem v kontaktu v časovém intervalu \<začátek,konec\>
  - vektor id je seřazen podle pořadí přidání kontaktu do DB a neobsahuje duplicity

## CTimeStamp
- zapouzdřuje časové razítko (rok, měsíc, den, hodina, minuta, sekunda)
- nemá za úkol validovat čas

## CContact
- zapouzdřuje kontakt mezi dvěma lidmi
- obsahuje id osob a časové razítko