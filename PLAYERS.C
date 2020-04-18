/* Datei: players.c
*/

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "players.h"

#define PLAYERS_FILE_NAME "dice4.inf"
/* Datei, in der die Spieler gespeichert werden */

#define PLAYERS_FORMAT_IDENT "players.c"
/* Identifikator: Spieledatenbank in der Datei GAMES_FILE_NAME */

#define PLAYERS_FORMAT_VERSION 1
/* Version des Formats der Datei GAMES_FILE_NAME */

typedef struct{
    char FirstName[30]; /* Name des Spielers */
    char LastName[30];
    char street[40]; /* Strasse */
    char city[40]; /* Stadt */
    char email[80]; /* e-mail-Adresse */
    char phone[30]; /* Telefon */
} tsPlayerRecord;

static tsPlayerRecord PlayersTable[PLAYERS_TABLE_LENGTH];
static int PlayersFirstUnused= 0;

static int PlayersDataChanged= 0;
/* 1: Daten in 'PlayersTable' sollten auf die Platte geschrieben werden;
   0: kein Schreiben erforderlich
*/

static int PlayersReadRecord(FILE *file, int version, int index)
/* liest naechsten Spieler-Datensatz aus 'file' entsprechend des
   Datenformats 'version'; der Datensatz wird in 'PlayersTable' unter
   dem 'index' abgelegt;
   Funktionswert: 0: alles OK;
                  1: Fehler aufgetreten;
                  EOF: der letzte gueltige Datensatz wurde gelesen;
*/
{
    int eof;
    int res= 0;

    if (index >= PLAYERS_TABLE_LENGTH)
    {
        printf("Tabelle zu klein...\n");
        return EOF;
    }
    if ((index < 0) || (index > PlayersFirstUnused))
    {
        printf(
        "Index fuer zu lesenden Datensatz fehlerhaft...\n");
        return EOF;
    }
    eof= UtilFileInput(file, PlayersTable[index].FirstName, 30);
    if (eof != 0) goto ErrorExit;
    eof= UtilFileInput(file, PlayersTable[index].LastName, 30);
    if (eof != 0) goto ErrorExit;
    eof= UtilFileInput(file, PlayersTable[index].street, 40);
    if (eof != 0)
        res= EOF;

    if (version > 1)
    {
        /* tue vorlaeufig nichts */
    }

    return res;

    ErrorExit:
        printf("kann Datensatz nicht lesen\n...");
        return 1;
} /* PlayersReadRecord */

static int PlayersReadFile(char *name)
/* Die Tabelle wird mit Hilfe der Daten in der angegebenen Datei
   neu gefuellt.
   Funktionswert: 0: alles OK; 1: hat nicht geklappt
*/
{
    FILE *file;
    int eof;
    int ident;
    char IdentString[100];

    PlayersDataChanged= 0;

    printf("lese Spieledatenbank '%s'...", name);

    file= fopen(name, "r");
    if (file == NULL)
    {
        printf("nicht gefunden (wird neu angelegt)\n");
        PlayersFirstUnused= 0;
        return 0;
    }

    eof= fscanf(file, "%s", &IdentString);
    if ((eof == 0) || (eof == EOF))
    {
        printf("kann nicht lesen\n");
        eof= fclose(file);
        return 1;
    }
    if (strcmp(IdentString, PLAYERS_FORMAT_IDENT))
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
    PlayersFirstUnused= 0;
    do{
        eof= PlayersReadRecord(file, ident, PlayersFirstUnused);
        if ((eof == 0) || (eof == EOF))
            PlayersFirstUnused++;
    } while (eof != EOF);

    if (fclose(file) != 0)
    {
        printf("kann Datei nicht korrekt schliessen\n");
        return 1;
    }
    printf("%d Datensaetze...OK\n", PlayersFirstUnused);
    return 0;
} /* PlayersReadFile */

static int PlayersWriteFile(char *name)
/* Die Tabelle wird in die angegebene Datei geschrieben.
   Funktionswert: 0: OK, 1: Fehler */
{
    FILE *file;
    int eof;
    int i;

    if (!PlayersDataChanged) return 0;

    printf("schreibe Spielerdatenbank '%s'... ", name);

    file= fopen(name, "w");
    if (file == NULL)
    {
        printf("kann Datei nicht oeffnen\n");
        return 1;
    }

    eof= fprintf(file, "%s\n%d\n",
            PLAYERS_FORMAT_IDENT, PLAYERS_FORMAT_VERSION);
    if ((eof == 0) || (eof == EOF))
    {
        fprintf(stderr,
        "kann nicht schreiben\n");
        return 1;
    }

    for(i=0; i<PlayersFirstUnused; i++)
    {
        eof= fprintf(file, "%s\n%s\n%s\n",
            PlayersTable[i].FirstName,
            PlayersTable[i].LastName,
            PlayersTable[i].street);
        if ((eof == 0) || (eof == EOF))
        {
            printf("kann Datensatz nicht schreiben\n");
            return 1;
        }
    }

    if (fclose(file) != 0)
    {
        printf("kann Datei nicht korrekt schliessen\n");
        return 1;
    }
    printf("OK\n");

    PlayersDataChanged= 0;

    return 0;
} /* PlayersWriteFile */

