#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdint.h>

int8_t regFile[64];
short int pc = 0;
uint8_t sreg = 0;
short int instructionMemory[1024];
int8_t dataMemory[2048];

//flags
int hlt= 0;
int flush=0;
int clkCycles = 1;

//structs(Pipeline Registers)
struct pipelineFD{
    short int instruction;
};
struct pipelineDE{

int opcode;
int r1;
int r2;
int8_t imm;
int8_t valueR1;
int8_t valueR2;

};

//struct instances
struct pipelineFD FD;
struct pipelineDE DE;


//func declarations
void decode();
void execute();
void updateSREG(int op, int8_t v1, int8_t v2,int8_t result);

//parsing
unsigned short int parseReg(char * input){ 
    char * inputMinusR = input+1;
    unsigned short int result = atoi(inputMinusR);
    return result;
}
int parseImm (char r2[]){
    int result;
    char* endptr;
    result  = strtol(r2,&endptr,10);
    return result&0b00000000000000000000000000111111;

}
short int parse(char op[],char r1[], char r2[]){
    short int instruction = 0;
    unsigned short int r1Index = parseReg(r1);
    //R
    if(strcmp(op, "ADD")==0){
        instruction|=parseReg(r1)<<6;
        instruction|=parseReg(r2);
    }
    //R
    else if(strcmp(op, "SUB")==0) 
    {   instruction|=1<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseReg(r2);
    }
    //R
    else if(strcmp(op, "MUL")==0) 
    {   instruction|=2<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseReg(r2);
    }
    //I
    else if(strcmp(op, "LDI")==0)
    {   instruction|=3<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseImm(r2);
    }
    //I
    else if(strcmp(op, "BEQZ")==0)
    {   instruction|=4<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseImm(r2);
    }
    //R
    else if(strcmp(op, "AND")==0)
    {   instruction|=5<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseReg(r2);
    }
    //R
    else if(strcmp(op, "OR")==0)
    {   instruction|=6<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseReg(r2);
    }
    //R
    else if(strcmp(op, "JR")==0)
    {   instruction|=7<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseReg(r2);
    }
    //I
    else if(strcmp(op, "SLC")==0)
    {   instruction|=8<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseImm(r2);
    }
    //I
    else if(strcmp(op, "SRC")==0)
    {   instruction|=9<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseImm(r2);
    }
    //I
    else if(strcmp(op, "LB")==0)
    {   instruction|=10<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseImm(r2);
    }
    //I
    else if(strcmp(op, "SB")==0)
    {   instruction|=11<<12;
        instruction|=parseReg(r1)<<6;
        instruction|=parseImm(r2);
    }
    return instruction;


}

