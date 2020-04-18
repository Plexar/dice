/* Datei: rnd.h
   Datum: 25.01.99
   
   Zuordnung der Generatoren:
       0: Default
       1: HippoWINNER Korrektur-Auswahl nach dem Wuerfeln
       2: HippoWINNER Teilnehmerauswahl
       3: HippoWINNER Punktewuerfel
       4: HippoWINNER Gewinner-Auswahl
*/

#define RND_COUNT 10
    /* Anzahl der Zufallsgeneratoren */

int RndStart(char *FileName);
/* Start Datenkapsel 'Rnd'; Funktionswert: 0->OK, 1->Fehler;
   Eingabeparameter:
       FileName: Name der Datei, in der der Anfangswerte gespeichert 
                 sind
*/

int RndEnd(void);
/* Ende Datenkapsel 'Rnd'; Funktionswert: 0->OK, 1->Fehler */

int RndGet(int low, int high);
/* Funktionsergebnis ist eine gleichverteilte ganzzahlige Zufallszahl
   im geschlossenen Intervall von 'low' bis 'high'.  
*/

void RndSelectGenerator(int number);
/* fuer die weitere Berechnung von Zufallszahlen wird der Zufalls-
   generator mit der angegebenen 'number' verwendet
*/

void RndGetStartNumber(float *number);
/* die aktuell gueltige Anfangszahl wird in 'number' zurueckgegeben */

void RndSetStartNumber(float number);
/* die aktuell gueltige Anfangszahl wird gesetzt */
