/* Datei: sum.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "stat.h"

static char *SumFormatIdent= "HippoCHARTS Sammelauswertung";
static int   SumFormatVersion= 1;

static FILE *SumFile1, *SumFile2;

static char SumAbbrev[5];
static char SumName[50];
static int  SumPoints;

static int  SumRecordValid;

static char SumPattern[5];
typedef struct _SumStructAbbrev {
    char                     abbrev[10];
    struct _SumStructAbbrev *next;
} SumStructAbbrev;

static SumStructAbbrev *SumAbbrevList= NULL; 
    /* einfach verkettete Liste von Kuerzeln */

static SumStructAbbrev *SumCurrentAbbrev= NULL;
    /* aktueller Datensatz in der Liste, auf deren Anfang  SumAbbrevList
       zeigt
    */

typedef struct _SumStructPoints{
    char                     name[50];
    int                      points;
    struct _SumStructPoints *next;
} SumStructPoints;

static SumStructPoints *SumPointsList= NULL;
    /* einfach verkettete Liste von Punktsummen */

static SumStructPoints *SumCurrentPoints= NULL;
    /* aktueller Datensatz in der Liste, auf deren Anfang  SumPointsList
       zeigt
    */

/******
    lokale Funktionen zur Handhabung der Daten fuer die Sammelauswertung:
    - SumOpenDataFile()
    - SumCloseDataFile()
    - SumResetDataFile()
        aufrufen, wenn die mit SumWriteRecord() geschriebenen Datensaetze
        die neue Datenbank bilden sollen
    - SumReadRecord()
        nach Aufruf Datensatz in SumAbbrev, SumName und 
        SumPoints, falls  SumRecordValid, sonst Dateiende erreicht
    - SumWriteRecord()
        schreibt Datensatz in SumAbbrev, SumName und SumPoints
    Funktionswerte: 0: ok; -1: Fehler;
******/

static int SumReadHeader(void)
{
    int   res;
    char  buffer[80];
    int   version;
    
    res= UtilFileInput(SumFile1, buffer, 80);
    if (res != 0) goto ErrorExit;
    if (strcmp(SumFormatIdent, buffer)) 
    {
        printf("Dateiformat nicht erkannt...");
        goto ErrorExit;
    }
    
    res= UtilFileInput(SumFile1, buffer, 80);
    /* hier keine Fehlerabfrage, da End-Of-File erlaubt */
    res= sscanf(buffer, "%d", &version);
    if (res != 1) goto ErrorExit;
    if (version < 1) 
    {
        printf("Versionsangabe %d fehlerhaft...", version);
        goto ErrorExit;
    }
    if (version > SumFormatVersion)
    {
        printf("Dateiformat %d zu neu...", version);
        goto ErrorExit;
    }
    
    return 0;
    
    ErrorExit:
        printf("Fehler\n");
        return -1;
} /* SumReadHeader */

static int SumWriteHeader(void)
{
    int res;
    
    res= fprintf(SumFile2, "%s\n%d\n", SumFormatIdent, 1);
    if (res == EOF) return -1;

    return 0;
} /* SumWriteHeader */

static int SumOpenDataFile(void)
/* SumDataFile1  (entspricht SumFile1) wird zum Lesen und 
   SumDataFile2  (entspricht SumFile2) wird zum Schreiben geoeffnet;
   
   return: 0: ok; -1: Fehler
*/
{  
    SumFile1= fopen(StatFileName(STAT_CHART_SUM_DATA), "r");
    if (SumFile1 == NULL) return -1;
    SumFile2= fopen(StatFileName(STAT_CHART_SUM_HELP), "w");
    if (SumFile2 == NULL) 
    {
        fclose(SumFile1);
        return -1;
    }
    
    if (SumReadHeader()) goto ErrorExit;
    if (SumWriteHeader()) goto ErrorExit;

    return 0;
    
    ErrorExit:
        fclose(SumFile1);
        fclose(SumFile2);
        return -1;
} /* SumOpenDataFile */

static int SumCloseDataFile(void)
{
    int res;
    
    res= fclose(SumFile1);
    if (res == EOF) 
    {
        res= fclose(SumFile2);
        return -1;
    }
    res= fclose(SumFile2);
    if (res == EOF) return -1;
    
    return 0;
} /* SumCloseDataFile */

