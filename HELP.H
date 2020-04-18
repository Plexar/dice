/* Datei: help.h */

void HelpStart(void);

void HelpEnd(void);

void HelpWriteText(void);
/* schreibt den im Modul gespeicherten Hilfetext in die 
   Datei 'helpout.txt'; Format des Inhalts passend zur Verwendung 
   von 'HelpConvertText()' nach Umbenennung von 'helpout.txt' in
   'help.txt'; nach dem Aufruf von 'HelpConvertText()' mit anschliessendem
   Neuuebersetzen des Programms und folgendem Aufruf von 'HelpWriteText()'
   sind die Inhalte der Dateien 'help.txt' und 'helpout.txt' identisch
*/

void HelpConvertText(void);
/* konvertiert den Text in der Datei 'help.txt' in die 
   Hilfe-Initialisierungsdatei 'helpinit.h';
   Format des Hilfetextes in  'help.txt':
   - Zeilen, die mit % beginnen werden als Kommentare behandelt und
     nicht als Hilfetext angezeigt
   - die Kapitel des Hilfetextes werden durch Marken getrennt;
     der Name der Marken fuer die einzelnen Kapitel erhaelt man, indem
     man in den Namen Makros der define-Anweisungen (s. u.) die
     Zeichenkette HELP_ durch ### ersetzt; eine Marke muss am Zeilen-
     anfang stehen
   - Seiten innerhalb der Kapitel sind durch ## am Zeilenanfang getrennt
   - eine Seite darf maximal 23 Zeilen und 79 Zeichen pro Zeile besitzen
*/

void HelpShow(int chapter, char *command);
/* Das angegebene Kapitel des Hilfetextes wird zum Durchblaettern 
   angezeigt. Werte fuer Chapter: defines s. u.;
   In '*command' wird ggf. ein Befehl zurueckgegeben, den die
   Funktion vom Benutzer erhalten hat und nicht selbst verarbeiten
   kann. Dieser Befehl ist dann durch die aufrufende Kommandoschleife
   abzuarbeiten. '*command' muss Platz fuer mindestens 100 Zeichen
   bieten.
*/

#define HELP_FEATURES 0  /* Auflistung der Features des Programms */   
#define HELP_INTRO    1  /* Kurzanleitung fuer das Programm */
#define HELP_CHANGES  2  /* Auflistung der Programmversionen mit den
                            Aenderungen zur jeweils vorangegangenen
                            Version */
#define HELP_COMMANDS 3  /* Auflistung der Befehle des Programms */
#define HELP_TODO     4  /* Auflistung, welche Befehle man in welcher
                            Reihenfolge aufrufen muss, um einen bestimmten
                            Zweck zu erfuellen */
#define HELP_LEGAL    5  /* Auflistung der rechtlichen Hinweise zum
                            Programm */