//file read
int readInstructions(){
    FILE *file_pointer;
    char file_name[] = "test.txt";
    char operation[5]; // Assuming operations are maximum 4 characters long (ADD, SUB, etc.)
    char operand1[4]; // Assuming operands are maximum 3 characters long (R1, R2, etc.)
    char operand2[10];

    // Open the file in read mode
    file_pointer = fopen(file_name, "r");

    // Check if file opened successfully
    if (file_pointer == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    int i = 0;
    // Read and process the contents of the file line by line
    while (fscanf(file_pointer, "%s %s %s ", operation, operand1, operand2) != EOF) {
       // printf("Operation: %s, Operand1: %s, Operand2: %s\n", operation, operand1, operand2);
        instructionMemory[i] = parse( operation, operand1, operand2);
        i++;
    }

    // Close the file
    fclose(file_pointer);

    return 0;
}

//prints
void printRegiters(){
for(int i =0;i<64;i++)
{
    printf("R%d: %d | ", i, regFile[i]);

}
printf("\n") ;
}
void printInstructionMemory(){
    printf("Instruction Memory: [\n");
    for(int i =0;i<50;i++)
    {
        printf("%d: %d | ", i, instructionMemory[i]);

    }
    printf("]\n");

}
void printDataMemory(){
    printf("Data Memory: [\n");
    for(int i =0;i<10;i++)
    {
        printf("%d: %d | ", i, dataMemory[i]);

    }
    printf("]\n");

}

//stages
void fetch() {
    FD.instruction = instructionMemory[pc];
    pc++;
    if(instructionMemory[pc]==0)
        hlt=1;

}
void decode() {
    //printf("instruction to be decoded: %d\n", FD.instruction);
        short int immMask = 0b0000000000100000;
        int opcode = 0; 
        int r1 = 0;      
        int r2 = 0;      
        int8_t imm = 0;     
        
        DE.opcode = (FD.instruction & 0b1111000000000000) >> 12 ;
        DE.r1 =     (FD.instruction & 0b0000111111000000) >> 6 ;
        DE.r2 = (FD.instruction &     0b0000000000111111) ;
        DE.imm = (FD.instruction &    0b0000000000111111);
        if((FD.instruction&immMask)>>5==1) 
            DE.imm|=0b11000000;
        DE.valueR1 = regFile[DE.r1];
        DE.valueR2 = regFile[DE.r2];
       // printf("instruction: %i,opcode : %i, r1: %i, r2: %i, imm: %i, valueR1 : %i, valueR2: %i\n",FD.instruction,DE.opcode,DE.r1,DE.r2, DE.imm,DE.valueR1,DE.valueR2);

}
void execute(){
    sreg = 0;   
    switch(DE.opcode){
        case 0://ADD
            regFile[DE.r1] = DE.valueR1+DE.valueR2;
            updateSREG(DE.opcode, DE.valueR1,DE.valueR2, regFile[DE.r1]);

            //update all flags
            break;
        case 1://SUB
            regFile[DE.r1] = DE.valueR1-DE.valueR2;
            updateSREG(DE.opcode, DE.valueR1,DE.valueR2, regFile[DE.r1]);
            //update all except carry
            break;
        case 2://MUL
            regFile[DE.r1] = DE.valueR1*DE.valueR2;
            updateSREG(DE.opcode, DE.valueR1,DE.valueR2, regFile[DE.r1]);
            //negative and zero
            break;
        case 3://LDI
            regFile[DE.r1] = DE.imm;
            break;
        case 4://BEQZ
            if(regFile[DE.r1]==0){
            pc = pc-1+DE.imm;
            flush =3;
            }
            //check for flush in pipeline
            break;
        case 5://AND
            regFile[DE.r1] = DE.valueR1&DE.valueR2;
            updateSREG(DE.opcode, DE.valueR1,DE.valueR2, regFile[DE.r1]);
            //negative and zero
            break;
        case 6://OR
            regFile[DE.r1] = DE.valueR1|DE.valueR2;
            updateSREG(DE.opcode, DE.valueR1,DE.valueR2, regFile[DE.r1]);
            //negative and zero
            break;
        case 7://JR
            pc = ((short int)DE.valueR1<<8)|DE.valueR2;
            printf("pc: %b\n", pc);
            flush =3;
            break;
        case 8://SLC
            regFile[DE.r1] =  (DE.valueR1 << DE.imm) | (DE.valueR1>>(8-DE.imm));
            updateSREG(DE.opcode, DE.valueR1,DE.valueR2, regFile[DE.r1]);
            //negative and zero
            break;
        case 9://SRC
            regFile[DE.r1] =  (DE.valueR1 >> DE.imm) | (DE.valueR1<<(8-DE.imm));
            updateSREG(DE.opcode, DE.valueR1,DE.valueR2, regFile[DE.r1]);
            //negative and zero
            break;
        case 10://LB
            regFile[DE.r1] = dataMemory[DE.imm];
            break;
        case 11://SB
            dataMemory[DE.imm] = DE.valueR1;
            break;
            
    }

}
void updateCarry(int8_t v1,int8_t v2){
    unsigned short int carrytemp = (uint8_t)v1+(uint8_t)v2; 
    int carry = (( carrytemp & 0x0100 )>>8) == 1;
    sreg |= carry<<4;

}
void updateSREG(int op, int8_t v1, int8_t v2,int8_t result){
    uint8_t temp;
    int overflow=0; 
    int negative = (result&0x80)>>7;
    //negative
    if(negative)
        sreg |= 0b00000100;

        //zero
    if(temp == 0)
        sreg &= 0xFE;
    
    if(op==0){
        updateCarry(v1,v2);
        overflow=(v1 > 0 && v2 > 0 && result < 0) || (v1 < 0 && v2 < 0 && result > 0);
     }
    if(op==1)
        overflow = (v1 > 0 && v2 < 0 && result < 0) || (v1 < 0 && v2 > 0 && result > 0);

    if((op==0)||(op==1)){
        if(overflow==1)
            sreg|=0x08;
        
        //sign
        if((negative^overflow)==1)
            sreg|= 0x02;
        }


}
void printAll(){
        printf("Clock Cycle: %i\n", clkCycles);
        printRegiters();
        printDataMemory();
        printf("sreg: %b\n", sreg);
        printf("\n--------------------------------------------------------------------------------------\n");
}
void mainSim()
{
    while(!hlt){
        if((clkCycles!=1&&clkCycles!=2)&&flush<1)
        execute();

        if(clkCycles!=1&&flush<2)
        decode();
        if(flush<3)
        fetch();
        
        printAll();
        
        if(flush>0){
        flush--;
        }
        clkCycles++;
    }

    execute();
    decode();
    printAll();
    clkCycles++;
    execute();
    printAll();
}
void clearPipline(){
    FD.instruction = 0;
    DE.opcode = DE.r1= DE.r2 = DE.imm= DE.valueR1= DE.valueR2=0;
}
int main(){
readInstructions();
mainSim();
}