/* Datei: help.c */

#include <stdio.h>
#include <string.h>
#include "help.h"
#include "util.h"

#include "helpinit.h"

#define HELP_CHAPTER_COUNT  7
    /* eins mehr als tatsaechlich fuer einfachere Fehlerbehandlung */

/* Default-Inhalt der Datei helpinit.h:
static int HelpCount= 0;
static int HelpChapters[HELP_CHAPTER_COUNT]= {0, 0, 0, 0, 0, 0, 0};
static int HelpPageCount[HELP_CHAPTER_COUNT] = {0, 0, 0, 0, 0, 0, 0};
static char *HelpText[] = {
""
};
*/

static char *HelpHeadings[HELP_CHAPTER_COUNT]=
    { "Features", "Kurzanleitung", "Programm-Aenderungen",
      "Befehlsliste", "Welche-Befehle-Wofuer", 
      "rechtliche Hinweise", "" };

#define HELP_LINES_PER_PAGE 23
    /* maximale Anzahl der Zeilen pro Hilfetext-Seite */

void HelpStart(void)
{
/*    int i;
    int PageNumber;
    int LastChapter;

    *
        initialisiere 'HelpChapters' mit den 'HelpText'-Indizes der 
        Kaptitelanfaenge und 'HelpPageCount' mit den zugehoerigen
        Seitenanzahlen der Kapitel:
    *
    PageNumber= 1;
    LastChapter= -1;    
    for(i= 0; i< HelpCount; i++)
    {
        if (strlen(HelpText[i]) >= 2)
        {
            if (!strcmp(HelpText[i], "##"))
                PageNumber++;
	        if (!strncmp(HelpText[i], "###", 3))
	        {
	            if (LastChapter != -1)
	            {
	                HelpPageCount[LastChapter]= PageNumber;
	                PageNumber= 1;
	            }

	            if (!strcmp(HelpText[i], "###FEATURES"))
	                LastChapter= HELP_FEATURES;
	            else if (!strcmp(HelpText[i], "###INTRO"))
	                LastChapter= HELP_INTRO;
	            else if (!strcmp(HelpText[i], "###CHANGES"))
	                LastChapter= HELP_CHANGES;
	            else if (!strcmp(HelpText[i], "###COMMANDS"))
	                LastChapter= HELP_COMMANDS;
	            else if (!strcmp(HelpText[i], "###TODO"))
	                LastChapter= HELP_TODO;
	            else if (!strcmp(HelpText[i], "###LEGAL"))
	                LastChapter= HELP_LEGAL;
	            else
	            {
	                LastChapter= HELP_CHAPTER_COUNT-1;
	                printf(
	                "HelpStart: unbekannte Marke '%s' in Zeile %d\n",
	                    HelpText[i], i+1);
	            }
	            HelpChapters[LastChapter]= i;
	        } * if *
	    } * if *
    } * for *
    if (LastChapter != -1)
        HelpPageCount[LastChapter]= PageNumber;
*/
} /* HelpStart */

void HelpEnd(void)
{
} /* HelpEnd */

void HelpWriteText(void)
{
    FILE *file;
    int   i;
    int   res;
    
    printf("schreibe Hilfetext in Datei 'helpout.txt'...");
    file= fopen("helpout.txt", "w");
    if (file == NULL) goto ErrorExit2;
    
    for(i=0; i<HelpCount; i++)
    {
        res= fprintf(file, "%s\n", HelpText[i]);
        if (res == EOF) goto ErrorExit;
    }

    if (fclose(file) == EOF) goto ErrorExit2;

    printf("OK\n");    
    return;
    
    ErrorExit:
        res= fclose(file);
    ErrorExit2:
        printf("Fehler\n");
        return;
} /* HelpWriteText */

