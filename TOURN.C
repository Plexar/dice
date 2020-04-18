/* Datei: tourn.c */

#include <stdio.h>
#include <string.h>
#include "tab.h"
#include "rnd.h"
#include "tourn.h"
#include "out.h"
#include "stat.h"

extern int TabFirstUnused;
extern tsTabPlayer TabPlayers[TabMaxPlayers];

static char TournFileName[30]= "";
static int  TournSelectedGame= 0; /* 1: Eishockey, 2: Tennis */

#define TournMaxPlayers  100

static int TournTab[TournMaxPlayers];
static int TournFirstUnused= 0;

void TournStart(void)
{
    TournFirstUnused= 0;
} /* TournStart */

static int TournCreateFirstRound(int TopIndex, int mode)
/* 'TournTab' und 'TournFirstUnused' werden fuer die erste Runde initiali-
   siert.

   'mode' gibt den Tourniermodus an: 0: KO-System, alle Freilose in der
   ersten Runde; 1: jeder gegen jeden;
   
   Funktionswert: 0: alles OK; 1: Fehler;
*/
{
    int TournIndex, TabIndex;
    int PlayerCount;
    int CorrectedPlayerCount;
    int valid;

    if (TopIndex >= TournMaxPlayers-1)
    {
        printf("Stimmzetteltabelle voll...Abbruch\n");
        return 1;
    }

    printf("erste Runde");

    TournIndex= 0;
    for(TabIndex=0; TabIndex<=TopIndex; TabIndex++)
    {
        switch(TournSelectedGame)
        {
        case TOURN_ICEHOCKEY:
            TabGetIceValid(TabIndex, &valid);
            break;
        case TOURN_TENNIS:
            TabGetTennisValid(TabIndex, &valid);
            break;
        default:
            printf("TournCreateFirstRound: Spiel %d unbekannt\n",
                TournSelectedGame);
            return 1;
        } /* switch */
        if (valid)
            TournTab[TournIndex++]= TabIndex;
    } /* for */

    TournFirstUnused= TournIndex;
    PlayerCount= TournFirstUnused;

    /*
        fuelle die Spieleranzahl bei 'KO-System' auf eine Zweierpotenz
        auf:
    */
    if ((TournFirstUnused > 0) && (mode == 0))
    {
        CorrectedPlayerCount= 1;
        while (TournFirstUnused > CorrectedPlayerCount)
            CorrectedPlayerCount= CorrectedPlayerCount * 2;
        while ((TournFirstUnused < CorrectedPlayerCount)
               && (TournFirstUnused < TournMaxPlayers-1))
        {
            TournTab[TournFirstUnused]= -1;
            TournFirstUnused++;
        }
    }

    printf(" (Anzahl der Teilnehmer %d)\n", PlayerCount);

    return 0;
} /* TournCreateFirstRound */

static void TournShuffle(void)
/* Der Inhalt von 'TournTab' wird gemischt. */
{
    int i;
    int swap;
    int one;
    int two;
    int count;

    /*
        damit das Freilos mit groesserer Wahrscheinlichkeit
        wechselt, bestimme zunaechst einen zufaelligen Spieler
        fuer den ggf. vorhandenen Freilos-Platz:
    */
    if ( (TournFirstUnused % 2) == 1 )
    {
        one= RndGet(0, TournFirstUnused-1);
        two= TournFirstUnused-1;
        swap= TournTab[one];
        TournTab[one]= TournTab[two];
        TournTab[two]= swap;
    }

    /*
        mische:
    */
    count= 11*TournFirstUnused;
    for(i= 0; i<count; i++)
    {
        one= RndGet(0, TournFirstUnused-1);
        two= RndGet(0, TournFirstUnused-1);
        swap= TournTab[one];
        TournTab[one]= TournTab[two];
        TournTab[two]= swap;
    }

    /*
        falls Fuelleintraege in 'TournTab' existieren, sortiere die
        Paarungen so, dass moeglichst wenig Spiele zustande kommen
        (fuer Freilose der ersten Runde)...
    */
    do
    {
        one= -1;
        two= -1;
        /*
            suche Dummy-Dummy-Paarung (speichere Index in 'one'):
        */
        i= 0;
        while (i < TournFirstUnused-1)
        {
            if ((TournTab[i] == -1) && (TournTab[i+1] == -1))
                one= i;
            i+= 2;
        }
        if (one > -1)
        {
            /*
                es wurde eine Dummy-Dummy-Paarung gefunden; suche
                jetzt eine Real-Real-Paarung (speichere Index in
                'two'):
            */
            i= 0;
            while (i < TournFirstUnused-1)
            {
                if ((TournTab[i] > -1) && (TournTab[i+1] > -1))
                    two= i;
                i+= 2;
            }
            if ((one > -1) && (two > -1))
            {
                /*
                    mache aus den gefundenen Paarungen zwei
                    Real-Dummy-Paarungen:
                */
                swap= TournTab[one];
                TournTab[one]= TournTab[two];
                TournTab[two]= swap;
            } /* if */
        } /* if */
    } while((one > -1) && (two > -1));
} /* TournShuffle */

