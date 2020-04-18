/* Datei: tab.c */

#include <stdio.h>
#include <string.h>
#include "tab.h"
#include "games.h"
#include "util.h"
#include "stat.h"

/****** Spielertabelle: *******/

int TabFirstUnused= 0;

tsTabPlayer TabPlayers[TabMaxPlayers];

static int DataChanged= 0; /* 1: Daten in 'TabPlayers' m"ussen auf 
                                 die Platte geschrieben werden */
static tsTabFileRecord FileData; /* Datensatz fuer die Headerdaten
                                    der aktuellen Spielerdatei */

void TabStart()
{
    /* bisher: tue nichts */
} /* TabStart */

void TabEnd()
/* Die Tabelle zur Spielerverwaltung wird deinitialisiert. */
{
    TabFirstUnused= 0;
} /* TabEnd */

static int TabReadRecord(FILE *file, int version, int index)
/* liest aus Datei 'file' den naechsten Datensatz und legt ihn in
   der Spielertabelle unter dem Index 'index' ab; 'version' gibt
   die Version des Dateiformats an, das beim Schreiben von 'file'
   verwendet wurde; der Funktionswert gibt an:
    0: alles OK, Dateiende noch nicht erreicht;
   -1: der letzte Datensatz wurde gelesen und unter 'index' abgelegt;
   -2: es konnte kein weiterer Datensatz gelesen werden; das Dateiende
       ist bereits erreicht;
   -3: es ist ein Fehler aufgetreten;
*/
{
    int i;
    int eof, eof2;
    char StringInput[100];

    if ((index < 0) || (index > TabFirstUnused) ||
        (index >= TabMaxPlayers))
    {
        printf(
            "Index fuer zu lesenden Datensatz fehlerhaft\n");
        return -3;
    }
    eof= UtilFileInput(file, StringInput, 99);
    if (eof != 0) return -2;
    strncpy(TabPlayers[index].name, StringInput, 30);
    TabPlayers[index].name[30]= '\0';
    
    for (i=0; i<4; i++)
    {
        eof= UtilFileInput(file, StringInput, 99);
        if ((eof != 0) && (i == 0)) return -2;
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d", 
                    &(TabPlayers[index].data[i].offense));
        if (eof != 1) goto ErrorExit;
        
        eof= UtilFileInput(file, StringInput, 99);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d", 
                    &(TabPlayers[index].data[i].defense));
        if (eof != 1) goto ErrorExit;
    }

    /*
        verarbeite die Erweiterungen im Datenformat von Version 1
        nach Version 2:
    */
    if (version >= 2)
    {
        eof= UtilFileInput(file, StringInput, 10);
        if (eof != 0) goto ErrorExit;        
        eof= sscanf(StringInput, "%d", &(TabPlayers[index].IceValid));
        if (eof == 0) goto ErrorExit;

        eof= UtilFileInput(file, StringInput, 10);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d", &(TabPlayers[index].member));
        if (eof == 0) goto ErrorExit;

        for (i=0; i<5; i++)
        {
            eof= UtilFileInput(file,
                        TabPlayers[index].charts[i].name, 30);
            if ((eof != 0) && (i<4)) goto ErrorExit;
            TabPlayers[index].charts[i].index= -1;
        }
    }
    else
    {
        /*
            das Datenformat ist aelter als 2; initialisiere die
            zusaetzlichen Daten mit sinnvollen Werten ...
        */
        TabPlayers[index].IceValid= 1;
        TabPlayers[index].member= 1;
        for (i=0; i<5; i++)
        {
            TabPlayers[index].charts[i].name[0]= '\0';
            TabPlayers[index].charts[i].index= -1;
        }
    } /* if() else */
    
    /*
        verarbeite die Erweiterungen im Datenformat von Version 2
        nach Version 3:
    */
    if (version >= 3)
    {
        /*
            Update-Werte fuer Eishockey:
        */
        eof= UtilFileInput(file, StringInput, 99);
        if (eof != 0) goto ErrorExit;
        strncpy(TabPlayers[index].IceUpdatedIn, StringInput, 4);
        TabPlayers[index].IceUpdatedIn[4]= '\0';
        
        eof= UtilFileInput(file, StringInput, 20);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d", 
                    &(TabPlayers[index].IceUpdateCount));
        if (eof == 0) goto ErrorExit;
        
        /* 
            Tennis-Spielwerte 
        */
        for (i=0; i<3; i++)
        {
            eof= UtilFileInput(file, StringInput, 20);
            if (eof != 0) goto ErrorExit;
            eof= sscanf(StringInput, "%d", 
                        &(TabPlayers[index].tennis[i]));
            if (eof == 0) goto ErrorExit;
        }
        eof= UtilFileInput(file, StringInput, 20);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d",
                    &(TabPlayers[index].TennisValid));
        if (eof == 0) goto ErrorExit;

        /*
            Update-Werte fuer Tennis:
        */
        eof= UtilFileInput(file, StringInput, 99);
        if (eof != 0) goto ErrorExit;
        strncpy(TabPlayers[index].TennisUpdatedIn, StringInput, 4);
        TabPlayers[index].TennisUpdatedIn[4]= '\0';

        eof= UtilFileInput(file, StringInput, 20);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d", 
                    &(TabPlayers[index].TennisUpdateCount));
        if (eof != 1) goto ErrorExit;
        
        /*
            Ranglisten-Punkte fuer Eishockey:
        */
        for (i= 0; i<10; i++)
        {
            eof= UtilFileInput(file, StringInput, 20);
            if (eof != 0) goto ErrorExit;
            eof= sscanf(StringInput, "%d",
                     &(TabPlayers[index].IceRankPoints[i]));
            if (eof != 1) goto ErrorExit;
        }
        eof= UtilFileInput(file, StringInput, 20);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d",
                 &(TabPlayers[index].IceOldRankHead));
        if (eof != 1) goto ErrorExit;
        
        eof= UtilFileInput(file, StringInput, 20);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d",
                 &(TabPlayers[index].IceNewRankHead));
        if (eof != 1) goto ErrorExit;
        
        /*
            Ranglisten-Punkte fuer Tennis:
        */
        for (i= 0; i<10; i++)
        {
            eof= UtilFileInput(file, StringInput, 20);
            if (eof != 0) goto ErrorExit;
            eof= sscanf(StringInput, "%d",
                     &(TabPlayers[index].TennisRankPoints[i]));
            if (eof != 1) goto ErrorExit;
        }
        eof= UtilFileInput(file, StringInput, 20);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(StringInput, "%d",
                 &(TabPlayers[index].TennisOldRankHead));
        if (eof != 1) goto ErrorExit;

        eof= UtilFileInput(file, StringInput, 20);
        /* Fehlerabfrage hier redundant (eof erlaubt) */
        eof2= sscanf(StringInput, "%d",
                 &(TabPlayers[index].TennisNewRankHead));
        if (eof2 != 1) goto ErrorExit;
    }
    else
    {
        /*
            das Datenformat ist aelter als 3; initialisiere die
            zusaetzlichen Daten mit sinnvollen Werten ...
        */
        TabPlayers[index].IceUpdatedIn[0]= '\0';
        TabPlayers[index].IceUpdateCount= 0;
        for (i=0; i<3; i++)
            TabPlayers[index].tennis[i]= 0;
        TabPlayers[index].TennisValid= 0;
        TabPlayers[index].TennisUpdatedIn[0]= '\0';
        TabPlayers[index].TennisUpdateCount= 0;
        for (i=0; i<10; i++)
        {
            TabPlayers[index].IceRankPoints[i]= 0;
            TabPlayers[index].TennisRankPoints[i]= 0;
        }
        TabPlayers[index].IceOldRankHead= 0;
        TabPlayers[index].IceNewRankHead= 0;
        TabPlayers[index].TennisOldRankHead= 0;
        TabPlayers[index].TennisNewRankHead= 0;
        
        eof= 0;
    } /* if() else */

    if (eof != 0) return -1; /* Dateiende erreicht */
    return 0;

    ErrorExit:
        printf("kann Datensatz %d nicht lesen...", index);
        return -3;
} /* TabReadRecord */