static int SumResetDataFile(void)
/* Der Inhalt von  SumDataFile2  wird  in  SumDataFile1
   geschrieben. return: 0: ok; -1: Fehler;
*/
{
    int   res;
    FILE *tmp;
    char  data[30];

    strcpy(data, StatFileName(STAT_CHART_SUM_DATA));
    res= UtilCopyFile(data, StatFileName(STAT_CHART_SUM_BACKUP));
    if (res != 0) goto ErrorExit;
    res= UtilCopyFile(StatFileName(STAT_CHART_SUM_HELP), data);
    if (res != 0) goto ErrorExit;

    return 0;

    ErrorExit:
        tmp= fopen(StatFileName(STAT_CHART_SUM_HELP), "w");
        res= fclose(tmp);
        res= UtilCopyFile(StatFileName(STAT_CHART_SUM_BACKUP), data);
        printf(
        "SumResetDataFile: Datenaenderungen wegen Verarbeitungsfehler\n");
        printf(
        "                  nicht fuer HippoCHARTS Sammelauswertung uebernommen\n");
        return -1;        
} /* SumResetDataFile */

static int SumReadRecord(void)
{
    char buffer[80];
    int  res;
    
    SumRecordValid= 0;
    
    res= UtilFileInput(SumFile1, buffer, 80);
    if (res != 0) return 1;
    if (strlen(buffer) > 4) goto ErrorExit;
    strcpy(SumAbbrev, buffer);
    
    res= UtilFileInput(SumFile1, buffer, 80);
    if ((res != 0) || (strlen(buffer) > 50)) goto ErrorExit;
    strcpy(SumName, buffer);
    
    res= UtilFileInput(SumFile1, buffer, 80);
    /* hier keine Fehlerabfrage, da End-Of-File erlaubt */
    res= sscanf(buffer, "%d", &SumPoints);
    if (res != 1) goto ErrorExit;

    SumRecordValid= 1;
    return 0;
    
    ErrorExit:
        printf("\nSumReadRecord: Datensatz nicht korrekt gelesen\n");
        return -1;
} /* SumReadRecord */

static int SumWriteRecord(void)
{
    int res;
    
    res= fprintf(SumFile2, "%s\n%s\n%d\n", SumAbbrev, SumName, SumPoints);
    if (res == EOF) return -1;
    return 0;
} /* SumWriteRecord */

/***********************************************************************/
/* Funktionen zur Handhabung der Kuerzelliste:
       SumClearAbbrev()
       SumEnterAbbrev()
       SumSortAbbrev()
*/

static void SumClearAbbrev(void)
/* Liste der Kuerzel wird geloescht */
{
    SumStructAbbrev *delete;
    
    SumCurrentAbbrev= SumAbbrevList;
    
    while (SumCurrentAbbrev != NULL)
    {
        delete= SumCurrentAbbrev;
        SumCurrentAbbrev= SumCurrentAbbrev->next;
        free(delete);
    } /* while */
    
    SumAbbrevList= NULL;
} /* SumClearAbbrev */

static int SumEnterAbbrev(char *abbrev)
/* Kuerzel in Kuerzelliste einfuegen, falls es dort noch nicht 
   vorhanden ist
*/
{
    SumStructAbbrev *new;
    SumStructAbbrev *current;
    int              found;
    
    current= SumAbbrevList;
    found= 0;
    while ((current != NULL) && !found)
    {
        found= !strcmp(abbrev, current->abbrev);
        current= current->next;
    }
    
    if (!found)
    {
        new= (SumStructAbbrev *) malloc(sizeof(SumStructAbbrev));
        if (new == NULL) 
        {
            printf("SumEnterAbbrev: Fehler: Hauptspeicher voll\n");
            return -1;
        }
        strcpy(new->abbrev, abbrev);
        new->next= SumAbbrevList;
        SumAbbrevList= new;
    }
    
    return 0;
} /* SumEnterAbbrev */

static void SumSortAbbrev(void)
/* Kuerzelliste wird alphabetisch sortiert (Bubble-Sort) */
{
    int swapped, SwapThis;
    SumStructAbbrev *current, *last, *next;
    
    do
    {
        current= SumAbbrevList;
        next= (current != NULL) ? current->next : NULL;
        last= NULL;
        swapped= 0;
        while ((current != NULL) && (next != NULL))
        {
            SwapThis= (strcmp(current->abbrev, next->abbrev) > 0);
            swapped= swapped || SwapThis;
            if (SwapThis)
            {
                /* 
                    tausche aus: 
                */
                current->next= next->next;
                next->next= current;
                if (last != NULL)
                    last->next= next;
                else
                    SumAbbrevList= next;
                    
                /* 
                    setze last fuer naechsten Durchlauf: 
                */
                last= next;
            }
            else
            {
                /* 
                    es wurde nicht getauscht: setze current und last weiter: 
                */
                last= current;
                current= next;                
            } /* else */
            next= current->next;
        } /* while ((current != NULL) && (next != NULL)) */
    } while (swapped);
    
    SumCurrentAbbrev= SumAbbrevList;
    
} /* SumSortAbbrev */

/***********************************************************************/
/* Funktionen zur Handhabung der Punkteliste:
       SumClearPoints()
       SumEnterPoints()
       SumSortPoints()
*/