static int TournMultipleFreeRounds(void)
/* Fuer alle Spieler, die in TournTab keinen Gegner haben, wird
   in die Protokolldatei ausgegeben, dass sie fuer die aktuelle Runde
   ein Freilos haben. Es wird angenommen, dass die Anzahl der Spieler
   in der Eishockeytabelle eine Zweierpotenz ist.

   Funktionswert: 0: alles OK; 1: Fehler;
*/
{
    int i;
    int IntroPrinted= 0;
    int PrintCount= 0;
    int FreeIndex;

    for(i=0; i<TournFirstUnused; i+= 2)
    {
        FreeIndex= -1;
        if ((TournTab[i] == -1) && (TournTab[i+1] != -1))
            FreeIndex= TournTab[i+1];
        if ((TournTab[i] != -1) && (TournTab[i+1] == -1))
            FreeIndex= TournTab[i];
        if (FreeIndex != -1)
        {
            if (!IntroPrinted)
            {
                if (OutText(
                       "Freilos in der ersten Runde f\"ur:"
                   ))
                       goto ErrorExit;
                if (OutNewLine()) goto ErrorExit;
                if (OutSpace()) goto ErrorExit;
                IntroPrinted= 1;
            } /* if */
            if (PrintCount > 0)
            {
                if (OutText(", ")) goto ErrorExit;
            }
            if (PrintCount >= 5)
            {
                if (OutNewLine()) goto ErrorExit;
                if (OutSpace()) goto ErrorExit;
                PrintCount= 0;
            }
            PrintCount++;
            if (OutText(TabPlayers[FreeIndex].name)) goto ErrorExit;
        } /* if */
    } /* for */

    if (IntroPrinted)
    {
        if (OutText(".")) goto ErrorExit;
        if (OutNewLine()) goto ErrorExit;
    }
    return 0;

    ErrorExit:
        return 1;
} /* TournMultipleFreeRounds */

static void TournOneIceAttack(int of, int de, int *ResOf, int *ResDe)
/* Es wird das Spiel Angriffsstaerke 'of' gegen Verteidigungsstaerke
   'de' ausgetragen. In '*ResOf' wird die Anzahl der Tore fuer die
   Offensive und in '*ResDe' die Anzahl der Tore fuer die Verteidigung
   zurueckgegeben.
*/
{
    int count;
    int add;
    int more;

    *ResOf= 0;
    *ResDe= 0;
    more= of-de;
    if (more > 0)
    {
        /* Tore fuer Offensive: */
        if (of == 1)
            /* 1 Punkt gegen 0 Punkte: 1 Tor fuer 1 Punkt: */
            (*ResOf)++;
        else
        {
	        add= 1;
	        count= 0;
	        do{
	            count+= add;
	            if (count <= more)
	            {
	                add*= 2;
	                (*ResOf)++;
	            }
	        } while (count < more);
        } /* else */
    }
    else
    {
        /* Tore fuer Defensive: */
        if ((de == 1) && (of == 0))
            /* 1 Punkt gegen 0 Punkte: 1 Tor fuer 1 Punkt: */
            (*ResDe)++;
        else
        {
	        if (de == 0) de= 1;
	        if (of == 0) of= 1;
	        add= of;
	        count= of;
	        do{
	            (*ResDe)++;
	            count+= add;
	            add*= 2;
	        } while (count <= de);
        } /* else */
    }
} /* TournOneIceAttack */

