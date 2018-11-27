#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stack.h"

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

enum operators
{
    PLUS = '+', MINUS = '-', TIMES = '*', DIVIDE = '/', POWER = '^'
};


enum argumentType
{
    OPERAND = 'd',
    OPERTOR = 'r'
};


typedef struct argument
{
    char type;
    char *data;
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
int pred(char op1,char op2){
    if (op1 == POWER){
        if (op2 == POWER){
            return 0;
        }
        return 1;
    }
    if(op1 == TIMES || op1 == DIVIDE){
        if (op2 == TIMES || op2 == DIVIDE){
            return 0;
        }
        else if(op2 == POWER){
            return 2;
        }
        return 1;
    }
    else if(op2 == POWER || op2 == TIMES || op2 == DIVIDE){
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


    Stack *stack = stackAlloc(sizeof(argument));
    char buffer[101];
    argument P[100];
    while (fgets(buffer, 101, stdin))
    {
        removeChar(buffer, LINE_CHAR);
        int i = 0;
        int argNum = 0;
        while (i < strlen(buffer))
        {
            if (isdigit(buffer[i]))
            {
                //create new number
                argument arg;
                arg.type = OPERAND;
                arg.data = (char *) allocMemory(NULL, sizeof(char) + 1);
                // insert the int to char and add space
                strncpy(arg.data, &buffer[i], 1);


                i++;
                // concat
                while (isdigit(buffer[i]) && i < strlen(buffer))
                {
                    strncat(arg.data, &buffer[i], 1);
                    i++;
                }
                //add to our array
                arg.number = convertToInt(arg.data);
                strcat(arg.data, " ");
                P[argNum] = arg;
                argNum++;
            }
            if (buffer[i] == '(')
            {
                push(stack, &buffer[i]);
            }
            if (buffer[i] == ')')
            {
                while (!isEmptyStack(stack) && strcmp(stack->_top->_data, "(") != 0)
                {
                    // add the popped value to P
                    argument head;
                    pop(stack, &head);
                    P[argNum] = head;
                    argNum++;

                }
                pop(stack, NULL); //Pop the left parenthesis
            }
            if (isOperator(buffer[i]))// operator is found
            {
                if (isEmptyStack(stack) || strcmp(stack->_top->_data, "(") == 0)
                {
                    // Push the operator onto the stack
                    argument arg;
                    arg.type = OPERTOR;
                    arg.data = (char *) allocMemory(NULL, sizeof(char) + 1);
                    strncpy(arg.data, &buffer[i], 1);
                    push(stack, &arg);

                }
                else
                {
                    while (!isEmptyStack(stack) && strcmp(stack->_top->_data, "(") != 0
                        && pred(buffer[i],(char)stack->_top->_data) < 2)
                    {
                        //Pop the stack and add the top value to P
                        argument head;
                        pop(stack, &head);
                        P[argNum] = head;
                        argNum++;

                    }


                }
            }
            i++;
        }



        while(!isEmptyStack(stack))
        {
            argument head;
            pop(stack,&head);
            P[argNum] = head;
            argNum++;
        }

//        for (int i = 0; i < 4; i++)
//        {
//
//                printf("print type: %c data: %s number: %d\n", P[i].type, P[i].data, P[i].number);
//
//        }

    }
    freeStack(&stack);







    return 0;
}



// gcc main.c stack.c stack.h -o ex3

// valgrind --leak-check=full --show-possibly-lost=yes --show-reachable=yes  --undef-value-errors=yes ex3