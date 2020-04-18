/*
    Datei: sum.h
    
    Verwaltung der Daten fuer eine HippoCHARTS-Sammelauswertung
*/

int SumStart(void);
/* Funktionswert: 0: ok; -1: Fehler */

int SumEnd(void);
/* Funktionswert: 0: ok; -1: Fehler */

/* fuer die Zukunft:
int SumPrepareAdd(void);
* wiederholte Aufrufe von 'SumAdd()' vorbereiten;
   Funktionswert: 0: ok; -1: Fehler 
*

int SumFinishAdd(void);
* nach vorangegangenem Aufruf von 'SumPrepareAdd()' und mindestens 0
  Aufrufen von 'SumAdd()' das Hinzufuegen abschliessen;
  Funktionswert: 0: ok; -1: Fehler
*/

int SumAdd(char *abbrev, char *name, int points);
/* neuen Eintrag hinzufuegen; muss durch 'SumPrepareAdd()' vorbereitet
   und durch 'SumFinishAdd()' nachbereitet werden;
   Funktionswert: 0: ok; -1: Fehler
*/

int SumDelete(char *abbrev);
/* Alle Eintraege mit dem angegebenen Kuerzel werden geloescht.
   Funktionswert: 0: ok; -1: Fehler
*/

int SumSetPattern(char *pattern);
/* ? ist Wildcard; filtert Ergebnisse von  SumNextEntry()  anhand des
   Musters;
   Funktionswert: 0: ok; -1: Fehler
*/

int SumListEntries(void);
/* SumNextEntry()-Aufrufe vorbereiten;
   Funktionswert: 0: ok; -1: Fehler;
*/

int SumNextEntry(char *abbrev, char *name, int *points);
/* Funktionswert: 0: ok; -1: Fehler; 1: Ende erreicht (Parameter
       ungueltig); nach SumListEntries() muss mindestens einmal
       SumNextEntry() aufgerufen werden, damit geoeffnete Dateien 
       wieder ordnungsgemaess geschlossen werden; nachdem 
       SumNextEntry() angefangen hat 1 als Funktionsergebnis zu 
       liefern, kann durch wiederholte Aufrufe von SumNextAbbrev()
       festgestellt werden, welche Kuerzel geliefert wurden
*/

int SumListAbbrev(void);
/* SumNextAbbrev()-Aufrufe werden vorbereitet;
   Funktionswert: 0: ok; -1: Fehler;
*/

int SumNextAbbrev(char *abbrev);
/* Funktionswert: 0: ok; -1: Fehler; 1: Ende erreicht (Parameter
       ungueltig)
*/

int SumListPoints(void);
/* SumNextPoints()-Aufrufe werden vorbereitet; durch wiederholte
   Aufrufe von SumNextAbbrev() kann festgestellt werden, welche Ein-
   zelauswertungen in die Sammelauswertung eingeflossen sind
   Funktionswert: 0: ok; -1: Fehler;
*/

int SumNextPoints(char *name, int *points);
/* Die gespeicherten Punktzahlen fuer Eintraege gleichen Namens werden
   addiert. Dabei werden jedoch nur Eintraege beruecksichtigt, die
   anhand des mit  SumSetPattern()  gesetzten Suchmusters gueltig
   sind. Die Ergebniseintraege werden nach den Summen sortiert
   und mit dem Eintrag, der die hoechsten Summe besitzt, zuerst in 
   aufeinanderfolgenden Aufrufen zurueckgegeben.
   Funktionswert: 0: ok; -1: Fehler; 1: Ende erreicht (Parameter 
       ungueltig)
*/
