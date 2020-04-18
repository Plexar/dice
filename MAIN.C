/* Programm: Dice
   Version : 4.3
   Datum   : 08.04.2006
   Autor   : Holger Burbach, Harkortstr. 43, 44225 Dortmund

   Datei   : main.c

   Dieses Programm dient zur Auswertung von Stimmzetteln.
   Das Programm verwendet keine systemspezifischen Aufrufe und ist
   selbsterklaerend.

   Es ist in ANSI-C geschrieben und sollte auf jedem C-Compiler
   uebersetzt werden koennen, der ANSI-C unterstuetzt.
   Uebersetzt und getestet wurden verschiedene Versionen des
   Programms (insbesondere V2.2, V2.99 alpha, V3.0) ohne weitere
   Aenderungen auf verschiedenen Systemen, u. a. den folgenden:
   - Atari ST, TOS 2.06, Pure C V1.1
   - Intel 80486, Windows 95, Microsoft Visual C++ V1.50
         - verwendete Einstellungen:
               MS-DOS Application, Large Memory Model
   - Intel Pentium-MMX 200, Windows NT 4.0, Microsoft Visual C++ fuer
     Windows NT (verwendete Compiler-Einstellung: Console Application)
   - Intel Pentium-MMX 200, Windows 95, Microsoft Visual C++ fuer
     Windows NT (!) (Programm unter Windows NT compiliert)
   - Sun Sparc 20, Solaris 2.4, Gnu C
         - Befehl zum kompilieren: gcc -o dice -lm *.c
         - Nach der Uebertragung der Quelltext vom Atari ST oder
           einem MS-DOS Rechner auf die Sun sind alle Grossbuchstaben
           in den Namen der Quelltextdateien durch die entsprechenden
           Kleinbuchstaben zu ersetzen.
           Zusaetzlich sind die Zeilenenden aller Quelltexte auf die
           Unix-Konvention anzupassen. Die beiden genannten Aenderungen
           koennen z. B. fuer die Datei  main.c  mit folgendem Befehl
           geschehen:
               tr -d \015 <MAIN.C >main.c
   - Commodore Amiga, OS 3.1, SAS/C V6.52
       (Optionen fuer C-Compiler DATA=FAR und CODE=FAR)

In der Weiterentwicklung zu tun:
    OK automatische Erstellung einer TeX-Hauptdatei <Kuerzel>.tex
         um aus den Auswertungen eine HippoPRESS-Minimalversion zu
         machen
    OK Ausgabe der Spiele-Liste und der Spieler-Liste durch Eingabe-
         aufforderung unterbrechen
    OK Hinweis auf Auswertungsprogramm in TeX-Hauptdatei
    OK Funktionen in tab.h und tab.c fertigstellen
    OK HippoWINNER auf Ausgeglichenheit testen
    OK Befehl ! zum Uebergeben von Befehlen an eine Shell
    OK TournOneMatch() fuer Beruecksichtigung von Tennis
      ueberarbeiten; Tennis-Eingaberoutine erstellen
    OK Tennis als alternatives HippoGAME:
        - fuer Programmversion V3.0
        - Regel:
            - 100 Punkte auf 1. 2. 3. Satz verteilen:
            - pro Spielpaarung Vergleich der einzelnen Saetze;
                A.1 > B.1  ---> 1:0
                A.1 == B.1 ----> Zufall
                Gesamtergebnis immer 2:1  ????
                Satzwertung: (z. B. A >= B)
                    A>B    6 : ( B DIV (A DIV 6) )
                               neu: Spielerberechnung nach Dreisatz 
                                    mit kaufmannischer Rundung
                    A==B   7 : 6
    OK schrittweises Auflisten der Stimmzettel und Spiele
       (Befehle g und gg)
    OK Aendern eines Spieledatensatzes in Befehl eg einbauen
    OK Befehl eg testen
    OK Release 3.0
    OK Fehler: w mit Datei  te10  (ein Datensatz):
        Tournament: Spiel 0 unbekannt
    OK HippoCHARTS: return --> keine Aenderung
    OK HippoWINNER: Ausgeglichenheit pruefen
    OK rechtliche Hinweise pruefen
    OK unveraenderte HippoCHARTS-Auswahlen aus uebernommener Datei
    OK Fehler: dice0498.dat: beim Starten per Doppelklick:
        Lesen: alte Version 3...Fehler
    OK falls fuer ein Merge-Record Autor und Verlag
        neu eingegeben werden, wird das Record damit permanent
        in die Datenbank aufgenommen
    OK dice3.inf: aktuell als Voreinstellung zu verwendende
        Modus Einstellungen; neues Modul  Stat;
        (implementiert, Funktionsaufrufe in Programm einbauen)
    OK leere HippoCHARTS-Eingabe moeglich
    OK Auswertung von Kommandozeilenparameter:
        - Stimmzetteldatei: automatisch Befehl b mit der Datei
    OK keine Spieledatenbank gefunden: initialisiere Datenbank
      mit Vorgabe durch das Programm; Befehl di: speichere aktuelle
      Datenbank als C-Initialisierungsfunktion (Befehl in dh -
      Liste aufnehmen)
      testen: korrekte Verarbeitung einer leeren Datenbank
    OK Sammelauswertung fuer HippoCHARTS;
        - neues Modul: Sum
        - jede HippoCHARTS-Auswertung wird automatisch in die
          Sammelauswertungs-Datenbank eingetragen
        - fertige HippoCHARTS-Auswertungen koennen von Hand in
          die Sammelauswertungs-Datenbank eingetragen werden
        - neue Befehle:
          ok  as - liste die HippoCHARTS-Auswertungen auf, die
                 in der Sammelauswertungs-Datenbank bekannt sind
          ok  ls - loesche Daten fuer eine Sammelauswertung
          xx  es - lies vom Benutzer eine HippoCHARTS-Auswertung
                 ein (hinter Release V4.0 verschoben)
          ok  ws - fuehre Sammelauswertung durch; beziehe alle
                 HippoCHARTS-Auswertungen ein, die mit Hilfe der
                 eingegebenen Suchmuster gefunden werden
          be  ms - Durchfuehrung der Sammelauswertung
                 ein-/ausschalten
          be  mp - Eingabe des Suchmuster fuer die Sammelauswertung
    OK fuer HelpShow() unbekannter Befehl wird an Hauptschleife
        uebergeben
    OK stat Integration
    OK (getestet) help Integration
    OK (getestet) neue Help-Befehle
        h (Befehlsliste), ha (Aenderungen), hf (Features),
        hk (Kurzanleitung), hw (welche Befehle wofuer),
        dc (convert help.txt -> helpinit.h),
        ds (helpout.txt schreiben)
    OK neues Modul Out um verschiedene Ausgabeformat einbauen zu
         koennen
    OK Berechnung von Startseiten und Seitenzahlen im Modul  help
      bereits bei Textkonvertierung
    OK neue Befehle testen:
        ws (Sammelausw.), ep (Eing. Suchmuster),
        ms (Sammelausw. ein/aus)
    OK Befehle g und gg in a und ad umbenannt
    OK Befehl m in Befehl mm umbenannt
    OK neuer Befehl m: Programmodus-Einstellungen auflisten
    OK Fehler: in Sammelauswertungskommentar werden beteiligte
        Kuerzel nicht aufgelistet
    OK Problem mit Unterscheidung zwischen Merge-Records und
      normalen Records bearbeiten
    OK Release 4.0
        ('DEBUG_MODE_ALLOWED' und 'DEBUG_MODE_ON' in 'main.c'
         korrekt einstellen!)
    - Eishockey und Tennis mit weniger als 100 Punkten erlaubt
    - Integration des Moduls  stat  pruefen u. ggf. vervollstaendigen
    - Eingabe HippoCHARTS: voreingestellte Auswahl kann fuer jeden
        Platz einzeln uebernommen werden
    - Uebernahme aus Alt-Datei:
        - per Modus-Einstellung koennen alle nicht zu alten Stimmzettel
            uebernommen werden
        - per Modus-Einstellung kann vorgegeben werden, nach wievielen
            Uebernahmen ein Stimmzettel als zu alt betrachtet wird
    - Umstellung auf Verwaltung des Programmzustandes in Stat;
        - z. B. Beseitigung von  TabSetGame()
        - Stimmzetteldateiformat bereinigen (alles, was in Stat
          gespeichert wird, muss nicht mehr in Tab gespeichert werden
        - ...
    - Befehl mv aus Befehlsliste (help.txt) entfernen
    - Stimmzettel aus Datei einlesbar
    - zusaetzliches Ausgabeformat HTML
    - Hilfe-Text mit HippoGAME-Spielregeln
    - neuer Modus-Befehl: bei Uebernahme aus Alt-Datei optional
        alle Stimmzettel uebernehmen
    - Fehler:
        - Eingabe Charts Platz 3; a; --> Eingabe Platz 5 wird
          erwartet
        OK Eishockey: 1 Punkt gegen 0 Punkte --> 1 Punkt schiesst
                     ein Tor
    - stat-Integration testen
        OK neuer Befehl: mv: --> aendert Flag im Modul stat:
            gespeicherte Werte sind permanent: ja/nein
    - Release 5.0
    - Spiel als Synonym fuer anderes Spiel markierbar um doppeltes
        Auftreten in den Auswertungen zu vermeiden
    - optional per Modus-Einstellung: Anzahl der in die
          Auswertungen eingeflossenen Stimmzettel
    - naechste Entwicklungen:
        - HippoThek (s. u.)
        - Spieler-Datenbank (Modul: player)
        - Rangliste
    - Stimmzetteldatenbank (Modul Tab) und Spieledatenbank
        (Modul Games) fuer grosse Datenmengen umstellen
        (Anzahl der Datensaetze nur abhaengig von Platz auf
         Hintergrundspeicher)
        - Auslagerungsdateien: binaer, Format systemabhaengig; werden
            benutzt, sobald interne Tabelle voll;
    - Release 6.0
    - Uebernahme einzelner Stimmzettel aus anderer Stimmzetteldatei
    - bei der Verwaltung der Identifikationskuerzel unterscheiden,
      ob das Betriebssystem bei Dateinamen zwischen Gross- und
      Kleinschreibung unterscheidet
    - portable Handhabung von Verzeichnissen und Pfaden pruefen
    - purifizieren
    - ws: gefundene Kuerzel im Kommentar auflisten
    - Funktionen
            ComSetCommandLineAbbrev(argv[1]+4);
            ComSetFiles();
        ersetzen durch neue Funktion 'ComSetFilesWithoutUser(...)'
    - SumAdd() durch SumPrepare() und SumFinish() effizienter
      gestalten
    - Befehl es (Eingabe von Sammelauswertungsdaten)
    - perror()-Einfuehrung
    - neues Ausgabeformat fuer Auswertung:
        Komma-separiert (nuetzlich fuer vielfaeltige Import-Zwecke)
    - Umstellung von EvalChartSum() von SumListEntries() auf
        SumListPoints() pruefen
    + verschiedene Ausgabeformate fuer Auswertungsdateien
        (ASCII, HTML, Postscript) (neues Modul: Out)
        (HippoCHARTS und HippoWINNER auf Modul  Out  umgestellt
         (d. h. Modul Eval ueberarbeitet); zu tun:
         Eishockey und Tennis umstellen; Modul Stat besser
         integrieren (--> Dateiname); Module Sum, Stat, Out, Com
         integrieren;
    - TeX-Hauptdatei enthaelt keine \input-Befehle fuer
      Auswertungen mehr, sondern bekommt die fertigen Auswertungen
      einkopiert
    - abfangen: Sonderzeichen in Benutzereingaben, die in generierten
        Ausgabedateiein auftauchen und in einem der moeglichen
        Ausgabeformate (z. B. TeX) Steuersequenzen einleiten
    - Behandlung von Sonderzeichen durch  OutText()
    - neue Befehle: nach Uebernahme der aktuellen Stimmzetteldatei aus
        einer alten:
        - Auflistung der noch nicht geaenderten Datensaetze
        - Datensatz als geaendert markieren
    - Ausfuehrung von Befehlen, die in der Kommandozeile uebergeben
      wurden; Exit-Wert: 0: ok; 10: Fehler
    - bei 'Spiel aendern': neuer Name (<return> --> keine Aenderung)
    - Ascii und Html in Modul  Out  einbauen; neuer Befehl zu Ausgabe-
          formatumschaltung; aktuelles Format im Modul  stat  speichern
    - Ausgabe der Eishockey- und Tennis-Aufstellungen nur im
        Debug-Modus
    - beim Schreiben der TeX-Hauptdatei Backup anlegen,
      bereits existierendes Backup nicht loeschen
      (--> neue Funktion UtilCopyFile())
    - GamesSetVotes() wird nicht mehr benutzt: Entfernung pruefen
    - Alternativnamen fr Spiele
    - Rangliste in Modul Tourn einbauen
    - Ausgabe in Datei um Liste der geaenderten Mannschaften
      ergaenzen; entsprechende Verwaltung aktivieren
    - testen (insbes. Uebernahme aus Altdatei) (--> release)
    - Befehle ml, t, ts, tp, ml, mt, ms, mp
    - extern-Deklarationen in tourn.c entfernen
    - Aufruf eines Editors
    - Anzeigen von Auswertungen durch Aufruf eines Pagers
    - Eingabe eines neuen Spiels oder Stimmzettels:
        Abbruch --> Datensatz des Spiels nicht speichern
    - Ausgabe HippoCHARTS: Daten eines Spiels f"ur bestimmte
      Tabellenspalte zu lang ---> mehrere Zeilen
    - vorgesehene aber noch fehlende Befehle einbauen
    - V5.0 HippoTHEK
    - V6.0 Rangliste fuer HippoGAMES
    - V7.0 Mitgliederverwaltung
    - V8.0 Computerspieler f. Eishockey und Tennis
    - V9.0 portable grafische Oberflaeche fuer ausgewaehlte
        Betriessysteme mit einer einzigen Quelltextversion
        (ifdef TOS... ifdef MSWINDOWS)
Ideen:
    - TabClearOldRecords(int count, int OnlyNonMembers);
             loesche Records mit <got from old file> == <updated in>;
             wird bei k aufgerufen
          Frage: Was passiert, wenn jemand gar nicht zum Club-Abend
                 kommt?
    - neuer Befehl: clear records older than <count>
    - neuer Befehl: count records by age
             age  count
             now   #a
             now+1 #b <= #a
             now+2 #c <= #b
             ...
    - neuer Befehl: fix record age <age limit>
    - ComEnterPlayers:
          - uebernommene Datei: Nach eingegebenem Spieler suchen
          - nicht uebernommene Datei: nicht nach Spieler suchen
          - gefunden: weitersuchen/Eingabe mit neuem Eishockeyteam
                                   (Kontroll-Ausgabe des alten Teams
                      /return --> Eingabe ohne neues Eishockeyteam
          - nicht gefunden: wie Eingabe mit neuem Eishockeyteam
          - Neueingabe-Mitglied an Neueingabe-Eishockeyteam gekoppelt
    - Rangliste fuer Eishockey und Tennis
        - Sieger 5 Punkte, 2. 3 Punkte, Halbfinale 2 Punkte,
          Viertelfinale 1 Punkt; 1., 2. oder 3. der Rangliste
          geschlagen: 1 Punkte zus"atzlich
        - die letzten 10 Tourniere werden gewertet;
          aktuelles Tournier: Punkte * 10;
          vorangegangenes Tournier: Punkte * 9;
          ...
          aeltestes Tournier: Punkte * 1
    - Verwaltung HippoTHEK (Unterschriftenlisten zum Spielekauf)
        - neues Modul
            - ThekStart();
              ThekEnd();
              ThekSetIdent(char *ident);
              ThekSweep(char *ident);
              ThekNew();
              ThekDelete();
              ThekNewVote(char *game, char *player);
              ThekEval();
        - neue Eingabe-Routine fuer HippoTHEK
        - in Dice-Statusdatei werden die letzten n neu erstellten
          Stimmzetteldateien gespeichert
              - sicherstellen, dass hier keine Stimmzetteldatei doppelt
                eingetragen wird oder leere Eintraege neu entstehen
        - bei Uebernahme der Daten einer alten Stimmzetteldatei
          in eine Neue wird geprueft:
              - vorgeschlagene Spiele mit 8 Befuerwortungen werden
                geloescht (Nachfrage!)
              - vorgeschlagene Spiele, die nicht in den letzten
                n (s. o.) Stimmzetteldateien vorgeschlagen wurden,
                werden geloescht
              - alle anderen vorgeschlagenen Spiele werden
                uebernommen
    - Spielerdatenbank (incl. Mitgliedschaft und Adresse)
        - neue Programmversion V5.0
        - wird durch Uebernahme alter Stimmzetteldateien (s. o.), neue
          HippoTHEK-Verwaltung (s. o.) und einige Ergaenzungen/Aenderungen
          erreicht
                - Dice-Statusdatei; (dice1.inf erweitern)
                - enthaelt <Kuerzel> der zuletzt bearbeiteten
                  Stimmzetteldatei
                - beim Anlegen einer neuen Stimmzetteldatei werden
                  die Daten aus der alten gelesen
        - Speicherung der Adresse jedes Spielers
        - Suche in aktueller Stimmzetteldatei nach Spielernamen analog zu
          Suche in Spieledatenbank
        - geaenderte Adressen werden in eigene Datei ausgegeben (fertig
          fuer das Versenden an denjenigen, der neue Adressen kennen muss)
        - Realisierung des Befehls:
            - alte Datei in Neue kopieren
            - Neue lesen
        - Kurzbemerkungen anpassen

Bemerkungen:
    - In den Benutzerdialogen wurde der Begriff 'Spieler' durch den
      Begriff 'Stimmzettel' ersetzt. In den Kommentaren im Quelltext
      ist weiterhin von 'Spieler' die Rede.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "tab.h"
#include "rnd.h"
#include "tourn.h"
#include "games.h"
#include "players.h"
#include "com.h"
#include "eval.h"
#include "out.h"
#include "stat.h"
#include "help.h"
#include "sum.h"

extern int TabFirstUnused;

#define VERSION_STRING  "V4.3"

#define DEBUG_MODE_ALLOWED 0
    /* Befehl '..' zum Einschalten des Debug-Modus aktiviert:
         0: nein
         1: ja
    */

