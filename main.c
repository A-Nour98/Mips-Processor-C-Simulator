#include<stdio.h>
#include <stdlib.h> 
#include<string.h>

int parseReg(char * input){
int i, result = 0;
 
    for (i = 1; input[i] != '\0'; i++) {
        result = result * 10 + (input[i] - '0');
    }
    return result;
}
int parse(char op[],char r1[], char r2[], char r3[]){


}
int readInstructions(){
    FILE *file_pointer;
    char file_name[] = "test.txt";
    char operation[4]; // Assuming operations are maximum 3 characters long (ADD, SUB, etc.)
    char operand1[3]; // Assuming operands are maximum 2 characters long (R1, R2, etc.)
    char operand2[3];
    char operand3[3];

    // Open the file in read mode
    file_pointer = fopen(file_name, "r");

    // Check if file opened successfully
    if (file_pointer == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    // Read and process the contents of the file line by line
    while (fscanf(file_pointer, "%s %s %s %s", operation, operand1, operand2, operand3) != EOF) {
        printf("Operation: %s, Operand1: %s, Operand2: %s, Operand3: %s\n", operation, operand1, operand2, operand3);
    }

    // Close the file
    fclose(file_pointer);

    return 0;

}



int main(){
// regFile[33] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0},{10,0},{11,0},{12,0},{13,0},{14,0},{15,0},{16,0},{17,0},{18,0},{19,0},{20,0},{21,0},{22,0},{23,0},{24,0},{25,0},{26,0},{27,0},{28,0},{29,0},{30,0},{31,0},{32,0}}
int regFile[33];
int mem[2048];
readInstructions();



}