void TabSetGamesReferences(void)
/*
    in allen Spielerdatensaetzen fehlende Referenzen auf 
    Spiele in der Spieledatenbank ergaenzen;
    ggf. werden MergeRecords in die Spieledatenbank eingefuegt
*/
{
    int i, j, index;
    
    printf("setze Referenzen auf Spieledatenbank...");
    for(i=0; i<TabFirstUnused; i++)
    {
        /*
            mit jedem Datensatz eines Spielers...
        */
        for(j=0; j<5; j++)
        {
            /*
                mit jedem Charts-Datensatz jedes Spielers...
            */
            if (TabPlayers[i].charts[j].name[0] != '\0')
            {
                GamesSearch(TabPlayers[i].charts[j].name, 0, 3,
                            &index);
                if (index == -1)
                {
                    GamesNew(&index, 1);
                    GamesSet(index, TabPlayers[i].charts[j].name,
                             "", "", 1);
                }
                TabPlayers[i].charts[j].index= index;
            }
            else
            {
                TabPlayers[i].charts[j].index= -1;
            }
        }
    }
    printf("OK\n");
} /* TabSetGamesReferences */

void TabDeleteGamesReference(int index)
/*
    Referenzen in der Spielertabelle auf das Spiel in der 
    Spieledatenbank mit dem angegebenen Index werden beseitigt
*/
{
    int i, j;

    for(i=0; i<TabFirstUnused; i++)
    {
        /*
            mit jedem Datensatz eines Spielers...
        */
        for(j=0; j<5; j++)
        {
            /*
                mit jedem Charts-Datensatz jedes Spielers...
            */
            if (TabPlayers[i].charts[j].index == index)
                TabPlayers[i].charts[j].index= -1;
        }
    }
} /* TabDeleteGamesReference */

void TabDeleteAllGamesReferences(void)
/*
    Referenzen in der Spielertabelle auf alle Spiele in der
    Spieledatenbank werden beseitigt
*/
{
    int i, j;

    for(i=0; i<TabFirstUnused; i++)
    {
        /*
            mit jedem Datensatz eines Spielers...
        */
        for(j=0; j<5; j++)
        {
            /*
                mit jedem Charts-Datensatz jedes Spielers...
            */
            TabPlayers[i].charts[j].index= -1;
        }
    }
} /* TabDeleteGamesReference */