static int TournOneIceMatch(int p1, int p2, int *saldo)
/* Die Spieler mit den 'TournTab'-Indizes 'p1' und 'p2' spielen
   gegeneinander. Das Ergebnis wird nach 'TournFile' ausgegeben.
   In '*saldo' wird der Torsaldo aus der Sicht von 'p1' zurueck-
   gegeben ( Tore von p1 minus Tore von p2 ).
   Funktionswert: 0: alles OK, p1 gewinnt;
                     1: alles OK, p2 gewinnt;
                     2: Fehler
*/
{
    int  goals1[4], goals2[4];
         /* Tore in den einzelnen Dritteln bzw. im Sudden Death */
    int  sum1=0, sum2=0;
         /* Gesamttore fuer 'p1' und 'p2' */
    int  r1, r2;

    int  i;
    int  SuddenDeath;
         /* 1: es wurde Sudden Death gespielt;
            0: Sudden Death wurde nicht gespielt */
    int  winner;
    char line[100];

    for(i=0; i<4; i++)
    {
        goals1[i]= 0; goals2[i]= 0;
        TournOneIceAttack(TabPlayers[TournTab[p1]].data[i].offense,
                     TabPlayers[TournTab[p2]].data[i].defense,
                     &r1, &r2);
        goals1[i]+= r1; goals2[i]+= r2;
        TournOneIceAttack(TabPlayers[TournTab[p2]].data[i].offense,
                     TabPlayers[TournTab[p1]].data[i].defense,
                     &r2, &r1);
        goals1[i]+= r1; goals2[i]+= r2;
        if (i < 3)
        {
            sum1+= goals1[i];
            sum2+= goals2[i];
        }
    }

    SuddenDeath= (sum1 == sum2);
    if (SuddenDeath)
    {
        if (goals1[3] > goals2[3])
        {
            goals1[3]= 1;
            goals2[3]= 0;
            sum1++;
        }
        else if (goals1[3] < goals2[3])
        {
            goals1[3]= 0;
            goals2[3]= 1;
            sum2++;
        }
        else /* goals1[3] == goals2[3] */
            if (RndGet(0,1) == 0)
            {
                goals1[3]= 1;
                goals2[3]= 0;
                sum1++;
            }
            else
            {
                goals1[3]= 0;
                goals2[3]= 1;
                sum2++;
            }
    }
    winner= (sum1 > sum2) ? p1 : p2;

    sprintf(line, "%s gegen %s  %d:%d ",
        TabPlayers[TournTab[p1]].name, TabPlayers[TournTab[p2]].name,
        sum1, sum2);
    if (OutText(line)) goto ErrorExit;
    if (SuddenDeath)
    {
        /* bis auf weiteres kein Hinweis auf Verlaengerung

        if (OutText("nach Verl\"angerung ")) goto ErrorExit;
        */
    }
    if (OutText("(")) goto ErrorExit;
    for(i=0; i<4; i++)
    {
        if ((i<3) || SuddenDeath)
        {
            sprintf(line, "%d:%d", goals1[i], goals2[i]);
            if (OutText(line)) goto ErrorExit;
        }
        if ( (i<2) || (SuddenDeath && (i==2)) )
        {
            if (OutText(", ")) goto ErrorExit;
        }
    }
    if (OutText(")")) goto ErrorExit;
    if (OutNewLine()) goto ErrorExit;

    *saldo= sum1 - sum2;
    return winner == p1 ? 0 : 1 ;

    ErrorExit:
        printf("kann Ausgabedatei nicht schreiben\n");
        return 2;
} /* TournOneIceMatch */

