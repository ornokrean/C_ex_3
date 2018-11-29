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

/**
 * @var char DIV_BY_ZERO_MSG
 * @brief error message for division by 0
 */
static const char DIV_BY_ZERO_MSG[] = "Division by 0!\n";

/**
 * @var char RESULT_MSG
 * @brief  message for result
 */
static const char RESULT_MSG[] = "The value is %d\n";

/**
 * @var char MEMORY_ERROR_MSG
 * @brief error message for memory allocation
 */
static const char MEMORY_ERROR_MSG[] = "ERROR: Memory allocation problem. Exiting...";

/**
 * @var char PRINT_OPERAND
 * @brief  message for operand print
 */
static const char PRINT_OPERAND[] = " %d ";

/**
 * @var char PRINT_OPERATOR
 * @brief  message for operator print
 */
static const char PRINT_OPERATOR[] = "%c";

/**
 * @var char POSTFIX_HEADER
 * @brief the header before printing postfix expression
 */

static const char POSTFIX_HEADER[] = "Postfix: ";

/**
 * @var char INFIX_HEADER
 * @brief the header before printing infix expression
 */
static const char INFIX_HEADER[] = "Infix: ";

/**
 * @var char LINE_BREAK
 * @brief line break string
 */
static const char LINE_BREAK[] = "\n";

/**
 * @var char INT_CONVERSION_ERROR_MSG
 * @brief rror message for int conversion
 */
static const char INT_CONVERSION_ERROR_MSG[] = "ERROR: int conversion error.";

/**
 * @def LINE_LEN 101
 * @brief the maximum length of a line
 */
#define LINE_LEN 101


enum argumentType
{
    PLUS = '+', MINUS = '-', TIMES = '*', DIVIDE = '/', POWER = '^',
    OPERAND = 'd', OPEN_PAR = '(', CLOSE_PAR = ')'
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
        fprintf(stderr, INT_CONVERSION_ERROR_MSG);
        exit(EXIT_FAILURE);
    }
    return res;
}

/**
 * this function checks the top of the stack given. it does not pop the value, only copies it to
 * head given
 * @param stack the stack to peek to
 * @param head the pointer to fill with the data extracted
 */
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
        fprintf(stderr, MEMORY_ERROR_MSG);
        exit(EXIT_FAILURE);
    }
    return newMemory;
}

/**
 * this function decides which operator has greater precedence
 * @param op1 the first operator
 * @param op2 the second operator
 * @return 0 if equal, 1 if op1 has greater precedence, 2 if op2 has greater precedence
 */
