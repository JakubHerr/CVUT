## Shrnutí zadání
- úkolem je implementovat třídu **CExam**, která slouží k organizaci testů
- každý student má unikátní číselný identifikátor (**studentID**)
- každý student má alespoň jednu kartu, může jich mít víc
- identifikátor karty (**CardID**) je alfanumerický řetězec
- identifikátorem testu je jeho název (**testName**)

## CExam
- **Load(istream &cardMap)**
  - načte z vstupního streamu studenty (**studentID**, studentName) a jejich karty (**CardID**)
  - **studentID** a **CardID** se nesmí opakovat (kontroluje i údaje načtené při přechozím volání funkce)
  - na každém řádku vstupu je jeden student a jeho karty
  - formát řádku
    - studentID:studentName:cardID, cardID, ... ,cardID\n
  - pokud má stream špatný formát, všechna nově načtená data se zahodí 
- **Register(cardID,testName)**
  - registruje studenta na test
- **Assess(studentID,testName,grade)**
  - přidělí studentovi hodnocení z testu
- **ListTest(testName,sortBy)**
  - vypíše výsledky z testu, seřazené podle volitelných kritérii
- **ListMissing(testName)**
  - vypíše studenty, kteří ještě nemají test ohodnocený