static int TabReadHeader(FILE *file, int version)
/*
    0: alles OK, Dateiende noch nicht erreicht;
   -1: beim Lesen des Headers wurde das Dateiende erreicht (kein 
       Fehler)
   -3: es ist ein Fehler aufgetreten (u. a. Header konnte nicht 
       korrekt gelesen werden);
*/
{
    int res;
    char ReadData[100];
    
    FileData.GotFrom[0]= '\0';
    FileData.myself[0]= '\0';
    FileData.game= StatGame();
    FileData.IceMode= StatIceTournMode();
    FileData.TennisMode= StatTennisTournMode();
    FileData.IceRankList= StatIceRankList();
    FileData.TennisRankList= StatTennisRankList();
    FileData.ChartSum= StatChartSum();
    
    if ((version < 3) || (version > 4))
        return -3;
    
    res= UtilFileInput(file, ReadData, 99);
    if (res != 0)
        return -3;
    strncpy(FileData.GotFrom, ReadData, 9);
    FileData.GotFrom[9]= '\0';
    
    res= UtilFileInput(file, ReadData, 99);
    if (res != 0)
        return -3;
    strncpy(FileData.myself, ReadData, 9);
    FileData.myself[9]= '\0';
    
    res= UtilFileInput(file, ReadData, 99);
    if (res != 0)
        return -3;
    res= sscanf(ReadData, "%d", &(FileData.game));
    if (res != 1)
        return -3;
    
    res= UtilFileInput(file, ReadData, 99);
    if (res != 0)
        return -3;
    res= sscanf(ReadData, "%d", &(FileData.IceMode));
    if (res != 1)
        return -3;
        
    res= UtilFileInput(file, ReadData, 99);
    if (res != 0)
        return -3;
    res= sscanf(ReadData, "%d", &(FileData.TennisMode));
    if (res != 1)
        return -3;

    res= UtilFileInput(file, ReadData, 99);
    if (res != 0)
        return -3;
    res= sscanf(ReadData, "%d", &(FileData.IceRankList));
    if (res != 1)
        return -3;
        
    res= UtilFileInput(file, ReadData, 99);
    if ((res != 0) && (res != 1))
        return -3;
    res= sscanf(ReadData, "%d", &(FileData.TennisRankList));
    if (res != 1)
        return -3;

    if (version >= 4)
    {
	    res= UtilFileInput(file, ReadData, 99);
	    if ((res != 0) && (res != 1))
	        return -3;
	    res= sscanf(ReadData, "%d", &(FileData.ChartSum));
	    if (res != 1)
	        return -3;
    }
    
    return 0;
} /* TabReadHeader */

int TabReadFile(void)
{
    FILE *file;
    int eof;
    int ident;
    char ReadBuffer[100];

    if ((StatFileName(STAT_DATA) != NULL) && DataChanged)
    {
        /*
            offensichtlich TabWriteFile() vergessen:
        */
        return 1;
    }

    printf("lese Stimmzetteldatei '%s'...", StatFileName(STAT_DATA));
    
    TabFirstUnused= 0;

    file= fopen(StatFileName(STAT_DATA), "r");
    if (file == NULL)
    {
        printf("nicht gefunden (wird neu angelegt)\n");
        
        FileData.GotFrom[0]= '\0';
        strcpy(FileData.myself, StatAbbrev());
        FileData.game= 1;
        FileData.IceMode= 1;
        FileData.TennisMode= 1;
        
        return 0;
    }

    eof= UtilFileInput(file, ReadBuffer, 99);
    if (eof != 0)
    {
        printf("kann nicht lesen\n");
        eof= fclose(file);
        StatSetAbbrev(""); /* damit die Schreibautomatik des Programms nichts
                              kaputt machen kann */
        return 1;
    }
    eof= sscanf(ReadBuffer, "%d", &ident);
    if (eof != 1)
    {
        printf("kann nicht lesen\n");
        eof= fclose(file);
        StatSetAbbrev(""); /* s. o. */
        return 1;
    }
    if ((ident > TAB_FORMAT_IDENT) || (ident < 1))
    {
        printf("verstehe Dateiformat nicht\n");
        eof= fclose(file);
        StatSetAbbrev(""); /* s. o. */
        return 1;
    }
    if (ident < TAB_FORMAT_IDENT)
        printf("altes Format %d...", ident);
    
    if (ident >= 3)
    {
        eof= TabReadHeader(file, ident);
        if (eof == -3)
            goto ErrorExit;
    }
    else
    {
        FileData.GotFrom[0]= '\0';
        strcpy(FileData.myself, StatAbbrev());
        FileData.game= 1;
        FileData.IceMode= 1;
        FileData.TennisMode= 1;
    }
    
    while ((eof != -1) && (eof != -2))
    {
        eof= TabReadRecord(file, ident, TabFirstUnused);
        if (eof == -3) goto ErrorExit;
        if ((eof == 0) || (eof == -1)) TabFirstUnused++;
    }
    
    if (ident < TAB_FORMAT_IDENT)
        DataChanged= 1;

    if (fclose(file) != 0)
    {
        printf("kann Datei nicht korrekt schliessen\n");
        goto ErrorExit;
    }
    printf("OK\n");

    TabSetGamesReferences();
    return 0;

    ErrorExit:
        StatSetAbbrev("");
        printf("Fehler");
        return 1;    
} /* TabReadFile */

