/*
    Datei: out.h
    
    Ausgabe von Auswertungsdateien in verschiedenen aus der ASCII-
    Darstellung abgeleiteten Formaten
*/

#define OUT_FORMAT_TEX         1
#define OUT_FORMAT_ASCII       2
#define OUT_FORMAT_HTML        3
#define OUT_FORMAT_POSTSCRIPT  4
/* defines kompatibel zu STAT_FORMAT_... defines im Modul  Stat */

int OutSelectFormat(int format);
/* format: 1: TeX (voreingestellt), 2: ASCII, 3: HTML, 4: Postscript
           (defines s. o.)
   Funktionswert: 0: OK; 1: Fehler
*/

void OutSetVersionString(char *VersionString);
/* Es wird die in den Ausgabedateien anzugebende Programmversion
   festgelegt.
*/

int OutText(char *text);
/* In die aktuelle Ausgabedatei wird der angegebene Text 
   ausgegeben. Das Erscheinungsbild im fuer den Leser sichtbaren 
   Ergebnistext ist formatabhaengig: 
       TeX  : *text wird ohne \n am Zeilenende in die Ausgabedatei
              ausgegeben (Formatierung automatisch durch TeX).
       ASCII: Ausgabe in die Ausgabedatei wie bei TeX (keine weitere
              automatische Formatierung)
   Funktionswert: 0: OK; 1: Fehler
   
   zu tun (noch nicht implementiert): 
       Behandlung von Sonderzeichen in  *text
*/

int OutNewLine(void);
/* In die aktuelle Ausgabedatei wird eine Steuersequenz geschrieben,
   die sicherstellt, dass die aktuelle Zeile im fuer den Leser
   sichtbaren Ergebnistext beendet wird. Die von dieser Funktion
   produzierten Ergebnisse sind formatabhaengig:
       TeX  : \\\n
       ASCII: \n
       
   Funktionswert: 0: ok; 1: Fehler
*/

int OutSpace(void);
/* In die aktuelle Ausgabedatei wird eine Steuersequenz geschrieben,
   die sicherstellt, dass an der aktuellen Stelle im fuer den Leser
   sichtbaren Ergebnistext ein leicht vergroesserter Zwischenraum
   erscheint. Die von dieser Funktion produzierten Ergebnisse sind
   formatabhaengig:
       TeX  : \hspace*{2em}
       ASCII: <2 Leerstellen>

   Funktionswert: 0: ok; 1: Fehler
*/

int OutComment(char *text);
/* In die aktuelle Ausgabedatei wird der angegebene Text als 
   Kommentar, der nicht im Text erscheinen soll, ausgegeben.
   Die von dieser Funktion produzierten Ergebnisse sind 
   formatabhaengig:
       TeX  : % *text \n
       ASCII: Kommentar wird nicht ausgegeben (Funktion wirkungslos)

   Funktionswert: 0: OK; 1: Fehler
*/

int OutTexMain(void);
/* Mit Hilfe des durch  StatAbbrev()  angegebenen Kuerzels wird eine 
   TeX-Hauptdatei geschrieben. 
   
   Funktionswert: 0: OK, -3: Fehler
*/

/****** Funktionen fuer HippoWINNER: ******/

int OutWinnerStart(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutWinnerTableStart(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutWinnerTableEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutWinnerTableLine(char *PlayerName,
        int a, int b, int c, int d, int e, int sum);
/* Funktionswert: 0: OK, 1: Fehler */

int OutWinnerEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */

/****** Funktionen fuer HippoCHARTS: ******/

int OutChartsStart(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutChartsTableStart(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutChartsTableEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutChartsTableLine(int rank, int points, int sum, int count, 
                       char *name, char *publisher, char *author);
/* rank: <0 --> no rank;
   Funktionswert: 0: OK, 1: Fehler 
*/                       

int OutChartsEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */

/****** Funktionen fuer Eishockey und Tennis: ******/

int OutGameStart(int game);
/* game: 1: Eishockey, 2: Tennis;
   Funktionswert: 0: OK, 1: Fehler 
*/

int OutGameTableStart(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutGameTableEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutGameTableLine(int rank, char *name, int wins, int points);
/* rank: <0 --> no rank;
   Funktionswert: 0: OK, 1: Fehler 
*/

int OutGameEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */

/****** Funktionen fuer HippoCHARTS--Sammelauswertung: ******/

int OutChartSumStart(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutChartSumTableStart(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutChartSumTableEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */

int OutChartSumTableLine(int rank, int points, 
                          char *name, char *publisher, char *author);
/* rank: <0 --> no rank;
   Funktionswert: 0: OK, 1: Fehler 
*/                       

int OutChartSumEnd(void);
/* Funktionswert: 0: OK, 1: Fehler */
