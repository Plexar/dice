/* Datei: tab.h */

#define TabMaxPlayers 100
    /* maximale Anzahl der Spieler, die verwaltet werden koennen  */

#define TabNameLength 50
    /* maximale Anzahl der Buchstaben, die der Name eines Spielers
       oder eines Spieles besitzen darf */

#define TAB_FORMAT_IDENT  4
    /* Identifikation der Version des Formats, das die Funktionen
       'TabWriteFile' und 'TabReadFile' fuer die Spielerdateien
       voraussetzen; ausserdem wird noch verstanden: 1, 2, 3
    */

typedef char tTabName[ TabNameLength ];
    /* Name eines Spielers */

typedef struct{
    int offense, defense;
} tsTabOneThird;
    /* Daten fuer ein Drittel */

typedef struct{
    char     GotFrom[10]; /* Kuerzel der Datei, deren Daten fuer die
                             aktuelle uebernommen wurden */
    char     myself[10]; /* Kuerzel der eigenen Datei */
    int      game; /* verwendetes Spiel: 
                         0: keins, 1: Eishockey, 2: Tennis, 3: beide
                   */
    int      IceMode;    
    int      TennisMode;
                   /* jeweils: 0: Normalmodus; 1: jeder gegen jeden */
    int      IceRankList;
    int      TennisRankList;
                   /* jeweils: 0: ausgeschaltet; 1: eingeschaltet */
    int      ChartSum;
                   /* 0: ausgeschaltet; 1: eingeschaltet */
} tsTabFileRecord;
    /* Datensatz einer Spielerdatei */
    
typedef struct{
    tTabName name;
    int      index; /* aktueller Index in der Liste aller Spiel;
                       -1: Index bisher nicht bestimmt
                    */
} tsTabGameRecord;
    /* Datensatz eines Spiels */

typedef struct{
    tTabName      name;   /* Name des Spielers */
    tsTabOneThird data[4]; /* Staerken fuer jedes Drittel und
                                  sudden death */
    int           IceValid; /* boolean: 1 --> nimmt am Eishockey teil
                                        0 --> nimmt nicht teil
                            */
    int           member; /* boolean: 1 --> Mitglied;
                                      0 --> kein Mitglied;
                          */
    tsTabGameRecord charts[5];
                          /* Auswahl HippoCharts des Spielers */                                    
    char          IceUpdatedIn[5]; 
                          /* Kuerzel der Datei, in der die Eishockey
                             Aufstellung zuletzt vom Benutzer ge-
                             aendert wurde */
    int           IceUpdateCount;
                          /* Anzahl der Uebernahmen des Datensatzes
                             aus einer alten Datei, ohne dass seitdem
                             die Eishockey Aufstellung vom Benutzer 
                             geaendert wurde */
    int           tennis[3]; /* Staerken fuer die drei Tennissaetze */    
    int           TennisValid; /* boolean: 1 --> nimmt am Tennis teil
                                           0 --> nimmt nicht teil
                               */
    char          TennisUpdatedIn[5];
                          /* wie 'IceUpdatedIn' jedoch fuer Tennis */
    int           TennisUpdateCount;
                          /* wie 'IceUpdatedIn' jedoch fuer Tennis */
    int           IceRankPoints[10];
                          /* Eishockey-Ranglisten-Punkte */
    int           IceOldRankHead;
                          /* alter RankHead-Wert fuer Sonderpunkte */
    int           IceNewRankHead;
                          /* neuer RankHead-Wert fuer Sonderpunkte */
    int           TennisRankPoints[10];
                          /* Tennis-Ranglisten-Punkte */
    int           TennisOldRankHead;
                          /* alter RankHead-Wert fuer Sonderpunkte */
    int           TennisNewRankHead;
                          /* neuer RankHead-Wert fuer Sonderpunkte */
} tsTabPlayer, *tpTabPlayer;
    /* Datensatz eines Spielers */

/*********** Modul- und Dateimanagement **************/

void TabStart(void);
/* Die Tabelle zur Verwaltung der Spieler wird initialisiert. */

void TabEnd(void);
/* Die Tabelle zur Spielerverwaltung wird deinitialisiert. */

int TabReadFile(void);
/* Die Tabelle wird mit Hilfe der Daten in der neu gefuellt,
   die mit TabSetName zuvor festgelegt wurde.
   Funktionswert: 0: OK, 1: Fehler */

int TabWriteFile(void);
/* Die Tabelle wird in die Datei geschrieben, die mit TabSetName
   zuvor festgelegt wurde.
   Funktionswert: 0: OK; 1: Fehler  */