int TabWriteFile(void)
/* Die Tabelle wird in die angegebene Datei geschrieben. */
{
    FILE *file;
    int eof;
    int i, j;

    if (!(DataChanged && (StatFileName(STAT_DATA) != NULL)))
    {
        /* nichts zu tun: */
        return 0;
    }
    printf("schreibe Stimmzetteldatei '%s'...", StatFileName(STAT_DATA));

    file= fopen(StatFileName(STAT_DATA), "w");
    if (file == NULL)
    {
        printf("kann Datei nicht oeffnen\n");
        return 1;
    }

    eof= fprintf(file, "%d\n", TAB_FORMAT_IDENT);
    if ((eof == 0) || (eof == EOF))
    {
        fprintf(stderr, "kann nicht schreiben\n");
        fclose(file);
        return 1;
    }

    eof= fprintf(file, "%s\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n", 
                 FileData.GotFrom,
                 FileData.myself, FileData.game,
                 FileData.IceMode, FileData.TennisMode,
                 FileData.IceRankList, FileData.TennisRankList,
                 FileData.ChartSum);
    if ((eof == 0) || (eof == EOF))
    {
        fprintf(stderr, "kann Header nicht schreiben\n");
        fclose(file);
        return 1;
    }

    for(i=0; i<TabFirstUnused; i++)
    {
        eof= fprintf(file, "%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",
            TabPlayers[i].name,
            TabPlayers[i].data[0].offense,
            TabPlayers[i].data[0].defense,
            TabPlayers[i].data[1].offense,
            TabPlayers[i].data[1].defense,
            TabPlayers[i].data[2].offense,
            TabPlayers[i].data[2].defense,
            TabPlayers[i].data[3].offense,
            TabPlayers[i].data[3].defense);
        if ((eof == 0) || (eof == EOF)) goto ErrorExit;

        eof= fprintf(file, "%d\n", TabPlayers[i].IceValid);
        if ((eof == 0) || (eof == EOF)) goto ErrorExit;

        eof= fprintf(file, "%d\n", TabPlayers[i].member);
        if ((eof == 0) || (eof == EOF)) goto ErrorExit;

        for (j=0; j<5; j++)
        {
	        eof= fprintf(file, "%s\n", TabPlayers[i].charts[j].name);
	        if ((eof == 0) || (eof == EOF)) goto ErrorExit;
	    }
	    
	    /*
	        zusaetzlich in Dateiformat-Version 3 ... 
	    */
        eof= fprintf(file, "%s\n%d\n", TabPlayers[i].IceUpdatedIn,
                     TabPlayers[i].IceUpdateCount);
        if ((eof == 0) || (eof == EOF)) goto ErrorExit;

        for (j=0; j<3; j++)
        {
	        eof= fprintf(file, "%d\n", TabPlayers[i].tennis[j]);
	        if ((eof == 0) || (eof == EOF)) goto ErrorExit;
	    }

        eof= fprintf(file, "%d\n%s\n%d\n", 
                     TabPlayers[i].TennisValid,
                     TabPlayers[i].TennisUpdatedIn,
                     TabPlayers[i].TennisUpdateCount);
        if ((eof == 0) || (eof == EOF)) goto ErrorExit;

        /*
            Ranglistenpunkte:
        */        
        for (j=0; j<10; j++)
        {
            eof= fprintf(file, "%d\n", TabPlayers[i].IceRankPoints[j]);
            if ((eof == 0) || (eof == EOF)) goto ErrorExit;
        }
        
        eof= fprintf(file, "%d\n%d\n",
                 TabPlayers[i].IceOldRankHead, 
                 TabPlayers[i].IceNewRankHead);
                 
        for (j=0; j<10; j++)
        {
            eof= fprintf(file, "%d\n", TabPlayers[i].TennisRankPoints[j]);
            if ((eof == 0) || (eof == EOF)) goto ErrorExit;
        }
        
        eof= fprintf(file, "%d\n%d\n",
                 TabPlayers[i].TennisOldRankHead, 
                 TabPlayers[i].TennisNewRankHead);
    } /* for */

    if (fclose(file) != 0)
    {
        printf("kann Datei nicht korrekt schliessen\n");
        return 1;
    }
    printf("OK\n");
    DataChanged= 0;
    return 0;
    
    ErrorExit:
        printf("kann Datensatz nicht schreiben\n");
        eof= fclose(file);
        return 1;
} /* TabWriteFile */

int TabNew()
{
    int i;
    
    if (TabFirstUnused >= TabMaxPlayers)
    {
        printf("TabNew: Tabelle voll\n");
        return -1;
    }
    TabPlayers[TabFirstUnused].IceValid= 0;
    TabPlayers[TabFirstUnused].TennisValid= 0;
    TabPlayers[TabFirstUnused].member= 1;
    
    for (i=1; i<=5; i++)
    {    
        TabPlayers[TabFirstUnused].charts[i].name[0]= '\0';
        TabPlayers[TabFirstUnused].charts[i].index= -1;
    }
    
    for (i=0; i<10; i++)
    {
        TabPlayers[TabFirstUnused].IceRankPoints[i]= 0;
        TabPlayers[TabFirstUnused].TennisRankPoints[i]= 0;
    }
    TabPlayers[TabFirstUnused].IceOldRankHead= 0;
    TabPlayers[TabFirstUnused].IceNewRankHead= 0;
    TabPlayers[TabFirstUnused].TennisOldRankHead= 0;
    TabPlayers[TabFirstUnused].TennisNewRankHead= 0;
    
    TabFirstUnused++;
    DataChanged= 1;
    return TabFirstUnused-1;
} /* TabNew */

char *TabGetName(int index)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabGetName: Index fehlerhaft\n");
        return "<nicht gefunden>";
    }
    return TabPlayers[index].name;
} /* TabGetName */

int TabGetMember(int index)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabGetName: Index fehlerhaft\n");
        return 0;
    }
    return TabPlayers[index].member;
} /* TabGetMember */

void TabGetIce(int index,
               int *o1, int *d1, int *o2, int *d2, 
               int *o3, int *d3, int *o4, int *d4)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetIce: Index fehlerhaft\n");
        return;
    }
    *o1= TabPlayers[index].data[0].offense;
    *d1= TabPlayers[index].data[0].defense;
    *o2= TabPlayers[index].data[1].offense;
    *d2= TabPlayers[index].data[1].defense;
    *o3= TabPlayers[index].data[2].offense;
    *d3= TabPlayers[index].data[2].defense;
    *o4= TabPlayers[index].data[3].offense;
    *d4= TabPlayers[index].data[3].defense;
} /* TabGetIce */