static void SumClearPoints(void)
/* Liste der Abstimmungspunkte wird geloescht */
{
    SumStructPoints *delete;
    
    SumCurrentPoints= SumPointsList;
    
    while (SumCurrentPoints != NULL)
    {
        delete= SumCurrentPoints;
        SumCurrentPoints= SumCurrentPoints->next;
        free(delete);
    } /* while */
    
    SumPointsList= NULL;
} /* SumClearPoints */

static int SumEnterPoints(void)
/* Die Punkte des Datensatzes (SumAbbrev, SumName, SumPoints) werden der
   Punktliste hinzugefuegt.
   Funktionswert: 0: ok; -1: Fehler;
*/
{
    SumStructPoints *new;
    SumStructPoints *current;
    int              found;
    
    current= SumPointsList;
    found= 0;
    while ((current != NULL) && !found)
    {
        found= !strcmp(SumName, current->name);
        if (found)
            current->points= current->points + SumPoints;
        current= current->next;
    }
    
    if (!found)
    {
        new= (SumStructPoints *) malloc(sizeof(SumStructPoints));
        if (new == NULL) 
        {
            printf("SumEnterPoints: Fehler: Hauptspeicher voll\n");
            return -1;
        }
        strcpy(new->name, SumName);
        new->points= SumPoints;
        new->next= SumPointsList;
        SumPointsList= new;
    }
    
    return 0;
} /* SumEnterPoints */

static void SumSortPoints(void)
/* Die Punkteliste wird nach den Punkten und alphabetisch (2. Kriterium)
   sortiert.
*/
{
    int swapped, SwapThis;
    SumStructPoints *current, *last, *next;
    
    do
    {
        current= SumPointsList;
        next= (current != NULL) ? current->next : NULL;
        last= NULL;
        swapped= 0;
        while (current->next != NULL)
        {
            SwapThis= (current->points < next->points) ||
                      (strcmp(current->name, next->name) > 0);
            swapped= swapped || SwapThis;
            if (SwapThis)
            {
                /* 
                    tausche aus: 
                */
                current->next= next->next;
                next->next= current;
                if (last != NULL)
                    last->next= next;
                else
                    SumPointsList= next;
                    
                /* 
                    setze last fuer naechsten Durchlauf: 
                */
                last= next;
            }
            else
            {
                /* 
                    es wurde nicht getauscht: setze current und last weiter: 
                */
                last= current;
                current= next;                
            } /* else */
            next= current->next;
        } /* while (current->next != NULL) */
    } while (swapped);
} /* SumSortPoints */

/***********************************************************************/

int SumStart(void)
{
    int exists, res;
    
/*    printf(
    "teste auf Existenz der Sammelauswertungsdatenbank '%s'...\n",
        StatFileName(STAT_CHART_SUM_DATA));*/
    printf("Datenbank fuer Sammelauswertungen...");

    exists= UtilFileExists(StatFileName(STAT_CHART_SUM_DATA));
    if (exists == -1) goto ErrorExit2;
    if (!exists)
    {
        printf("wird neu angelegt...");

        SumFile2= fopen(StatFileName(STAT_CHART_SUM_DATA), "w");
        if (SumFile2 == NULL) goto ErrorExit2;
        res= SumWriteHeader();
        if (res) goto ErrorExit;
        res= fclose(SumFile2);
        if (res == EOF) goto ErrorExit2;
    }
/*    else
        printf("    existiert...");*/

    printf("OK\n");    
    return 0;

    ErrorExit:
        res= fclose(SumFile2);    
    ErrorExit2:
        printf("Fehler\n");
        return -1;
} /* SumStart */

int SumEnd(void)
{
    SumClearAbbrev();
    SumClearPoints();
    return 0;
} /* SumEnd */

int SumAdd(char *abbrev, char *name, int points)
{
    FILE *file;
    int   res;
    
    if ((strlen(abbrev) < 1) || (strlen(abbrev) > 4))
    {
        printf("SumAdd: Laenge des Identifikationskuerzels fehlerhaft\n");
        return -1;
    }
    if (strlen(name) > 50)
    {
        printf("SumAdd: Name zu lang\n");
        return -1;
    }

    file= fopen(StatFileName(STAT_CHART_SUM_DATA), "a");
    if (file == NULL) goto ErrorExit2;
    
    res= fprintf(file, "%s\n%s\n%d\n", abbrev, name, points);
    if (res == EOF) goto ErrorExit;
    
    res= fclose(file);
    if (res == EOF) goto ErrorExit2;
    
    return 0;
    
    ErrorExit:
        res= fclose(file);
    ErrorExit2:
        return -1;    
} /* SumAdd */

