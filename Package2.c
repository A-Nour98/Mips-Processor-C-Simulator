#include<stdio.h>
#include <stdlib.h> 
#include<string.h>
#include <stdbool.h>

signed char regFile[65];
unsigned short InstructionMem[1024];
signed char DataMem[2048];
bool bnef = false;
bool fetched = false;
bool decoded = false;
bool executed = false;
bool memoried = false;
bool flush = false;
int numberofinstructions = 0;
unsigned short pc = 0;

signed short signExtend(short number)
{
    signed short ans = number & 0b0000000111111111;
 
    // Checking if sign-bit of number is 0 or 1
    if (number & 0b100000) {
 
        // If number is negative, append
        // leading 1's to the 9-bit sequence
          ans = ans | 0b1111111111000000;
    }
 
    return ans;
}

char checkCarry(unsigned short operand1,unsigned short operand2){
    char carrybit = 0;
    operand1 = operand1 & 0b011111111;
    operand2 = operand2 & 0b011111111 ;
    unsigned short result = operand1 + operand2;
    printf("carry result %i \n",result);
    if(result&0b100000000){

       carrybit = 1;
    }
    return carrybit;
}

char checkOverflow(unsigned short operand1,unsigned short operand2,unsigned short operation){
    char Overflowbit = 0;
    if(operation == 0){
      unsigned short result = operand1 + operand2;
      if((operand1&0b10000000) ==  (operand2&0b10000000)){
       if((operand1&0b10000000)!= (result&0b10000000)){
          Overflowbit = 1;
       }
    }

    }else{
      unsigned short result = operand1 - operand2;
      if((operand1&0b10000000) !=  (operand2&0b10000000)){
       if((operand2&0b10000000)== (result&0b10000000)){
          Overflowbit = 1;
       }
    }

    }

    return Overflowbit;
}

unsigned short  getinstruction(int opcodei,int op1i,int op2i,int immediatei){

  unsigned short instruction = 0;
  instruction = instruction | (opcodei << 12) ;
  instruction = instruction | (op1i << 6) ;

  if(opcodei == 0 || opcodei == 1 || opcodei == 2 || opcodei == 5 ||opcodei == 6 || opcodei == 7){
  instruction = instruction | op2i;
  }else{
    immediatei = immediatei & 0b111111;
    instruction = instruction | immediatei;
  }

  return instruction;

}
int readInstructions(){
  char file_name[] = "test.txt";
  FILE *input = fopen(file_name, "r");
  char operation[5]; // Assuming operations are maximum 3 characters long (ADD, SUB, etc.)
  char operand1[4]; // Assuming operands are maximum 2 characters long (R1, R2, etc.)
  char operand2[4];
  if( !input ) {
    err(EXIT_FAILURE, "could not open '%s'", file_name);
  }

  static char line[128];
  int n;
  int i = 0;
  unsigned short operationi;
  unsigned short op1i;
  unsigned short op2i;
  signed short immediatei;

  while( fgets(line, sizeof(line), input) != NULL ) {


    if( 3 == sscanf(line, "%s %s %s", &operation, &operand1,&operand2) ) {
      //printf( "%s %s %s %s\n",operation, operand1, operand2,operand3 );
    } else {
      //printf( ">>> %s\n", line );
    }
        if(strcmp(operation,"ADD") == 0){
            operationi = 0;
        }else if (strcmp(operation,"SUB") == 0){
            operationi = 1;
        }else if (strcmp(operation,"MUL") == 0){
            operationi = 2;
        }else if (strcmp(operation,"LDI") == 0){
            operationi = 3;
        }else if (strcmp(operation,"BEQZ") == 0){
            operationi = 4;
        }else if (strcmp(operation,"AND") == 0){
            operationi = 5;
        }else if (strcmp(operation,"OR") == 0){
            operationi = 6;
        }else if (strcmp(operation,"JR") == 0){
            operationi = 7;
        }else if (strcmp(operation,"SLC") == 0){
            operationi = 8;
        }else if (strcmp(operation,"SRC") == 0){
            operationi = 9;
        }else if (strcmp(operation,"LB") == 0){
            operationi = 10;
        }else if (strcmp(operation,"SB") == 0){
            operationi = 11;
        }


        if(strlen(operand1)==2){
          op1i = operand1[1] - '0';
        }else{
          char operand1s[3];
          sprintf(operand1s,"%c%c",operand1[1],operand1[2]);
          op1i = atoi(operand1s);

        }
        if(strlen(operand2)==2){
          op2i = operand2[1] - '0';
        }else{
          char operand2s[3];
          sprintf(operand2s,"%c%c",operand2[1],operand2[2]);
          op2i = atoi(operand2s);

        }
        

        immediatei = atoi(operand2);
        

         
         unsigned short ins = getinstruction(operationi,op1i,op2i,immediatei);

         InstructionMem[i] = ins;
         i++;

  }
  numberofinstructions = i;

  if( !feof(input) ) {
    err(EXIT_FAILURE, "error reading %s", file_name);
  }

    return 0;

}

