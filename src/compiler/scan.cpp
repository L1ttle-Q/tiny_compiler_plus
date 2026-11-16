/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
   { START,INASSIGN,INCOMMENT,ININT,INFLOAT,INEXP,INID,DONE }
   StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { lineno++;
    if (fgets(lineBuf,BUFLEN-1,source))
    { if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* lookup table of reserved words */
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE},{"int",DecINT},{"float",DecFLOAT}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{  /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   TokenType currentToken;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c))
           state = ININT;
         else if (isalpha(c))
           state = INID;
         else if (c == ':')
           state = INASSIGN;
         else if ((c == ' ') || (c == '\t') || (c == '\n'))
           save = FALSE;
         else if (c == '{')
         { save = FALSE;
           state = INCOMMENT;
         }
         else
         { state = DONE;
           switch (c)
           { case EOF:
               save = FALSE;
               currentToken = ENDFILE;
               break;
             case '=':
               currentToken = EQ;
               break;
             case '<':
               currentToken = LT;
               if (getNextChar() == '=')
                 currentToken = LET;
               else ungetNextChar();
               break;
             case '>':
               currentToken = GT;
               if (getNextChar() == '=')
                 currentToken = GET;
               else ungetNextChar();
               break;
             case '+':
               currentToken = PLUS;
               break;
             case '-':
               currentToken = MINUS;
               break;
             case '^':
               currentToken = XOR;
               break;
             case '*':
               currentToken = TIMES;
               break;
             case '/':
               c = getNextChar();
               if (c == '*')
               {
                 state = INCOMMENT;
                 save = FALSE;
               }
               else
               {
                 ungetNextChar();
                 currentToken = OVER;
               }
               break;
             case '(':
               currentToken = LPAREN;
               break;
             case ')':
               currentToken = RPAREN;
               break;
             case ';':
               currentToken = SEMI;
               break;
             case ',':
               currentToken = COMMA;
               break;
             default:
               currentToken = ERROR;
               break;
           }
         }
         break;
       case INCOMMENT:
         save = FALSE;
         if (c == EOF)
         { state = DONE;
           currentToken = ENDFILE;
         }
         else if (c == '}') state = START;
         else if (c == '*')
         {
           c = getNextChar();
           if (c == '/')
             state = START;
           else
             ungetNextChar();
         }
         break;
       case INASSIGN:
         state = DONE;
         if (c == '=')
           currentToken = ASSIGN;
         else
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           currentToken = ERROR;
         }
         break;
       case ININT:
         if (!isdigit(c))
         { /* backup in the input */
           if (c == '.')
           {
             tokenString[tokenStringIndex++] = (char) c;
             c = getNextChar();
             if (isdigit(c))
               state = INFLOAT;
             else
             {
              tokenStringIndex--;
              ungetNextChar(); ungetNextChar();
              save = FALSE;
              state = DONE;
              currentToken = INT;
             }
           }
           else if (c == 'E' || c == 'e')
           {
             tokenString[tokenStringIndex++] = (char) c;
             c = getNextChar();
             if (isdigit(c) || c == '+' || c == '-')
               state = INEXP;
             else
             {
              tokenStringIndex--;
              ungetNextChar(); ungetNextChar();
              save = FALSE;
              state = DONE;
              currentToken = INT;
             }
           }
           else
           {
             ungetNextChar();
             save = FALSE;
             state = DONE;
             currentToken = INT;
           }
         }
         break;
       case INFLOAT:
         if (!isdigit(c))
         {
           if (c == 'e' || c == 'E')
           {
             tokenString[tokenStringIndex++] = (char) c;
             c = getNextChar();
             if (isdigit(c) || c == '+' || c == '-')
               state = INEXP;
             else
             {
               tokenStringIndex--;
               ungetNextChar(); ungetNextChar();
               save = FALSE;
               state = DONE;
               currentToken = FLOAT;
             }
           }
           else
           {
            ungetNextChar();
            save = FALSE;
            state = DONE;
            currentToken = FLOAT;
           }
         }
         break;
       case INEXP:
         if (!isdigit(c))
         {
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = FLOAT;
         }
         break;
       case INID:
         if (!isalpha(c))
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = ID;
         }
         break;
       case DONE:
       default: /* should never happen */
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     printToken(currentToken,tokenString);
   }
   return currentToken;
} /* end getToken */