int pred(char op1, char op2)
{

    if (op1 == POWER) // operator 1 is ^, check the other one
    {
        if (op2 == POWER)
        {
            return 0; // same precedence
        }
        return 1; // power is bigger than any other
    }
    if (op1 == TIMES || op1 == DIVIDE) // operator 1 is * or /, check the other one
    {
        if (op2 == TIMES || op2 == DIVIDE)
        {
            return 0; // same precedence
        }
        else if (op2 == POWER) // power is bigger than any other
        {
            return 2;
        }
        return 1; // operator 2 is + or -, smaller than * or /
    }
    else if (op2 == POWER || op2 == TIMES || op2 == DIVIDE) // operator 1 can only be + or -
    {
        return 2; // ^ or * or / is bigger than + or -
    }
    return 0; // same precedence
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

/**
 * this function activates when a digit is found, and read from the line until there are no more
 * consecutive digits
 * @param line the line to read from
 * @param arguments the array of arguments
 * @param argNum number of items in arguments array
 * @param index the current line index
 * @return
 */
int handleInfixDigit(char *line, argument *arguments, int *argNum, int index)
{//create new number
    arguments[(*argNum)].type = OPERAND;
    // insert the int to char and add space
    char currNum[LINE_LEN] = {0};
    int currIndex = 0;
    // concat
    while (isdigit(line[index]) && index < (int) strlen(line))
    {
        // add to cur number
        currNum[currIndex] = line[index];
        currIndex++;
        index++;
    }

    //add to our array
    arguments[(*argNum)].number = convertToInt(currNum);
    (*argNum)++;
    return index;
}

/**
 * this function pops the to of the stack and adds it into the given arguments array
 * @param stack the stack to push/pop from
 * @param arguments the array of arguments
 * @param argNum number of items in arguments array
 */
void popAndAddArgument(Stack *stack, argument *arguments, int *argNum)
{
    argument head;
    pop(stack, &head);
    arguments[(*argNum)] = head;
    (*argNum)++;
}

/**
 * this function handles the situation of right parenthesis according to given algorithm
 * @param stack the stack to push/pop from
 * @param arguments the array of arguments
 * @param argNum number of items in arguments array
 * @param tempHead a temp to help with memory problems
 */
void handleInfixRightParenthesis(Stack *stack, argument *arguments, int *argNum,
                                 argument **tempHead)
{
    checkTop(stack, (*tempHead));
    while (!isEmptyStack(stack) && (*tempHead)->type != OPEN_PAR)
    {
        // add the popped value to arguments
        popAndAddArgument(stack, arguments, argNum);
        checkTop(stack, (*tempHead));

    }
    argument D;
    pop(stack, &D); //Pop the left parenthesis //TODO problem here stack is empty
}

/**
 * this function handles the infix operator, according to given algorithm
 * @param line the line to read
 * @param stack the stack to push/pop from
 * @param arguments the array of arguments
 * @param argNum number of items in arguments array
 * @param tempHead a temp to help with memory problems
 * @param index the current line index
 */
void
handleInfixOperator(char *line, Stack *stack, argument *arguments, int *argNum, argument **tempHead,
                    int index)
{
    checkTop(stack, (*tempHead));
    argument latestOperator;
    latestOperator.type = line[index];
    if (isEmptyStack(stack) || (*tempHead)->type == OPEN_PAR)
    {
        // Push the operator onto the stack
        push(stack, &latestOperator);

    }
    else
    {
        checkTop(stack, (*tempHead));

        while (!isEmptyStack(stack) && (*tempHead)->type != OPEN_PAR
               && pred((*tempHead)->type, line[index]) < 2)
        {

            //Pop the stack and add the top value to arguments
            popAndAddArgument(stack, arguments, argNum);
            checkTop(stack, (*tempHead));

        }
        push(stack, &latestOperator);
    }
}

/**
 * this function adds all the items from the stack to the argument list
 * @param stack the stack to add from
 * @param arguments
 * @param argNum
 */
void addAllFromStack(Stack *stack, argument *arguments, int *argNum)
{
    while (!isEmptyStack(stack))
    {
        popAndAddArgument(stack, arguments, argNum);
    }
}

/**
 * this function reads a infix expression from given line
 * @param line the line to parse
 * @param stack the stack to work with while reading the line
 * @param arguments an array of arguments to work with
 * @param argNum the number of items in the arguments array
 * @param tempHead a temp argument pointer to handle some memory problems
 */
void readInfix(char *line, Stack *stack, argument *arguments, int *argNum, argument **tempHead)
{
    printf(INFIX_HEADER);


    (*argNum) = 0;
    (*tempHead) = (argument *) allocMemory(NULL, sizeof(argument));
    removeChar(line, LINE_CHAR);
    int i = 0;
    while (i < (int) strlen(line))
    {
        if (isdigit(line[i]))
        {
            i = handleInfixDigit(line, arguments, argNum, i);
            printf(PRINT_OPERAND, arguments[(*argNum) - 1].number);
            continue; //TODO maybe remove it
        }
        else
        {
            printf(PRINT_OPERATOR, line[i]);
            if (line[i] == OPEN_PAR)
            {
                push(stack, &line[i]);
            }
            else if (line[i] == CLOSE_PAR)
            {
                handleInfixRightParenthesis(stack, arguments, argNum, tempHead);
            }
            else if (isOperator(line[i]))// operator is found
            {
                handleInfixOperator(line, stack, arguments, argNum, tempHead, i);
            }
        }
        i++;
    }
    addAllFromStack(stack, arguments, argNum);


    printf(LINE_BREAK);

}

/**
 * this function calculates an expression given by "B operator A" while A is the first item in the
 * stack, B is the second item in the stack, and operator given
 * @param stack a stack of arguments
 * @param operator the operator to work with
 */
void calcExpression(Stack *stack, argument operator)
{
    argument A;
    pop(stack, &A);
    argument B;
    pop(stack, &B);
    argument res;
    res.type = OPERAND;
    if (operator.type == POWER)
    {
        res.number = (int) (pow(B.number, A.number) + 0.5);

    }
    else if (operator.type == TIMES)
    {
        res.number = B.number * A.number;
    }
    else if (operator.type == DIVIDE)
    {
        if (A.number == 0)
        {
            fprintf(stderr, DIV_BY_ZERO_MSG);
        }
        else
        {
            res.number = B.number / A.number;
        }
    }
    else if (operator.type == PLUS)
    {
        res.number = B.number + A.number;
    }
    else if (operator.type == MINUS)
    {
        res.number = B.number - A.number;
    }
    push(stack, &res);
}

/**
 * this functions reads a postfix given and calculates it
 * @param stack the stack of arguments to be calculated
 * @param arguments the postfix expression
 * @param argNum the number of items in the arguments array
 * @return the given stack
 */
int calcPostfix(Stack *stack, argument *arguments, int argNum)
{
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
            calcExpression(stack, arguments[j]);
        }
        j++;
    }
    argument answer;
    pop(stack, &answer);
    return answer.number;
}

/**
 * this function frees all allocated pointers in the program
 * @param stack a pointer to a memory to br freed
 * @param arguments a pointer to a memory to br freed
 * @param tempHead a pointer to a memory to br freed
 */
void freeAllAllocs(Stack **stack, argument *arguments, argument *tempHead)
{
    freeStack(stack);
    free(tempHead);
    free(arguments);
}

/**
 * this function prints the postfix expression to the screen
 * @param arguments the array of arguments to print
 * @param argNum the number of items in arguments array
 */
void printPostfix(argument *arguments, int argNum)
{
    printf(POSTFIX_HEADER);

    for (int i = 0; i < argNum; ++i)
    {
        if (arguments[i].type == OPERAND)
        {
            printf(PRINT_OPERAND, arguments[i].number);
        }
        else
        {
            printf(PRINT_OPERATOR, arguments[i].type);
        }
    }
    printf(LINE_BREAK);
}

/**
 * this function prints the given result calculated from the expression
 * @param result the result to print
 */
void printExpressionResult(int result)
{ printf(RESULT_MSG, result); }

int main()
{


    char line[LINE_LEN];
    while (fgets(line, LINE_LEN, stdin))
    {
        //for each row entered:

        Stack *stack = stackAlloc(sizeof(argument));
        argument *arguments = (argument *) allocMemory(NULL, sizeof(argument) * strlen(line));
        int argNum;
        argument *tempHead;


        readInfix(line, stack, arguments, &argNum, &tempHead);
        printPostfix(arguments, argNum);
        freeStack(&stack);
        int result = calcPostfix(stack, arguments, argNum);
        printExpressionResult(result);
        freeAllAllocs(&stack, arguments, tempHead);

    }
    return 0;
}



// gcc -Wextra -Wall -Wvla -lm -std=c99 main.c stack.c stack.h -o ex3

// valgrind --leak-check=full --show-possibly-lost=yes --show-reachable=yes  --undef-value-errors=yes ex3