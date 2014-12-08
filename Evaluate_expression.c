/* 
 * File:   main.c
 * Author: Maksim
 *
 * Created on December 4, 2014, 10:40 PM
 */

#define M_E 2.71828182845904523536
#define M_PI 3.14159265358979323846

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "math.h"

/*
 * 
 */
 
// evaluate1 calculate the value of operation involving 1 arguments
float evaluate1(char *result_stack, int arg_count, char operator, float *arguments, bool radians) {
    float arg_right = 0, result = 0;
    char i1, i2, index_str[2];
    
    i1 = result_stack[(arg_count - 1)*2];
    i2 = result_stack[((arg_count - 1)*2)+1];
    sprintf(index_str, "%c%c", i1, i2);    
    
    //printf("\n index_str: %s \n", index_str);
    
    arg_right = arguments[atoi(index_str)];
                
    switch (operator) {
        case 'f': // log 10
            result = log10(arg_right);
            break;
        case 'l': // ln
            result = log(arg_right);
            break;
        case 'd': // arc sine
            // degrees or radians?
            result = radians ? asin(arg_right) : (asin(arg_right)* 180 / M_PI);
            break;
        case 'b': // arc cos
            result = radians ? acos(arg_right) : (acos(arg_right)* 180 / M_PI);
            break;
        case 'a': // arc tan
            result = radians ? atan(arg_right) : (atan(arg_right)* 180 / M_PI);
            break;
        case 't': // tan
            result = radians ? tan(arg_right) : tan(arg_right * M_PI / 180);
            break;
        case 'q': // sqrt
            result = sqrt(arg_right);
            break;
        case 'c': // cosine
            result = radians ? cos(arg_right) : cos(arg_right * M_PI / 180);
            break;
        case 's': // sine
            result = radians ? sin(arg_right) : sin(arg_right * M_PI / 180);
            break;
        case '~': // negate highest priority
        case '!': // negate low priority
            result = -arg_right;
            break;    
        default:
            break;
    }
        
    // Put the result of calculation into the float array
    arguments[atoi(index_str)] = result;
    
    result_stack -= 2;
    *result_stack = index_str[0];
    result_stack++;
    *result_stack = index_str[1];
    result_stack++;
    
    return result;
}

// evaluate2 calculate the value of operation involving 2 arguments
float evaluate2(char *result_stack, int arg_count, char operator, float *arguments) {
    float arg_left = 0, arg_right = 0, result = 0;
    char i1, i2, index_str[2];
    
    i1 = result_stack[(arg_count - 1)*2];
    i2 = result_stack[((arg_count - 1)*2)+1];
    sprintf(index_str, "%c%c", i1, i2);    
    
    arg_right = arguments[atoi(index_str)];
    
    i1 = result_stack[(arg_count - 2)*2];
    i2 = result_stack[((arg_count - 2)*2)+1];
    sprintf(index_str, "%c%c", i1, i2);    
    
    arg_left = arguments[atoi(index_str)];
            
    switch (operator) {
        case '^':
            result = pow(arg_left, arg_right);
            break;    
        case '+':
            result = arg_left + arg_right;
            break;
        case '-':
            result = arg_left - arg_right;
            break;
        case '*':
            result = arg_left * arg_right;
            break;
        case '/':
            result = arg_left / arg_right;
            break;
        default:
            break;
    }
       
    // Put the result of calculation into the float array
    arguments[atoi(index_str)] = result;
    
    result_stack -= 4;
    *result_stack = index_str[0];
    result_stack++;
    *result_stack = index_str[1];
    result_stack++;
    
    return result;
}

float get_constant_value(char c) {
    switch (c) {
			  //case 'p':
        case 247:
        return M_PI;
        case 'e':
        return M_E;
        default:
        return 0;
    }
}

bool is_constant(char c) { // Check if 'c' is a constant
    switch (c) {
        //case 'p':
        case 247:
        case 'e': // Base of natural logarithm
        return true;
        
        default:
        return false;
    }
}

bool is_operator(char c) { // Check if 'c' is an operator
    switch (c) {
        case 'f': // log10
        case 'l': // ln
        case 'd': // arc sin
        case 'b': // arc cos
        case 'a': // arc tan
        case 't': // tan
        case 'q': // sqrt
        case 'c': // cosine
        case 's': // sine
        case '~': // highest priority negate
        case '!': // just regular negate
        case '^':
        case '+':
        case '-':
        case '/':
        case '*':
        return true;
        
        default:
        return false;
    }
}

