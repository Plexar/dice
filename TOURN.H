
/* Datei: tourn.h

   Berechnung eines Tourniers anhand der im Modul  Tab  abgelegten
   Daten; das Ergebnisprotokoll wird mit Hilfe des Moduls  Out
   geschrieben; zur Verwaltung des Programmstatus wird auf das Modul
   Stat  zurueckgegriffen
*/

#define TOURN_ICEHOCKEY 1
#define TOURN_TENNIS 2

void Tournament(int game);
/* Anhand der geladenen Spielerdatei wird ein Tournier veranstaltet.
   Das Ergebnis wird in eine Protokolldatei ausgegeben. Die Namen
   der Protokolldateien werden automatisch mit Hilfe des Moduls  Stat
   bestimmt.
   game== 1: Eishockey-Tournier
   game== 2: Tennis-Tournier
   (defines s. o.)
*/
