/* Datei: games.h
   
   Datenkapsel: Handhabung einer Spieledatenbank
*/

#define GAMES_TABLE_LENGTH 500

int GamesStart(void);
/* Datenkapsel initialisieren;
   Funktionswert: 0: OK; 1: Fehler
*/

int GamesEnd(void);
/* Datenkapsel deinitialisieren;
   Funktionswert: 0: OK; 1: Fehler
*/

int GamesFlush(void);
/* Die Datenbank wird auf den Hintergrundspeicher geschrieben.
   Funktionswert: 0: OK, 1: Fehler
*/

void GamesSearch(char *name, int StartIndex, int mode,
                 int *FoundIndex);
/*
    sucht Spiel mit 'name' in Datenbank; beginnt Suche bei 
    'StartIndex'; 
    mode: Suchmodus; Bedeutung bitweise: 
        Bit 0: Flagge fuer 'Suche exakt':
            1: 'name' gilt als gefunden, wenn identisch mit Spiel in 
                Datenbank;
            0: 'name' gilt als gefunden, wenn 'name' im Namen des 
               Spiels in der Datenbank vorkommt;
        Bit 1: Flagge fuer 'Gross-/Kleinschreibung beachten':
            1: beim Suchen wird die Gross-/Kleinschreibung beachtet
            0: ...nicht beachtet; (langsamer !)
    gefunden: Index in '*FoundIndex';
    nicht gefunden: '*FoundIndex == -1';    
*/

int GamesGet(int index, char **name, char **author, char **publisher);
/* Funktionswert: 0: ok; 1: 'index' fehlerhaft
*/ 

void GamesNew(int *NewIndex, int MergeRecord);

void GamesSet(int index, char *name, char *author, char *publisher,
              int UpdateMergeRecord);
/* UpdateMergeRecord: 
       1: falls es sich bei dem Datensatz, den 'index' angibt, um
          ein Merge-Record handelt, wird dieses nicht automatisch
          in ein normales Record ueberfuehrt;
       0: falls es sich bei dem Datensatz um ein Merge-Record handelt,
          wird dieses in Zukunft normal behandelt
*/

void GamesGetVotes(int index, int *points, int *CountOfVoters);

void GamesSetVotes(int index, int points, int CountOfVoters);

int GamesTableSize(void);
/* Index des ersten unbenutzten Tabellenelements */

void GamesDelete(int index);

void GamesSort(void);
/* Die Spieledatenbank wird alphabetisch nach den Namen der Spiele
   sortiert.
*/

int GamesTestMergeRecord(int index);
/* Funktionswert: 1: Spiel mit angegebenem 'index' ist ein 
                     Merge-Record
                  0: sonst
*/

void GamesWriteInit(void);
/* Der Inhalt der Spieledatenbank wird als in die Initialisierungsdatei
   gameinit.h  ausgegeben.
*/