void HelpConvertText(void)
{
    FILE *in, *out;
    int  res;
    int  StringValid;
    char buffer[100];
    int  i;
    int  LineNumber;
    int  PageLine;
    int  IsComment;
    
    int PageNumber;
    int LastChapter;

    int  count= 0;
    int  chapters[HELP_CHAPTER_COUNT];
    int  PageCount[HELP_CHAPTER_COUNT];

    printf("konvertiere Hilfetext 'help.txt' in");
    printf(" Hilfe-Initialisierungsdatei 'helpinit.h':\n");

    for(i=0; i<HELP_CHAPTER_COUNT; i++)
        chapters[i]= PageCount[i]= 0;
    
    in= fopen("help.txt", "r");
    if (in == NULL) goto ErrorExit3;
    PageNumber= 1;
    LastChapter= -1;    
    do
    {
        res= UtilFileInput(in, buffer, 100);
        
        StringValid= !res || (strlen(buffer) > 0);
        
        if (StringValid)
        {
            count++;
            
	        if (strlen(buffer) >= 2)
	        {
	            if (!strcmp(buffer, "##"))
	                PageNumber++;
		        if (!strncmp(buffer, "###", 3))
		        {
		            if (LastChapter != -1)
		            {
		                PageCount[LastChapter]= PageNumber;
		                PageNumber= 1;
		            }
	
		            if (!strcmp(buffer, "###FEATURES"))
		                LastChapter= HELP_FEATURES;
		            else if (!strcmp(buffer, "###INTRO"))
		                LastChapter= HELP_INTRO;
		            else if (!strcmp(buffer, "###CHANGES"))
		                LastChapter= HELP_CHANGES;
		            else if (!strcmp(buffer, "###COMMANDS"))
		                LastChapter= HELP_COMMANDS;
		            else if (!strcmp(buffer, "###TODO"))
		                LastChapter= HELP_TODO;
		            else if (!strcmp(buffer, "###LEGAL"))
		                LastChapter= HELP_LEGAL;
		            else
		            {
		                LastChapter= HELP_CHAPTER_COUNT-1;
		                printf(
		                "HelpStart: unbekannte Marke '%s' in Zeile %d\n",
		                    buffer, count);
		            }
		            chapters[LastChapter]= count-1;
		        } /* if */
		    } /* if (strlen(buffer) >= 2) */
        } /* if (StringValid) */
    }
    while(!res);
    if (LastChapter != -1)
        PageCount[LastChapter]= PageNumber;

    if (fclose(in) == EOF) goto ErrorExit3;
    in= NULL;
    
    printf("    %d Zeilen\n", count);
    printf("    Startzeilen der Kapitel:\n        ");
    for(i= 0; i<HELP_CHAPTER_COUNT; i++)
        printf("%5d", chapters[i]);
    printf("\n    Anzahlen der Seiten in den Kapiteln:\n        ");
    for(i= 0; i<HELP_CHAPTER_COUNT; i++)
        printf("%5d", PageCount[i]);
    printf("\n");
    
    in= fopen("help.txt", "r");
    if (in == NULL) goto ErrorExit3;
    out= fopen("helpinit.h", "w");
    if (out == NULL) goto ErrorExit2;
    
    /*
        schreibe Definition der Variable 'HelpCount':
    */
    res= fprintf(out, "static int HelpCount= %d;\n", count);
    if (res == EOF) goto ErrorExit;

    /*
        schreibe Definition des Feldes 'HelpChapters':
    */
    res= fprintf(out, "static int HelpChapters[%d]= {\n",
                 HELP_CHAPTER_COUNT);
    if (res == EOF) goto ErrorExit;
    for (i=0; i<HELP_CHAPTER_COUNT; i++)
    {
        if (i < HELP_CHAPTER_COUNT-1)
            fprintf(out, "%d,", chapters[i]);
        else
            fprintf(out, "%d\n", chapters[i]);            
        if (res == EOF) goto ErrorExit;
    }
    res= fprintf(out, "};\n");
    if (res == EOF) goto ErrorExit;
    
    /*
        schreibe Definition des Feldes 'HelpPageCount':
    */
    res= fprintf(out, "static int HelpPageCount[%d]= {\n",
                 HELP_CHAPTER_COUNT);
    if (res == EOF) goto ErrorExit;
    for (i=0; i<HELP_CHAPTER_COUNT; i++)
    {
        if (i < HELP_CHAPTER_COUNT-1)
            fprintf(out, "%d,", PageCount[i]);
        else
            fprintf(out, "%d\n", PageCount[i]);
        if (res == EOF) goto ErrorExit;
    }
    res= fprintf(out, "};\n");
    if (res == EOF) goto ErrorExit;

    /*
        schreibe Definition des Feldes 'HelpText':
    */
    res= fprintf(out, "static char *HelpText[]= {\n");
    if (res == EOF) goto ErrorExit;
    LineNumber= 0;
    PageLine= 0;
    if (count > 0)
        for(i=0; i<count; i++)
        {
            res= UtilFileInput(in, buffer, 100);
            LineNumber++;

            IsComment= (strlen(buffer) > 0) ? (buffer[0] == '%') : 0;
            if (!IsComment)
            {
                if (strlen(buffer) > 79)
                {
                    printf(
                    "    Zeile %d ist laenger als 79 Zeichen\n",
                        LineNumber);
                    printf(
                    "        Hilfetext-Anzeige evtl. fehlerhaft formatiert\n");
                }
                if (strncmp(buffer, "##", 2))
                {
                    /* 
                        ist kein Kommentar und beginnt nicht mit ## : 
                    */
                    PageLine++;
                    if (PageLine == HELP_LINES_PER_PAGE+1)
                    {
                        printf(
                        "    Hilfetext-Seite ist ab Zeile %d laenger",
                            LineNumber);
                        printf(" als %d Zeilen\n", HELP_LINES_PER_PAGE);
                        printf("        korrekte Seitenzaehlung in der");
                        printf(" Hilfetext-Anzeige nicht moeglich\n");
                    }
                }
                else
                    PageLine= 0;
            } /* if */

            if (UtilWriteInitString(out, buffer)) goto ErrorExit;
            if (i < count-1)
                if (fprintf(out, ",\n") == EOF) goto ErrorExit;
        } /* for */
    else
        if (fprintf(out, "\"\"") == EOF) goto ErrorExit;
    
    res= fprintf(out, "\n};\n");
    if (res == EOF) goto ErrorExit;

    if (fclose(out) == EOF) goto ErrorExit2;
    if (fclose(in) == EOF) goto ErrorExit3;

    printf("Konvertierung beendet...OK\n");
    return;
    
    ErrorExit:
        res= fclose(out);
    ErrorExit2:
        res= fclose(in);
    ErrorExit3:
        printf("Fehler\n");
        return;
} /* HelpConvertText */