void TabSetIce(int index, 
               int o1, int d1, int o2, int d2, 
               int o3, int d3, int o4, int d4)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetIce: Index fehlerhaft\n");
        return;
    }
    TabPlayers[index].data[0].offense= o1;
    TabPlayers[index].data[0].defense= d1;
    TabPlayers[index].data[1].offense= o2;
    TabPlayers[index].data[1].defense= d2;
    TabPlayers[index].data[2].offense= o3;
    TabPlayers[index].data[2].defense= d3;
    TabPlayers[index].data[3].offense= o4;
    TabPlayers[index].data[3].defense= d4;
    
    TabPlayers[index].IceValid= (o1+o2+o3+o4+d1+d2+d3+d4 == 100);
    
    TabPlayers[index].IceUpdateCount= 0;
    if (strlen(FileData.myself) > 4)
        printf("TabSetIce: strlen(FileData.myself) > 4\n");
    
    strncpy(TabPlayers[index].IceUpdatedIn, FileData.myself, 4);
    TabPlayers[index].IceUpdatedIn[4]= '\0';
    
    DataChanged= 1;
} /* TabSetIce */

void TabSetIceValid(int index, int valid)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabSetIceValid: fehlerhafter Index %d\n", index);
        return;
    }
    
    TabPlayers[index].IceValid= valid;
} /* TabSetIceValid */

void TabGetIceValid(int index, int *valid)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetIceValid: fehlerhafter Index %d\n", index);
        return;
    }
    
    *valid= TabPlayers[index].IceValid;
} /* TabGetIceValid */

void TabSetName(int index, tTabName name)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetName: Index fehlerhaft\n");
        return;
    }
    strcpy(TabPlayers[index].name, name);
    DataChanged= 1;
} /* TabSetName */

void TabSetMember(int index, int member)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetMember: Index fehlerhaft\n");
        return;
    }
    TabPlayers[index].member= member;
    DataChanged= 1;
} /* TabSetMember */

void TabSetCharts(int index, int rank, char *name, int GameIndex)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetCharts: Index fehlerhaft\n");
        return;
    }
    strcpy(TabPlayers[index].charts[rank].name, name);
    TabPlayers[index].charts[rank].index= GameIndex;
    DataChanged= 1;
} /* TabSetCharts */

void TabGetCharts(int index, int rank, char **name, int *GameIndex)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetCharts: Index fehlerhaft\n");
        *name= NULL;
        return;
    }
    *name= TabPlayers[index].charts[rank].name;
    *GameIndex= TabPlayers[index].charts[rank].index;
} /* TabGetCharts */

static void TabPlayerSwap(tsTabPlayer *p1, tsTabPlayer *p2);
/* Die Inhalte der Datensaetze '*p1' und '*p2' werden vertauscht.
*/

static void TabPlayerAssign(tsTabPlayer *source, tsTabPlayer *dest)
/* Der Inhalt des Datensatzes '*source' wird dem Datensatz '*dest'
   zugewiesen.
*/
{
    int i;

    strcpy(dest->name, source->name);
    for(i=0; i<4; i++)
    {
        (dest->data)[i].offense= (source->data)[i].offense;
        (dest->data)[i].defense= (source->data)[i].defense;
    }
    dest->IceValid= source->IceValid;
    dest->member= source->member;
    for(i=0; i<5; i++)
    {
        strcpy((dest->charts)[i].name, (source->charts)[i].name);
        dest->charts[i].index= source->charts[i].index;
    }
    strcpy(dest->IceUpdatedIn, source->IceUpdatedIn);
    dest->IceUpdateCount= source->IceUpdateCount;
    for(i=0; i<3; i++)
        dest->tennis[i]= source->tennis[i];
    dest->TennisValid= source->TennisValid;
    strcpy(dest->TennisUpdatedIn, source->TennisUpdatedIn);
    dest->TennisUpdateCount= source->IceUpdateCount;
    
} /* TabPlayerAssign */

static void TabPlayerSwap(tsTabPlayer *p1, tsTabPlayer *p2)
/* Die Inhalte der Datensaetze '*p1' und '*p2' werden vertauscht.
*/
{
    tsTabPlayer hilf;

    TabPlayerAssign(p1, &hilf);
    TabPlayerAssign(p2, p1);
    TabPlayerAssign(&hilf, p2);
} /* TabPlayerSwap */

int TabDelete(int index)
{
    int i;

    if ((index >= TabFirstUnused) || (index < 0))
        return -1;

    for(i=index; i<TabFirstUnused-1; i++)
        TabPlayerAssign(&(TabPlayers[i+1]), &(TabPlayers[i]));
    TabFirstUnused--;

    DataChanged= 1;

    return 0;
} /* TabDelete */

void TabSort(void)
/* Der Inhalt der Spielertabelle wird alphabetisch nach dem Namen
   sortiert (Bubble-Sort).
*/
{
    int    swapped= 1;
    int CurrentSwapped;
    int i;

    printf("sortiere Stimmzetteldaten...");

    while(swapped)
    {
        swapped= 0;
        for(i=0; i<TabFirstUnused-1; i++)
        {
            CurrentSwapped=
                (strcmp(TabPlayers[i].name, TabPlayers[i+1].name)  > 0);
            if (CurrentSwapped)
                TabPlayerSwap( &(TabPlayers[i]), &(TabPlayers[i+1]) );
            swapped= swapped || CurrentSwapped;
            DataChanged= DataChanged || CurrentSwapped;
        }
    }

    printf("OK\n");
} /* TabSort */