int SumDelete(char *abbrev)
{ 
    int deleted= 0;

    if (SumOpenDataFile()) return -1;
    
    if (SumReadRecord() < 0) goto ErrorExit;
    while(SumRecordValid)
    {
        if (strcmp(SumAbbrev, abbrev))
        {
            if (SumWriteRecord()) goto ErrorExit;
        }
        else
            deleted= 1;
        if (SumReadRecord() < 0) goto ErrorExit;
    } /* while */

    if (SumCloseDataFile()) return -1;
    if (deleted)
        if (SumResetDataFile()) return -1;

    return 0;
    
    ErrorExit:
        SumCloseDataFile();
        return -1;
} /* SumDelete */

int SumSetPattern(char *pattern)
/* ? ist Wildcard; filtert Ergebnisse von  SumNextEntry()  anhand des
   Musters;
   Funktionswert: 0: ok; -1: Fehler
*/
{
    if (strlen(pattern) != 4) return -1;
    strcpy(SumPattern, pattern);

    return 0;
} /* SumSetPattern */

static int SumCheckPattern(char *abbrev)
/* Es wird geprueft, ob  abbrev  durch das in  SumPattern  abgelegte
   Muster abgedeckt wird.
   Funktionswert: 0: nicht abgedeckt; 1: abgedeckt;
*/
{
    int   i= 0;
    char *pat1, *pat2;
    int   equ;
    
    pat1= abbrev; pat2= SumPattern;
    
    do
    {
        equ= ((*pat1 == *pat2) || (*pat2 == '?'));
        pat1++; pat2++; i++;
    } while(equ && (i < 4));
    
    return equ;
} /* SumCheckPattern */

int SumListEntries(void)
/* SumNextEntry()-Aufrufe vorbereiten;
   Funktionswert: 0: ok; -1: Fehler;
*/
{
    int res;
    
    SumClearAbbrev();
    res= SumOpenDataFile();
    if (res) return -1;
    
    do 
    {
        if (SumReadRecord() < 0) return -1;
    }
    while ((!SumCheckPattern(SumAbbrev)) && SumRecordValid);
        
    return 0;
} /* SumListEntries */

int SumNextEntry(char *abbrev, char *name, int *points)
{
    if (!SumRecordValid)
    {
        SumSortAbbrev();
        SumCurrentAbbrev= SumAbbrevList;
        if (SumCloseDataFile()) goto ErrorExit;
        return 1;
    }

    if (SumEnterAbbrev(SumAbbrev)) goto ErrorExit;
    SumCurrentAbbrev= SumAbbrevList;
    strcpy(abbrev, SumAbbrev);
    strcpy(name, SumName);
    *points= SumPoints;
    
    do 
    {
        if (SumReadRecord() < 0) goto ErrorExit;
    }
    while ((!SumCheckPattern(SumAbbrev)) && SumRecordValid);

    return 0;
    
    ErrorExit:
        return -1;
} /* SumNextEntry */

int SumListAbbrev(void)
{
    SumClearAbbrev();
    if (SumOpenDataFile()) return -1;
    
    do
    {
        if (SumReadRecord() < 0) goto ErrorExit;
        if (SumRecordValid)
            if (SumEnterAbbrev(SumAbbrev)) goto ErrorExit;
    } while (SumRecordValid);

    SumSortAbbrev();
    SumCurrentAbbrev= SumAbbrevList;
    if (SumCloseDataFile()) goto ErrorExit;
    
    return 0;
    
    ErrorExit:
        SumCloseDataFile();
        return -1;
} /* SumListAbbrev */

int SumNextAbbrev(char *abbrev)
{
    if (SumCurrentAbbrev == NULL) return 1;
    
    strcpy(abbrev, SumCurrentAbbrev->abbrev);
    SumCurrentAbbrev= SumCurrentAbbrev->next;
    
    return 0;
} /* SumNextAbbrev */

int SumListPoints(void)
{
    SumClearPoints();
    SumClearAbbrev();
    if (SumOpenDataFile()) return -1;
    
    do
    {
        if (SumReadRecord() < 0) goto ErrorExit;
        if (SumRecordValid)
            if (SumCheckPattern(SumAbbrev))
            {
                if (SumEnterPoints()) goto ErrorExit;
                if (SumEnterAbbrev(SumAbbrev)) goto ErrorExit;
            }
    } while (SumRecordValid);

    SumSortPoints();
    SumCurrentPoints= SumPointsList;
    SumSortAbbrev();
    SumCurrentAbbrev= SumAbbrevList;
    SumCloseDataFile();
    
    return 0;
    
    ErrorExit:
        SumCloseDataFile();
        return -1;
} /* SumListPoints */

int SumNextPoints(char *name, int *points)
{
    if (SumCurrentPoints == NULL) return 1;
    
    strcpy(name, SumCurrentPoints->name);
    *points= SumCurrentPoints->points;
    SumCurrentPoints= SumCurrentPoints->next;
    
    return 0;
} /* SumNextPoints */