static int TournOneTennisMatch(int p1, int p2, int *saldo)
/* Die Spieler mit den 'TournTab'-Indizes 'p1' und 'p2' spielen
   gegeneinander. Das Ergebnis wird nach 'TournFile' ausgegeben.
   In '*saldo' wird der Torsaldo aus der Sicht von 'p1' zurueck-
   gegeben ( Tore von p1 minus Tore von p2 ).
   Funktionswert: 0: alles OK, p1 gewinnt;
                     1: alles OK, p2 gewinnt;
                     2: Fehler
*/
{
    int points1[3], points2[3];
        /* Staerkepunkte in den einzelnen Saetzen */
    int games1[3], games2[3];
        /* Spiele in den einzelnen Saetzen */
    int sum1=0, sum2=0;
        /* Gesamtspiele fuer 'p1' und 'p2' */
    int sets1=0, sets2=0;
        /* Gesamtsaetze fuer 'p1' und 'p2' */
    int ThisSet;
    float fgames1, fgames2, fpoints1, fpoints2;

    int  i;
    int  winner;
    char line[100];
    
    TabGetTennis(TournTab[p1], &points1[0], &points1[1], &points1[2]);
    TabGetTennis(TournTab[p2], &points2[0], &points2[1], &points2[2]);

    for(i=0; i<3; i++)
    {
        if (points1[i] == points2[i])
        {
            games1[i]= 6; games2[i]= 6;
            ThisSet= RndGet(0, 1);
            if (ThisSet == 0)
                games1[i]++;
            else
                games2[i]++;
        }
        else
        {
            if (points1[i] > points2[i])
            {
                /* games1[i]= 6;
                games2[i]= (points2[i] * 6) / points1[i];
                      zu beachten: hier Integer-Arithmetik */

                fpoints1= points1[i];
                fpoints2= points2[i];
                fgames1= 6;
                fgames2= fgames1 * fpoints2 / fpoints1;
                games2[i]= (fgames2 + 0.5); /* Rundung! */
                games1[i]= fgames1;
            }
            else /* points1[i] < points2[i] */
            {
                /* games1[i]= (points1[i] * 6) / points2[i];
                games2[i]= 6; */
                
                fpoints1= points1[i];
                fpoints2= points2[i];
                fgames2= 6;
                fgames1= fgames2 * fpoints1 / fpoints2;
                games1[i]= (fgames1 + 0.5); /* Rundung! */
                games2[i]= fgames2;
            }
            if (games1[i] == games2[i])
            {
                /* durch Rundung gleiche Punktezahl, daher: */
                if (points1[i] > points2[i])
                    games1[i]++; 
                else 
                    games2[i]++;
            }
        } /* ...else */
    
        if (games1[i] > games2[i])
            sets1++;
        else
            sets2++;

        sum1+= games1[i];
        sum2+= games2[i];
    } /* for */
    
    winner= (sets1 > sets2) ? p1 : p2;

    sprintf(line, "%s gegen %s  %d:%d ",
                    TabPlayers[TournTab[p1]].name,
                    TabPlayers[TournTab[p2]].name,
                    sets1, sets2);
    if (OutText(line)) goto ErrorExit;

    if (OutText("(")) goto ErrorExit;
    for(i=0; i<3; i++)
    {
        sprintf(line, "%d:%d", games1[i], games2[i]);
        if (OutText(line)) goto ErrorExit;

        if (i<2)
            if (OutText(", ")) goto ErrorExit;
    }
    if (OutText(")")) goto ErrorExit;
    if (OutNewLine()) goto ErrorExit;

    *saldo= sum1 - sum2;
    return winner == p1 ? 0 : 1 ;

    ErrorExit:
        printf("kann Ausgabedatei nicht schreiben\n");
        return 2;
} /* TournOneTennisMatch */

static int TournOneMatch(int p1, int p2, int *saldo)
/* Die Spieler mit den 'TournTab'-Indizes 'p1' und 'p2' spielen
   gegeneinander. Das Ergebnis wird nach 'TournFile' ausgegeben.
   In '*saldo' wird der Torsaldo aus der Sicht von 'p1' zurueck-
   gegeben ( Tore von p1 minus Tore von p2 ).
   Funktionswert: 0: alles OK, p1 gewinnt;
                  1: alles OK, p2 gewinnt;
                  2: Fehler
*/
{
    switch (TournSelectedGame)
    {
    case TOURN_ICEHOCKEY:
        return TournOneIceMatch(p1, p2, saldo);
    case TOURN_TENNIS:
        return TournOneTennisMatch(p1, p2, saldo);
    default:
        printf("TournOneMatch: Spiel %d unbekannt\n", 
            TournSelectedGame);
        return 2;
    } /* switch */
} /* TournOneMatch */

