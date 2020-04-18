/*
    Datei: out.c
*/

#include <stdio.h>
#include <string.h>
#include "out.h"
#include "stat.h"
#include "tab.h"

static int CurrentFormat= OUT_FORMAT_TEX; 
    /* aktuelles Ausgabeformat */
    
static int OutputInProgress= 0;
    /* !=0: Ausgabedatei wird gerade geschrieben; Wechsel des Wertes
          von 'CurrentFormat' verboten;
       0: sonst
       1: aktuell wird HippoWINNER-Datei geschrieben
       2: aktuell wird HippoCHARTS-Datei geschrieben
       3: aktuell wird Eishockey-Datei geschrieben
       4: aktuell wird Tennis-Datei geschrieben
       5: aktuell wird HippoCHARTS-Sammelauswertung geschrieben
    */
#define OUT_WINNER     1
#define OUT_CHARTS     2
#define OUT_ICEHOCKEY  3
#define OUT_TENNIS     4
#define OUT_CHART_SUM  5

static FILE *CurrentFile= NULL;
    /* Handle der aktuellen Ausgabedatei */

static char OutVersionString[30]= "";
    /* aktuelle Zeichenkette zur Identifikation der Programmversion */

static int OutGame= 0;

int OutSelectFormat(int format)
{
    if ((format < OUT_FORMAT_TEX) || (format > OUT_FORMAT_POSTSCRIPT))
    {
        printf("OutSelectFormat: Format %d unbekannt\n", format);
        goto ErrorExit;
    }
    if (format > OUT_FORMAT_TEX)
    {
        printf("OutSelectFormat: Format %d nicht implementiert\n", format);
        goto ErrorExit;
    }
    if (OutputInProgress)
    {
        printf("OutSelectFormat: Fehler: Ausgabe gerade aktiv\n");
        goto ErrorExit;
    }
        
    CurrentFormat= format;
    
    return 0;
    
    ErrorExit:
        return 1;
} /* OutSelectFormat */

void OutSetVersionString(char *VersionString)
{ 
    strcpy(OutVersionString, VersionString);
} /* OutSetVersionString */

int OutText(char *text)
{
    int res;
    
    if (OutputInProgress == 0)
    {
        printf("OutComment: keine aktive Ausgabe\n");
        goto ErrorExit;
    }

    res= fprintf(CurrentFile, "%s", text);
    
    if ((res==0) || (res==EOF))
        goto ErrorExit;
        
    return 0;
    
    ErrorExit:
        return 1;
} /* OutText */

int OutNewLine(void)
{
    int res;
    
    if (OutputInProgress == 0)
    {
        printf("OutComment: keine aktive Ausgabe\n");
        goto ErrorExit;
    }

    res= fprintf(CurrentFile, "\\\\\n");    
    if ((res==0) || (res==EOF)) goto ErrorExit;
        
    return 0;
    
    ErrorExit:
        return 1;
} /* OutNewLine */

int OutSpace(void)
{
    int res;
    
    if (OutputInProgress == 0)
    {
        printf("OutComment: keine aktive Ausgabe\n");
        goto ErrorExit;
    }

    res= fprintf(CurrentFile, "\\hspace*{2em}");    
    if ((res==0) || (res==EOF)) goto ErrorExit;
        
    return 0;
    
    ErrorExit:
        return 1;
} /* OutSpace */

int OutComment(char *text)
{
    int res;

    if (OutputInProgress == 0)
    {
        printf("OutComment: keine aktive Ausgabe\n");
        goto ErrorExit;
    }

    /* bisher nur TeX-Kommentare: */
    res= fprintf(CurrentFile, "%% %s\n", text);
    if ((res==0) || (res==EOF)) goto ErrorExit;

    return 0;
    
    ErrorExit:
        return 1;
} /* OutComment */

