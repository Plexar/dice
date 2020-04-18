/* Datei: eval.c */

#include <stdio.h>
#include <string.h>
#include "rnd.h"
#include "tab.h"
#include "tourn.h"
#include "games.h"
#include "out.h"
#include "sum.h"
#include "stat.h"

extern int TabFirstUnused;
extern tsTabPlayer TabPlayers[TabMaxPlayers];

static int games[GAMES_TABLE_LENGTH+1];
static int points[GAMES_TABLE_LENGTH+1];
static int votes[GAMES_TABLE_LENGTH+1];

void EvalCharts(void)
{
    int   i,j;
    char *name, *author, *publisher, *name2, *author2, *publisher2;
    int   GameIndex;
    int   swapped, swap;
    int   PrintOrder, NextLoop;
    int   multiple;
    int   LineCount;
    char  line[100];
    int   prod1, prod2;

    if (StatFileName(STAT_CHARTS) == NULL)
    {
        printf(
            "kein Dateiname fuer Ausgabedatei bekannt...Abbruch\n");
        return;
    }
    
    printf("Ausgabedatei '%s'\n", StatFileName(STAT_CHARTS));
    /*
        initialisiere die Auswertungsdaten:
    */
    printf("initialisiere Auswertungstabelle...");
    for (i=0; i<GAMES_TABLE_LENGTH; i++)
    {
        games[i]= i;
        points[i]= 0;
        votes[i]= 0;
    }

    /*
        sammle die Abstimmungsergebnisse:
    */
    printf("OK\nsammle Abstimmungsergebnisse...");
    for (i=0; i<TabFirstUnused; i++)
    {
        for (j=0; j<5; j++)
        {
            TabGetCharts(i, j, &name, &GameIndex);
            if ((GameIndex >= 0) && (GameIndex < GamesTableSize()))
            {
                votes[GameIndex]++;
                points[GameIndex]= points[GameIndex]+(5-j);
            }
        }
    }

    /*
        sortiere die Spiele anhand der Gesamtpunktzahlen:
    */
    printf("OK\nsortiere die Spiele...");
    swapped= 0;
    do
    {
        swapped= 0;
        for (i=0; i<GamesTableSize()-1; i++)
        {
            GamesGet(games[i], &name, &author, &publisher);
            GamesGet(games[i+1], &name2, &author2, &publisher2);
            prod1= points[i]*votes[i];
            prod2= points[i+1]*votes[i+1];
            if ((prod1 < prod2)
                || ( (prod1 == prod2)
                     && (strcmp(name, name2) > 0)
                   )
               )
            {
                swapped= 1;

                swap= games[i];
                games[i]= games[i+1];
                games[i+1]= swap;

                swap= points[i];
                points[i]= points[i+1];
                points[i+1]= swap;
                
                swap= votes[i];
                votes[i]= votes[i+1];
                votes[i+1]= swap;
            }
        }
    } while (swapped);

    /*
        schreibe die Auswertung in die Datei...
    */

    /*
        oeffne die Ausgabedatei:
    */
    printf("OK\nschreibe Ausgabedatei...");
    if (OutChartsStart())
    {
        printf("kann Datei nicht oeffnen\n");
        return;
    }
    if (points[0]== 0)
    {
        if (OutText("im Augenblick wohl keine HippoCHARTS\n"))
            goto ErrorExit;
        OutChartsEnd();
        return;
    }

    /*
        schreibe den Tabellenvorspann:
    */
    sprintf(line, "Datei %s", StatFileName(STAT_CHARTS));
    if (OutComment(line)) goto ErrorExit;
    sprintf(line,
        "erstellt durch das HippoPRESS-Auswertungsprogramm %s",
        StatVersionString());
    if (OutComment(line)) goto ErrorExit;

    if (OutChartsTableStart()) goto ErrorExit;

    /*
        schreibe den Tabelleninhalt:
    */
    i= 0;
    prod1= -1;
    prod2= -1;
    do
    {
        GamesGet(games[i], &name, &author, &publisher);
        prod1= prod2;
        prod2= points[i]*votes[i];

        PrintOrder= (i==0);
        if (!PrintOrder)
            PrintOrder= (prod2 != prod1);
        if (PrintOrder)
        {
            if (OutChartsTableLine(i+1, 
                    prod2, points[i], votes[i],
                    name, publisher, author))
                goto ErrorExit;
        }
        else
        {
            if (OutChartsTableLine(-1, 
                    prod2, points[i], votes[i],
                    name, publisher, author))
                goto ErrorExit;
        }

        i++;
        NextLoop= (i < GamesTableSize());
        if (NextLoop)
            NextLoop= (votes[i] > 1) || (points[i] == 5);
    } while (NextLoop);

    /*
        schreibe den Tabellennachspann:
    */
    if (OutChartsTableEnd()) goto ErrorExit;

    /*
        schreibe die sonst noch genannten Spiele:
    */
    if (i < GamesTableSize())
    {
        if (points[i] > 0)
        {
            if (i < GamesTableSize()-1)
                multiple= (points[i+1] > 0);
            else
                multiple= 0;

            sprintf(line,
                "Au\"serdem %s noch genannt:",
                multiple ? "wurden" : "wurde");
            if (OutText(line)) goto ErrorExit;
            if (OutNewLine()) goto ErrorExit;

            LineCount= 0;
            do
            {
                NextLoop= (i+1 < GamesTableSize());
                if (NextLoop)
                    NextLoop= (points[i+1] > 0);

                GamesGet(games[i], &name, &author, &publisher);
                if (OutText(name)) goto ErrorExit;
                LineCount++;

                if (NextLoop)
                {
                    if (OutText(", ")) goto ErrorExit;
                }
                else
                    if (OutText(". ")) goto ErrorExit;

                if ((LineCount > 5) || (!NextLoop))
                {
                    if (OutText("\n")) goto ErrorExit;
                    LineCount= 0;
                }

                i++;
            } while (NextLoop);
        }
    }

    /*
        schreibe die Erl"auterung:
    */
    if (OutText(
        "\nZur Erl\"auterung: unter (1) steht die Punktzahl laut\n"))
        goto ErrorExit;
    if (OutText(
        "Stimmzettel (addiert), unter (2) steht die Anzahl der\n"))
        goto ErrorExit;
    if (OutText(
        "W\"ahler und unter Punkte der Wert unter (1)\n"))
        goto ErrorExit;
    if (OutText(
        "multipliziert mit dem unter (2).\n"))
        goto ErrorExit;

    if (OutChartsEnd())
    {
        printf("kann Datei nicht schliessen\n");
        return;
    }

    /*
        uebernehme die Daten fuer die HippoCHARTS Sammelauswertung:
    */
    printf("OK\nuebernehme Daten fuer Sammelauswertung:\n");
    printf("    loesche veraltete Datensaetze...");
    if (SumDelete(StatAbbrev())) goto ErrorExit2;
    
    printf("OK\n    schreibe neue Datensaetze...");
    for(i= 0; i<GamesTableSize(); i++)
        if (points[i] > 0)
        {
            GamesGet(games[i], &name, &author, &publisher);
            if (SumAdd(StatAbbrev(), name, points[i]*votes[i])) 
                goto ErrorExit2;
        } /* if; for */
    printf("OK\n");
    
    return;

    ErrorExit:
        OutChartsEnd();
    ErrorExit2:
        printf("Fehler\n");
        return;
} /* EvalCharts */