unsigned short FetchInstruction(){
    unsigned short instruction = 0;
    //printf("%i \n",pc);
    instruction = InstructionMem[pc];
    if(flush){
      instruction = 0;
    }
    pc += 1;
    return instruction;
    
}

 void DecodeInstruction(unsigned short ins,signed short* decoded){
    decoded[0] = (ins & 0b1111000000000000) >> 12;
    signed short op2 = (ins & 0b0000000000111111);
    decoded[1] =  (ins & 0b0000111111000000) >> 6;
    decoded[2] = regFile[(ins & 0b0000111111000000) >> 6];
    decoded[3] = regFile[(ins & 0b0000000000111111)];
    decoded[4] = (ins & 0b0000000000111111);

    if(flush){
      for(int f=0;f<5;f++){
        decoded[f] = 0;
      }
    }
    decoded[4] = signExtend(decoded[4]);
    printf("decoded fields: \n");
  if(decoded[0] == 0 || decoded[0] == 1 || decoded[0] == 2 || decoded[0] == 5 ||decoded[0] == 6 || decoded[0] == 7){
    printf("opcode: %hu, operand 1: R%hu, operand 2: R%hu\n",decoded[0],decoded[1],op2);
  }else{
    printf("opcode: %hu, operand 1: R%hu, immediate: %hi\n",decoded[0],decoded[1],decoded[4]);
  }
    

 }
 