int TabCopyFile(char *source, char *target);
/*
    Die Datei mit dem Kuerzel '*source' wird in die Datei mit dem
    Kuerzel '*target' kopiert.
    
    Funktionswert: 0: ok; 1: Fehler
*/

int TabFileExists(char *file, int *FileExists);
/* 
    Es wird getestet, ob die Datei mit dem Kuerzel (d. h. 4 Zeichen !)
    '*file' existiert; '*FileExists == 1' --> ja; '*FileExists == 2' 
    --> nein;
    
    Funktionswert: 0: ok; 1: Fehler    
*/
   
/************ Header-Datensatz *************/

char *TabGetMyself(void);
/* liefert Kuerzel der eigenen Datei */

void TabSetGame(int flag);
/* setze Flagge fuer ein-/ausgeschaltete Spiele:
   0: kein Spiel, 1: Eishockey, 2: Tennis, 3: beide
*/

int TabGetGame(void);
/* lese Flagge fuer ein-/ausgeschaltete Spiele
*/

void TabSetIceMode(int mode);
/*
    Tourniermodus: 0: KO-System; 1: jeder gegen jeden
*/

void TabGetIceMode(int *mode);

void TabSetTennisMode(int mode);
/*
    0: Normalmodus; 1: jeder gegen jeden
*/

void TabGetTennisMode(int *mode);

void TabGetIceRankList(int *flag);
/* 0: ausgeschaltet; 1: eingeschaltet */

void TabSetIceRankList(int flag);

void TabGetTennisRankList(int *flag);

void TabSetTennisRankList(int flag);

void TabSetChartSum(int flag);

int TabChartSum(void);

/************ Handhabung aller Datensaetze ************/

void TabSetGamesReferences(void);
/* Referenzen Spielertabelle-->Spieledatenbank werden gesetzt */

void TabDeleteGamesReference(int index);
/* Referenz 'index' Spielertabelle-->Spieledatenbank wird geloescht */

void TabDeleteAllGamesReferences(void);
/* 
    alle Referenzen Spielertabelle-->Spieledatenbank werden geloescht 
*/

int TabGetFirstUnused(void);
/* liefert als Funktionswert den Index des ersten unbenutzten 
   Eintrags in der Stimmzetteltabelle
*/

void TabSweepAfterCopyFile(void);
/*
    Die im Speicher befindliche Datei wird bereinigt in Annahme einer
    vorangegangenen Kopieroperation mit 'TabCopyFile()'. 
    
    Auswirkungen:
    - Eishockey- und Tennismannschaften altern (Zaehler fuer Ueber-
      nahmen wird inkrementiert)
    - Ranglistenpunkte altern (Werte werden in der Liste nach hinten
      geschoben; aktueller Wert wird auf 0 gesetzt; Wert fuer 
      'RankHead-Sonderpunkte (--> vgl. entsprechende Funktionen)
      in 'New' wird auf 'Old' uebertragen)
    - Eishockey- und Tennismannschaften, deren Aufstellungen zu lange
      nicht geaendert wurden, werden deaktiviert
    - Eintraege deren Eishocke- und Tennismannschaften deaktiviert
      sind und die keine Ranglistenpunkte aufweisen, werden geloescht
    - HippoWINNER-Teilnahme wird ausgeschaltet
*/

void TabSearch(char *name, int StartIndex, int mode,
               int *FoundIndex);
/*
    sucht Spieler mit 'name' in Stimmzetteldatei; beginnt Suche bei 
    'StartIndex'; 
    mode: Suchmodus; Bedeutung bitweise: 
        Bit 0: Flagge fuer 'Suche exakt':
            1: 'name' gilt als gefunden, wenn identisch mit Spieler-
                name in Datenbank;
            0: 'name' gilt als gefunden, wenn 'name' im Namen des 
               Spielers in der Datenbank vorkommt;
        Bit 1: Flagge fuer 'Gross-/Kleinschreibung beachten':
            1: beim Suchen wird die Gross-/Kleinschreibung beachtet
            0: ...nicht beachtet;
    gefunden: Index in '*FoundIndex'; 
    nicht gefunden: '*FoundIndex == -1';    
*/

void TabSort(void);
/* Der Inhalt der Spielertabelle wird alphabetisch nach dem Namen
   sortiert (Bubble-Sort).
*/

void TabPrint(void);
/* Die Spielertabelle wird auf den Bildschirm ausgegeben.
*/

/************ Handhabung einzelner Datensaetze ***********/

int TabNew(void);
/* Funktionsergebnis ist der Index eines neuen Tabellenelementes.
   Dieser Index ist nur g"ultig, solange die Tabelle nicht
   sortiert wurde.
*/

