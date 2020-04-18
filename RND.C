/* Datei: rnd.c */

/****** Zufallsgenerator ******/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "rnd.h"
#include "util.h"

#define RndC 99991.0
#define RndA 24298.0
#define RndM 199017.0

#define RND_FORMAT_IDENT 2 
    /* Datenstrukturversion */

static float RndX= 138.0;
static char RndFile[100];
static int RndXChanged= 0;
static float RndXField[RND_COUNT] 
                = { 1197.0, 949.0, 8384.0, 3553.0, 3399.0,
                    9880.0, 1134.0, 6543.0, 9988.0, 88776.0 };
static int RndCurrentGenerator= 0;


int RndStart(char *FileName)
{
    int   eof;
    FILE *file;
    char  ReadBuffer[80];
    int   i;
    int   version= 0;

    strcpy(RndFile, FileName);
    
/*    printf("lese '%s'...",RndFile); */
    printf("Zufallszahlen...");
    file= fopen(RndFile, "r");
    if (file == NULL)
    {
        printf("nicht gefunden (wird neu angelegt)\n");
        RndXChanged= 1;
        return 0;
    }

    eof= UtilFileInput(file, ReadBuffer, 80);
    if (eof != 0) 
    {
        printf("altes Format 1...");
        
        eof= fclose(file);
        if (eof == EOF) goto ErrorExit;
        
        RndXChanged= 1;
        printf("OK\n");
        
        return 0;
    }
    eof= sscanf(ReadBuffer, "%f", &(RndXField[0]));
    if (eof != 1) goto ErrorExit;

    if (RndXField[0] != 0.0)
    {
        /* 
            Datenstrukturversion 1
        */
        printf("altes Format 1...");
    }
    else
    {
        /*
            Datenstrukturversion > 2
        */
        eof= UtilFileInput(file, ReadBuffer, 80);
        if (eof != 0) goto ErrorExit;
        eof= sscanf(ReadBuffer, "%d", &version);
        if (eof != 1) goto ErrorExit;
        
        if (version < RND_FORMAT_IDENT)
            printf("altes Format %d...", version);
     
        if (version != 2)
        {
            printf("Dateiformat nicht erkannt...");
            goto ErrorExit;
        }
        for(i=0; i<RND_COUNT; i++)
        {
            eof= UtilFileInput(file, ReadBuffer, 80);
            if ((eof != 0) && (i < RND_COUNT-1)) goto ErrorExit;
            eof= sscanf(ReadBuffer, "%f", &(RndXField[i]));
            if (eof != 1) goto ErrorExit;
        }
    }

    RndX= RndXField[0];
    RndCurrentGenerator= 0;

    eof= fclose(file);
    if (eof == EOF) goto ErrorExit;
    
    if (version < RND_FORMAT_IDENT)
        RndXChanged= 1;
    else
        RndXChanged= 0;

    printf("OK\n");
    return 0;

    ErrorExit:
        printf("Fehler\n");
        return 1;
} /* RndStart */

int RndEnd(void)
{
    int   eof;
    FILE *file;
    int   i;

    if (!RndXChanged)
        return 0;

/*    printf("schreibe %s...",RndFile); */
    printf("Zufallszahlen...");
    file= fopen(RndFile, "w");
    if (file == NULL) goto ErrorExit;
    
    RndXField[RndCurrentGenerator]= RndX;

    eof= fprintf(file, "%f\n%d\n", 0.0, RND_FORMAT_IDENT);
    if ((eof==0) || (eof == EOF)) goto ErrorExit;

    for (i=0; i<RND_COUNT; i++)
    {
        eof= fprintf(file, "%f\n", RndXField[i]);
        if ((eof==0) || (eof == EOF)) goto ErrorExit;
    }

    eof= fclose(file);
    if (eof == EOF) goto ErrorExit;
    printf("OK\n");
    return 0;

    ErrorExit:
        printf("Fehler\n");
        return 1;
} /* RndEnd */

int RndGet(int low, int high)
{
    float fRes;
    int res;

    high++;

    RndX= fmod(RndA * RndX + RndC, RndM);
        /* fmod(x,y): Funktionsergebnis ist Rest von  x/y */
    fRes= (RndX / RndM) * (high - low) + low;

    res= fRes;
    RndXChanged= 1;
    return res;
} /* RndGet */

void RndSelectGenerator(int number)
{
    RndXField[RndCurrentGenerator]= RndX;
    RndX= RndXField[number];
    RndCurrentGenerator= number;
} /* RndSelectGenerator */

void RndGetStartNumber(float *number)
{
    *number= RndX;
} /* RndGetStartNumber */

void RndSetStartNumber(float number)
{    
    RndX= number;
} /* RndSetStartNumber */