static void HelpShowPage(int head)
/* Zeige die Seite an, deren 'HelpText'-Startindex 'head' angibt. */
{
    int i;
    int LineCount;

    i= head;
    if (strlen(HelpText[head]) > 0)
        if (!strncmp(HelpText[head], "##", 2))
            i++;

    LineCount= 0; 
    do
    {
        if (i < HelpCount)
        {
            if (strlen(HelpText[i]) > 0)
            {
                if (!strncmp(HelpText[i], "##", 2))
                {
                    printf("\n"); 
                    LineCount++;
                }
                else if (HelpText[i][0] == '%')
                    i++;
                else
                {
                    printf("%s\n", HelpText[i]);
                    i++;
                    LineCount++;
                }
            }
            else
            {
                printf("\n");
                i++; LineCount++;
            }
        }
        else
        {
            printf("\n");
            LineCount++;
        }
    } while(LineCount < HELP_LINES_PER_PAGE);
} /* HelpShowPage */

static void HelpNewHead(int OldHead, int backwards, int *NewHead)
/* Suche 'HelpText'-Index des naechsten Seitenanfangs. 'OldHead' ist
   der Index des alten Seitenanfangs. 'backwards' == 0: suche
   vorwaerts, :1 suche rueckwaerts; '*NewHead' ist der Index des
   gefundenen Seitenanfangs; OldHead == *NewHead: es wurde keine
   Seite gefunden: Kapitel- oder Textende erreicht
*/
{
    int i;
    int found= 0;
    int EndOfSearch= 0;
    
    i= OldHead;
    *NewHead= OldHead;
    
    /*
        am Kapitelanfang darf man nicht rueckwaertsblaettern:
    */
    if ((backwards) && !strncmp(HelpText[OldHead], "###", 3))
        return;
    
    while (!EndOfSearch && (i >= 0) && (i < HelpCount))
    {
        if (backwards) i--; else i++;
        if ((i >= 0) && (i < HelpCount))
        {
            /*
                suche Seitengrenze; beim Rueckwaertssuchen zaehlt
                auch eine Kapitelgrenze als Seitengrenze:
            */
            if (backwards)
                found= !strncmp(HelpText[i], "##", 2);
            else
                found= !strcmp(HelpText[i], "##");
                
            /*
                beende die Suche an einer Kapitel- oder einer
                Seitengrenze:
            */
            EndOfSearch= found || !strncmp(HelpText[i], "###", 3);
        }
        else
            found= 0;
    } /* while */

    if (found)
        *NewHead= i;
} /* HelpNewHead */

