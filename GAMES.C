/* Datei: games.c */

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "games.h"
#include "stat.h"
#include "gameinit.h"

/* Default-Inhalt der Datei  gameinit.h:
static int GameInitCount= 0;
static char *GameInitTable[]= {
""
};
*/

#define GAMES_FORMAT_IDENT "games.c"
/* Identifikator: Spieledatenbank in der Datei GAMES_FILE_NAME */

#define GAMES_FORMAT_VERSION 1
/* Version des Formats der Datei GAMES_FILE_NAME */

typedef struct{
    char name[30]; /* Name des Spiels */
    char author[30]; /* Name des Autors */
    char publisher[30]; /* Name des Verlages */
    int  points; /* Anzahl der Punkte fuer das Spiel */
    int  CountOfVoters; /* Anzahl der Personen, die fuer das Spiel
                           Punkte verteilt haben */
    int  MergeRecord; /* 1: der Datensatz ist durch das Laden einer
                            Spielerdatei entstanden, deren Eintraege
                            nicht zur Spieledatenbank passen;
                            der Eintrag kann nicht von Hand geloescht
                            oder editiert werden; der Eintrag wird
                            beim Abspeichern auch nicht in einer
                            Datei abgelegt
                         0: normaler Datensatz
                      */
} tsGameRecord;

static tsGameRecord GamesTable[GAMES_TABLE_LENGTH];
static int GamesFirstUnused= 0;

static int GamesDataChanged= 0;
/* 1: Daten in 'GamesTable' sollten auf die Platte geschrieben werden;
   0: kein Schreiben erforderlich
*/

static int GamesReadRecord(FILE *file, int version, int index)
/* liest naechsten Spiele-Datensatz aus 'file' entsprechend des
   Datenformats 'version'; der Datensatz wird in 'GamesTable' unter
   dem 'index' abgelegt;
   Funktionswert: 0: alles OK;
                  1: Fehler aufgetreten;
                  2: Dateiende bereits erreicht; es wurde kein
                     Datensatz gelesen;
                  EOF: der letzte gueltige Datensatz wurde gelesen;
*/
{
    int eof;
    int res= 0;

    if (index >= GAMES_TABLE_LENGTH)
    {
        printf("Tabelle zu klein...\n");
        return EOF;
    }
    if ((index < 0) || (index > GamesFirstUnused))
    {
        printf(
        "Index fuer zu lesenden Datensatz fehlerhaft...\n");
        return EOF;
    }
    eof= UtilFileInput(file, GamesTable[index].name, 30);
    if (eof != 0)
        return 2;
    eof= UtilFileInput(file, GamesTable[index].author, 30);
    if (eof != 0) goto ErrorExit;
    eof= UtilFileInput(file, GamesTable[index].publisher, 30);
    if (eof != 0)
        res= EOF;

    GamesTable[index].points= 0;
    GamesTable[index].CountOfVoters= 0;
    GamesTable[index].MergeRecord= 0;

    if (version > 1)
    {
        /* tue vorlaeufig nichts */
    }

    return res;

    ErrorExit:
        printf("kann Datensatz nicht lesen\n...");
        return 1;
} /* GamesReadRecord */