static int TournAllCombinations(int mode)
/* Die verbliebenen Spieler in 'TournTab' spielen 'jeder gegen jeden'.
   Der mit den meisten Siegen ist der Gewinner. 'mode' gibt den
   Tourniermodus an: 0: KO-System, 1: Jeder-gegen-Jeden.
   Bei 'Jeder-gegen-Jeden' wird im Anschluss an die Spiele eine
   Tabelle ausgegeben.

   Funktionswert:
       0: alles OK, normales Match;
       1: alles OK, Gewinner durch Losentscheidung ermittelt;
       2: Fehler;
*/
{
    int GoalTable[TournMaxPlayers];
    int WinTable[TournMaxPlayers];
    int i, j;
    int res;
    int saldo;
    int MostWins; /* Gewinner nach Anzahl der Siege */
    int EqualWins; /* Anzahl der Gewinner nach Siegen */
    int MostGoals; /* Gewinner nach Anzahl der Tore */
    int EqualGoals; /* Anzahl der Gewinner nach Toren */
    int winner; /* Gesamtgewinner */
    int swapped, CurrentSwapped, hilf;
    int PrintOrder;

    if (TournFirstUnused < 2) return 0;

    /*
        initialisiere Tabellen:
    */
    for(i= 0; i<TournMaxPlayers; i++)
    {
        GoalTable[i]= 0;
        WinTable[i]= 0;
    }

    /*
        spiele jeder gegen jeden:
    */
    for(i=0; i<TournFirstUnused; i++)
        for(j=i+1; j<TournFirstUnused; j++)
        {
            res= TournOneMatch(i, j, &saldo);
            if (res > 1) goto ErrorExit;
            if (res==0)
                WinTable[i]++;
            else
                WinTable[j]++;
            GoalTable[i]+= saldo;
            GoalTable[j]-= saldo;
        }

    /*
        sortiere Spielergebnisse (Bubble-Sort):
    */
    printf("sortiere Spielergebnisse...");

    swapped= 1;
    while(swapped)
    {
        swapped= 0;
        for(i=0; i<TournFirstUnused-1; i++)
        {
            CurrentSwapped= WinTable[i+1] > WinTable[i];
            if (WinTable[i+1] == WinTable[i])
                CurrentSwapped= GoalTable[i+1] > GoalTable[i];
            if (CurrentSwapped)
            {
                hilf= TournTab[i+1];
                TournTab[i+1]= TournTab[i];
                TournTab[i]= hilf;

                hilf= WinTable[i+1];
                WinTable[i+1]= WinTable[i];
                WinTable[i]= hilf;

                hilf= GoalTable[i+1];
                GoalTable[i+1]= GoalTable[i];
                GoalTable[i]= hilf;
            }
            swapped= swapped || CurrentSwapped;
        } /* for */
    } /* while */
    printf("OK\n");

    if (mode == 1)
    {
        /*
            gesamtes Tournier 'jeder gegen jeder'...

            schreibe die Ergebnistabelle:
        */
        printf("schreibe Ergebnistabelle...");

        if (OutGameTableStart()) goto ErrorExit;

        for(i=0; i<TournFirstUnused; i++)
        {
            /*
                stelle fest, ob die Platzziffer ausgegeben werden muss:
            */
            PrintOrder= (i == 0);
            if (!PrintOrder)
                PrintOrder= ((WinTable[i] != WinTable[i-1]) ||
                             (GoalTable[i] != GoalTable[i-1]));
            /*
                schreibe die Ergebniszeile:
            */
            if (PrintOrder)
            {
                if (OutGameTableLine(
                        i+1, TabPlayers[TournTab[i]].name, 
                        WinTable[i], GoalTable[i])    
                   )
                   goto ErrorExit;
            }
            else
            {
                if (OutGameTableLine(
                        -1, TabPlayers[TournTab[i]].name, 
                        WinTable[i], GoalTable[i])    
                   )
                   goto ErrorExit;
            } /* if */
        } /* for */

        if (OutGameTableEnd()) goto ErrorExit;
        
        printf("OK\n");
    } /* if */

    /*
        suche Gewinner (alter V2.4-Code tuts auch :-) ):
    */
    MostWins= 0;
    EqualWins= 1;
    MostGoals= 0;
    EqualGoals= 1;
    for(j=1; j<TournFirstUnused; j++)
    {
        if (WinTable[j] > WinTable[MostWins])
        {
            MostWins= j;
            EqualWins= 1;
            MostGoals= j;
            EqualGoals= 1;
        }
        else
        if (WinTable[j] == WinTable[MostWins])
        {
            EqualWins++;

            if (GoalTable[j] > GoalTable[MostGoals])
            {
                MostGoals= j;
                EqualGoals= 1;
            }
            else
                if (GoalTable[j] == GoalTable[MostGoals])
                    EqualGoals++;
        }
    } /* for */

    /*
        bestimme Gewinner:
    */
    if (EqualWins < 2)
        winner= MostWins;
    else if (EqualGoals < 2)
        winner= MostGoals;
    else
        /*
            der Gewinner ist zufaellig unter den Spieler  TournTab[0]  bis
            TournTab[EqualWins-1]  zu bestimmen:
        */
        winner= RndGet(0, EqualWins-1);

    /*
        loesche Verlierer:
    */
    for (j=0; j<TournFirstUnused; j++)
        if (j != winner)
            TournTab[j]= -1;

    return ((EqualWins > 1) && (EqualGoals > 1)) ? 1 : 0;

    ErrorExit:
        printf("Fehler\n");
        return 2;
} /* TournAllCombinations */