void TabPrint(void)
{
    int i, j;
    char UserInput[100];
    char command;
    char PrintBuffer[100];

    printf("Ausgabeformat:\n");
    printf("    Index des Datensatzes | Name | Mitglied");
    printf(" | HippoCHARTS: Platz 1 | Platz2\n");
    printf("    HippoCHARTS: Platz3 | Platz4 | Platz5\n");
    printf("    HippoGAMEs (O/D S.1; O/D S.2; O/D S.3; O/D S.D)");
    printf(" (S.1; S.2; S.3)\n");

    printf("Anzahl der Datensaetze: %d\n", TabFirstUnused);
    i= 0; j= 3;
    while (i < TabFirstUnused) 
    {
        PrintBuffer[0]= '\0';
        strcat(PrintBuffer, "*** ");
        strcat(PrintBuffer, TabPlayers[i].name);
        strcat(PrintBuffer, " ***");
        printf("%3d| %-20s|=%1d=| %-23s| %-23s\n",
               i, PrintBuffer, TabPlayers[i].member, 
               TabPlayers[i].charts[0].name, 
               TabPlayers[i].charts[1].name);
        printf("   | %-24s| %-23s| %-23s\n",
               TabPlayers[i].charts[2].name,
               TabPlayers[i].charts[3].name,
               TabPlayers[i].charts[4].name);
        if (TabPlayers[i].IceValid)
        {
          printf(
          "    =Eishockey= (%2d,%2d; %2d,%2d; %2d,%2d; %2d,%2d)",
            TabPlayers[i].data[0].offense, TabPlayers[i].data[0].defense,
            TabPlayers[i].data[1].offense, TabPlayers[i].data[1].defense,
            TabPlayers[i].data[2].offense, TabPlayers[i].data[2].defense,
            TabPlayers[i].data[3].offense, TabPlayers[i].data[3].defense);
        }
        if (TabPlayers[i].TennisValid)
        {
          printf("    =Tennis= (%2d; %2d; %2d)",
            TabPlayers[i].tennis[0], TabPlayers[i].tennis[1],
            TabPlayers[i].tennis[2]);
        }
        printf("\n");
        i++; j++;
        if (j >= 8)
        {
            do 
            {
                printf("<return> --> weiter; q --> Abbruch ? ");
                UtilInput(UserInput);
                if (strlen(UserInput) == 0) 
                    command= ' ';
                else if (strlen(UserInput) > 1) 
                    command= 'f';
                else
                    command= UserInput[0];
                if (command == 'f') 
                    printf("Eingabefehler!\n");
            } while ((command != ' ') && (command != 'q'));
            if (command == 'q')
                i= TabFirstUnused;
            j= 0;
        }
     } /* while (i<TabFirstUnused) */
} /* TabPrint */


int TabFileExists(char *file, int *FileExists)
{
    char  AccessName[100];
    char  TmpAbbrev[5];
    int   exists;
   
    strcpy(TmpAbbrev, StatAbbrev());
    StatSetAbbrev(file);
    strcpy(AccessName, StatFileName(STAT_DATA));
    StatSetAbbrev(TmpAbbrev);
    
    printf("teste auf Existenz von '%s'...", AccessName);

    *FileExists= 0;
    exists= UtilFileExists(AccessName);
    if (exists == -1) goto ErrorExit;
    if (exists)
    {
        printf("existiert...");
        *FileExists= 1;
    }
    else
        printf("existiert nicht...");

    printf("OK\n");
    return 0;
    
    ErrorExit:
        perror(NULL);
        return 1;
} /* TabFileExists */

int TabCopyFile(char *source, char *target)
{
    int   res;
    char  SourceName[100], TargetName[100];
    char  TmpAbbrev[5];
    
    strcpy(TmpAbbrev, StatAbbrev());
    StatSetAbbrev(source);
    strcpy(SourceName, StatFileName(STAT_DATA));
    StatSetAbbrev(target);
    strcpy(TargetName, StatFileName(STAT_DATA));
    StatSetAbbrev(TmpAbbrev);
    
    printf("kopiere '%s' nach '%s'...", SourceName, TargetName);
    
    res= UtilCopyFile(SourceName, TargetName);
    if (res) goto ErrorExit;

    printf("OK\n");
    return 0;
    
    ErrorExit:
        return 1;
} /* TabCopyFile */

