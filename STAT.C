/* Datei: stat.c */

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "stat.h"

static char *StatFormatIdent= "Status-Variablen";

static int   StatChanged= 0;

static int   DebugMode= 0; /* wird nicht gesichert */
static int   VarPreset= 0;
static char  abbrev[6]= "";
static int   VarFormat= 1;
static int   game= 1;
static int   IceTournMode= 1;
static int   TennisTournMode= 1;
static char  VarVersionString[20]= ""; /* wird nicht gesichert */
static int   VarChartSum= 0;
static char  VarChartSumPattern[6]= "????";
static int   VarIceRankList= 0;
static int   VarTennisRankList= 0;

int StatStart(void)
{
    int   eof;
    FILE *file;
    char  ReadBuffer[80];
    int   version;
    
/*    printf("lese '%s'...", StatFileName(STAT_STAT)); */
    printf("Modus-Einstellungen...");
    file= fopen(StatFileName(STAT_STAT), "r");
    if (file == NULL)
    {
        printf("werden neu angelegt\n");
        StatChanged= 1;
        return 0;
    }

    StatChanged= 0;

    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    if (strcmp(ReadBuffer, StatFormatIdent) != 0)
    {
        printf("Dateiformat nicht erkannt...");
        goto ErrorExit;
    }
    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &version);
    if (eof != 1) goto ErrorExit;
    if (version > 1)
    {
        printf("Dateiformat %d zu neu...", version);
        goto ErrorExit;
    }
    else if (version < 1)
    {
        printf("Versionsangabe %d fehlerhaft...", version);
        goto ErrorExit;
    }
    
    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &VarPreset);
    if (eof != 1) goto ErrorExit;
    
    eof= UtilFileInput(file, abbrev, 5);
    if (eof != 0) goto ErrorExit;

    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &VarFormat);
    if (eof != 1) goto ErrorExit;

    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &game);
    if (eof != 1) goto ErrorExit;

    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &IceTournMode);
    if (eof != 1) goto ErrorExit;

    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &TennisTournMode);
    if (eof != 1) goto ErrorExit;
    
    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &VarChartSum);
    if (eof != 1) goto ErrorExit;

    eof= UtilFileInput(file, VarChartSumPattern, 5);
    if (eof != 0) goto ErrorExit;
    
    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) goto ErrorExit;
    eof= sscanf(ReadBuffer, "%d", &VarIceRankList);
    if (eof != 1) goto ErrorExit;
    
    eof= UtilFileInput(file, ReadBuffer, 80);
    /* Fehlerabfrage hier redundant; Dateiende erlaubt */
    eof= sscanf(ReadBuffer, "%d", &VarTennisRankList);
    if (eof != 1) goto ErrorExit;
    
    eof= fclose(file);
    if (eof == EOF) goto ErrorExit;
    
    printf("OK\n");
    return 0;

    ErrorExit:
        printf("Fehler\n");
        return 1;
} /* StatStart */

int StatEnd(void)
{
    int   eof;
    FILE *file;

    if (!StatChanged)
        return 0;

/*    printf("schreibe '%s' ...",StatFileName(STAT_STAT)); */
    printf("Modus-Einstellungen...");
    file= fopen(StatFileName(STAT_STAT), "w");
    if (file == NULL) goto ErrorExit;

    eof= fprintf(file, "%s\n%d\n", StatFormatIdent, 1);
    if ((eof==0) || (eof == EOF)) goto ErrorExit;

    eof= fprintf(file, "%d\n%s\n%d\n%d\n%d\n%d\n%d\n%s\n%d\n%d\n", 
         VarPreset,
         abbrev, VarFormat, game, IceTournMode, TennisTournMode,
         VarChartSum, VarChartSumPattern,
         VarIceRankList, VarTennisRankList);
    if ((eof==0) || (eof == EOF)) goto ErrorExit;

    eof= fclose(file);
    if (eof == EOF) goto ErrorExit;
    printf("OK\n");

    StatChanged= 0;
    return 0;

    ErrorExit:
        printf("Fehler\n");
        return 1;
} /* StatEnd */

void StatSetPreset(int flag)
{
    VarPreset= flag;
} /* StatSetPreset */

int StatPreset(void)
{
    return VarPreset;
} /* StatPreset */

void StatSetDebugMode(int mode)
{
    DebugMode= mode;
} /* StatSetDebugMode */

int StatDebugMode(void)
{
    return DebugMode;
} /* StatDebugMode */

void StatSetAbbrev(char *SetAbbrev)
{
    if (strcmp(SetAbbrev, abbrev))
    {
        if (strlen(abbrev) > 4)
            return;
        strcpy(abbrev, SetAbbrev);
        StatChanged= 1;
    }
} /* StatSetAbbrev */

void StatSetOutFormat(int format)
{
    if (format != VarFormat)
    {
        VarFormat= format;
        StatChanged= 1;
    }
} /* StatSetOutFormat */

int  StatOutFormat(void)
{
    return VarFormat;
} /* StatOutFormat */