void JandBranchexecute(signed short finalresult){
  pc = finalresult;
  flush = false;
}
signed short ExecuteInstruction(unsigned short decodedins[],int PC){
  signed char finalresult;
  regFile[64] = 0;
  unsigned short pcvalue;
  char C;
  char V;
  char N;
  char S;
  char Z;
  printf("execution parameters and final result: \n");
  if(decodedins[0] == 0){
    finalresult = decodedins[2] + decodedins[3];
    regFile[decodedins[1]] = finalresult;
    char C = checkCarry(decodedins[2],decodedins[3]);
    regFile[64] = (regFile[64] | (C<<4));
    char V = checkOverflow(decodedins[2],decodedins[3],decodedins[0]);
    regFile[64] = (regFile[64] | (V<<3));
    N = (finalresult<0) ? 1 : 0;
    regFile[64] = (regFile[64] | (N<<2));
    S = N^V;
    regFile[64] = (regFile[64] | (S<<1));
    Z = (finalresult==0) ? 1 : 0;
    regFile[64] = (regFile[64] | Z);
    printf("opcode: %hu, operand 1 value: %hi, operand 2 value: %hi, R%hu ==> %hi \n",decodedins[0],decodedins[2],decodedins[3],decodedins[1],finalresult);
    printf("Carry Flag: %i,Overflow Flag: %i,Negative Flag: %i,Sign Flag: %i,Zero Flag: %i\n",C,V,N,S,Z);

  }else if(decodedins[0] == 1){
    finalresult = decodedins[2] - decodedins[3];
    regFile[decodedins[1]] = finalresult;
    char V = checkOverflow(decodedins[2],decodedins[3],decodedins[0]);
    regFile[64] = (regFile[64] | (V<<3));
    N = (finalresult<0) ? 1 : 0;
    regFile[64] = (regFile[64] | (N<<2));
    S = N^V;
    regFile[64] = (regFile[64] | (S<<1));
    Z = (finalresult==0) ? 1 : 0;
    regFile[64] = (regFile[64] | Z);
    printf("opcode: %hu, operand 1 value: %hi, operand 2 value: %hi, R%hu ==> %hi \n",decodedins[0],decodedins[2],decodedins[3],decodedins[1],finalresult);
    printf("Overflow Flag: %i,Negative Flag: %i,Sign Flag: %i,Zero Flag: %i\n",V,N,S,Z);

  }else if(decodedins[0] == 2){
    finalresult = decodedins[2] * decodedins[3];
    regFile[decodedins[1]] = finalresult;
    N = (finalresult<0) ? 1 : 0;
    regFile[64] = (regFile[64] | (N<<2));
    Z = (finalresult==0) ? 1 : 0;
    regFile[64] = (regFile[64] | Z);
    printf("opcode: %hu, operand 1 value: %hi, operand 2 value: %hi, R%hu ==> %hi  \n",decodedins[0],decodedins[2],decodedins[3],decodedins[1],finalresult);
    printf("Negative Flag: %i,Zero Flag: %i\n",N,Z);

  }else if(decodedins[0] == 3){
    regFile[decodedins[1]] = decodedins[4];
    printf("opcode: %hu, R%hu ==> %hi \n",decodedins[0],decodedins[1],decodedins[4]);
  }else if(decodedins[0] == 4){
    if(decodedins[2] == 0){
      pcvalue = PC + 1 + decodedins[4];
      //pc = pcvalue;
      flush = true;
      printf("opcode: %hu, operand 1 value: %hi, immediate value: %hu, branch to %hu \n",decodedins[0],decodedins[2],decodedins[4],pcvalue);
    }else{
      printf("opcode: %hu, operand 1 value: %hi, immediate value: %hu, no branching \n",decodedins[0],decodedins[2],decodedins[4]);
    }
  }else if(decodedins[0] == 5){
    finalresult = decodedins[2] & decodedins[3];
    N = (finalresult<0) ? 1 : 0;
    regFile[64] = (regFile[64] | (N<<2));
    Z = (finalresult==0) ? 1 : 0;
    regFile[64] = (regFile[64] | Z);
    regFile[decodedins[1]] = finalresult;
    printf("opcode: %hu, operand 1 value: %hi, operand 2 value: %hi, R%hu ==> %hi  \n",decodedins[0],decodedins[2],decodedins[3],decodedins[1],finalresult);
    printf("Negative Flag: %i,Zero Flag: %i\n",N,Z);

  }else if(decodedins[0] == 6){
    finalresult = decodedins[2] | decodedins[3];
    N = (finalresult<0) ? 1 : 0;
    regFile[64] = (regFile[64] | (N<<2));
    Z = (finalresult==0) ? 1 : 0;
    regFile[64] = (regFile[64] | Z);
    regFile[decodedins[1]] = finalresult;
    printf("opcode: %hu, operand 1 value: %hi, operand 2 value: %hi, R%hu ==> %hi  \n",decodedins[0],decodedins[2],decodedins[3],decodedins[1],finalresult);
    printf("Negative Flag: %i,Zero Flag: %i\n",N,Z);


  }else if(decodedins[0] == 7){
    pcvalue = (decodedins[2]<<8)|(decodedins[3]);
    //pc = pcvalue;
    flush = true;
    printf("opcode: %hu, jump to address: %hu \n",decodedins[0],pcvalue);


  }else if(decodedins[0] == 8){
    finalresult = (decodedins[2]<<decodedins[4])|(decodedins[2]>>(8-decodedins[4]));
    regFile[decodedins[1]] = finalresult;
    N = (finalresult<0) ? 1 : 0;
    regFile[64] = (regFile[64] | (N<<2));
    Z = (finalresult==0) ? 1 : 0;
    regFile[64] = (regFile[64] | Z);
    printf("opcode: %hu, operand value: %hi, immediate: %hi, R%hu ==> %hi  \n",decodedins[0],decodedins[2],decodedins[4],decodedins[1],finalresult);
    printf("Negative Flag: %i,Zero Flag: %i\n",N,Z);


  }else if(decodedins[0] == 9){
    finalresult = (decodedins[2]>>decodedins[4])|(decodedins[2]<<(8-decodedins[4]));
    regFile[decodedins[1]] = finalresult;
    N = (finalresult<0) ? 1 : 0;
    regFile[64] = (regFile[64] | (N<<2));
    Z = (finalresult==0) ? 1 : 0;
    regFile[64] = (regFile[64] | Z);
    printf("opcode: %hu, operand value: %hi, immediate: %hi, R%hu ==> %hi \n",decodedins[0],decodedins[2],decodedins[4],decodedins[1],finalresult);
    printf("Negative Flag: %i,Zero Flag: %i\n",N,Z);


  }else if(decodedins[0] == 10){
    regFile[decodedins[1]]=DataMem[decodedins[4]];
    printf("opcode: %hu, R%hu ==> Mem[%hu]  of value %hi \n",decodedins[0],decodedins[1],decodedins[4],DataMem[decodedins[4]]);


  }else if(decodedins[0] == 11){
    DataMem[decodedins[4]]=regFile[decodedins[1]];
    printf("opcode: %hu, Mem[%hu] ==> R%hu  of value %hi \n",decodedins[0],decodedins[4],decodedins[1],regFile[decodedins[1]]);


  }
  printf("SREG Value: %hu\n",regFile[64]);
  return pcvalue;
}