static int GamesReadFile(char *name)
/* Die Tabelle wird mit Hilfe der Daten in der angegebenen Datei
   neu gefuellt.
   Funktionswert: 0: alles OK; 1: hat nicht geklappt
*/
{
    FILE *file;
    int eof;
    int ident;
    char IdentString[100];
    int  i;

    GamesDataChanged= 0;

/*    printf("lese Spieledatenbank '%s'...", name);*/
    printf("Spieledatenbank...");

    file= fopen(name, "r");
    if (file == NULL)
    {
        printf("nicht gefunden (wird neu angelegt)\n");
        /*
            initialisiere Datenbank mit Vorgabe durch das
            Programm:
        */
        for(i=0; i<GameInitCount; i++)
        {
            strcpy(GamesTable[i].name, GameInitTable[i*3]);
            strcpy(GamesTable[i].author, GameInitTable[i*3+1]);
            strcpy(GamesTable[i].publisher, GameInitTable[i*3+2]);
            GamesTable[i].points= 0;
            GamesTable[i].CountOfVoters= 0;
            GamesTable[i].MergeRecord= 0;
        }
        GamesFirstUnused= GameInitCount;
        GamesDataChanged= 1;
        return 0;
    }

    eof= fscanf(file, "%s", &IdentString);
    if ((eof == 0) || (eof == EOF))
    {
        printf("kann nicht lesen\n");
        eof= fclose(file);
        return 1;
    }
    if (strcmp(IdentString, GAMES_FORMAT_IDENT))
    {
        printf("Dateiformat nicht identifiziert\n");
        eof= fclose(file);
        return 1;
    }
    eof= fscanf(file, "%d\n", &ident);
    if ((eof == 0) || (eof == EOF))
    {
        printf("kann nicht lesen\n");
        eof= fclose(file);
        return 1;
    }
    if ((ident > 1) || (ident < 1))
    {
        printf("verstehe Dateiformat nicht\n");
        eof= fclose(file);
        return 1;
    }
    GamesFirstUnused= 0;
    do{
        eof= GamesReadRecord(file, ident, GamesFirstUnused);
        if ((eof == 0) || (eof == EOF))
            GamesFirstUnused++;
    } while ((eof != EOF) && (eof != 2));

    if (fclose(file) != 0)
    {
        printf("kann Datei nicht korrekt schliessen\n");
        return 1;
    }
    printf("%d Datensaetze...OK\n", GamesFirstUnused);
    return 0;
} /* GamesReadFile */

static int GamesWriteFile(char *name)
/* Die Tabelle wird in die angegebene Datei geschrieben.
   Funktionswert: 0: OK, 1: Fehler */
{
    FILE *file;
    int eof;
    int i;

    if (!GamesDataChanged) return 0;

/*    printf("schreibe Spieledatenbank '%s'... ", name);*/
    printf("Spieledatenbank...");

    file= fopen(name, "w");
    if (file == NULL)
    {
        printf("kann Datei nicht oeffnen\n");
        return 1;
    }

    eof= fprintf(file, "%s\n%d\n",
            GAMES_FORMAT_IDENT, GAMES_FORMAT_VERSION);
    if ((eof == 0) || (eof == EOF))
    {
        fprintf(stderr,
        "kann nicht schreiben\n");
        return 1;
    }

    for(i=0; i<GamesFirstUnused; i++)
    {
        if (!(GamesTable[i].MergeRecord))
        {
            eof= fprintf(file, "%s\n%s\n%s\n",
                GamesTable[i].name,
                GamesTable[i].author,
                GamesTable[i].publisher);
            if ((eof == 0) || (eof == EOF))
            {
                printf("kann Datensatz nicht schreiben\n");
                return 1;
            }
        }
    }

    if (fclose(file) != 0)
    {
        printf("kann Datei nicht korrekt schliessen\n");
        return 1;
    }
    printf("OK\n");

    GamesDataChanged= 0;

    return 0;
} /* GamesWriteFile */

int GamesStart(void)
{
    int res;
    res= GamesReadFile(StatFileName(STAT_GAMES));
    if (res == 0)
        GamesSort();
    return res;
} /* GamesStart */

int GamesEnd(void)
{
    return GamesWriteFile(StatFileName(STAT_GAMES));
} /* GamesEnd */

int GamesFlush(void)
{
    return GamesWriteFile(StatFileName(STAT_GAMES));
} /* GamesFlush */