int OutTexMain(void)
{
    char *TeXFile[50]= {
    "%format latexg",
    "%##", /*1*/
    "%##", /*2*/
    "\\documentstyle[german,din_a4]{article}",
    "",
    "\\parindent0em",
    "\\parskip2ex plus0.5ex minus0.5ex",
    "",
    "\\begin{document}",
    "\\begin{center}",
    "    \\LARGE HippoPRESS\\\\\\small",
    "%##", /*3*/
    "\\end{center}",
    "\\begin{center} \\Large HippoWINNER \\end{center}",
    "%##", /*4*/
    "\\begin{center} \\Large HippoCHARTS \\end{center}",
    "%##", /*5*/
    "%##", /*6*/
    "\\begin{center} \\Large HippoGAMES \\end{center}",
    "%##", /*7*/
    "%##", /*8*/
    "\\end{document}",
    "%####"
    };
    int res= 0;
    int i, loop;
    int MarkFound, MarkCount;
    FILE *MainFile;
    char line[100];

    if (StatFileName(STAT_MAIN) == NULL)
        return -3;

    printf("schreibe TeX-Hauptdatei '%s'...", StatFileName(STAT_MAIN));
    
    MainFile= fopen(StatFileName(STAT_MAIN),"w");
    if (MainFile == NULL)
    {
        printf("kann Datei nicht oeffnen\n");
        return -3;
    }

    i= 0;
    MarkCount= 0;
    do
    {
        loop= strcmp(TeXFile[i], "%####");
        if (loop)
        {
            MarkFound= !strcmp(TeXFile[i],"%##");
            if (MarkFound)
            {
                MarkCount++;
                line[0]= '\0';
                strcat(line,"\\input{");
                switch(MarkCount)
                {
                case 1:
                    /* Dateiname im TeX-Kommentar am Dateianfang */
                    res= fprintf(MainFile,
                        "%% Datei %s.tex\n", StatAbbrev());
                    break;
                case 2:
                    /* Hinweis auf HippoPRESS-Auswerungsprogramm am
                       Dateianfang: */
                    res= fprintf(MainFile,
                    "%% erstellt durch das HippoPRESS-Auswertungsprogramm %s\n",
                    StatVersionString());
                    break;
                case 3:
                    /* Nummer der HippoPRESS-Ausgabe in der
                       Ueberschrift */
                    res= fprintf(MainFile, "%s\n", StatAbbrev());
                    break;
                case 4:
                    /* \input-Befehl fuer HippoWINNER-Auswertung */
                    strcat(line, "winn");
                    strcat(line, StatAbbrev());
                    strcat(line, "}");
                    res= fprintf(MainFile, "%s\n", line);
                    break;
                case 5:
                    /* \input-Befehl fuer HippoCHARTS-Auswertung */
                    strcat(line, "char");
                    strcat(line, StatAbbrev());
                    strcat(line, "}");
                    res= fprintf(MainFile, "%s\n", line);
                    break;
                case 6:
                    /* \input-Befehl fuer HippoCHARTS-Sammelauswertung */
                 /* yyy   if (TabChartSum())
                    {
                        strcat(line, "csum");
                        strcat(line, StatAbbrev());
                        strcat(line, "}");
                        res= fprintf(MainFile, "%s\n", line);
                    } */
                    break;
                case 7:
                    /* \input-Befehl fuer HippoGAMES-Eishockey-Auswertung */
                    if (TabGetGame() & 1)
                    {
                        strcat(line, "ice");
                        strcat(line, StatAbbrev());
                        strcat(line, "}");
                        res= fprintf(MainFile, "%s\n", line);
                    }
                    break;
                case 8:
                    /* \input-Befehl fuer HippoGAMES-Tennis-Auswertung */
                    if (TabGetGame() & 2)
                    {
                        strcat(line, "tenn");
                        strcat(line, StatAbbrev());
                        strcat(line, "}");
                    }
                    res= fprintf(MainFile, "%s\n", line);
                    break;
                default:
                    printf("zu viele Marken...");
                    goto ErrorExit;
                } /* switch */
                if ((res == 0) || (res == EOF)) goto ErrorExit;
            }
            else
            {
                res= fprintf(MainFile, "%s\n", TeXFile[i]);
                if ((res == 0) || (res == EOF)) goto ErrorExit;
            } /* if (MarkFound) else */
            i++;
        } /* if (loop) */
    } while(loop && (i < 50));

    res= fclose(MainFile);
    if (res == EOF)
    {
        printf("kann Datei nicht schliessen\n");
        return -3;
    }

    printf("OK\n");
    return 0;

    ErrorExit:
        printf("Fehler\n");
        res= fclose(MainFile); 
        return -3;
} /* OutTexMain */

/****** Funktionen fuer HippoWINNER: ******/