void TabSweepAfterCopyFile(void)
{
    int i,j;
    int IceDelCount= 0, TennisDelCount= 0, RecordDelCount= 0;
    int IceValidCount= 0, TennisValidCount= 0;
    int IceDelFlag, TennisDelFlag;
    int IceValid, TennisValid;
    int IceRankSum, TennisRankSum;
    char *name;

    printf("bereinige Stimmzettel nach Uebernahme aus Altdatei...\n");
    printf("  ( HippoGAME-Nichtteilnehmer werden geloescht;\n");
    printf("%s %s\n",
           "    nur neu abgegebene Stimmzettel nehmen an HippoWINNER",
           "teil )");
    
    strcpy(FileData.GotFrom, FileData.myself);
    strcpy(FileData.myself, StatAbbrev());

    for(i=0; i<TabFirstUnused; i++)
    {
        /* bis auf weiteres werden die HippoCharts-Auswahlen bei
           der Uebernahme nicht geloescht; daher auskommentiert
        for(j=0; j<5; j++)
            TabPlayers[i].charts[j].name[0]= '\0'; 
        */
        TabGetIceValid(i, &IceValid);
        TabGetTennisValid(i, &TennisValid);
        
        /*
            nur neu abgegebene Stimmzettel nehmen an der HippoWINNER-
            Auswertung teil:
        */
        TabSetMember(i, 0);
        
        if (IceValid)
        {
            IceValidCount++;
            TabPlayers[i].IceUpdateCount++;
            IceDelFlag= (TabPlayers[i].IceUpdateCount >= 12);
            if (IceDelFlag)
            {
                TabSetIceValid(i, 0);
                IceDelCount++;
            }
        }
        if (TennisValid)
        {
            TennisValidCount++;
            TabPlayers[i].TennisUpdateCount++;
            TennisDelFlag= (TabPlayers[i].TennisUpdateCount >= 12);
            if (TennisDelFlag)
            {
                TabSetTennisValid(i, 0);
                TennisDelCount++;
            }
        }

        for(j=1; j<= 10; j++)
        {
            TabPlayers[i].IceRankPoints[j]= 
                                 TabPlayers[i].IceRankPoints[j-1];
            TabPlayers[i].TennisRankPoints[j]=
                                 TabPlayers[i].TennisRankPoints[j-1];
        }
        TabPlayers[i].IceRankPoints[0]= 0;
        TabPlayers[i].TennisRankPoints[0]= 0;
        TabPlayers[i].IceOldRankHead= TabPlayers[i].IceNewRankHead;
        TabPlayers[i].IceNewRankHead= 0;
        TabPlayers[i].TennisOldRankHead= 
                                      TabPlayers[i].TennisNewRankHead;
        TabPlayers[i].TennisNewRankHead= 0;
    } /* for */

    if (IceValidCount)
    {
        if (IceDelCount == 0)
            printf(
              "alle Eishockey-Mannschaften nehmen weiterhin teil...\n");
        else
            printf(
              "%d Eishockey-Mannschaften nehmen nicht mehr teil...\n", IceDelCount);
    } /* if */
 
    if (TennisValidCount)
    {   
        if (TennisDelCount == 0)
            printf(
                "alle Tennis-Mannschaften nehmen weiterhin teil...\n");
        else
            printf(
                "%d Tennis-Mannschaften nehmen nicht mehr teil...\n", TennisDelCount);
    } /* if */

    printf("Datensaetze werden geloescht von...\n");
    i= 0; RecordDelCount= 0;
    while (i<TabFirstUnused)
    {
        TabGetIceValid(i, &IceValid);
        TabGetTennisValid(i, &TennisValid);
        TabGetIceRankSum(i, &IceRankSum);
        TabGetTennisRankSum(i, &TennisRankSum);
        if (   !IceValid && !TennisValid && (IceRankSum == 0)
            && (TennisRankSum == 0)
           )
        {
            RecordDelCount++;
            name= TabGetName(i);
            printf("    %s ...\n", name);
            TabDelete(i);
        }
        else
            i++;
    } /* while */
    if (RecordDelCount == 0)
        printf("    ...niemandem...\n");

    printf("...OK\n");
    DataChanged= 1;
} /* TabSweepAfterCopyFile */

void TabGetIceUpdatedIn(int index, char **UpdatedIn)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetIceUpdatedIn: fehlerhafter Index %d\n", index);
        return;
    }
    
    *UpdatedIn= TabPlayers[index].IceUpdatedIn;
} /* TabGetIceUpdatedIn */

void TabGetIceUpdateCount(int index, int *UpdateCount)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetIceUpdatedCount: fehlerhafter Index %d\n", index);
        return;
    }
    
    *UpdateCount= TabPlayers[index].IceUpdateCount;
} /* TabGetIceUpdateCount */

void TabSetIceMode(int mode)
{
    FileData.IceMode= mode;
    DataChanged= 1;
} /* TabSetIceMode */

void TabGetIceMode(int *mode)
{
    *mode= FileData.IceMode;
} /* TabGetIceMode */

void TabGetTennis(int index, int *s1, int *s2, int *s3)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabGetTennis: fehlerhafter Index %d\n", index);
        return;
    }
    
    *s1= TabPlayers[index].tennis[0];
    *s2= TabPlayers[index].tennis[1];
    *s3= TabPlayers[index].tennis[2];
} /* TabGetTennis */

void TabSetTennis(int index, int s1, int s2, int s3)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetTennis: fehlerhafter Index %d\n", index);
        return;
    }
    
    TabPlayers[index].tennis[0]= s1;
    TabPlayers[index].tennis[1]= s2;
    TabPlayers[index].tennis[2]= s3;
    
    TabPlayers[index].TennisValid= (s1+s2+s3 == 100);
    
    TabPlayers[index].TennisUpdateCount= 0;
    if (strlen(FileData.myself) > 4)
        printf("TabSetIce: strlen(FileData.myself) > 4\n");
    
    strncpy(TabPlayers[index].TennisUpdatedIn, FileData.myself, 4);
    TabPlayers[index].IceUpdatedIn[4]= '\0';
    
    DataChanged= 1;
} /* TabSetTennis */

void TabGetTennisValid(int index, int *TennisValid)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabGetTennisValid: fehlerhafter Index %d\n", index);
        return;
    }
    
    *TennisValid= TabPlayers[index].TennisValid;
} /* TabGetTennisValid */

void TabSetTennisValid(int index, int TennisValid)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf("TabSetTennisValid: fehlerhafter Index %d\n", index);
        return;
    }
    
    TabPlayers[index].TennisValid= TennisValid;
    
    DataChanged= 1;
} /* TabSetTennisValid */

void TabGetTennisUpdatedIn(int index, char **UpdatedIn)
{
    if ((index < 0) || (index > TabFirstUnused))
    {
        printf(
        "TabGetTennisUpdatedIn: fehlerhafter Index %d\n", index);
        return;
    }
    
    *UpdatedIn= TabPlayers[index].TennisUpdatedIn;
} /* TabGetTennisUpdatedIn */

void TabGetTennisUpdateCount(int index, int *UpdateCount)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetTennisUpdateCount: fehlerhafter Index %d\n", index);
        return;
    }
    
    *UpdateCount= TabPlayers[index].TennisUpdateCount;
    
    DataChanged= 1;
} /* TabGetTennisUpdateCount */