static int TournFreeRound(int p)
/* Es wird ausgegeben, dass der Spieler mit den 'TournTab'-Index 'p'
   ein Freilos hat.
   Funktionswert: 0: alles OK; 1: Fehler
*/
{
    char line[100];

    sprintf(line,"%s hat in dieser Runde ein Freilos.",
        TabPlayers[TournTab[p]].name);
    if (OutText(line)) return 1;
    if (OutNewLine()) return 1;
    return 0;
} /* TournFreeRound */

static void TournSort(void)
/* 'TournTab' wird nach den Indizes aufsteigend sortiert. (Bubble-Sort)
*/
{
    int swapped= 1;
    int CurrentSwapped;
    int i;
    int hilf;

    while(swapped)
    {
        swapped= 0;
        for(i=0; i<TournFirstUnused-1; i++)
        {
            CurrentSwapped= TournTab[i] > TournTab[i+1];
            if (CurrentSwapped)
            {
                hilf= TournTab[i+1];
                TournTab[i+1]= TournTab[i];
                TournTab[i]= hilf;
            }
            swapped= swapped || CurrentSwapped;
        }
    }
} /* TournSort */

static int TournNextRound(void)
/* 'TournTab' wird mit Hilfe der Daten der vorangegangenen Runde fuer
   die naechste Runde neu initialisiert. (Verlierer fliegen raus)
   Funktionsergebis: 0-> OK; 1->Fehler;
*/
{
    int  new;
    int  old;
    int  i;

    printf("berechne Teilnehmer der naechsten Runde");

    /*
        eleminiere die Verlierer...
    */
    new= 0;
    /* suche den ersten Verlierer: */
    while((TournTab[new] >= 0) && (new < TournFirstUnused))
        new++;
    /* suche die weiteren Gewinner und schiebe sie zum Tabellenanfang
       hin in die Luecken, die durch die Verlierer entstehen: */
    for (old= new+1; old<TournFirstUnused; old++)
        if (TournTab[old] >= 0)
        {
            TournTab[new]= TournTab[old];
            new++;
        }
    TournFirstUnused= new;

    printf(" (Anzahl: %d)\n", TournFirstUnused);

    TournSort();

    if (TournFirstUnused > 3)
    {
        if (OutText("\nIn die n\"achste Runde kommen:")) goto ErrorExit;
        if (OutNewLine()) goto ErrorExit;
        if (OutSpace()) goto ErrorExit;
    }
    else if (TournFirstUnused == 3)
    {
        if (OutText("\nIn die Endrunde kommen:")) goto ErrorExit;
        if (OutNewLine()) goto ErrorExit;
        if (OutSpace()) goto ErrorExit;
    }
    else if (TournFirstUnused == 2)
    {
        if (OutText("\nIns Finale kommen: ")) goto ErrorExit;
    }
    else /* TournFirstUnused == 1 */
        if (OutText("\nDas Tournier gewinnt ")) goto ErrorExit;

    for(i= 0; i<TournFirstUnused; i++)
    {
        if (OutText(TabPlayers[TournTab[i]].name)) goto ErrorExit;

        if (i < TournFirstUnused-1)
        {
            if (TournFirstUnused == 2)
            {
                if (OutText(" und ")) goto ErrorExit;
            }
            else
                if (OutText(", ")) goto ErrorExit;
        }
        if ( ((i+1) % 5 == 0) && (i<TournFirstUnused-1) )
        {
            if (OutNewLine()) goto ErrorExit;
            if (OutSpace()) goto ErrorExit;
        }
    }
    if (OutText(".")) goto ErrorExit;
    if (OutNewLine()) goto ErrorExit;

    return 0;
    
    ErrorExit:
        printf("kann nicht in Ausgabedatei schreiben\n");
        return 1;
} /* TournNextRound */

