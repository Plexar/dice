/* Datei: eval.h */

void EvalCharts(void);
/* Auswertung HippoCHARTS */

void EvalWinner(int TestRuns);
/* Auswertung HippoWINNER; 
   TestRuns: 0 --> normale Auswertung; 
   TestRuns: > 0 --> Testauswertung; Wert gibt Anzahl der 
                     Durchlaeufe an
*/

void EvalChartSum(void);
/* HippoCHARTS Sammelauswertung */