#define DEBUG_MODE_ON 1
    /* Debug-Modus erzwungenermassen immer eingeschaltet:
        0: nein (Befehl '..' benutzen, falls dieser aktiviert ist)
        1: ja
    */

int main(int argc, char *argv[])
{
    char   input, input2;
    char   StringInput[200];
    float  RndNumber;
    int    GeneratorNumber;
    int    mode;
    int    RunCount;
    int    res;
    int    TabGame;
    char   AutoExecute[100]= "";
    char  *ParamFileName= NULL;

    printf("\n\n");
    printf("****** HippoPRESS-Auswertung %s ******\n",
           VERSION_STRING);
    printf("           von Holger Burbach\n\n");

    if (RndStart(StatFileName(STAT_RANDOM)) != 0) goto ErrorExit;
    if (GamesStart() != 0) goto ErrorExit;
    if (StatStart() != 0) goto ErrorExit;
    if (SumStart() != 0) goto ErrorExit;
    HelpStart();
    StatSetVersionString(VERSION_STRING);

    printf("\n");

    if (argc == 2)
    {
        int ok= 0;

        printf("werte Kommandozeilenparameter aus:\n");
        if (strlen(argv[1]) >= 12)
        {
            ParamFileName= argv[1]+(strlen(argv[1])-12);
            printf("gefundener Dateiname: '%s'\n", ParamFileName);
            ok= ( !strncmp(ParamFileName, "dice", 4) &&
                  !strncmp(ParamFileName+8, ".dat", 4)
                ) ||
                ( !strncmp(ParamFileName, "DICE", 4) &&
                  !strncmp(ParamFileName+8, ".DAT", 4)
                );
        }
        if (ok)
        {
            ComSetCommandLineAbbrev(ParamFileName+4);
            ComSetFiles();
        }
        else
        {
            printf(
            "argv[1] == '%s'\n", argv[1]);
            printf("Parameter ungueltig...ignoriert\n");
        }
        printf("\n\n");
    } /* if */
    else
    {
        /*
            keine Datei in der Kommandozeile uebergeben, teste
            letzte benutzte Datei:
        */
        if (strlen(StatAbbrev()) > 0)
        {
            res= UtilFileExists(StatFileName(STAT_DATA));
            if (res == -1)
            {
                printf("Fehler beim Zugriff auf Stimmzetteldatei '%s'\n",
                    StatFileName(STAT_DATA));
                goto ErrorExit;
            }
            if (res)
            {
                printf("aktiviere letzte verwendete Stimmzetteldatei:\n");
                ComSetCommandLineAbbrev(StatAbbrev());
                ComSetFiles();
            }
            else
                StatSetAbbrev("");
        } /* if */
    } /* else */

    do
    {
        if (strlen(AutoExecute) == 0)
        {
            if (StatFileName(STAT_DATA) != NULL)
            {
                int game;

                printf("\naktuelle Stimmzetteldatei: %s\n",
                    StatFileName(STAT_DATA));
                TabGetIceMode(&mode);
                printf("Tourniermodus: ");
                if (mode == 0)
                {
                    printf(
                    "KO-System; alle Freilose in der ersten Runde");
                } else if (mode == 1)
                {
                    printf("jeder-gegen-jeden");
                } else
                {
                    printf("%d (unbekannt--> Fehler)", mode);
                }
                game= TabGetGame();
                printf("\naktivierte Spiele: ");
                if (game & 1)
                    printf("Eishockey  ");
                if (game & 2)
                    printf("Tennis");
                printf("\n");
            }
            else
                printf("\nkeine aktuelle Stimmzetteldatei\n");
            printf("Eingabe (h --> Hilfe)? ");
            UtilInput(StringInput);
        } /* if (strlen(AutoExecute) == 0 */
        else
        {
            strcpy(StringInput, AutoExecute);
            strcpy(AutoExecute, "");
        }

        input= ' '; input2= ' ';
        if (strlen(StringInput) > 0)
        {
            input= StringInput[0];
            if (strlen(StringInput) > 1)
                input2= StringInput[1];
        }

        if ((input == 'a') && (input2 == ' '))
        {
            /* Stimmzettel auf den Bildschirm ausgeben */
            printf("\n****** Stimmzetteldaten ausgeben ******\n");
            TabPrint();
        } else if ((input == 'a') && (input2 == 'g'))
        {
            /* Spieledatenbank auf den Bildschirm ausgeben */
            printf("\n****** Spieledatenbank ausgeben ******\n");
            ComPrintGames();
        } else if ((input == 'a') && (input2 == 's'))
        {
            /* Daten einer Sammelauswertung auf den Bildschirm
               ausgeben */
            printf(
            "\n****** Gespeicherte HippoCHARTS-Auswertungen (fuer Sammelauswertung) ******\n");
            ComSumListAbbrev();
        } else if ((input == 'b') && (input2 == ' '))
        {
            /* Stimmzetteldatei .B.estimmen */
            printf("\n****** Stimmzetteldatei bestimmen ******\n");
            ComSetFiles();
        } else if ( (input == '.') && (input2 == '.')
                    && DEBUG_MODE_ALLOWED
                  )
        {
            /* Debug-Modus: ein-/ausschalten */
            StatSetDebugMode(StatDebugMode() ? 0 : 1);
            printf("Debug-Modus ist jetzt %sgeschaltet\n",
                StatDebugMode() ? "EIN" : "AUS");
        } else if ( (input == 'd') && (input2 == 'c')
                    && (StatDebugMode() || DEBUG_MODE_ON)
                  )
        {
            HelpConvertText();
        } else if ( (input == 'd') && (input2 == 'h')
                    && (StatDebugMode() || DEBUG_MODE_ON)
                  )
        {
            /* Debug-Modus Hilfe */
            printf("\n****** Debug-Modus: Hilfe ******\n");
            printf(" ..  Debug-Modus ein-/ausschalten\n");
            printf(" dc  Hilfetext aus 'help.txt' in Init-Datei");
                printf(" 'helpinit.h' konvertieren\n");
            printf(" dh  Hilfe fuer Debug-Modus ausgeben\n");
            printf(" di  Initialisierungsdatei 'gameinit.h' schreiben\n");
            printf(" ds  Hilfetext in Datei 'helpout.txt' schreiben\n");
            printf(" dw  Zufallsgeneratoren fuer HippoWINNER testen\n");
            printf(" dz  Zufallsgenerator setzen\n");
        } else if ( (input == 'd') && (input2 == 'i')
                    && (StatDebugMode() || DEBUG_MODE_ON)
                  )
        {
            GamesWriteInit();
        } else if ( (input == 'd') && (input2 == 's')
                    && (StatDebugMode() || DEBUG_MODE_ON)
                  )
        {
            HelpWriteText();
        } else if ( (input == 'd') && (input2 == 'w')
                    && (StatDebugMode() || DEBUG_MODE_ON)
                  )
        {
            /* Debug-Modus: HippoWINNER-Test */
            printf(
            "\n****** Debug-Modus: HippoWINNER-Test ******\n");
            printf("Anzahl der Durchlaeufe? ");
            UtilInput(StringInput);
            sscanf(StringInput, "%d", &RunCount);
            if (RunCount > 0) EvalWinner(RunCount);
        } else if ( (input == 'd') && (input2 == 'z')
                    && (StatDebugMode() || DEBUG_MODE_ON)
                  )
        {
            /* Debug-Modus: Zufallsgeneratoren setzen */
            printf(
            "\n****** Debug-Modus: Zufallsgenerator setzen ******\n");
            printf("Nummer des Zufallsgenerators? ");
            UtilIntInput(&GeneratorNumber);
            printf("Anfangswert des Zufallsgenerators? ");
            UtilInput(StringInput);
            sscanf(StringInput, "%f", &RndNumber);

            RndSelectGenerator(GeneratorNumber);
            RndSetStartNumber(RndNumber);
            RndSelectGenerator(0);
        } else if ((input == 'e') && (input2 == ' '))
        {
            /* Stimmzettel eingeben */
            printf("\n****** Stimmzetteldaten eingeben ******\n");
            ComEnterPlayers();
        } else if ((input == 'e') && (input2 == 'g'))
        {
            /* Datensaetze in die Spieledatenbank eingeben */
            printf("\n****** Spiele eingeben/aendern ******\n");
            ComInputGames();
        } else if ((input == 'h') && (input2 == ' '))
        {
            printf("\n****** Befehlsliste ******\n");
            HelpShow(HELP_COMMANDS, AutoExecute);
        } else if ((input == 'h') && (input2 == 'a'))
        {
            printf(
            "\n****** Aenderungen in den Programmversionen ******\n");
            HelpShow(HELP_CHANGES, AutoExecute);
        } else if ((input == 'h') && (input2 == 'f'))
        {
            printf("\n****** Features des Programms ******\n");
            HelpShow(HELP_FEATURES, AutoExecute);
        } else if ((input == 'h') && (input2 == 'k'))
        {
            printf("\n****** Kurzanleitung ******\n");
            HelpShow(HELP_INTRO, AutoExecute);
        } else if ((input == 'h') && (input2 == 'r'))
        {
            printf("\n****** rechtliche Hinweise ******\n");
            HelpShow(HELP_LEGAL, AutoExecute);
        } else if ((input == 'h') && (input2 == 'w'))
        {
            printf("\n****** Welche Befehle fuer welchen Zweck ******\n");
            HelpShow(HELP_TODO, AutoExecute);
        } else if ((input == 'l') && (input2 == ' '))
        {
            /* Stimmzetteldaten loeschen */
            printf("\n****** Stimmzetteldaten loeschen ******\n");
            ComDeletePlayer();
        } else if ((input == 'l') && (input2 == 'g'))
        {
            /* Datensatz aus Spieledatenbank loeschen */
            printf(
            "\n****** Datensatz aus Spieledatenbank loeschen ******\n");
            ComDeleteGames();
        } else if ((input == 'l') && (input2 == 's'))
        {
            /* HippoCHARTS-Auswertung aus Datenbank fuer
               Sammelauswertungen loeschen */
            printf(
            "\n****** HippoCHARTS-Auswertung aus der Datenbank ******\n");
            printf(
              "******    fuer die Sammelauswertung loeschen    ******\n");
            ComSumDelete();
        } else if ((input == 'm') && (input2 == ' '))
        {
            /* Programmodus-Einstellungen auflisten: */
            printf("\n****** Modus-Einstellungen ******\n");
            /* Befehl mv nicht eingebaut, daher: 
            printf("%s%s%s\n",
                "  Uebernahme von Voreinstellungen (Befehl mv) ist ",
                StatPreset() ? "EIN" : "AUS", "geschaltet");
            */
            if (StatDebugMode())
                printf("  Debug-Modus ist EINgeschaltet\n");
            printf("  Suchmuster fuer HippoCHARTS-Sammelauswertung: %s\n",
                StatChartSumPattern());
            printf("  automatische HippoCHARTS-Sammelauswertung ist %sgeschaltet\n",
                StatChartSum() ? "EIN" : "AUS");
            /* Tourniermodus
            printf("  HippoGAME Eishockey ist %sgeschaltet\n",
                (TabGetGame() & 1) ? "EIN" : "AUS");
            printf("  HippoGAME Tennis ist %sgeschaltet\n",
                (TabGetGame() & 2) ? "EIN" : "AUS");
            */
        } else if ((input == 'm') && (input2 == 'e'))
        {
            /* Eishockey ein-/ausschalten */
            int game;

            game= TabGetGame();
            game ^= 1;
            TabSetGame(game);

            game= StatGame();
            game ^= 1;
            StatSetGame(game);
        } else if ((input == 'm') && (input2 == 'm'))
        {
            /* Tourniermodus KO-System/jeder-gegen-jeden
               umschalten */
            TabGetIceMode(&mode);
            mode= (mode == 0) ? 1 : 0;
            TabSetIceMode(mode);
            TabSetTennisMode(mode);

            mode= StatIceTournMode();
            mode= (mode == 0) ? 1 : 0;
            StatSetIceTournMode(mode);
            StatSetTennisTournMode(mode);
        } else if ((input == 'm') && (input2 == 'p'))
        {
            /* Eingabe Suchmuster fuer HippoCHARTS Sammelauswertung */
            printf("\n****** Suchmuster fuer HippoCHARTS ");
            printf("Sammelauswertung festlegen ******\n");
            printf("Suchmuster (? fuer: Zeichen beliebig) ? ");
            UtilInput(StringInput);
            if (strlen(StringInput) != 4)
                printf("Fehler: genau 4 Zeichen erforderlich\n");
            else
                StatSetChartSumPattern(StringInput);
        } else if ((input == 'm') && (input2 == 's'))
        {
            /* HippoCHARTS Sammelauswertung ein-/ausschalten */
            TabSetChartSum( TabChartSum() ? 0 : 1 );
            printf("HippoCHARTS Sammelauswertung ist jetzt %sgeschaltet\n",
                TabChartSum() ? "EIN" : "AUS");

            StatSetChartSum(TabChartSum());
        } else if ((input == 'm') && (input2 == 't'))
        {
            /* Tennis ein-/ausschalten */
            int game;

            game= TabGetGame();
            game ^= 2;
            TabSetGame(game);

            game= StatGame();
            game ^= 2;
            StatSetGame(game);
        } else if ((input == 'w') && (input2 == ' '))
        {
            /* komplette Auswertung */
            printf("\n****** komplette Auswertung: ******\n");
            if (TabFirstUnused < 1)
            {
                printf("keine Stimmzettel vorhanden...Abbruch\n");
                break;
            }
            printf("\n****** HippoCHARTS aktuelle Auswertung ******\n");
            EvalCharts();
            if (TabChartSum())
            {
                printf("\n****** HippoCHARTS Sammelauswertung ******\n");
                EvalChartSum();
            }
            printf("\n****** HippoGAMES: ******\n");
            TabGame= TabGetGame();
            if (TabGame == 0)
                printf("keine Spiele eingeschaltet\b");
            else
            {
                if (TabGame & 1)
                {
                    printf("\n****** Eishockey ******\n");
                    Tournament(TOURN_ICEHOCKEY);
                }
                if (TabGame & 2)
                {
                    printf("\n****** Tennis ******\n");
                    Tournament(TOURN_TENNIS);
                }
            }
            printf("\n****** HippoWINNER ******\n");
            EvalWinner(0);
        } else if ((input == 'w') && (input2 == 't'))
        {
            /* TeX-Hauptdatei fuer Auswertung: */
            printf("\n****** TeX-Hauptdatei fuer Auswertung ******\n");
            OutTexMain();
        } else if ((input == 'w') && (input2 == 'c'))
        {
            /* Auswertung HippoCHARTS */
            printf("\n****** Auswertung: aktuelle HippoCHARTS ******\n");
            if (TabFirstUnused < 1)
            {
                printf("keine Stimmzettel vorhanden...Abbruch\n");
                break;
            }
            EvalCharts();
        } else if ((input == 'w') && (input2 == 'e'))
        {
            /* Auswertung HippoGAME  */
            printf("\n****** HippoGAMES-Auswertung: ******\n");
            if (TabFirstUnused < 1)
            {
                printf("keine Stimmzettel vorhanden...Abbruch\n");
                break;
            }
            TabGame= TabGetGame();
            if (TabGame == 0)
                printf("keine Spiele eingeschaltet\b");
            else
            {
                if (TabGame & 1)
                {
                    printf("\n    ****** Eishockey ******\n");
                    Tournament(TOURN_ICEHOCKEY);
                }
                if (TabGame & 2)
                {
                    printf("\n    ****** Tennis ******\n");
                    Tournament(TOURN_TENNIS);
                }
            }
        } else if ((input == 'w') && (input2 == 's'))
        {
            /* HippoCHARTS Sammelauswertung */
            printf("\n****** HippoCHARTS Sammelauswertung ******\n");
            EvalChartSum();
        } else if ((input == 'w') && (input2 == 'w'))
        {
            /* Auswertung HippoWINNER */
            printf("\n****** Auswertung HippoWINNER ******\n");
            if (TabFirstUnused < 1)
            {
                printf("keine Stimmzettel vorhanden...Abbruch\n");
                break;
            }
            EvalWinner(0);
        } else if ((input == 'v') && (input2 == ' '))
        {
            /* Programm verlassen */
            /* tue nichts */
        } else if ((input == 'v') && (input2 == '!'))
        {
            /* Programm abbrechen (Stimmzetteldatei nicht schreiben) */
            printf("\n****** Programmabbruch ");
            printf(" (evtl. Datenverluste) ******\n");
            printf("Sicher (j/n)? ");
            UtilInput(StringInput);
            if ((StringInput[0] != 'j') && (StringInput[0] != 'J'))
                input= ' ';
        } else if (input == '!')
        {
            /* Befehl an Shell absetzen: */
            res= system(NULL);
            if (res != 0)
                res= system(StringInput+1);
            else
                printf("Fehler: keine Shell verfuegbar\n");
        } else
        {
            printf("Eingabefehler!\n");
        }
    } while (    ((input != 'v') || (input2 != ' '))
              && ((input != 'v') || (input2 != '!'))
            );
    if (input2 != '!')
    {
        TabWriteFile();
        GamesEnd();

        printf("\n");
        StatEnd();
        GamesEnd();
        RndEnd();
    }

    return 0;

    ErrorExit:
        printf("\nProgrammabbruch!");
        return 1;
} /* main */