int main(){
// regFile[33] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0},{10,0},{11,0},{12,0},{13,0},{14,0},{15,0},{16,0},{17,0},{18,0},{19,0},{20,0},{21,0},{22,0},{23,0},{24,0},{25,0},{26,0},{27,0},{28,0},{29,0},{30,0},{31,0},{32,0}}
readInstructions();
unsigned short ins;
signed short decodedin[5];
signed short currexec[5];
signed short finalresult;
int clkcycle = 1;
int fetched = 0;
int decoded = 0;
int executed = 0;
int tobeexec = pc;
unsigned short currdec;
bool readyfordecode = false;
bool readyforexecute = false;
bool finishedprogram = false;
bool terminatecode = false;
printf("\n");
while(!terminatecode){
  finishedprogram = InstructionMem[pc] == 0;
  printf("clock cycle: %i \n",clkcycle);
  if(readyforexecute){
    for (int i = 0; i < 5; i++) { 
        currexec[i] = decodedin[i]; 
    } 
    executed = decoded;
    printf("executed instruction %i:  \n",executed);
    finalresult = ExecuteInstruction(currexec,executed-1);
    readyforexecute = false; 
    if(executed == fetched){
        terminatecode = true;
    } 
  }
  if(readyfordecode){
    currdec = ins;
    decoded = fetched;
    printf("decoded instruction %i:  \n",decoded);
    DecodeInstruction(currdec,decodedin);
    readyforexecute = true;
    readyfordecode = false;  
  }
  if(!finishedprogram){
  ins = FetchInstruction();
  fetched = pc;
  printf("fetched instruction %i: %hu \n",fetched,ins);
  readyfordecode = true;
  }
  if(flush){
    JandBranchexecute(finalresult);
  }
  clkcycle++;
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
}
printf("\n");
printf("registers :\n");
for(int i=0;i<65;i++){
  if(i==64){
    printf("SREG: %i \n",regFile[i]);
  }else{
  printf("R%i: %i \n",i,regFile[i]);
  }
  
}
printf("PC: %i \n",pc);





}