void EvalWinner(int TestRuns) 
{
    int   MemberTable[TabMaxPlayers];
    int   FirstUnusedMember= 0;
    int   i, j, a;
    int   WinnerTable[5], PointTable[5][6];
    int   WinnerCount= 0;
    int   WinnPoints;
    float RndNumber;
    int   TestTable[TabMaxPlayers];
    int   TestMode;
    int   RunCount= 0;
    int   SelectedMemberTable; /* Index in MemberTable */
    int   SelectedWinnerTable;  /* Index in WinnerTable */
    char  line[100];

    /* zur Fehlersuche: */
    int   DebugWinner;
	int   scan= 0;
	float MinCount= 0, MaxCount= 0;

    TestMode= (TestRuns > 0);
    for (i=0; i<TabMaxPlayers; i++)
        TestTable[i]= 0;

    if (StatFileName(STAT_WINNER) == NULL)
    {
        printf("Dateiname fuer Ausgabedatei nicht bekannt\n");
        return;
    }
    
    /*
        oeffne die Ausgabedatei:
    */
	if (!TestMode) {
	    printf("Ausgabedatei '%s'...", StatFileName(STAT_WINNER));
	    if (OutWinnerStart())
	    {
	        printf("kann Datei nicht oeffnen\n");
	        return;
	    }
	    /* zur Fehlersuche: */
	    sprintf(line, "Datei %s", StatFileName(STAT_WINNER)); 
	    if (OutComment(line)) goto ErrorExit;
	    sprintf(line, 
	        "erstellt durch das HippoPRESS-Auswertungsprogramm %s",
	        StatVersionString());
	    if (OutComment(line)) goto ErrorExit;
	
	    RndSelectGenerator(1);
	        /* Generator fuer zur Bestimmung des Gewinners nach 
	           dem Wuerfeln */
	    RndGetStartNumber(&RndNumber);
	    sprintf(line, "Anfangswert des Zufallsgenerators #1: %f",
	                 RndNumber);
	    if (OutComment(line)) goto ErrorExit;
	
	    RndSelectGenerator(2);
	        /* Generator fuer Auswahl der Teilnehmer an der 
	           Wuerfelrunde */
	    RndGetStartNumber(&RndNumber);
	    sprintf(line, "Anfangswert des Zufallsgenerators #2: %f",
	                 RndNumber);
	    if (OutComment(line)) goto ErrorExit;
	    
	    RndSelectGenerator(3);
	        /* Generator fuer Berechnung der Wuerfelpunkte */
	    RndGetStartNumber(&RndNumber);
	    sprintf(line, "Anfangswert des Zufallsgenerators #3: %f",
	                 RndNumber);
	    if (OutComment(line)) goto ErrorExit;
	    
	    RndSelectGenerator(4);
	        /* Generator fuer Gewinnerauswahl */
	    RndGetStartNumber(&RndNumber);
	    sprintf(line, "Anfangswert des Zufallsgenerators #4: %f",
	                 RndNumber);
	    if (OutComment(line)) goto ErrorExit;
	}/*if (!TestMode)*/

    /*
        fuehre Auswertungsdurchlaeufe aus, bis dies nicht mehr 
        gewuenscht ist:
    */	
	do{
	    /*
	        sammle die Mitglieder:
	    */
	    FirstUnusedMember= 0;
	    for (i=0; i<TabFirstUnused; i++)
	        if (TabPlayers[i].member)
	            MemberTable[FirstUnusedMember++]= i;
	    if (FirstUnusedMember <= 0)
	    {
	        /* nichts zu tun */
	        if (!TestMode) 
	        {
	            if (OutText(
	                "f\"allt dieses Mal leider aus\n")) goto ErrorExit;
	            OutWinnerEnd();
	            printf("OK\n");
	        }
	        return;
	    }
	
	    /*
	        es gibt mindestens einen Mitspieler: bestimme den 
	        zukuenftigen Gewinner hier, um staerkere Ausgeglichenheit 
	        zu gewaehrleisten:
	    */
	    RndSelectGenerator(4);
	   
	    SelectedMemberTable= RndGet(0, FirstUnusedMember-1);
	    /* aaa zur Fehlersuche: 
	    SelectedMemberTable= scan;
	    */
	    scan= (scan + 1) % FirstUnusedMember;
	    DebugWinner= MemberTable[SelectedMemberTable];
	    SelectedWinnerTable= -1;
	
	    /*
	        waehle maximal fuenf (bzw. vier) Mitglieder zufaellig aus:
	    */
	    if (SelectedMemberTable >= 0)
	    {
	        /*
	            sorge dafuer, dass der gleichverteilt zufaellig bestimmte
	            Gewinner unter den ausgewaehlten Mitgliedern ist:
	        */
	        WinnerCount= 1;
	        WinnerTable[0]= MemberTable[SelectedMemberTable];
	        MemberTable[SelectedMemberTable]= -1;
	        SelectedWinnerTable= 0;
	    }
	    RndSelectGenerator(2);
	    if (FirstUnusedMember == 1)
	    {
	        if (!TestMode)
	            if (OutText("Es gewinnt der einzige Teilnehmer.\n"))
	                goto ErrorExit;
	    }
	    else if (FirstUnusedMember <= 4)
	    {
	        WinnerCount= FirstUnusedMember;
	        j=1;
	        for(i=0; i<FirstUnusedMember; i++)
	            if (MemberTable[i] >= 0)
	                WinnerTable[j++]= MemberTable[i];
	    }
	    else /* FirstUnusedMember > 4 */
	    {
	        do
	        {
	            a= RndGet(0, FirstUnusedMember-1);
	            if (MemberTable[a] >= 0)
	            {
	                WinnerTable[WinnerCount]= MemberTable[a];
	                WinnerCount++;
	                MemberTable[a]= -1;
	            }
	        } while (WinnerCount < 5);
	    }
	
	    while (WinnerCount > 1)
	    {
	        int MaxIndex;
	
	        WinnPoints= 0;
	
	        /*
	            berechne die Punkte:
	        */
	        RndSelectGenerator(3);
	        for (i=0; i<WinnerCount; i++)
	        {
	            PointTable[i][5]= 0;
	            for (j=0; j<=4; j++)
	            {
	                PointTable[i][j]= RndGet(1,6);
	                PointTable[i][5]= PointTable[i][5]+PointTable[i][j];
	            }
	        }
	        
	        /*
	            gib die Tabelle aus:
	        */
	        if (!TestMode) 
	            if (OutWinnerTableStart()) goto ErrorExit;
	        
	        for (i=0; i<WinnerCount; i++)
	        {
	            /* suche hoechste Punktsumme (beachte bei gleichen
	               Punkten die alphabetische Reihenfolge der Namen):
	            */
	            MaxIndex= 0;
	            for (j=1; j<WinnerCount; j++)
	                if (PointTable[j][5] > 0)
	                  if ( (PointTable[j][5] > PointTable[MaxIndex][5])
	                     || ( (PointTable[j][5] == PointTable[MaxIndex][5])
	                          && ( strcmp(
	                                   TabPlayers[WinnerTable[MaxIndex]].name,
	                                   TabPlayers[WinnerTable[j]].name
	                               )
	                               > 0
	                             )
	                        )
	                     )
	                      MaxIndex= j;
	
	            if (    (MaxIndex != SelectedWinnerTable)
	                   && (SelectedWinnerTable >= 0) && (i == 0)
	               )
	            {
		            /* der gleichverteilt zufaellig ausgewaehlte 
		               Gewinner gewinnt nicht: erhoehe seine Punktzahl:
		            */
		            int incremented;
		            int SelectedField;
		            int corrected;
		              
		            RndSelectGenerator(1);
	                corrected= 0;
	                do 
		            {
		                /* 
		                    inkrementiere einmal und pruefe dann den
		                    Korrekturerfolg:
		                */
		                incremented= 0;
		                while (!incremented && (PointTable[SelectedWinnerTable][5] < 30))
		                {
		                    SelectedField= RndGet(0, 4);
		                    incremented= (PointTable[SelectedWinnerTable][SelectedField] < 6);
		                    if (incremented)
		                    {
		                        PointTable[SelectedWinnerTable][5]++;
		                        PointTable[SelectedWinnerTable][SelectedField]++;
		                    }
		                } /* while */
		                corrected=
		                    ( (PointTable[MaxIndex][5] < 30)
		                      && (PointTable[SelectedWinnerTable][5] > PointTable[MaxIndex][5])
		                    ) || (PointTable[SelectedWinnerTable][5] == 30);
		            } while (!corrected);
		            MaxIndex= SelectedWinnerTable;
		            RndSelectGenerator(3);
	            } /* if */
	
	            if (!TestMode)
		            /* gib Tabellenzeile aus: */
		            if (OutWinnerTableLine(
		                    TabPlayers[WinnerTable[MaxIndex]].name,
		                    PointTable[MaxIndex][0], PointTable[MaxIndex][1],
		                    PointTable[MaxIndex][2], PointTable[MaxIndex][3],
		                    PointTable[MaxIndex][4], PointTable[MaxIndex][5]
		                )
		               ) goto ErrorExit;
	
	            /* sichere die Punktsumme des Siegers bzw. deaktiviere
	               Spieler mit weniger Punkten als der Sieger: */
	            if (i==0)
	                WinnPoints= PointTable[MaxIndex][5];
	            else
	            {
	                if (PointTable[MaxIndex][5] < WinnPoints)
	                    WinnerTable[MaxIndex]= -1;
	            }
	
	            /* deaktiviere den Eintrag: */
	            PointTable[MaxIndex][5]= -1;
	        } /* for i */
	        if (!TestMode) 
	            if (OutWinnerTableEnd()) goto ErrorExit;
	
	        /*
	            eliminiere die Eintraege mit weniger Punkten als der
	            Sieger (--> Stechen):
	        */
	        i= 0;
	        for (j= 0; j<WinnerCount; j++)
	            if (WinnerTable[j] > -1)
	            {
	                WinnerTable[i++]= WinnerTable[j];
	                if (j == SelectedWinnerTable)
	                    SelectedWinnerTable= i-1;
	            }
	
	        WinnerCount= i;
	
	        if (WinnerCount > 1)
	        {
	            if (!TestMode) {
	                if (OutText("Das Stechen ergibt:\n")) goto ErrorExit;
	            }
	        }
	    } /* while */
	    RndSelectGenerator(0);
	
		if (!TestMode) {
		    /*
		        schreibe Glueckwuensche:
		    */
		    sprintf(line, "Die Gl\"uckw\"unsche gehen an %s!",
		             TabPlayers[WinnerTable[0]].name);
		    if (OutText(line)) goto ErrorExit;
		
		    if (OutWinnerEnd())
		    {
		        printf("kann Datei nicht schliessen\n");
		        return;
		    }
		
		    printf("OK\n");
		}
		else
		{
		    TestTable[WinnerTable[0]]++;
		    if (WinnerTable[0] != DebugWinner)
		        printf("!?");
		    RunCount++;
		    if ((RunCount % 20) == 0) 
		        printf("%d ...\n", RunCount);
		    else
		        printf(".");
		} /* if (!TestMode) else */
	} while(TestMode && (RunCount < TestRuns));
	
	if (TestMode)
	{
	    float FloatRuns;
	    
	    printf("\n");
	    MaxCount= 0;
	    MinCount= TestRuns;
	    FloatRuns= TestRuns;
	    for (i=0; i<TabFirstUnused; i++)
	    {
	        if ((TestTable[i] < MinCount) && (TestTable[i] > 0)) 
	            MinCount= TestTable[i];
	        if (TestTable[i] > MaxCount) MaxCount= TestTable[i];
	        printf("%3d: %4d\n", i, TestTable[i]);
	    }
	    printf("%s %d, %s %d, %s %2.1f%%\n",
	        "minimal:", (int) MinCount, "maximal:", (int) MaxCount,
	        "Ausgeglichenheit:", 
	        (FloatRuns-(MaxCount-MinCount)) * 100.0 / FloatRuns);
	        /* Ausgeglichenheit: Min-Max-Differenz als Prozentwert 
	           von der Gesamtzahl der Durchlaeufe */
	}

    return;

    ErrorExit:
        RndSelectGenerator(0);
        printf("Fehler\n");
        if (!TestMode) OutWinnerEnd();
        return;
} /* EvalWinner */

