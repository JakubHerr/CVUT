## Shrnutí zadání
- úkolem programu je implementovat databázi automobilů a jejich majitelů
- unikátním ID auta je registrační značka
- unikátním ID člověka je kombinace jména a příjmení
- auto má vždy právě jednoho vlastníka
- jeden člověk může vlastnit více aut
- program se skládá z  povinných tříd **CRegister**, **CCarList** a **CPersonList**

## CRegister
- hlavní třída programu, slouží k interakci s DB
- **bool AddCar(RZ,name,surname)**
  - pokud auto není v DB, přidá ho a vrátí true
  - jinak vrátí false
- **bool DelCar(RZ)**
  - pokud auto je v DB, odebere ho a vrátí true
  - jinak vrátí false
  - odebere majitele z DB, pokud již nevlastní žádné auto
- **bool Transfer(RZ,name,surname)**
  - pokud auto není v DB, nebo je současný a nový majitel ten samý člověk, vrátí false
  - jinak přepíše auto na nového majitele a vrátí true
  - starý majitel je odebrán z DB, pokud již nevlastní žádné auto
- **CCarList ListCars(name,surname)**
  - vrací seznam všech aut vlastněných daným člověkem
- **int CountCars(name,surname)**
  - vrací počet aut, které daný člověk vlastní (0 pokud není v DB)
- **CPersonList ListPersons()**
  - vrací seznam všech lidí v DB

## CCarList
- jednosměrně zřetězený spojový seznam, slouží k procházení RZ aut vlastněných jedním člověkem
- má metody pro přístup k prvku (RZ), posunu (Next) a kontrole konce seznamu (AtEnd)
- odkazuje se na data v DB, nevytváří jejich kopii

## CPersonList
- jednosměrně zřetězený spojový seznam, slouží k procházení všech osob v DB
- má podobné metody jako CCarList
- odkazuje se na data v DB, nevytváří jejich kopii

## Specifika zadání
- program musel být časově a paměťově efektivní
- jediný povolený kontejner z STL byl vektor


