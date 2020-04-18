/*
    Datei: stat.h
    
    Verwaltung des Programmzustandes; Speicherung ausgewaehlter 
    Zustandsparameter in einer Datei zur Aufbewahrung bis zum 
    naechsten Programmstart
    
    restliche Module des Programms verwenden  Stat  um globale
    Programmzustandsdaten zu speichern
*/

int StatStart(void);
/* Statusdatei wird gelesen;
   Funktionswert: 0: ok; -1: Fehler
*/

int StatEnd(void);
/* Statusdatei wird geschrieben, falls noetig;
   Funktionswert: 0: ok; -1: Fehler
*/

void StatSetPreset(int flag);
/* flag: 
       1: Aenderungen der Modus-Einstellungen werden fuer die Befehle 
          me, mm, mn, ms und mt als Voreinstellung fuer neu angelegte 
          Stimmzettel-Dateien gespeichert, fuer die keine Stimmzettel 
          aus einer Alt-Datei uebernommen werden;
       0: ...werden nicht als Voreinstellungen uebernommen
*/

int StatPreset(void);
/* Funktionswert: zuvor mit 'StatSetPreset' gesetzter Wert fuer 'flag' 
*/

/******/

void StatSetDebugMode(int mode);
/* mode: 1: ein, 0: aus; 
   wird nicht in Statusdatei gespeichert 
*/

int StatDebugMode(void);

/******/

void StatSetAbbrev(char *SetAbbrev);
/* Kuerzel der aktuellen Stimmzetteldatei */

char *StatAbbrev(void);

/******/

void StatSetOutFormat(int format);
/* aktuelles Ausgabeformat fuer Auswertungsdateien; moegliche
   Werte fuer  format  : defines s. u. */

int  StatOutFormat(void);

#define STAT_FORMAT_TEX         1
#define STAT_FORMAT_ASCII       2
#define STAT_FORMAT_HTML        3
#define STAT_FORMAT_POSTSCRIPT  4

/******/

char *StatFileName(int purpose);
/* Der ggf. anhand des mit  StatSetAbbrev()  festgesetzten Kuerzels
   und des mit  StatSetOutFormat()  festgesetzten Ausgabeformats
   automatisch generierte Dateiname fuer den mit  purpose  angegebenen
   Zweck wird zurueckgegeben. 
   Funktionswert:  purpose  gueltig und jeweilige Datei soll 
   
   StatSetGame() StatSetChartSum()
   Moeglich Werte fuer  purpose  :
*/

/* permanete vom Programm verwaltete Dateien (z. B. Datenbanken): */
#define STAT_RANDOM           1 /* Zufallszahlen fuer Modul  rnd  */
#define STAT_GAMES            2 /* Spieledatenbank fuer Modul  games  */
#define STAT_STAT             3 /* Statusdatei fuer Modul  stat  */
#define STAT_CHART_SUM_DATA   4 /* Ergebnisdatenbank fuer HippoCHARTS-
                                  Sammelauswertung */
#define STAT_CHART_SUM_BACKUP 5 /* Backup der letzten konsistenten 
                                   ungeaenderten Version von
                                   STAT_CHART_SUM  bei Aenderungen an
                                   dieser Datei */                                  
#define STAT_CHART_SUM_HELP   6 /* Hilfsdatei fuer Bearbeitung der
                                   Datei  STAT_CHART_SUM */

/* Dateien fuer die jeweilige HippoPRESS-Auswertung: */
#define STAT_DATA             10 /* Stimmzetteldatei der aktuellen
                                    Auswertung */
#define STAT_MAIN             11 /* Hauptdatei der generierten Auswertungen
                                    (im TeX-Format die zu uebersetzende
                                     TeX-Hauptdatei) */
#define STAT_MAIN_BACKUP      12 /* Backup der alten Version von 
                                    STAT_MAIN */
#define STAT_WINNER           13 /* HippoWINNER */
#define STAT_CHARTS           14 /* HippoCHARTS */
#define STAT_ICEHOCKEY        15 /* HippoGAMES: Eishockey */
#define STAT_TENNIS           16 /* HippoGAMES: Tennis */
#define STAT_CHART_SUM        17 /* HippoCHARTS-Sammelauswertung */

/******/

void StatSetGame(int SetGame);
/* SetGame:  0: kein Spiel; 1: Eishockey; 2: Tennis; 3: beide
             (jedes Spiel ein Bit: Eishockey: Bit 0; Tennis: Bit 1)
*/

int StatGame(void);

/******/

void StatSetIceTournMode(int mode);

int StatIceTournMode(void);

/******/

void StatSetTennisTournMode(int mode);

int StatTennisTournMode(void);

/******/

void StatSetVersionString(char *string);
/* wird nicht Statusdatei gespeichert */

char *StatVersionString(void);

/******/

void StatSetChartSum(int flag);
/* flag: 1: HippoCHARTS-Sammelauswertung soll stattfinden;
         0: sonst
*/

int StatChartSum(void);

/******/

void StatSetChartSumPattern(char *pattern);
/* pattern: aktuelles Kuerzel-Muster der fuer die Sammelauswertung
            zu beachtenden Daten
*/

char *StatChartSumPattern(void);

/******/

void StatSetIceRankList(int flag);
/* Eishockey-Rangliste: 0: aus; 1: ein */

int StatIceRankList(void);

/******/

void StatSetTennisRankList(int flag);
/* Tennis-Rangliste: 0: aus; 1: ein */

int StatTennisRankList(void);