void EvalChartSum(void)
{
    int   i;
    char  abbrev[5], NameEntry[100];
    int   res;
    int   FoundIndex;
    char *name, *author, *publisher, *name2, *author2, *publisher2;
    int   swapped, swap;
    int   PrintOrder, NextLoop, multiple, LineCount;
    char  line[100];
    int   EntryPoints;
        
    /*
        sammle die Punktesummen...
    */
    printf("Ausgabedatei '%s'\n", StatFileName(STAT_CHART_SUM));
    printf("sammle Punkte...");
    for (i=0; i<GamesTableSize(); i++)
    {
        games[i]= i;
        points[i]= 0;
    } /* for */
    
    if (SumSetPattern(StatChartSumPattern())) goto ErrorExit;
    
    if (SumListEntries()) goto ErrorExit;
    do
    {
        res= SumNextEntry(abbrev, NameEntry, &EntryPoints);
        if (res == 0)
        {
            GamesSearch(NameEntry, 0, 1, &FoundIndex);
            if (FoundIndex == -1)
            {
                GamesNew(&FoundIndex, 1);
                GamesSet(FoundIndex, NameEntry, "", "", 1);
                games[FoundIndex]= FoundIndex;
                points[i]= 0;
            }
            points[FoundIndex]+= EntryPoints;
        } 
        else 
            if (res != 1) goto ErrorExit;
    } while (res == 0);
    
    /*
        sortiere die Spiele...
    */
    printf("OK\nsortiere Spiele...");
    swapped= 0;
    do
    {
        swapped= 0;
        for (i=0; i<GamesTableSize()-1; i++)
        {
            GamesGet(games[i], &name, &author, &publisher);
            GamesGet(games[i+1], &name2, &author2, &publisher2);
            if ((points[i] < points[i+1])
                || ( (points[i] == points[i+1])
                     && (strcmp(name, name2) > 0)
                   )
               )
            {
                swapped= 1;

                swap= games[i];
                games[i]= games[i+1];
                games[i+1]= swap;

                swap= points[i];
                points[i]= points[i+1];
                points[i+1]= swap;
            }
        }
    } while (swapped);

    /*
        schreibe die Ausgabedatei...
    */

    /*
        oeffne die Ausgabedatei:
    */
    printf("OK\nschreibe Ausgabedatei...");
    if (OutChartSumStart())
    {
        printf("kann Datei nicht oeffnen\n");
        return;
    }
    if (points[0]== 0)
    {
        if (OutText("im Augenblick wohl keine HippoCHARTS Sammelauswertung\n"))
            goto ErrorExit;
        OutChartSumEnd();
        return;
    }

    /*
        schreibe die Einleitungskommentare:
    */
    sprintf(line, "Datei %s", StatFileName(STAT_CHART_SUM));
    if (OutComment(line)) goto ErrorExit;
    sprintf(line, 
        "erstellt durch das HippoPRESS-Auswertungsprogramm %s",
        StatVersionString());
    if (OutComment(line)) goto ErrorExit;
    sprintf(line,
        "Suchmuster %s", StatChartSumPattern());
    if (OutComment(line)) goto ErrorExit;

    if (OutComment("beteiligte Einzelauswertungen:")) goto ErrorExit;
    line[0]= '\0'; 
    i= 0;
    res= SumNextAbbrev(abbrev);
    if (res == -1) goto ErrorExit;
    while (res == 0)
    {
        strcat(line, abbrev);
        strcat(line, " ");
        i++;
        if (i >= 15)
        {
            if (OutComment(line)) goto ErrorExit;
            i= 0;
            line[0]= '\0';
        }
        res= SumNextAbbrev(abbrev);
        if (res == -1) goto ErrorExit;
    } /* while */
    if (OutComment(line)) goto ErrorExit;

    /*
        schreibe den Tabellenvorspann:
    */
    if (OutChartSumTableStart()) goto ErrorExit;    

    /*
        schreibe den Tabelleninhalt:
    */
    i= 0;
    do
    {
        GamesGet(games[i], &name, &author, &publisher);

        PrintOrder= (i==0);
        if (!PrintOrder)
            PrintOrder= (points[i] != points[i-1]);
        if (PrintOrder)
        {
            if (OutChartSumTableLine(i+1, 
                    points[i], name, publisher, author))
                goto ErrorExit;
        }
        else
        {
            if (OutChartSumTableLine(-1, 
                    points[i], name, publisher, author))
                goto ErrorExit;
        }

        i++;
        NextLoop= (i < GamesTableSize());
        if (NextLoop)
            NextLoop= (points[i] >= 5);
    } while (NextLoop);

    /*
        schreibe den Tabellennachspann:
    */
    if (OutChartSumTableEnd()) goto ErrorExit;

    /*
        schreibe die sonst noch genannten Spiele:
    */
    if (i < GamesTableSize())
    {
        if (points[i] > 0)
        {
            if (i < GamesTableSize()-1)
                multiple= (points[i+1] > 0);
            else
                multiple= 0;

            sprintf(line,
                "Au\"serdem %s noch genannt:",
                multiple ? "wurden" : "wurde");
            if (OutText(line)) goto ErrorExit;
            if (OutNewLine()) goto ErrorExit;

            LineCount= 0;
            do
            {
                NextLoop= (i+1 < GamesTableSize());
                if (NextLoop)
                    NextLoop= (points[i+1] > 0);

                GamesGet(games[i], &name, &author, &publisher);
                if (OutText(name)) goto ErrorExit;
                LineCount++;

                if (NextLoop)
                {
                    if (OutText(", ")) goto ErrorExit;
                }
                else
                    if (OutText(". ")) goto ErrorExit;

                if ((LineCount > 5) || (!NextLoop))
                {
                    if (OutText("\n")) goto ErrorExit;
                    LineCount= 0;
                }

                i++;
            } while (NextLoop);
        }
    }

    if (OutChartSumEnd())
    {
        printf("kann Datei nicht schliessen\n");
        return;
    }

    printf("OK\n");
    return;
    
    ErrorExit:
        OutChartSumEnd();
        printf("Fehler\n");
        return;
} /* EvalChartSum */