void GamesSearch(char *name, int StartIndex, int mode,
                 int *FoundIndex)
{
    int  CurrentIndex;
    int  found= 0;
    char SearchName[100];
    char CheckName[100];
    
    CurrentIndex= StartIndex;
    *FoundIndex= -1;
    if (strlen(name) > 100) 
    {
        printf("GamesSearch: Parameter 'name': Zeichenkette zu lang\n");
        return;
    }
    strcpy(SearchName, name);
    if (!(mode & 2))
        UtilStrlwr(SearchName);
    while ((CurrentIndex < GamesFirstUnused) && (!found))
    {
        if (strlen(GamesTable[CurrentIndex].name) > 100)
        {
            printf("GamesSearch: Name des Spiels mit Index %d zu lang\n",
                CurrentIndex);
            return;
        }
        strcpy(CheckName, GamesTable[CurrentIndex].name);
        if (!(mode & 2))
            UtilStrlwr(CheckName);
        if (mode & 1)
            found= !strcmp(CheckName, SearchName);
        else
            found= (strstr(CheckName, SearchName) != NULL);
        CurrentIndex++;
    } /* while */
    if (found)
        *FoundIndex= CurrentIndex-1;
} /* GamesSearch */

int GamesGet(int index, char **name, char **author, char **publisher)
{
    if ((index < 0) || (index >= GamesFirstUnused))
    {
        *name= NULL;
        *author= NULL;
        *publisher= NULL;
        return 1;
    }

    *name= GamesTable[index].name;
    *author= GamesTable[index].author;
    *publisher= GamesTable[index].publisher;
    return 0;
} /* GamesGet */

void GamesNew(int *NewIndex, int MergeRecord)
{
    *NewIndex= GamesFirstUnused;
    GamesFirstUnused++;

    GamesTable[*NewIndex].name[0]= '\0';
    GamesTable[*NewIndex].author[0]= '\0';
    GamesTable[*NewIndex].publisher[0]= '\0';
    GamesTable[*NewIndex].points= 0;
    GamesTable[*NewIndex].CountOfVoters= 0;
    GamesTable[*NewIndex].MergeRecord= MergeRecord;

    if (GamesFirstUnused >= GAMES_TABLE_LENGTH)
    {
        printf("GamesNew: games table full\n");
    }

    if (!MergeRecord)
        GamesDataChanged= 1;
} /* GamesNew */

void GamesSet(int index, char *name, char *author, char *publisher,
              int UpdateMergeRecord)
{
    strcpy(GamesTable[index].name, name);
    strcpy(GamesTable[index].author, author);
    strcpy(GamesTable[index].publisher, publisher);

    /*
        falls der Inhalt eines Merge-Records neu gesetzt wird,
        entfaellt damit der Merge-Record-Status:
    */
    if (!UpdateMergeRecord)
        GamesTable[index].MergeRecord= 0;

    GamesDataChanged= 1;
} /* GamesSet */

void GamesGetVotes(int index, int *points, int *CountOfVoters)
{
    *points= GamesTable[index].points;
    *CountOfVoters= GamesTable[index].CountOfVoters;
} /* GamesGetVotes */

void GamesSetVotes(int index, int points, int CountOfVoters)
{
    GamesTable[index].points= points;
    GamesTable[index].CountOfVoters= CountOfVoters;
} /* GamesSetVotes */

int GamesTableSize()
{
    return GamesFirstUnused;
} /* GamesTableSize */

void GamesDelete(int index)
{
    int i;

    if ((index < 0) || (index >= GamesFirstUnused))
    {
        printf("GamesDelete: ungueltiger Index\n");
        return;
    }
    for(i=index+1; i<GamesFirstUnused; i++)
    {
        strcpy(GamesTable[i-1].name, GamesTable[i].name);
        strcpy(GamesTable[i-1].author, GamesTable[i].author);
        strcpy(GamesTable[i-1].publisher, GamesTable[i].publisher);
        GamesTable[i-1].points= GamesTable[i].points;
        GamesTable[i-1].CountOfVoters= GamesTable[i].CountOfVoters;
        GamesTable[i-1].MergeRecord= GamesTable[i].MergeRecord;
    }
    GamesFirstUnused--;
    GamesDataChanged= 1;
} /* GamesDelete */

