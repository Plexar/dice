/* Datei: players.h

   Datenkapsel: Handhabung einer Mitgliederdatenbank
   
   <Stand  dice V3.1 11/98  : 
       Modul unbenutzt, ungetestet, uebersetzbar, Kopie von Modul  
       games mit Minimalaufwand angepasst --> Anpassung unvoll-
       staendig>
*/

#define PLAYERS_TABLE_LENGTH 50

int PlayersStart(void);
/* Datenkapsel initialisieren;
   Funktionswert: 0: OK; 1: Fehler
*/

int PlayersEnd(void);
/* Datenkapsel deinitialisieren;
   Funktionswert: 0: OK; 1: Fehler
*/

int PlayersFlush(void);
/* Die Datenbank wird auf den Hintergrundspeicher geschrieben.
   Funktionswert: 0: OK, 1: Fehler
*/

void PlayersSearch(char *name, int StartIndex, int exact,
                 int *FoundIndex);
/*
    sucht Spiel mit 'name' in Datenbank; beginnt Suche bei
    'StartIndex'; exact= 1: 'name' gilt als gefunden, wenn identisch
    mit Spiel in Datenbank; exact= 0: 'name' gilt als gefunden, wenn
    'name' im Namen des Spiels in der Datenbank vorkommt;
    gefunden: Index in '*FoundIndex';
    nicht gefunden: '*FoundIndex == -1';
*/

void PlayersNew(int *NewIndex);
/* neuen Datensatz anlegen */

/* XXXX */
void PlayersGetAdr(int index, char **FirstName, char **LastName,
                   char **street, char **city);
void PlayersGetMember(int index, int *member);
void PlayersGetCountDown(int index, int *ChangeCountDown);
void PlayersGetRecognized(int index, int *recognized);
void PlayersGetIce(int index,
                   int *o1, int *d1, int *o2, int *d2,
                   int *o3, int *d3, int *o4, int *d4);

/* XXXX */
void PlayersSetAdr(int index, char *FirstName, char *LastName,
                   char *street, char *city);
void PlayersSetMember(int index, int member);
void PlayersSetCountDown(int index, int ChangeCountDown);
void PlayersSetRecognized(int index, int recognized);
void PlayersSetIce(int index,
                   int o1, int d1, int o2, int d2,
                   int o3, int d3, int o4, int d4);

int PlayersTableSize(void);
/* Index des ersten unbenutzten Tabellenelements */

void PlayersDelete(int index);

void PlayersSort(void);
/* Die Mitgliederdatenbank wird alphabetisch nach den Nachnamen der
   Mitglieder sortiert.
*/