static int TournWriteTeams(void)
/* Die in der Spielertabelle festgehaltenen Mannschaftsaufstellungen
   werden in die angegebene bereits geoeffnete Datei ausgegeben.
   
   Funktionswert: 0: alles OK; 1: Fehler;
*/
{
    int  i;
    int  valid;
    char line[100];
    
    if (!StatDebugMode())
        return 0;

    for (i=0; i<TabFirstUnused; i++)
    {
        switch(TournSelectedGame)
        {
        case TOURN_ICEHOCKEY:
            TabGetIceValid(i, &valid);
            break;
        case TOURN_TENNIS:
            TabGetTennisValid(i, &valid);
            break;
        default:
            printf("TournCreateFirstRound: Spiel %d unbekannt\n",
                TournSelectedGame);
            goto ErrorExit;
        } /* switch */

        if (valid)
        {
            switch(TournSelectedGame)
            {
            case TOURN_ICEHOCKEY:
                sprintf(line,
                    "%s (%2d,%2d; %2d,%2d; %2d,%2d; %2d,%2d)",
                TabPlayers[i].name,
                TabPlayers[i].data[0].offense, TabPlayers[i].data[0].defense,
                TabPlayers[i].data[1].offense, TabPlayers[i].data[1].defense,
                TabPlayers[i].data[2].offense, TabPlayers[i].data[2].defense,
                TabPlayers[i].data[3].offense, TabPlayers[i].data[3].defense);
                if (OutComment(line)) goto ErrorExit;
                break;
            case TOURN_TENNIS:
                sprintf(line,
                    "%s (%2d, %2d, %2d)",
                    TabPlayers[i].name, TabPlayers[i].tennis[0],
                    TabPlayers[i].tennis[1], TabPlayers[i].tennis[2]);
                if (OutComment(line)) goto ErrorExit;
                break;
            default:
                printf("TournWriteTeams: Spiel %d unbekannt\n",
                    TournSelectedGame);
                goto ErrorExit;
            } /* switch */
        } /* if */
    } /* for */
    return 0;
    
    ErrorExit:
        return 1;
} /* TournWriteTeams */

static int TournCheckValid(void)
/* Funktionswert: 
       1: es existiert mindestens ein Spieler mit einer gueltigen
          Spielaufstellung;
       0: sonst
*/
{
    int i= 0;
    int valid= 0;
    
    while ((i < TabFirstUnused) && !valid)
        switch(TournSelectedGame)
        {
        case TOURN_ICEHOCKEY:
            TabGetIceValid(i++, &valid);
            break;
        case TOURN_TENNIS:
            TabGetTennisValid(i++, &valid);
            break;
        default:
            printf("TournCheckValid: Spiel %d unbekannt\n",
                   TournSelectedGame);
            return 0;
        }
    
    return valid;
} /* TournCheckValid() */