char *StatFileName(int purpose)
{
    static char res[30]= "";
    char *suffix= NULL;
    
    if ((purpose >= STAT_DATA) && (strlen(abbrev) < 1))
        return NULL;
    
    switch(VarFormat)
    {
    case STAT_FORMAT_TEX:
        suffix= ".tex";
        break;
    case STAT_FORMAT_ASCII:
        suffix= ".txt";
        break;
    case STAT_FORMAT_HTML:
        suffix= ".htm";
        break;
    case STAT_FORMAT_POSTSCRIPT:
        suffix= ".ps";
        break;
    default:
        suffix= ".err";
        break;
    } /* switch */
    
    switch(purpose)
    {
    case STAT_RANDOM:
        strcpy(res, "dice1.inf");
        break;
    case STAT_GAMES:
        strcpy(res, "dice2.inf");
        break;
    case STAT_STAT:
        strcpy(res, "dice3.inf");
        break;
    case STAT_CHART_SUM_DATA:
        strcpy(res, "dice4.inf");
        break;
    case STAT_CHART_SUM_BACKUP:
        strcpy(res, "dice5.inf");
        break;
    case STAT_CHART_SUM_HELP:
        strcpy(res, "dice6.inf");
        break;
    /******/
    case STAT_DATA:
        strcpy(res, "dice");
        strcat(res, abbrev);
        strcat(res, ".dat");
        break;
    case STAT_MAIN:
        strcpy(res, abbrev);
        strcat(res, suffix);
        break;
    case STAT_MAIN_BACKUP:
        strcpy(res, abbrev);
        strcat(res, ".bak");
        break;
    case STAT_WINNER:
        strcpy(res, "winn");
        strcat(res, abbrev);
        strcat(res, suffix);
        break;
    case STAT_CHARTS:
        strcpy(res, "char");
        strcat(res, abbrev);
        strcat(res, suffix);
        break;
    case STAT_ICEHOCKEY:
        strcpy(res, "ice");
        strcat(res, abbrev);
        strcat(res, suffix);
        break;
    case STAT_TENNIS:
        strcpy(res, "tenn");
        strcat(res, abbrev);
        strcat(res, suffix);
        break;
    case STAT_CHART_SUM:
        strcpy(res, "csum");
        strcat(res, abbrev);
        strcat(res, suffix);
        break;
    default:
        printf("StatFileName: Zweck %d unbekannt\n", purpose);
        return NULL;
    } /* switch */
    
    return res;
} /* StatFileName */

char *StatAbbrev(void)
{
    return abbrev;
} /* StatAbbrev */

void StatSetGame(int SetGame)
{
    if (SetGame != game)
    {
        game= SetGame;
        StatChanged= 1;
    }
} /* StatSetGame */

int StatGame(void)
{
    return game;
} /* StatGame */

void StatSetIceTournMode(int mode)
{
    if (mode != IceTournMode)
    {
        IceTournMode= mode;
        StatChanged= 1;
    }
} /* StatSetIceTournMode */

int StatIceTournMode(void)
{
    return IceTournMode;
} /* StatIceTournMode */

void StatSetTennisTournMode(int mode)
{
    if (mode != TennisTournMode)
    {
        TennisTournMode= mode;
        StatChanged= 1;
    }
} /* StatSetTennisTournMode */

int StatTennisTournMode(void)
{
    return TennisTournMode;
} /* StatTennisTournMode */

void StatSetVersionString(char *string)
{
    if (strlen(string) > 20)
        printf("StatSetVersionString: zu lang\n");
    else
        strcpy(VarVersionString, string); 
} /* StatSetVersionString */

char *StatVersionString(void)
{
    return VarVersionString;
} /* StatVersionString */

void StatSetChartSum(int flag)
{
    if (flag != VarChartSum)
    {
        VarChartSum= flag;
        StatChanged= 1;
    }
} /* StatSetChartSum */

int StatChartSum(void)
{
    return VarChartSum;
} /* StatChartSum */

void StatSetChartSumPattern(char *pattern)
{
    if (strcmp(pattern, VarChartSumPattern))
    {
        if (strlen(pattern) > 4)
            printf("StatSetChartSumPattern: zu lang\n");
        else
            strcpy(VarChartSumPattern, pattern);
        StatChanged= 1;
    }
} /* StatSetChartSumPattern */

char *StatChartSumPattern(void)
{
    return VarChartSumPattern;
} /* StatChartSumPattern */

void StatSetIceRankList(int flag)
{
    if (flag != VarIceRankList)
    {
        VarIceRankList= flag;
        StatChanged= 1;
    }
} /* StatSetIceRankList */

int StatIceRankList(void)
{
    return VarIceRankList;
} /* StatIceRankList */

void StatSetTennisRankList(int flag)
{
    if (flag != VarTennisRankList)
    {
        VarTennisRankList= flag;
        StatChanged= 1;
    }
} /* StatSetTennisRankList */

int StatTennisRankList(void)
{
    return VarTennisRankList;
} /* StatTennisRankList */