char get_operator_for_a_function(char *function) {
       
    if (strcmp(function, "sin") == 0) {
        return 's';
    }
    
    if (strcmp(function, "cos") == 0) {
        return 'c';
    }
    
    if (strcmp(function, "sqrt") == 0) {
        return 'q';
    }
    
    if (strcmp(function, "tan") == 0) {
        return 't';
    }
    
    if (strcmp(function, "atan") == 0) {
        return 'a';
    }
    
    if (strcmp(function, "acos") == 0) {
        return 'b';
    }
    
    if (strcmp(function, "asin") == 0) {
        return 'd';
    }
    
    if (strcmp(function, "ln") == 0) {
        return 'l';
    }
    
    if (strcmp(function, "log") == 0) {
        return 'f';
    }
    
    return 0;
        
    
}

bool is_number(char c) { // Check if 'c' is a variable (including decimal point)
    if (((c >= '0') && (c <= '9')) || (c == '.')) 
        return true;
    else 
        return false;
}

bool is_letter(char c) {
    if ((c >= 'a') && (c <= 'z')) 
        return true;
    else 
        return false;
}

char operator_priority(char op) { // Check the priority of the operator
    switch (op) {
        case 'f': // log
        case 'l': // ln
        case 'd': // asin
        case 'b': // acos
        case 'a': // atan
        case 't': // tan
        case 's': // sine
        case 'c': // cosine
        case 'q': // sqrt
        return 4;
        case '~': // highest priority negate
        return 3;
        case '^': 
        return 2;
        case '!': // just regular negate
        case '*':
        case '/':
        return 1; // higher level of priority
        case '+':
        case '-':
        return 0; // low level of priority
        default:
        return -1; // unknown priority
    }
}

char operator_args(char op) { // How many arguments per operator
    switch (op) {
        case '~':
        case '!':
        case 'f': // log
        case 'l': // ln
        case 'd': // asin
        case 'b': // acos
        case 'a': // atan
        case 't': // tan
        case 's': // sine
        case 'c': // cosine
        case 'q': // sqrt
        return 1;
        case '*':
        case '^':
        case '/':
        case '+':
        case '-':
        return 2;
        default:
        return -1;
    }
}

bool shunting_yard(char *input, char *output) {
        
    char c, op_stack[100];
    int o_size = 0; // size of the operator stack (op_stack)
    bool brackets_ok = false;
    
    while (*input) {
               
        c = *input;
        
         // if number put it into output stack
        if (is_number(c)) {
        
            *output = c; // write into the current "index"
            output++; // increase "index"
        
        } else if (is_operator(c)) {
            
            // while operator stack is not empty
            while (o_size > 0) {
                // if priority of the previous operator is higher or equal to the current one
                // put it into the output stack, ignore brackets '(' and ')'
                if (is_operator(c) && (operator_priority(op_stack[o_size-1])) >= operator_priority(c)) {
                    *output = op_stack[o_size-1];
                    output++;
                    o_size--;
                } else {
                    break;
                }
            }
            
            op_stack[o_size] = c;
            o_size++;
            
        } else if (c == '(') {
            
            // just put it into the operator stack
            op_stack[o_size] = c;
            o_size++;
        
        } else if (c == ')') {
            
            // Go through every operator until you find '('
            while (o_size > 0) {
                
                brackets_ok = false;
                
                if (op_stack[o_size-1] == '(') {
                    
                    brackets_ok = true;
                    break;
                    
                } else {
                    
                    // add the operator to the output stack
                    *output = op_stack[o_size-1];
                    output++;
                    o_size--;
                    
                }
                
            }
            // bracket was not closed
            if (!brackets_ok) return false; 
        
            // if we are here then
            // everything is ok, but there is a bracket left in the operator stack
            // get rid of it
            o_size--;
            
            
        } else {   
            // uknown input
            return false;
            
        }
                      
        input++;
    }
    
    // If operators are left in the stack move them to the output stack

    while (o_size > 0) {

        // There should be any brackets left in the operator stack
        if ((op_stack[o_size-1] == ')') || (op_stack[o_size-1] == '(')) {
            // if there is one
            return false;
        }
            
        *output = op_stack[o_size-1];
        o_size--;
        output++;

    }
    
    *output = 0; // Because output is an array, 0 defines the end of it
    
    return true;
    
}