void Tournament(int game)
{
    int    eof;
    int    res;
    int    saldo;
    int    i;
    float  RndNumber;
    int    FirstRound= 1;
    int    TournMode;
    char  *name;
    char   line[100];

    TournSelectedGame= game;

    switch(TournSelectedGame)
    {
    case TOURN_ICEHOCKEY:
        name= StatFileName(STAT_ICEHOCKEY);
        break;
    case TOURN_TENNIS:
        name= StatFileName(STAT_TENNIS);
        break;
    default:
        printf("Tournament: Spiel %d unbekannt\n", TournSelectedGame);
        return;
    } /* switch */

    if (name == NULL)
    {
        printf("kein Name fuer Ausgabedatei bekannt...Abbruch\n");
        return;
    }
    else
        strcpy(TournFileName, name);
    
    TournSelectedGame= game;
    
    if (!TournCheckValid())
    {
        switch(TournSelectedGame)
        {
        case TOURN_ICEHOCKEY:
            printf("keine Eishockeyteilnehmer...kein Eishockey...OK\n");
            return;
        case TOURN_TENNIS:
            printf("keine Tennisteilnehmer...kein Tennis...OK\n");
            return;
        default:
            /* darf nicht vorkommen: */
            printf( 
                "Tournament: Spiel %d unbekannt\n", TournSelectedGame);
                
            return;
        }
    }

    TournStart();
    printf("Ausgabedatei '%s'\n", TournFileName);

    if (OutGameStart(TournSelectedGame))  
    {
        printf("\nkann Ausgabedatei nicht oeffnen...Abbruch\n");
        return;
    }

    sprintf(line, "Datei %s", TournFileName);
    if (OutComment(line)) goto ErrorExit;
    sprintf(line,
        "erstellt durch das HippoPRESS-Auswertungsprogramm %s",
        StatVersionString());
    if (OutComment(line)) goto ErrorExit;

    switch(TournSelectedGame)
    {
    case TOURN_ICEHOCKEY:
        sprintf(line, "Eishockey-Auswertung");
        break;
    case TOURN_TENNIS:
        sprintf(line, "Tennis-Auswertung");
        break;
    default:
        sprintf(line, 
        "Tournament(): Spiel %d unbekannt", TournSelectedGame);
        break;
    } /* switch */    
    if (OutComment(line)) goto ErrorExit;

    RndSelectGenerator(0);
    RndGetStartNumber(&RndNumber);
    sprintf(line, "Anfangswert des Zufallsgenerators #0: %f", RndNumber);
    if (OutComment(line)) goto ErrorExit;

    /*
        schreibe Aufstellungen:
    */
    res= TournWriteTeams();
    if (res != 0) goto ErrorExit;

    /*
        gib den Tourniermodus aus:
    */
    switch(TournSelectedGame)
    {
    case TOURN_ICEHOCKEY:
        TabGetIceMode(&TournMode);
        break;
    case TOURN_TENNIS:
        TabGetTennisMode(&TournMode);
        break;
    default:
        printf("Tournament: Spiel %d unbekannt\n", TournSelectedGame);
        goto ErrorExit;
    } /* switch */
    if (TournMode == 1)
    {
        if (OutText("\nTourniermodus: jeder gegen jeden\n\n"))
            goto ErrorExit;
    }
    else if (TournMode == 0)
    {
        if (OutText(
    "\nTourniermodus: KO--System; alle Freilose in der ersten Runde\n\n"))
            goto ErrorExit;
    }
    else
    {
        printf("TournTournier: unbekannter Tourniermodus %d\n", TournMode);
        goto ErrorExit;
    }

    res= TournCreateFirstRound(TabFirstUnused-1, TournMode);
    if (res != 0) goto ErrorExit;

    if (TournFirstUnused < 1)
    {
        if (OutText("heute kein Tournier")) goto ErrorExit;
        if (OutNewLine()) goto ErrorExit;
    }
    else if (TournFirstUnused == 1)
    {
        sprintf(line, "Es gewinnt der einzige Teilnehmer: %s",
            TabGetName(TournTab[0]) );
        if (OutText(line)) goto ErrorExit;
        if (OutNewLine()) goto ErrorExit;
    }

    while(TournFirstUnused > 1)
    {
        if (TournMode == 0)
            TournShuffle();

        if (FirstRound && (TournMode == 0))
        {
            /* neu ab V3.0: alle Freilose in der ersten Runde; fuer die
               Anzahl der Spieler wird dazu immer eine Zweierpotenz
               angenommen; es finden nur die Partien mit zwei realen
               Gegnern statt
            */
            res= TournMultipleFreeRounds();
            if (res != 0) goto ErrorExit;

            FirstRound= 0;
        }
        if ((TournFirstUnused < 4) || (TournMode == 1))
        {
            /*
                nur noch hoechstens 3 Teilnehmer oder Tourniermodus
                'jeder gegen jeden':
            */
            res= TournAllCombinations(TournMode);
            if (res > 1) goto ErrorExit;
            if (res == 1)
            {
                if (OutText("\nLosentscheidung!")) goto ErrorExit;
                if (OutNewLine()) goto ErrorExit;
            }
        }
        else
        {
            i= 0;
            do{
                if ((TournTab[i] >= 0) && (TournTab[i+1] >= 0))
                {
                    res= TournOneMatch(i, i+1, &saldo);
                    if (res > 1) goto ErrorExit;
                    if (res == 0)
                        TournTab[i+1] = -1;
                    else
                        TournTab[i]= -1;
                }
                i+= 2;
                if (i == TournFirstUnused-1)
                {
                    res= TournFreeRound(i);
                    if (res != 0) goto ErrorExit;
                    i++;
                }
            } while(i < TournFirstUnused);
        }
        TournNextRound();
    }
    
    eof= OutGameEnd();
    if (eof != 0)
        printf("\n...kann Ausgabedatei '%s' nicht schliessen\n",
               TournFileName);
               
    return;

    ErrorExit:
        printf("Tournament: Fehler\n");
        eof= OutGameEnd();
        return;
} /* Tournament */
