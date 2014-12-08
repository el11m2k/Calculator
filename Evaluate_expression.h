/* 
 * File:   main.h
 * Author: Maksim
 *
 * Created on December 7, 2014, 4:25 PM
 */
 
 #include "stdbool.h"

float evaluate1(char *result_stack, int arg_count, char operator, float *arguments, bool radians);
float evaluate2(char *result_stack, int arg_count, char operator, float *arguments);
float get_constant_value(char c);
bool is_constant(char c);
bool is_operator(char c);
char get_operator_for_a_function(char *function);
bool is_number(char c);
bool is_letter(char c);
char operator_priority(char op);
char operator_args(char op);



bool calculate(char *input, char *result, float *arguments, bool radians);
bool process_input(char *input, char *input_processed, float *arguments);
bool shunting_yard(char *input, char *output);