void TabSetTennisMode(int mode)
{
    FileData.TennisMode= mode;
    
    DataChanged= 1;
} /* TabSetTennisMode */

void TabGetTennisMode(int *mode)
{
    *mode= FileData.TennisMode;
} /* TabGetTennisMode */

void TabSearch(char *name, int StartIndex, int mode,
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
        printf("TabSearch: Parameter 'name': Zeichenkette zu lang\n");
        return;
    }
    strcpy(SearchName, name);
    if (!(mode & 2))
        UtilStrlwr(SearchName);
    while ((CurrentIndex < TabFirstUnused) && (!found))
    {
        if (strlen(TabPlayers[CurrentIndex].name) > 100)
        {
            printf("TabSearch: Name des Spiels mit Index %d zu lang\n",
                CurrentIndex);
            return;
        }
        strcpy(CheckName, TabPlayers[CurrentIndex].name);
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
} /* TabSearch */

int TabGetFirstUnused(void)
{
    return TabFirstUnused;
} /* TabGetFirstUnused */

void TabSetIceRankPoints(int index, int points)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabSetIceRankPoints: fehlerhafter Index %d\n", index);
        return;
    }
    
    TabPlayers[index].IceRankPoints[0]= points;
    
    DataChanged= 1;
} /* TabSetIceRankPoints */

void TabGetIceRankPoints(int index, int *points)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetIceRankPoints: fehlerhafter Index %d\n", index);
        *points= 0;
        return;
    }
    
    *points= TabPlayers[index].IceRankPoints[0];
} /* TabGetIceRankPoints */

void TabGetIceRankSum(int index, int *points)
{
    int i;

    *points= 0;
        
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetIceRankSum: fehlerhafter Index %d\n", index);
        return;
    }
    
    for(i=0; i<10; i++)
        *points= *points + TabPlayers[index].IceRankPoints[i] * (10-i);
} /* TabGetIceRankSum */

void TabGetOldIceRankHead(int index, int *flag)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetOldIceRankHead: fehlerhafter Index %d\n", index);
        *flag= 0;
        return;
    }
    
    *flag= TabPlayers[index].IceOldRankHead;
} /* TabGetOldIceRankHead */

void TabSetNewIceRankHead(int index, int flag)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabSetNewIceRankHead: fehlerhafter Index %d\n", index);
        return;
    }
    
    TabPlayers[index].IceNewRankHead= flag;
    
    DataChanged= 1;
} /* TabSetNewIceRankHead */

void TabGetNewIceRankHead(int index, int *flag)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetNewIceRankHead: fehlerhafter Index %d\n", index);
        *flag= 0;
        return;
    }
    
    *flag= TabPlayers[index].IceNewRankHead;
} /* TabGetNewIceRankHead */

void TabSetTennisRankPoints(int index, int points)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabSetTennisRankPoints: fehlerhafter Index %d\n", index);
        return;
    }
    
    TabPlayers[index].TennisRankPoints[0]= points;
    
    DataChanged= 1;
} /* TabSetTennisRankPoints */

void TabGetTennisRankPoints(int index, int *points)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetTennisRankPoints: fehlerhafter Index %d\n", index);
        *points= 0;
        return;
    }
    
    *points= TabPlayers[index].TennisRankPoints[0];
} /* TabGetTennisRankPoints */

void TabGetTennisRankSum(int index, int *points)
{
    int i;

    *points= 0;
        
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetTennisRankSum: fehlerhafter Index %d\n", index);
        return;
    }
    
    for(i=0; i<10; i++)
        *points= *points + TabPlayers[index].TennisRankPoints[i] * (10-i);
} /* TabGetTennisRankSum */

void TabGetOldTennisRankHead(int index, int *flag)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetTennisRankHead: fehlerhafter Index %d\n", index);
        *flag= 0;
        return;
    }
    
    *flag= TabPlayers[index].TennisOldRankHead;
} /* TabGetOldTennisRankHead */

void TabSetNewTennisRankHead(int index, int flag)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabSetNewTennisRankHead: fehlerhafter Index %d\n", index);
        return;
    }
    
    TabPlayers[index].TennisNewRankHead= flag;
    
    DataChanged= 1;
} /* TabSetNewTennisRankHead */

void TabGetNewTennisRankHead(int index, int *flag)
{
    if ((index < 0) || (index >= TabFirstUnused))
    {
        printf(
        "TabGetNewTennisRankHead: fehlerhafter Index %d\n", index);
        *flag= 0;
        return;
    }
    
    *flag= TabPlayers[index].TennisNewRankHead;
} /* TabGetNewTennisRankHead */

void TabGetIceRankList(int *flag)
{
    *flag= FileData.IceRankList;
} /* TabGetIceRankList */

void TabSetIceRankList(int flag)
{
    FileData.IceRankList= flag;
    DataChanged= 1;
} /* TabSetIceRankList */

void TabGetTennisRankList(int *flag)
{
    *flag= FileData.TennisRankList;
} /* TabGetTennisRankList */

void TabSetTennisRankList(int flag)
{
    FileData.TennisRankList= flag;
    DataChanged= 1;
} /* TabSetTennisRankList */

void TabSetChartSum(int flag)
{
    FileData.ChartSum= flag;
    DataChanged= 1;
} /* TabSetChartSum */

int TabChartSum(void)
{
    return FileData.ChartSum;
} /* TabChartSum */

char *TabGetMyself(void)
{
    return FileData.myself;
} /* TabGetMyself */

void TabSetGame(int flag)
{
    FileData.game= flag;
    DataChanged= 1;
} /* TabSetGame */

int TabGetGame(void)
{
    return FileData.game;
} /* TabGetGame */