int OutWinnerStart(void)
{
    if (OutputInProgress != 0)
    {
        printf(
            "OutWinnerStart: aktive Ausgabe: %d; erwartet: 0\n",
            OutputInProgress);
        goto ErrorExit;
    }

    CurrentFile= fopen(StatFileName(STAT_WINNER), "w");
    if (CurrentFile == NULL) goto ErrorExit;

    OutputInProgress= OUT_WINNER;
    return 0;
    
    ErrorExit:
        return 1;
} /* OutWinnerStart */

int OutWinnerTableStart(void)
{
    int eof;
    
    if (OutputInProgress != OUT_WINNER)
    {
        printf(
            "OutWinnerTableStart: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_WINNER);
        goto ErrorExit;
    }
    
    eof= fprintf(CurrentFile,"\\begin{center}\n");
    if ( (eof == 0) || (eof == EOF) ) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\begin{tabular}[h]{|l||ccccc|c|}\n");
    if ( (eof == 0) || (eof == EOF) ) goto ErrorExit;
    eof= fprintf(CurrentFile,"    \\hline\n");
    if ( (eof == 0) || (eof == EOF) ) goto ErrorExit;

    return 0;
    
    ErrorExit:
        return 1;
} /* OutWinnerTableStart */

int OutWinnerTableEnd(void)
{
    int eof;
    
    if (OutputInProgress != OUT_WINNER)
    {
        printf("OutWinnerTableEnd: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_WINNER);
        goto ErrorExit;
    }
        
    eof= fprintf(CurrentFile,"\\end{tabular}\n");
    if ( (eof == 0) || (eof == EOF) ) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\end{center}\n");
    if ( (eof == 0) || (eof == EOF) ) goto ErrorExit;
    
    return 0;
    
    ErrorExit:
        return 1;
} /* OutWinnerTableEnd */

int OutWinnerTableLine(char *name, 
        int a, int b, int c, int d, int e, int sum)
{
    int eof;
    
    if (OutputInProgress != OUT_WINNER)
    {
        printf("OutWinnerTableLine: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_WINNER);
        goto ErrorExit;
    }
    
    eof= fprintf(CurrentFile,
        "    %s & %d & %d & %d & %d & %d & %d \\\\ \\hline\n",
        name, a, b, c, d, e, sum
    );
    if ( (eof == 0) || (eof == EOF) ) goto ErrorExit;
    
    return 0;
    
    ErrorExit:
        return 1;
} /* OutWinnerTableLine */

int OutWinnerEnd(void)
{
    int res;
    
    if (OutputInProgress != OUT_WINNER)
    {
        printf("OutWinnerEnd: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_WINNER);
        goto ErrorExit;
    }
    
    OutputInProgress= 0;
    
    res= fclose(CurrentFile); 
    CurrentFile= NULL;
    if (res != 0) goto ErrorExit;
    
    return 0;
    
    ErrorExit:
        return 1;
} /* OutWinnerEnd */

/****** Funktionen fuer HippoCHARTS: ******/

int OutChartsStart(void)
{
    if (OutputInProgress != 0)
    {
        printf(
            "OutChartsStart: aktive Ausgabe: %d; erwartet: 0\n",
            OutputInProgress);
        goto ErrorExit;
    }

    CurrentFile= fopen(StatFileName(STAT_CHARTS), "w");
    if (CurrentFile == NULL) goto ErrorExit;

    OutputInProgress= OUT_CHARTS;

    return 0;
    
    ErrorExit:
        return 1;
} /* OutChartsStart */

int OutChartsTableStart(void)
{
    int eof;

    if (OutputInProgress != OUT_CHARTS)
    {
        printf(
            "OutChartsTableStart: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHARTS);
        goto ErrorExit;
    }
    
    eof= fprintf(CurrentFile,"\\begin{center}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\begin{tabular}[h]{|r|r|r|r|l|l|l|}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"  \\hline\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"  Platz & Pkt. & (1) & (2) & Titel &");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile," Verlag & Autor\\\\\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"  \\hline \\hline\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;

    return 0;

    ErrorExit:
        return 1;
} /* OutWinnerTableStart */

int OutChartsTableEnd(void)
{
    int eof;

    if (OutputInProgress != OUT_CHARTS)
    {
        printf(
            "OutChartsTableEnd: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHARTS);
        goto ErrorExit;
    }
    
    eof= fprintf(CurrentFile,"  \\hline\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\end{tabular}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\end{center}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;

    return 0;

    ErrorExit:
        return 1;
} /* OutChartsTableEnd */

int OutChartsTableLine(int rank, int points, int sum, int count,
                       char *name, char *publisher, char *author)
{
    int eof;
    
    if (OutputInProgress != OUT_CHARTS)
    {
        printf(
            "OutChartsTableLine: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHARTS);
        goto ErrorExit;
    }
    
    if (rank >= 0)
    {
        eof= fprintf(CurrentFile,
                "  %3d & %3d & %2d & %2d & %s & %s & %s \\\\\n",
                rank, points, sum, count, name, publisher, author);
        if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    }
    else
    {
        eof= fprintf(CurrentFile,
                 "      & %3d & %2d & %2d & %s & %s & %s \\\\\n",
                 points, sum, count, name, publisher, author);
        if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    } 

    return 0;

    ErrorExit:
        return 1;
} /* OutChartsTableLine */

int OutChartsEnd(void)
{
    int res;
    
    if (OutputInProgress != OUT_CHARTS)
    {
        printf(
            "OutChartsEnd: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHARTS);
        goto ErrorExit;
    }
    
    OutputInProgress= 0;
    
    res= fclose(CurrentFile); 
    CurrentFile= NULL;
    if (res != 0) goto ErrorExit;
    
    return 0;
    
    ErrorExit:
        return 1;
} /* OutChartsEnd */

/****** Funktionen fuer Eishockey und Tennis: ******/

int OutGameStart(int game)
{
    char *name= NULL;

    if (OutputInProgress != 0)
    {
        printf(
            "OutChartsStart: aktive Ausgabe: %d; erwartet: 0\n",
            OutputInProgress);
        goto ErrorExit;
    }

    switch (game)
    {
    case 1: /* Eishockey */
        name= StatFileName(STAT_ICEHOCKEY);
        if (name == NULL)
        {
            printf("OutGameStart: Dateiname fuer Eishockey unbekannt\n");
            goto ErrorExit;
        }
        OutputInProgress= OUT_ICEHOCKEY;
        break;
    case 2: /* Tennis */
        name= StatFileName(STAT_TENNIS);
        if (name == NULL)
        {
            printf("OutGameStart: Dateiname fuer Tennis unbekannt\n");
            goto ErrorExit;
        }
        OutputInProgress= OUT_TENNIS;
        break;
    default:
        printf("OutGameStart: Spiel %d unbekannt\n", game);
        goto ErrorExit;
    } /* switch */
    CurrentFile= fopen(name, "w");
    if (CurrentFile == NULL) goto ErrorExit;
    OutGame= game;

    return 0;
    
    ErrorExit:
        OutputInProgress= 0;
        return 1;
} /* OutGameStart */

int OutGameTableStart(void)
{
    int res;

    if (   (OutputInProgress != OUT_ICEHOCKEY)
        && (OutputInProgress != OUT_TENNIS) )
    {
		printf("OutChartsTableStart: unerwartete aktive Ausgabe: %d\n", OutputInProgress);
        goto ErrorExit;
    }
    
    res= fprintf(CurrentFile,
        "\\begin{center}\n  Ergebnistabelle\\\\[1ex]\n");
    if ((res == 0) || (res == EOF)) goto ErrorExit;
    res= fprintf(CurrentFile,
        "  \\begin{tabular}[h]{|c|l|c|c|} \\hline\n");
    if ((res == 0) || (res == EOF)) goto ErrorExit;
    res= fprintf(CurrentFile,
        "    Platz & Name & Siege & %s \\\\ \\hline \\hline\n",
        (OutGame == 1) ? 
            "Tore" : "Spiele" );
    if ((res == 0) || (res == EOF)) goto ErrorExit;
    
    return 0;

    ErrorExit:
        return 1;
} /* OutGameTableStart */

int OutGameTableEnd(void)
{
    int res;

    if (   (OutputInProgress != OUT_ICEHOCKEY)
        && (OutputInProgress != OUT_TENNIS) )
    {
		printf("OutGameTableEnd: unerwartete aktive Ausgabe: %d\n", OutputInProgress);
        goto ErrorExit;
    }
    
    res= fprintf(CurrentFile,
             "  \\hline\n  \\end{tabular}\n\\end{center}");
    if ((res == 0) || (res == EOF)) goto ErrorExit;

    return 0;

    ErrorExit:
        return 1;
} /* OutGameTableEnd */

int OutGameTableLine(int rank, char *name, int wins, int goals)
{
    int  res;

    if (   (OutputInProgress != OUT_ICEHOCKEY)
        && (OutputInProgress != OUT_TENNIS))
    {
        printf("OutGameTableLine: unerwartete aktive Ausgabe: %d\n", OutputInProgress);
        goto ErrorExit;
    }

    if (rank >= 0)
    {
        res= fprintf(CurrentFile, 
            "    %2d & %-20s & %2d & $%+3d$ \\\\\n",
            rank, name, wins, goals);
    }
    else
    {
        res= fprintf(CurrentFile,
            "       & %-20s & %2d & $%+3d$ \\\\\n",
            name, wins, goals);
    } /* if */
    if ((res == 0) || (res == EOF)) goto ErrorExit;

   return 0;

    ErrorExit:
        return 1;
} /* OutGameTableLine */

int OutGameEnd(void)
{
    int res;

    if (   (OutputInProgress != OUT_ICEHOCKEY)
        && (OutputInProgress != OUT_TENNIS) )
    {
        printf("OutGameTableEnd: unerwartete aktive Ausgabe: %d\n", OutputInProgress);
        goto ErrorExit;
    }
    
    OutputInProgress= 0;
    OutGame= 0;
    
    res= fclose(CurrentFile);
    CurrentFile= NULL;
    if (res != 0) goto ErrorExit;
    

    return 0;

    ErrorExit:
        return 1;
} /* OutGameEnd */

/********** Funktionen fuer HippoCHARTS-Sammelauswertung: *********/

int OutChartSumStart(void)
{
    if (OutputInProgress != 0)
    {
        printf(
            "OutChartsStart: aktive Ausgabe: %d; erwartet: 0\n",
            OutputInProgress);
        goto ErrorExit;
    }

    CurrentFile= fopen(StatFileName(STAT_CHART_SUM), "w");
    if (CurrentFile == NULL) goto ErrorExit;

    OutputInProgress= OUT_CHART_SUM;

    return 0;
    
    ErrorExit:
        return 1;
} /* OutChartSumStart */

int OutChartSumTableStart(void)
{
    int eof;

    if (OutputInProgress != OUT_CHART_SUM)
    {
        printf(
            "OutChartsTableStart: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHART_SUM);
        goto ErrorExit;
    }
    
    eof= fprintf(CurrentFile,"\\begin{center}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\begin{tabular}[h]{|r|r|l|l|l|}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"  \\hline\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"  Platz & Pkt. & Titel &");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile," Verlag & Autor\\\\\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"  \\hline \\hline\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;

    return 0;

    ErrorExit:
        return 1;
} /* OutChartSumTableStart */

int OutChartSumTableEnd(void)
{
    int eof;

    if (OutputInProgress != OUT_CHART_SUM)
    {
        printf(
            "OutChartsTableEnd: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHART_SUM);
        goto ErrorExit;
    }
    
    eof= fprintf(CurrentFile,"  \\hline\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\end{tabular}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    eof= fprintf(CurrentFile,"\\end{center}\n");
    if ((eof == EOF) || (eof == 0)) goto ErrorExit;

    return 0;

    ErrorExit:
        return 1;
} /* OutChartSumTableEnd */

int OutChartSumTableLine(int rank, int sum,
                         char *name, char *publisher, char *author)
{
    int eof;
    
    if (OutputInProgress != OUT_CHART_SUM)
    {
        printf(
            "OutChartsTableLine: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHART_SUM);
        goto ErrorExit;
    }
    
    if (rank >= 0)
    {
        eof= fprintf(CurrentFile,
                "  %3d & %3d & %s & %s & %s \\\\\n",
                rank, sum, name, publisher, author);
        if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    }
    else
    {
        eof= fprintf(CurrentFile,
                 "      & %3d & %s & %s & %s \\\\\n",
                 sum, name, publisher, author);
        if ((eof == EOF) || (eof == 0)) goto ErrorExit;
    } 

    return 0;

    ErrorExit:
        return 1;
} /* OutChartSumTableLine */

int OutChartSumEnd(void)
{
    int res;
    
    if (OutputInProgress != OUT_CHART_SUM)
    {
        printf(
            "OutChartSumEnd: aktive Ausgabe: %d; erwartet: %d\n",
            OutputInProgress, OUT_CHART_SUM);
        goto ErrorExit;
    }
    
    OutputInProgress= 0;
    
    res= fclose(CurrentFile); 
    CurrentFile= NULL;
    if (res != 0) goto ErrorExit;
    
    return 0;
    
    ErrorExit:
        return 1;
} /* OutChartSumEnd */

