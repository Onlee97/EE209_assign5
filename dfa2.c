/*------------------------------------------------------------------------*/
/* dfa.c                                                                  */
/* Original Author: Bob Dondero                                           */
/* Modified by Younghwan Go                                               */
/* Illustrate lexical analysis using a deterministic finite state         */
/* automaton (DFA)                                                        */
/*------------------------------------------------------------------------*/

#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum { MAX_LINE_SIZE = 1024 };
enum { FALSE, TRUE };
enum TokenType { TOKEN_NUMBER, TOKEN_WORD };
enum LexState { STATE_START, STATE_IN_NUMBER, STATE_IN_WORD, STATE_ERROR, STATE_EXIT };

/* A Token is either a number or a word ,expressed as a string. */
struct Token
{
     /* The type of the token. */
     enum TokenType eType;
   
     /* The string which is the token's value. */
     char *pcValue;
};

/* Read a line from stdin, and write to stdout each number and word that it contains. 
   Repeat until EOF. Return 0. */
int main(void)
{
     char acLine[MAX_LINE_SIZE];
     DynArray_T oTokens;
     int iSuccessful;

     printf("------------------------------------\n");
     while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL) {
          oTokens = DynArray_new(0);

          iSuccessful = lexLine(acLine, oTokens);
          if (iSuccessful) {
               printf("Numbers: ");
               DynArray_map(oTokens, printNumberToken, NULL);
               printf("\n");
               printf("Words: ");
               DynArray_map(oTokens, printWordToken, NULL);
               printf("\n");
          }
          else
               printf("Invalid line\n");
          printf("------------------------------------\n");

          DynArray_map(oTokens, freeToken, NULL);
          DynArray_free(oTokens);
     }

     return 0;
}
