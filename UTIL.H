/* Datei: util.h

   diverse Hilfsroutinen
*/

int UtilFileInput(FILE *file, char *String, int MaxCount);
/* liest Zeichenkette bis zum naechsten Zeilenende aus 'file'; maximal
   werden jedoch 'MaxCount' Zeichen gelesen; 'MaxCount <= 100' 
   erforderlich; als Zeilenende gilt; \r, \n, \r\n, \n\r, EOF;
   Funktionswert: 0: alles normal;
                  1: Dateiende erreicht;
*/

void UtilInput(char *input);
/* liest Eingabe vom Terminal (max. 100 Zeichen) incl. Leerstellen;
   'input' muss beim Aufruf auf genuegend gueltigen Speicherplatz
   verweisen;
*/

void UtilIntInput(int *input);
/* liest Eingabe vom Terminal; die Eingabe wird als Zahl vom Typ
   'int' interpretiert und ihr Wert in '*input' zurueckgegeben
*/

int UtilCopyFile(char *source, char *target);
/* Datei mit dem Namen  source  wird in die Datei mit dem Namen
   target  kopiert.
   Funktionswert: 0: ok; -1: Fehler
*/

int UtilWriteInitString(FILE *file, char *string);
/* Die Angegebene Zeichenkette wird in die angegebene Datei geschrieben.
   Dies geschieht in der Form, die die Zeichenkette als Initiali-
   sierungsstring in einem C-Quelltext verwendbar macht.
   Funktionswert: 0: ok; -1: Fehler
*/

int UtilFileExists(char *file);
/* Es wird festgestellt, ob die Datei mit dem Namen 'file' existiert.
   Funktionswert: 0: existiert nicht; 1: existiert; -1: Fehler;
*/

char *UtilStrlwr(char *string);
/* Die Grossbuchstaben im uebergebenen '*string' werden durch 
   Kleinbuchstaben ersetzt. 
   Funktionswert: Zeiger auf erstes Zeichen des Strings 
                  (gleich 'string')
*/
