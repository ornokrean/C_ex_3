#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stack.h"
#include <math.h>

/**
 * @var char EMPTY_CHAR
 * @brief empty char to replace non wanted char
 */
static const char EMPTY_CHAR = '\0';

/**
 * @var char LINE_CHAR
 * @brief line jump to replace - non wanted char
 */
static const char LINE_CHAR = '\n';


enum argumentType
{
    PLUS = '+', MINUS = '-', TIMES = '*', DIVIDE = '/', POWER = '^',
    OPERAND = 'd'
};


typedef struct argument
{
    char type;
    int number;
} argument;

/**
 * this function checks if given char is operator
 * @param c the char to check
 * @return 1 if this is an operator, 0 otherwise
 */
int isOperator(char c)
{
    if (c == PLUS || c == MINUS || c == TIMES || c == DIVIDE || c == POWER)
    {
        return 1;
    }

    return 0;
}

/**
 * this function converts given string to an int, and prints error on fail
 * @param to_convert the string to convert
 * @return int, the converted string
 */
int convertToInt(const char *to_convert)
{
    char *temp = NULL;
    const int res = (int) strtol(to_convert, &temp, 10);
    if (strlen(temp))
    {
        fprintf(stderr, "eree"); //FIXME
        exit(EXIT_FAILURE);
    }
    return res;
}

void checkTop(Stack *stack, argument *head)
{
    if (!isEmptyStack(stack))
    {
        memcpy(head, stack->_top->_data, stack->_elementSize);
    }
}


/**
 * this function sets a new or old memory to given pointer. if ths memory allocation fails, it
 * will exit the program and print a message.
 * @param oldMemory the old memory, if exists, to copy to the new memory
 * @param newSize the new memory size to create.
 * @return a pointer to the new memory if created
 */
void *allocMemory(void *oldMemory, size_t newSize)
{
    void *newMemory = (oldMemory == NULL) ? malloc(newSize) : realloc(oldMemory, newSize);
    if (newMemory == NULL)
    {
//        fprintf(stderr, MEMORY_ERROR); //FIXME
        printf("hehewgliwjesvfds");
        exit(EXIT_FAILURE);
    }
    return newMemory;
}

/**
 * this funtion decides which operator has greater precedence
 * @param op1 the first operator
 * @param op2 the second operator
 * @return 0 if equal, 1 if op1 has greater precedence, 2 if op2 has greater precedence
 */
int pred(char op1, char op2)
{

    if (op1 == POWER)
    {
        if (op2 == POWER)
        {
            return 0;
        }
        return 1;
    }
    if (op1 == TIMES || op1 == DIVIDE)
    {
        if (op2 == TIMES || op2 == DIVIDE)
        {
            return 0;
        }
        else if (op2 == POWER)
        {
            return 2;
        }
        return 1;
    }
    else if (op2 == POWER || op2 == TIMES || op2 == DIVIDE)
    {
        return 2;
    }
    return 0;
}

/**
 * this function removes a given char from the given string, and will replace it with EMPTY_CHAR.
 * @param currLine the line to remove the given char from
 * @param to_remove the char to remove
 */
void removeChar(const char *currLine, char to_remove)
{
    char *pos;
    if ((pos = strchr(currLine, to_remove)) != NULL)
    {
        *pos = EMPTY_CHAR;
    }
}



int main()
{


    char line[101];
    while (fgets(line, 101, stdin))
    {
        //for each row entered:
        Stack *stack = stackAlloc(sizeof(argument));
        argument *arguments = (argument *) allocMemory(NULL, sizeof(argument) * strlen(line));
        removeChar(line, LINE_CHAR);
        int i = 0;
        int argNum = 0;
        argument *tempHead = (argument *) allocMemory(NULL, sizeof(argument));
        while (i < (int)strlen(line))
        {
            if (isdigit(line[i]))
            {
                //create new number
                arguments[argNum].type = OPERAND;
                // insert the int to char and add space
                char currNum[100] = {0};
                int currIndex = 0;
                // concat
                while (isdigit(line[i]) && i < (int)strlen(line))
                {
                    // add to cur number
                    currNum[currIndex] = line[i];
                    currIndex++;
                    i++;
                }

                //add to our array
                arguments[argNum].number = convertToInt(currNum);
                argNum++;
                continue;
            }
            else if (line[i] == '(')
            {
                push(stack, &line[i]);
            }
            else if (line[i] == ')')
            {
                checkTop(stack, tempHead);
                while (!isEmptyStack(stack) && tempHead->type != '(')
                {
                    // add the popped value to arguments
                    argument head;
                    pop(stack, &head);
                    arguments[argNum] = head;
                    argNum++;
                }
                pop(stack, NULL); //Pop the left parenthesis
            }
            else if (isOperator(line[i]))// operator is found
            {
                checkTop(stack, tempHead);
                argument latestOp;
                if (isEmptyStack(stack) || tempHead->type == '(')
                {

                    // Push the operator onto the stack
                    latestOp.type = line[i];
                    push(stack, &latestOp);

                }
                else
                {
                    checkTop(stack, tempHead);
                    while (!isEmptyStack(stack) && tempHead->type != '('
                           && pred(line[i], tempHead->type) < 2)
                    {
                        //Pop the stack and add the top value to arguments
                        argument head;
                        pop(stack, &head);
                        arguments[argNum] = head;
                        argNum++;

                    }
                    push(stack, &latestOp);
                }
            }
            i++;
        }


        while (!isEmptyStack(stack))
        {
            argument head;
            pop(stack, &head);
            arguments[argNum] = head;
            argNum++;
        }

//        for (int i = 0; i < 4; i++)
//        {
//
//                printf("print type: %c data: %s number: %d\n", arguments[i].type, arguments[i].data, arguments[i].number);
//
//        }
//        for (int j = 0; j < argNum; ++j)
//        {
//            printf("print type: %c  number: %d\n", arguments[j].type,
//                    arguments[j].number);
//        }
//         12   if (arguments[j].data != NULL)
//            {
//                free(arguments[j].data);
//            }
//        }
        freeStack(&stack);

        //make it postfix:
        int j = 0;
        stack = stackAlloc(sizeof(argument));
        while (j < argNum)
        {
            if (!isOperator(arguments[j].type)) // operand
            {
                push(stack, &arguments[j]);
            }
            else // operator
            {
                argument A;
                pop(stack, &A);
                argument B;
                pop(stack, &B);
                argument res;
                res.type = OPERAND;
                if (arguments[j].type == POWER){
                    res.number = (int) (pow(B.number, A.number) + 0.5);

                }
                else if (arguments[j].type == TIMES){
                    res.number =  B.number*A.number;
                }else if (arguments[j].type == DIVIDE){
                    if (A.number == 0){
                        printf("error");
                    }else{
                        res.number =  B.number/A.number;
                    }
                }else if (arguments[j].type == PLUS){
                    res.number =  B.number+A.number;
                }else if (arguments[j].type == MINUS){
                    res.number =  B.number-A.number;
                }

//                res.number= calcExp(B,A,arguments[j]);
                push(stack,&res);
            }
            j++;
        }

        argument answer;
        pop(stack,&answer);
        printf("%d",answer.number);
        freeStack(&stack);
        free(tempHead);
        free(arguments);

    }


    return 0;
}



// gcc main.c stack.c stack.h -o ex3

// valgrind --leak-check=full --show-possibly-lost=yes --show-reachable=yes  --undef-value-errors=yes ex3