void HelpShow(int chapter, char *command)
{
    int head, NewHead;
    char UserInput[100];
    char com, com2;
    /*int ExitByReturn; */
    int CurrentPage;

    if ((chapter < 0) || (chapter >= HELP_CHAPTER_COUNT-1))
    {
        printf("HelpShow: fehlerhafte Kapitelnummer: %d\n", chapter);
        return;
    } 
    head= HelpChapters[chapter];

    /*ExitByReturn= 0;  yyy
        wenn zusammen mit Code s. u. aktiviert dann: nach letzter 
        Seite zweimal <return> zum Verlassen des Textes */
    CurrentPage= 1;
    do
    {
        HelpShowPage(head);
        printf(
            "'%s' (Seite %d von %d)   Befehl (h --> Hilfe) ? ",
            HelpHeadings[chapter], CurrentPage, HelpPageCount[chapter]);
        UtilInput(UserInput);
        
        com= ' '; com2= ' ';
        if (strlen(UserInput) > 0)
        {
            com= UserInput[0];
            if (strlen(UserInput) > 1)
                com2= UserInput[1];
            else 
                com2= ' ';
        }
        else
            com= ' ';
        
        if ((com == 'h') && (com2 == ' '))
        {
            /*yyyExitByReturn= 0;*/
            printf("****** Befehle im Hilfe-Modus: ******\n");
            printf("<return>   Seite vorwaertsblaettern\n");
            printf("-          Seite zurueckblaettern\n");
            printf("v          Text verlassen\n");
            printf("sonstige Eingabe: Hilfemodus beenden und");
            printf(" Eingabe als normalen Befehl behandeln\n");
            printf("Bitte <return> betaetigen. \n");
            UtilInput(UserInput);
        } else if ((com == '-') && (com2 == ' '))
        {
            /*yyyExitByReturn= 0;*/
            HelpNewHead(head, 1, &NewHead);
            if (head != NewHead)
                CurrentPage--;
            head= NewHead;
        } else if ((com == 'v') && (com2 == ' '))
        { 
            /* tue nichts */
        } else if ((com == ' ') && (com2 == ' '))
        {
            HelpNewHead(head, 0, &NewHead);
            if (head == NewHead)
                com= 'v';
            /*yyy
            {
                if (!ExitByReturn)
                {
                    ExitByReturn= 1;
                    printf("letzte Seite; <return> --> Ende\n");
                }
                else
                    com= 'v';
            }*/
            else
                CurrentPage++;
            head= NewHead;
        } else
        {
            com= 'v'; com2= ' ';
            strcpy(command, UserInput);
        } /* switch */
    } while (!((com == 'v') && (com2 == ' ')));
} /* HelpShow */