int PlayersStart(void)
{
    int res;
    res= PlayersReadFile(PLAYERS_FILE_NAME);
    if (res == 0)
        PlayersSort();
    return res;
} /* PlayersStart */

int PlayersEnd(void)
{
    return PlayersWriteFile(PLAYERS_FILE_NAME);
} /* PlayersEnd */

int PlayersFlush(void)
{
    return PlayersWriteFile(PLAYERS_FILE_NAME);
} /* PlayersFlush */

void PlayersSearch(char *name, int StartIndex, int exact,
                 int *FoundIndex)
{
    int  CurrentIndex;
    int  found= 0;

    CurrentIndex= StartIndex;
    *FoundIndex= -1;
    while ((CurrentIndex < PlayersFirstUnused) && (!found))
    {
        if (exact)
            found= !strcmp(PlayersTable[CurrentIndex].FirstName, name);
        else
            found=
                (strstr(PlayersTable[CurrentIndex].FirstName, name) != NULL);
        CurrentIndex++;
    }
    if (found)
        *FoundIndex= CurrentIndex-1;
} /* PlayersSearch */

void PlayersGet(int index, char **name, char **author, char **publisher)
{
    *name= PlayersTable[index].FirstName;
    *author= PlayersTable[index].LastName;
    *publisher= PlayersTable[index].street;
} /* PlayersGet */

void PlayersNew(int *NewIndex)
{
    *NewIndex= PlayersFirstUnused;
    PlayersFirstUnused++;

    PlayersTable[*NewIndex].FirstName[0]= '\0';
    PlayersTable[*NewIndex].LastName[0]= '\0';
    PlayersTable[*NewIndex].street[0]= '\0';

    if (PlayersFirstUnused >= PLAYERS_TABLE_LENGTH)
    {
        printf("PlayersNew: players table full\n");
    }

    PlayersDataChanged= 1;
} /* PlayersNew */

void PlayersSet(int index, char *name, char *author, char *publisher)
{
    strcpy(PlayersTable[index].FirstName, name);
    strcpy(PlayersTable[index].LastName, author);
    strcpy(PlayersTable[index].street, publisher);

    PlayersDataChanged= 1;
} /* PlayersSet */

int PlayersTableSize()
{
    return PlayersFirstUnused;
} /* PlayersTableSize */

void PlayersDelete(int index)
{
    int i;

    if ((index < 0) || (index >= PlayersFirstUnused))
    {
        printf("PlayersDelete: ungueltiger Index\n");
        return;
    }
    for(i=index+1; i<PlayersFirstUnused; i++)
    {
        strcpy(PlayersTable[i-1].FirstName, PlayersTable[i].FirstName);
        strcpy(PlayersTable[i-1].LastName, PlayersTable[i].LastName);
        strcpy(PlayersTable[i-1].street, PlayersTable[i].street);
    }
    PlayersFirstUnused--;
} /* PlayersDelete */

void PlayersSort(void)
{
    int i, swapped, res;
    tsPlayerRecord swap;

    printf("sortiere Spieledatenbank...");
    do
    {
        swapped= 0;
        for (i=0; i<PlayersFirstUnused-1; i++)
        {
            res= strcmp(PlayersTable[i].FirstName, 
                        PlayersTable[i+1].FirstName);
            if (res > 0)
            {
                printf(".");
                swapped= 1;
                PlayersDataChanged= 1;

                strcpy(swap.FirstName, PlayersTable[i].FirstName);
                strcpy(swap.LastName, PlayersTable[i].LastName);
                strcpy(swap.street, PlayersTable[i].street);

                strcpy(PlayersTable[i].FirstName, PlayersTable[i+1].FirstName);
                strcpy(PlayersTable[i].LastName, PlayersTable[i+1].LastName);
                strcpy(PlayersTable[i].street,
                       PlayersTable[i+1].street);

                strcpy(PlayersTable[i+1].FirstName, swap.FirstName);
                strcpy(PlayersTable[i+1].LastName, swap.LastName);
                strcpy(PlayersTable[i+1].street, swap.street);
            }
        }
    } while (swapped);
    printf("OK\n");
} /* PlayersSort */
