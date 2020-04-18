/* Datei: util.c */

#include <stdio.h>
#include <string.h>

int UtilFileInput(FILE *file, char *String, int MaxCount)
{
    char StringInput[101];
    int  i, c;
    char CurrentChar= '\0',
         LastChar= '\0';
    int  OneNewlineRead= 0;
            /* 1: mindestens ein Zeilenende-Zeichen wurde gelesen;
               0: sonst;
             */
    int  TwoNewlineRead= 0;
            /* 1: Zeilenende wurde erkannt;
               0: sonst
            */
    int  res= 0;
            /* Funktionswert */

    /* begrenze die Laenge der Eingabe: */    
    if (MaxCount > 100)
        MaxCount= 100;

    /* Fehler in C-Bibliothek: 
         fgets ueberliest mehrere aufeinander folgende Leerzeilen;
         deshalb:
    */
    i= 0;
    do
    {
        c= fgetc(file);
        LastChar= CurrentChar;
        CurrentChar= c;
        switch(CurrentChar)
        {
        case '\r':
            if (OneNewlineRead)
            {
                TwoNewlineRead= 1;
                if (LastChar == '\r')
                    c= ungetc(c, file);
            }
            else
                OneNewlineRead= 1;

            StringInput[i]= '\0';
            break;
        case '\n':
            if (OneNewlineRead)
            {
                TwoNewlineRead= 1;
                if (LastChar == '\n')
                    c= ungetc(c, file);
            }
            else
                OneNewlineRead= 1;

            StringInput[i]= '\0';
            break;
        case EOF:
            StringInput[i]= '\0';
            TwoNewlineRead= 1;
            res= 1;
            break;
        default:
            if (OneNewlineRead)
            {
                TwoNewlineRead= 1;
                c= ungetc(c, file);
            }
            else
            {
                StringInput[i++]= CurrentChar;
                StringInput[i]= '.';
            }
            break;
        }
    }
    while(!(TwoNewlineRead) && (i<MaxCount));

    StringInput[i]= '\0';

    strcpy(String, StringInput);
    return res;
} /* UtilFileInput */

void UtilInput(char *input)
{
    char CurrentInput[100]= "";
    int  waiting;
    int  FirstUnused= 0;
    char c;

    *input= '\0'; 
    do{
        c= getc(stdin); 
        switch (c)
        {
        case '\n':
        case '\r':
        case EOF:
            break;
        case '\b':
            if (FirstUnused > 0)
                FirstUnused--;
            break;
        default:
            if (FirstUnused < 99)
            {
                CurrentInput[FirstUnused]= c;
                FirstUnused++;
            }
            break;        
        }
        CurrentInput[FirstUnused]= '\0';
        waiting= ((c != '\n') && (c != EOF) && (c != '\r'));
    } while (waiting);
    strcat(input, CurrentInput);
    printf("\n");
} /* UtilInput */

void UtilIntInput(int *input)
{
    char StringInput[101]= "";
    
    UtilInput(StringInput);
    sscanf(StringInput, "%d", input);
} /* UtilIntInput */

int UtilCopyFile(char *source, char *target)
{
    register int  res;
    register int  data;
    FILE         *from, *to;

    from= fopen(source, "r");
    if (from == NULL) return -1;

    to= fopen(target, "w");
    if (to == NULL)
    {
        res= fclose(from);
        return -1;
    }
    
    data= fgetc(from);
    while (data != EOF)
    {
        res= fputc(data, to);
        if (res == EOF) 
        {
            res= fclose(from);
            res= fclose(to);
            return -1;
        }
        data= fgetc(from);
    } /* while */
    
    res= fclose(from);
    if (res == EOF)
    {
        res= fclose(to);
        return -1;
    }
    res= fclose(to);
    if (res == EOF) return -1;

    return 0;
} /* UtilCopyFile */

int UtilWriteInitString(FILE *file, char *string)
/* Funktionswert: 0: Fehler, sonst: ok */
{
    char *current;
    int   eof;

    if (fprintf(file, "\"") == EOF) goto ErrorExit;

    for(current= string; *current != '\0'; current++)
    {
        if (*current == '\"')
            eof= fprintf(file, "\\\"");
        else if (*current == '\\')
            eof= fprintf(file, "\\\\");
        else
            eof= fprintf(file, "%c", *current);
        if (eof == EOF) goto ErrorExit;
    } /* for */

    if (fprintf(file, "\"") == EOF) goto ErrorExit;

    return 0;

    ErrorExit:
        return -1;
} /* UtilWriteInitString */

int UtilFileExists(char *file)
{
    FILE *FilePtr= NULL;
    int   res;
    int   exists;
   
    FilePtr= fopen(file, "r");
    exists= (FilePtr != NULL);
    if (exists)
    {
        res= fclose(FilePtr);
        if (res == EOF) goto ErrorExit;
        return 1;        
    }
        
    return 0;
    
    ErrorExit:
        return -1;
} /* UtilFileExists */

char *UtilStrlwr(char *string)
{
    char *res= string;
    
    while (*string)
    {
        *string = (('A' <= *string) && (*string <= 'Z')) ?
                 'a'+(*string - 'A') : *string;
        string++;
    }
    return res;
} /* UtilStrlwr */
