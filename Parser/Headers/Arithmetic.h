#ifndef ARITHMETIC_H
#define ARITHMETIC_H

/**
 * @file
 * @brief [Infix to
 * Postfix](https://condor.depaul.edu/ichu/csc415/notes/notes9/Infix.htm)
 * Expression Conversion
 * @details Convert Infixed expressions to Postfix expression.
 * @author [Harsh Karande](https://github.com/harshcut)
 */

// include header files
#include <stdio.h>  /// for printf() and scanf()

/**
 * @brief a globally declared structure with an array and an variable that
 * points to the topmost index of the array
 */
struct __arith_Stack
{
    char arr[10];  ///> static array of integers
    int tos;       ///> stores index on topmost element in stack
};

// function headers
void __arith_push(struct __arith_Stack *p, char ch);  // pust element in stack
char __arith_pop(struct __arith_Stack *p);            // __arith_pop topmost element from the stack
int __arith_isOprnd(char ch);                 // check if element is operand or not
int __arith_isEmpty(struct __arith_Stack s);          // check if stack is empty
int __arith_getPrecendence (char op1, char op2);        // check operator precedence
void __arith_infix_convert(char infix[],
             char postfix[]);  // convert infix to postfix expression

/**
 * @brief __arith_push function
 * @param *p : used as a pointer variable of stack
 * @param x : char to be __arith_pushed in stack
 * @returns void
 */
void __arith_push(struct __arith_Stack *p, char x)
{
    if (p->tos == 9)  // check if stack has reached its max limit
    {
        printf("Stack Overflow!");
        return;
    }

    p->tos += 1;         // increment tos
    p->arr[p->tos] = x;  // assign char x to index of stack pointed by tos
}

/**
 * @brief __arith_pop function
 * @param *p : used as a pointer variable of stack
 * @returns x or \0 on exit
 */
char __arith_pop(struct __arith_Stack *p)
{
    char x;

    if (p->tos == -1)
    {
        printf("Stack Underflow!");
        return '\0';
    }

    x = p->arr[p->tos];  // assign the value of stack at index tos to x
    p->tos -= 1;         // decrement tos

    return x;
}

/**
 * @brief __arith_isOprnd function
 * @param ch : this is the element from the infix array
 * @returns 1 or 0 on exit
 */
int __arith_isOprnd(char ch)
{
    if ((ch >= 65 && ch <= 90) ||
        (ch >= 97 && ch <= 122) ||  // check if ch is an operator or
        (ch >= 48 && ch <= 57))     // operand using ASCII values
    {
        return 1;  // return for true result
    }
    else
    {
        return 0;  // return for false result
    }
}

/**
 * @brief __arith_isEmpty function
 * @param s : it is the object reference of stack
 * @returns 1 or 0 on exit
 */
int __arith_isEmpty(struct __arith_Stack s)
{
    if (s.tos == -1)  // check if stack is empty
    {
        return 1;  // return for true result
    }
    else
    {
        return 0;  // return for false result
    }
}

/**
 * @brief __arith_infix_convert function
 * @param infix[] : infix array provided by user
 * @param postfix[] : empty array to be given to __arith_infix_convert()
 * @returns postfixed expresion or \0 on exit
 */
void __arith_infix_convert(char infix[], char postfix[])
{
    struct __arith_Stack s;  // initialze object reference of stack
    s.tos = -1;      // initalize the tos

    int i, j = 0, pr;
    char ch, temp;

    for (i = 0; infix[i] != '\0'; i++)
    {
        ch = infix[i];

        if (__arith_isOprnd(ch) == 1)  // check if char is operand or operator
        {
            postfix[j] = ch;  // assign ch to postfix array with index j
            j++;              // incement j
        }
        else
        {
            if (ch == '(')
            {
                __arith_push(&s, ch);
            }
            else
            {
                if (ch == ')')
                {
                    while ((temp = __arith_pop(&s)) != '(')
                    {
                        postfix[j] = temp;
                        j++;
                    }
                }
                else
                {
                    while (__arith_isEmpty(s) == 0)  // check if stack is empty
                    {
                        pr = __arith_getPrecendence (ch,
                                   s.arr[s.tos]);  // check operator precedence

                        if (pr == 1)
                        {
                            break;  // if ch has a greater precedence than
                                    // s.arr[s.top]
                        }

                        postfix[j] = __arith_pop(&s);
                        j++;
                    }

                    __arith_push(&s, ch);  // __arith_push ch to stack
                }
            }
        }
    }

    while (__arith_isEmpty(s) == 0)  // check if stack is empty
    {
        postfix[j] = __arith_pop(&s);
        j++;
    }

    postfix[j] = '\0';
}

/**
 * @brief __arith_getPrecendence function returns the precedence after comparing two operators passed as parameter.
 * @param op1 : first operator
 * @param op2 : second operator
 * @returns 1 or 0 on exit
 */
int __arith_getPrecendence (char op1, char op2)
{
    if (op2 == '$')
    {
        return 0;
    }
    else if (op1 == '$')
    {
        return 1;
    }
    else if (op2 == '*' || op2 == '/' || op2 == '%')
    {
        return 0;
    }
    else if (op1 == '*' || op1 == '/' || op1 == '%')
    {
        return 1;
    }
    else if (op2 == '+' || op2 == '-')
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

#endif