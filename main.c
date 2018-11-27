#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum operators
{
    PLUS = '+', MINUS = '-', TIMES = '*', DIVIDE = '/', POWER = '^'
} operator;
enum argumentType
{
   OPERAND = 'd',
   OPERTOR = 'r'
} argumentType;
typedef struct argument
{
    char type;
    char *data;
} argument;

int isOperator( char c)
{
    if (c == PLUS || c == MINUS || c == TIMES || c == DIVIDE || c == POWER)
    {
        return 1;
    }

    return 0;
}


int main()
{
//    char *Q = "71 * ( 2 + 6 ) - 15 / 3";
//      71 * ( 2 + 6 ) - 15 / 3
    char buffer[101];
    size_t newNumLen;
    size_t PSize = 0;
    argument *P = (argument*) malloc(PSize);
    while (fgets(buffer, 101, stdin))
    {
        int i = 0;
        while(i<strlen(buffer))
        {
            char newnumber[100] = "";
            while (isOperator(buffer[i]) == 0)
            {
                strncat(newnumber, &buffer[i], 1);
                i++;
            }
            argument arg;
            // operand is found
            if ((newNumLen = strlen(newnumber)) != 0)
            {
                arg.type = OPERAND;
                arg.data = malloc(newNumLen * sizeof(char) + 1);
                strcat(newnumber," ");
                strcpy(arg.data, newnumber);
                PSize += sizeof(argument);
                P = (argument*) realloc(P,PSize);
                P[PSize/ sizeof(argument)] = arg;
            }
            else if (buffer[i] == '('){
                //push in stack
            }
            else if(buffer[i] == ')'){

            }
            else // operator is found
            {
                arg.type = OPERTOR;
                strcpy(arg.data, &buffer[i]);
                PSize += sizeof(argument);
                P = (argument*) realloc(P,PSize);
                P[PSize/ sizeof(argument)] = arg;
            }
            printf("type %c data %s\n", arg.type, arg.data);
            i++;
        }

    }


//    While (we have not reached the end of Q)
//       If (an operand is found)
//           Add it to P
//       End-If
//       If (a left parenthesis is found)
//           Push it onto the stack
//       End-If
//       If (a right parenthesis is found)
//           While (the stack is not empty AND the top item is not a left parenthesis)
//                Pop the stack and add the popped value to P
//           End-While
//       Pop the left parenthesis from the stack and discard it
//       End-If
//       If (an operator is found)
//           If (the stack is empty OR if the top element is a left parenthesis)
//              Push the operator onto the stack
//           Else
//              While (the stack is not empty AND the top of the stack is not a left parenthesis AND
//                                precedence of the operator <= precedence of the top of the stack)
//
//                      Pop the stack and add the top value to P
//              End-While
//              Push the latest operator onto the stack
//           End-If
//       End-If
//    End-While
//
//
//    While (the stack is not empty)
//          Pop the stack and add the popped value to P
//    End-While





    return 0;
}