#include<stdio.h>
#include <stdlib.h> 
#include<string.h>
int regFile[31];

int mem[2048]={0};
const int R0=0;
int pc=0;
int currinstructionintxt=0;
struct fetch f;
struct decode dec;
struct execute exec;
struct memory m;
int cycles=1;
int parseReg(char * input){
int i, result = 0;
 
    for (i = 1; input[i] != '\0'; i++) {
        result = result * 10 + (input[i] - '0');
    }
    return result;
}
int parse(char op[],char r1[], char r2[], char r3[]){



}
char* intToBinaryString(int num) {
    // Calculate the maximum length needed for the binary representation
    int maxLength = sizeof(int) * 8 + 1; // Size of integer in bits + 1 for null terminator
    // Allocate memory for the binary string
    char* binaryString = (char*)malloc(maxLength * sizeof(char));
    if (binaryString == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Iterate over each bit of the integer and set the corresponding bit in the binary string
    for (int i = 0; i < sizeof(int) * 8; i++) {
        int mask = 1 << (sizeof(int) * 8 - 1 - i); // Create a mask with a single bit set
        binaryString[i] = (num & mask) ? '1' : '0'; // Check if the bit is set and assign '1' or '0'
    }
    
    // Add null terminator to the end of the string
    binaryString[sizeof(int) * 8] = '\0';
    
    return binaryString;
}
struct fetch{
int currentinstruction;
int pc;
};
struct decode{
    unsigned int opcode;
    int jumpaddress;
    int r1;
    int r2;
    int rdestvalue;
    int rdest; //destination register
    int imm;
    int shamt;
    int pc;
};

struct execute{
    int result;
    int opcode;
    int rdest;
    int imm;
};
struct memory{
    int memoutput;
    int opcode;
    int rdest;
    int imm;
    int result;
};

void fetch(){
if(cycles%2!=0){
    f.pc=pc;
    if(mem[pc]!=0){
    f.currentinstruction=mem[pc];
    pc++;
    }else{
    f.currentinstruction=NULL;
    }
    

}
}
void decode(){
   
    if(cycles%2==0){
        int instr=f.currentinstruction;
        dec.pc=f.pc;
        if(instr!=NULL){
           dec.opcode=(unsigned int)(instr>>28);
           dec.opcode=((0b00000000000000000000000000001111)&dec.opcode);                  
           printf("opcode is %i\n",dec.opcode);
           printf("opcode is %s\n",intToBinaryString(dec.opcode));
           dec.rdest=(int)((instr&(0b00001111100000000000000000000000))>>23);
           //printf("rdest is %d\n",dec.rdest);
           dec.rdestvalue=regFile[dec.rdest];
           int r1=(int)((instr&(0b00000000011111000000000000000000))>>18);
           dec.r1=regFile[(int)((instr&(0b00000000011111000000000000000000))>>18)];
           //printf("r1 is %d\n",dec.r1);
          int r2=(int)((instr&(0b00000000011111000000000000000000))>>13);
          
           dec.r2=regFile[(int)((instr&(0b00000000000000111110000000000000))>>13)];
           dec.shamt= (int) (instr & (0b00000000000000000001111111111111));
           dec.imm=(int) (instr & (0b00000000000000111111111111111111));
           //if(dec.opcode==4){
            //printf("r1 is %d\n",rdest);
           // printf("r2 is %d\n",r1);
            //printf("r1 value  is %d\n",r1);
           //}
           dec.jumpaddress=(int)(instr& (0b00001111111111111111111111111111));
           int signOfIMD =  ((instr & (0b00000000000000100000000000000000)) >> 17);
           if (signOfIMD == 1) {
                        dec.imm |= 0b11111111111111000000000000000000;
                    }
                    printf("x is %d\n",dec.imm);
           printf("binary is %s\n",intToBinaryString(dec.imm));
        }
    }

}
void execute(){
if(cycles%2==0 && cycles>=4){
   
    if(dec.opcode==0){ //add
        exec.opcode=0;
        exec.result=dec.r1+dec.r2;
    }else if (dec.opcode==1) //sub
    {
        exec.opcode=1;
        exec.result=dec.r1-dec.r2;
    }else if (dec.opcode==2) //muli
    {
        exec.opcode=2;
     exec.result=dec.r1*dec.imm;   
    }else if (dec.opcode==3)
    { //addi
    exec.opcode=3;
        exec.result=dec.r1+dec.imm;
    }else if (dec.opcode==4){ //bneq
             exec.opcode=4;
            if(dec.rdestvalue!=dec.r1){
                 printf("pc before=%d\n",dec.pc);
                pc=dec.pc+1+dec.imm;
                printf("pc after=%d\n",pc);
                dec.opcode=NULL;
                f.currentinstruction=NULL;
            }
    }else if (dec.opcode==5)
    { //andi
    exec.opcode=5;
        exec.result=dec.r1&dec.imm;
    }else if(dec.opcode==6){
        exec.opcode=6; //ori
        exec.result=dec.r1|dec.imm;
    }else if(dec.opcode==7){ //jump
        exec.opcode=7;
        pc=(pc>>28)|dec.jumpaddress;
        f.currentinstruction=NULL;
        dec.opcode=NULL;
        dec.opcode=NULL;

        printf("Jump address= %d\n",pc);
    }else if(dec.opcode==8){//shift left
        exec.opcode=8;
        exec.result=dec.r1<<dec.shamt;
    }else if(dec.opcode==9){//shift right
        exec.opcode=9;
        printf("shift right before=%d\n",dec.r1);
        exec.result=(unsigned int)dec.r1>>dec.shamt;
        printf("shift right after=%d\n",exec.result);

        
    }else //LW,SW
    {
        if(dec.opcode==10){
        exec.opcode=10;
        exec.result=dec.r1+dec.imm;
    }else if(dec.opcode==11){
        exec.opcode=11;
        exec.result=dec.r1+dec.imm;
    }
        
        printf("ALU is %d\n",exec.result);
    }
    
     exec.imm=dec.imm;
    exec.rdest=dec.rdest;
}

    
    
}
void memory(){
    if(cycles%2==0 && cycles>5 ){
       
         if(exec.opcode==10){
              m.memoutput=mem[exec.result];
              printf("memout is %d\n",m.memoutput);
         }else if (exec.opcode==11)
         {
            mem[exec.result]=regFile[exec.rdest];
            printf("exec.rdest=%d and mem[%d]=%d",exec.rdest,exec.result,mem[exec.result]);
                      
         }
          m.opcode=exec.opcode;
        m.imm=exec.imm;
        m.rdest=exec.rdest;
        m.result=exec.result;
         
         
    }
}
void writeback(){
    if(cycles%2!=0 && cycles>=7){
    if(m.opcode==10){
        if(m.rdest!=0)
        regFile[m.rdest]=m.memoutput;
        printf("LW REG %d =%d\n",m.rdest,regFile[m.rdest] );
    }else if (!(m.opcode==4 || m.opcode==7 || m.opcode==11))
    {
        if(m.rdest!=0)
         regFile[m.rdest]=m.result;
         printf("ALU REG %d =%d\n",exec.rdest,regFile[exec.rdest] );
    }else if (m.opcode==7){
       // printf("pc=%d",pc);

        //pc=m.result;
        //printf("pc= %d\n",pc);
        //printf("cycles=%d\n",cycles);
       // printf("fetch.opcode=%d",);
    }else if (m.opcode==4){
        //if(m.result==0){
          //  pc=pc+1+m.imm;
        //}
    }
    }
    

}


void encode(char * operation, char * operand1, char * operand2, char * operand3, char* address){
    int instruction=0;
if(strcmp(operation,"ADD")==0){
instruction|=0<<28;
instruction|=parseReg(operand1)<<23;
instruction|=parseReg(operand2)<<18;
instruction|=parseReg(operand3)<<13;

}else if (strcmp(operation,"SUB")==0)
{
    instruction|=1<<28;
instruction|=parseReg(operand1)<<23;
instruction|=parseReg(operand2)<<18;
instruction|=parseReg(operand3)<<13;
}else if (strcmp(operation,"MULI")==0)
{
 instruction|=2<<28;
instruction|=parseReg(operand1)<<23;
instruction|=parseReg(operand2)<<18;
 instruction|=atoi(operand3); 
}else if (strcmp(operation,"ADDI")==0)
{
    instruction|=3<<28;
instruction|=parseReg(operand1)<<23;
//printf("%s\n",intToBinaryString(instruction));
instruction|=parseReg(operand2)<<18;
//printf("%s\n",intToBinaryString(instruction));

 int immtmp=(atoi(operand3) &(0b00000000000000111111111111111111));
instruction|=immtmp;
 //printf("imm=%s\n",intToBinaryString(atoi(operand3)));
//printf("%s\n",intToBinaryString(instruction));

}
else if (strcmp(operation,"BNE")==0)
{
     instruction|=4<<28;
instruction|=parseReg(operand1)<<23;
instruction|=parseReg(operand2)<<18;
 instruction|=atoi(operand3); 
    /* code */
}
else if (strcmp(operation,"ANDI")==0)
{
    /* code */
    instruction|=5<<28;
instruction|=parseReg(operand1)<<23;
instruction|=parseReg(operand2)<<18;
 int immtmp=(atoi(operand3) &(0b00000000000000111111111111111111));
instruction|=immtmp;

}else if (strcmp(operation,"ORI")==0)
{
     instruction|=6<<28;
instruction|=parseReg(operand1)<<23;
instruction|=parseReg(operand2)<<18;
  int immtmp=(atoi(operand3) &(0b00000000000000111111111111111111));
instruction|=immtmp;

}else if (strcmp(operation,"J")==0)
{
    instruction|=7<<28;
    instruction|=atoi(address);

}else if (strcmp(operation,"SLL")==0)
{
     instruction|=8<<28;
     instruction|=parseReg(operand1)<<23;
    instruction|=parseReg(operand2)<<18;
     instruction|=atoi(operand3);

}else if (strcmp(operation,"SRL")==0)
{
     instruction|=9<<28;
     instruction|=parseReg(operand1)<<23;
    instruction|=parseReg(operand2)<<18;
     instruction|=atoi(operand3);


}
else if (strcmp(operation,"LW")==0)
{
     instruction|=10<<28;
     instruction|=parseReg(operand1)<<23;
     instruction|=parseReg(operand2)<<18;
     instruction|=atoi(operand3);

}
else if (strcmp(operation,"SW")==0)
{
     instruction|=11<<28;
     instruction|=parseReg(operand1)<<23;
     instruction|=parseReg(operand2)<<18;
     instruction|=atoi(operand3);

}



mem[currinstructionintxt]=instruction;

printf("Instruction=%s\n",intToBinaryString(mem[currinstructionintxt]));
currinstructionintxt++;


}




int readInstructions(){
    
    char file_name[] = "test.txt";
    char operation[5]; // Assuming operations are maximum 3 characters long (ADD, SUB, etc.)
    char operand1[4]; // Assuming operands are maximum 2 characters long (R1, R2, etc.)
    char operand2[4];
    char operand3[5];
     char address[6];
    static char line[128];


    // Open the file in read mode
    FILE *file_pointer  = fopen(file_name, "r");

    // Check if file opened successfully
    if (file_pointer == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    // Read and process the contents of the file line by line
   // while (fscanf(file_pointer, "%s %s %s %s", operation, operand1, operand2, operand3) != EOF) {
     //   printf("Operation: %s, Operand1: %s, Operand2: %s, Operand3: %s\n", operation, operand1, operand2, operand3);
    
       // encode(operation,operand1,operand2,operand3);     
              
   // }
    if( !file_pointer ) {
    err(EXIT_FAILURE, "could not open '%s'", file_name);
  }
     while( fgets(line, sizeof(line), file_pointer) != NULL ) {


    if( 4 == sscanf(line, "%s %s %s %s", &operation, &operand1,&operand2,&operand3) ) {
      printf( "%s %s %s %s\n",operation, operand1, operand2,operand3 );
      encode(operation,operand1,operand2,operand3,address);
    } else if( 2 == sscanf(line, "%s %s", &operation, &address) ) {
      printf( "%s %s\n",operation, address );
      encode(operation,operand1,operand2,operand3,address);
    } else {
      //printf( ">>> %s\n", line );
    }
     
     }
    



    // Close the file
    fclose(file_pointer);

    return 0;
     }

// Function to convert a long integer to binary string






int main(){
 //regFile[33] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0},{10,0},{11,0},{12,0},{13,0},{14,0},{15,0},{16,0},{17,0},{18,0},{19,0},{20,0},{21,0},{22,0},{23,0},{24,0},{25,0},{26,0},{27,0},{28,0},{29,0},{30,0},{31,0},{32,0}}
regFile[5]=0;
regFile[25]=45;
regFile[31]=8;
regFile[18]=86;
mem[19]=7;


readInstructions();



//printf("%d",parseReg("R18"));
//readInstructions();
int maxcycles=7+((currinstructionintxt-1)*2);
printf("Register 31 is %d\n",regFile[31]);
while (cycles<=maxcycles)
{
    writeback();
    memory();
    execute();
    decode();
    fetch();
    if(cycles==7 || cycles==25 || cycles==27 || cycles==29 || cycles==31){
    printf("R0=%d\n",regFile[0]);
    }if(cycles==9){
       printf("R1=%d\n",regFile[1]);
    }
    if(cycles==11){
        printf("R2=%d\n",regFile[2]);
    }
     if(cycles==13){
        printf("R3=%d\n",regFile[3]);
    }
    if(cycles==15){
        printf("R4=%d\n",regFile[4]);
    }
    if(cycles==17){
        printf("R5=%d\n",regFile[5]);
    }
    if(cycles==19){
           printf("R6=%d\n",regFile[6]);
    }
    if(cycles==21){
        printf("R7=%d\n",regFile[7]);
    }if(cycles==23){
        printf("R31=%d\n",regFile[31]);
    }
    if(cycles==33)
    printf("R8=%d\n",regFile[8]);
    if(cycles==35)
    printf("R9=%d\n",regFile[9]);
    if(cycles==37)
    printf("R10=%d\n",regFile[10]);
    if(cycles==39)
    printf("R11=%d\n",regFile[11]);
    if(cycles==41)
    printf("R12=%d\n",regFile[12]);
    if(cycles==43)
    printf("R13=%d\n",regFile[13]);
    if(cycles==45)
    printf("R14=%d\n",regFile[14]);
    if(cycles==46)
    printf("Mem[1024]=%d\n",mem[1024]);
     if(cycles==48)
    printf("Mem[1025]=%d\n",mem[1025]);
     if(cycles==50)
    printf("Mem[1026]=%d\n",mem[1026]);
     if(cycles==52)
    printf("Mem[1027]=%d\n",mem[1027]);
     if(cycles==55)
    printf("R15=%d\n",regFile[15]);

    
    cycles++;
}
//printf("Register 31 is %d\n",regFile[31]);
//printf("Register 7 is %d\n",regFile[7]);
//printf("Mem[19]=%d",mem[19+1024]);
//printf("R4=%d\n",regFile[4]);
//printf("R5=%d\n",regFile[5]);

printf("cycles=%d\n",cycles-1);



return 0;


}