// replaces numbers in input with indexes of the array (arguments)
bool process_input(char *input, char *input_processed, float *arguments) {
    
    char temp_number[50], c, arg_index_str[2], temp_function[10], op_for_function;
    int i = 0, j = 0, cur_arg = 0, f = 0;
    bool opening_bracket = false;
    bool closing_bracket = false;
    bool decimal_point = false;
    // i is a digit index of the number 
		
    while (*input) {
        
        c = *input;
        // if number
        
        // check if there is a function (cos, sin, etc.)
        if (is_constant(c)) {
            
            // convert temp_number into float and add it to arguments
            *arguments = get_constant_value(c);
            
            //printf("\n Test %f: \n", *arguments);
            
            arguments++;
            // convert argument index into string
            sprintf(arg_index_str, "%02d", cur_arg);
            
            j = 0;
            while(arg_index_str[j]) {            
                *input_processed = arg_index_str[j];          
                input_processed++;
                j++;
            }
                  
            // add current c (it is definitely an operator or bracket)      
            //*input_processed = c;
            //input_processed++;
            // increase number of arguments
            cur_arg++;
            
        } else if (is_letter(c)) {
            temp_function[f] = c;
            f++;
        } else if (f > 0) {
            // there must be an opening bracket after the function
            if (c != '(') return false;
            
            // terminate temp_function
            temp_function[f] = 0;
            
            op_for_function = get_operator_for_a_function(temp_function);
            
            if (op_for_function) {
                // add the operator to the processed input
                *input_processed = op_for_function;
                input_processed++;
            } else {
                return false; // no such function
            }
            *input_processed = c;
            input_processed++;
            // reset f for a next potential function
            f = 0;
        } else if (is_number(c)) {
            temp_number[i] = c;
            i++;
            // Check for multiple decimal points in a row
            if ((c == '.') && (!decimal_point)) {
                decimal_point = true;
            } else if ((c == '.') && (decimal_point)) {
                // there was a decimal point in this number before
                // invalid input
                return false;
            }
        // if something other than number, but previously there was a function
        } else if (i > 0) {
            // reset decimal point for a next number
            decimal_point = false;            
            // termination, because this variable might have been used before
            temp_number[i] = 0; 
            // convert temp_number into float and add it to arguments
            *arguments = atof(temp_number);
            arguments++;
            // convert argument index into string
            sprintf(arg_index_str, "%02d", cur_arg);
            
            j = 0;
            while(arg_index_str[j]) {            
                *input_processed = arg_index_str[j];          
                input_processed++;
                j++;
            }
                       
            //add current c (it is definitely an operator or bracket)      
            *input_processed = c;
            input_processed++;
            // reset i for the next number
            i = 0;
            //
            cur_arg++;
        // if after the operator or bracket there is not a number
        } else {
            
            if (c == '-') {
                if ((opening_bracket) || (cur_arg == 0)) *input_processed = '!'; // negate function (less priority)
                else if (closing_bracket) *input_processed = c;
                else *input_processed = '~'; // ~negate function
                input_processed++;
                
            } else if (c == '+') {
                if (closing_bracket) {
                    *input_processed = c;
                    input_processed++;
                }
           
            } else {
                *input_processed = c;
                input_processed++;
            }
            
        }
        
        if (c == '(') opening_bracket = true;
        else opening_bracket = false;
        
        if (c == ')') closing_bracket = true;
        else closing_bracket = false;
        
        input++;
    }
    
    // if there is a temp_number left
    if (i > 0) {
        // termination, because this variable might have been used before
        temp_number[i] = 0; 
        // convert temp_number into float and add it to arguments
        *arguments = atof(temp_number);
        arguments++;
        
        // convert argument index into string
        sprintf(arg_index_str, "%02d", cur_arg);

        j = 0;
        while(arg_index_str[j]) {            
            *input_processed = arg_index_str[j];          
            input_processed++;
            j++;
        }
    }
    
    // Termination
    *input_processed = 0;
		
    return true;
}

bool calculate(char *input, char *result, float *arguments, bool radians) {
    char result_stack[50], needed_args = 0;
    int i = 0, arg_count = 0;
    volatile float my_result = 0;
    
    while (*input) {
        
        if (is_number(*input)) {
            // Put the index of number to the result stack
            result_stack[i] = *input;
            input++;
            i++;
            result_stack[i] = *input;
            input++;
            i++;
            
            arg_count++;
                        
        } else if (is_operator(*input)) {
            
            // i = 2 when there is 1 argument
            
            // There must at least n (needed_args) arguments to apply an operation
            needed_args = operator_args(*input);
            
            //printf("\n %c needed_args = %d,  arg_coung = %d ", *input, needed_args, arg_count);
            
            if (arg_count < needed_args) return false; 
            //arg_count is a current number of arguments
            if (needed_args == 2) {
                my_result = evaluate2(result_stack, arg_count, *input, arguments);
                arg_count -= 1;
                i -= 2;
            } else if (needed_args == 1) {
                my_result = evaluate1(result_stack, arg_count, *input, arguments, radians);
            }
            
            //printf("intermittant result: %f \n", my_result);
            result_stack[i] = 0;
            input++;   
        }
    }
    
    // if everything goes well result_stack should have only one value
    if (arg_count == 1) {
				sprintf(result, "%.4g", arguments[0]);
        return true;
        
    } else {
        return false;
    }
    
}