void GamesSort(void)
{
    int i, swapped, res;
    tsGameRecord swap;

    printf("sortiere Spieledatenbank...");
    do
    {
        swapped= 0;
        for (i=0; i<GamesFirstUnused-1; i++)
        {
            res= strcmp(GamesTable[i].name, GamesTable[i+1].name);
            if (res > 0)
            {
                /* vorlaeufig keine Punkte: printf("."); */
                swapped= 1;
                if ( !(GamesTable[i].MergeRecord)
                     || !(GamesTable[i].MergeRecord)
                   )
                    GamesDataChanged= 1;

                strcpy(swap.name, GamesTable[i].name);
                strcpy(swap.author, GamesTable[i].author);
                strcpy(swap.publisher, GamesTable[i].publisher);
                swap.points= GamesTable[i].points;
                swap.CountOfVoters= GamesTable[i].CountOfVoters;
                swap.MergeRecord= GamesTable[i].MergeRecord;

                strcpy(GamesTable[i].name, GamesTable[i+1].name);
                strcpy(GamesTable[i].author, GamesTable[i+1].author);
                strcpy(GamesTable[i].publisher,
                       GamesTable[i+1].publisher);
                GamesTable[i].points= GamesTable[i+1].points;
                GamesTable[i].CountOfVoters=
                                       GamesTable[i+1].CountOfVoters;
                GamesTable[i].MergeRecord= GamesTable[i+1].MergeRecord;

                strcpy(GamesTable[i+1].name, swap.name);
                strcpy(GamesTable[i+1].author, swap.author);
                strcpy(GamesTable[i+1].publisher, swap.publisher);
                GamesTable[i+1].points= swap.points;
                GamesTable[i+1].CountOfVoters= swap.CountOfVoters;
                GamesTable[i+1].MergeRecord= swap.MergeRecord;
            }
        }
    } while (swapped);
    printf("OK\n");
} /* GamesSort */

int GamesTestMergeRecord(int index)
{
    if ((index < 0) || (index >= GamesFirstUnused))
        return 0;

    return GamesTable[index].MergeRecord;
} /* GamesTestMergeRecord */

void GamesWriteInit(void)
{
    int eof;
    int i;
    FILE *file;

    printf("schreibe Initialisierungsdatei gameinit.h...");
    file= fopen("gameinit.h", "w");
    if (file == NULL) goto ErrorExit;

    eof= fprintf(file, "static int GameInitCount= %d;\n",
                 GamesFirstUnused);
    if (eof == 0) goto ErrorExit;

    eof= fprintf(file, "static char *GameInitTable[]= {\n");
    if (eof == 0) goto ErrorExit;

    if (GamesFirstUnused > 0) 
    {
	    for(i=0; i<GamesFirstUnused; i++)
	    {
	        if (UtilWriteInitString(file, GamesTable[i].name))
	            goto ErrorExit;
	        if (fprintf(file, ",\n") == EOF) goto ErrorExit;
	
	        if (UtilWriteInitString(file, GamesTable[i].author))
	            goto ErrorExit;
	        if (fprintf(file, ",\n") == EOF) goto ErrorExit;
	
	        if (UtilWriteInitString(file, GamesTable[i].publisher))
	            goto ErrorExit;
	        if (i < GamesFirstUnused-1)
	            if (fprintf(file, ",\n") == EOF) goto ErrorExit;
	    }/* for */
	}
	else
	{
	    if (fprintf(file, "\"\"")) goto ErrorExit;
	}

    eof= fprintf(file, "\n};\n");
    if (eof == 0) goto ErrorExit;
    eof= fclose(file);
    if (eof != 0) goto ErrorExit;

    printf("OK\n");
    return;

    ErrorExit:
        printf("Fehler\n");
        return;
} /* GamesWriteInit */