void TabSetName(int index, tTabName name);
/* Fuer den Datensatz mit dem angegebenen 'index' wird der Name
   neu gesetzt.
*/

char *TabGetName(int index);
/* Funktionswert: Name des Spielers, dessen Index der Parameter 
   angibt
*/

int TabDelete(int index);
/* Der Datensatz mit dem angegebenen Index wird geloescht.
   Funktionswert: 0: ok; -1: Fehler aufgetreten;
*/

/************** HippoWINNER ****************/

void TabSetMember(int index, int member);
/* Fuer den Datensatz mit dem angegebenen 'index' wird die 
   Mitgliedschaft neu gesetzt.
*/

int TabGetMember(int index);
/* Funktionswert: mit 'TabSetMember()' gesetzter Wert fuer die Mitglied-
                  schaft
*/

/************ HippoCHARTS *************/

void TabSetCharts(int index, int rank, char *name, int GameIndex);
/* Fuer den Datensatz mit dem angegebenen 'index' wird der
   Charts-Spielename fuer den mit 'rank' angegebenen Platz neu
   gesetzt.
*/

void TabGetCharts(int index, int rank, char **name, int *GameIndex);
/*
    Aus dem Datensatz mit dem angegebenen 'index' wird der 
    Charts-Spielename und der zugehoerige Index fuer den mit 'rank'
    angegebenen Platz gelesen.
*/

/************** Eishockey **************/

void TabGetIce(int index, 
               int *o1, int *d1, int *o2, int *d2, 
               int *o3, int *d3, int *o4, int *d4);
/* Fuer den Datensatz mit dem angegebenen 'index' werden die
   Daten fuer das Eishockey-Spiel gelesen
*/

void TabSetIce(int index, 
               int o1, int d1, int o2, int d2, 
               int o3, int d3, int o4, int d4);
/* Fuer den Datensatz mit dem angegebenen 'index' werden die
   Daten fuer das Eishockey-Spiel neu gesetzt.
*/

void TabSetIceValid(int index, int valid);

void TabGetIceValid(int index, int *valid);

void TabGetIceUpdatedIn(int index, char **IceUpdatedIn);

void TabGetIceUpdateCount(int index, int *UpdateCount);

void TabSetIceRankPoints(int index, int points);
/* Ranglisten-Punkte des aktuellen Zeitraumes setzen */

void TabGetIceRankPoints(int index, int *points);
/* Ranglisten-Punkte des aktuellen Zeitraumes lesen */

void TabGetIceRankSum(int index, int *points);
/* Summe aller Ranglisten-Punkte lesen (--> bestimmend fuer Ranglisten-
   Platz) */

void TabGetOldIceRankHead(int index, int *flag);
/* alten RankHead-Wert lesen (bestimmend fuer Ranglisten-Sonderpunkte 
   in der aktuellen Auswertung) */

void TabSetNewIceRankHead(int index, int flag);
/* neuen RankHead-Wert setzen (bestimmend fuer Ranglisten-Sonderpunkte
   der Auswertung im naechsten Zeitraum */

void TabGetNewIceRankHead(int index, int *flag);
/* neuen RankHead-Wert lesen */

/************* Tennis *************/

void TabGetTennis(int index, int *s1, int *s2, int *s3);

void TabSetTennis(int index, int s1, int s2, int s3);

void TabGetTennisValid(int index, int *TennisValid);

void TabSetTennisValid(int index, int TennisValid);

void TabGetTennisUpdatedIn(int index, char **UpdatedIn);

void TabGetTennisUpdateCount(int index, int *UpdateCount);

void TabSetTennisRankPoints(int index, int points);
/* Ranglisten-Punkte des aktuellen Zeitraumes setzen */

void TabGetTennisRankPoints(int index, int *points);
/* Ranglisten-Punkte des aktuellen Zeitraumes lesen */

void TabGetTennisRankSum(int index, int *points);
/* Summe aller Ranglisten-Punkte lesen (--> bestimmend fuer Ranglisten-
   Platz) */

void TabGetOldTennisRankHead(int index, int *flag);
/* alten RankHead-Wert lesen (bestimmend fuer Ranglisten-Sonderpunkte 
   in der aktuellen Auswertung) */

void TabSetNewTennisRankHead(int index, int flag);
/* neuen RankHead-Wert setzen (bestimmend fuer Ranglisten-Sonderpunkte
   der Auswertung im naechsten Zeitraum */

void TabGetNewTennisRankHead(int index, int *flag);
/* neuen RankHead-Wert lesen */
