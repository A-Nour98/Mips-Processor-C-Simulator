#include<stdio.h>
#include <stdlib.h> 
#include<string.h>
#include <stdbool.h>

int regFile[33];
char mem[2048][sizeof(char)*33];
bool bnef = false;
bool fetched = false;
bool decoded = false;
bool executed = false;
bool memoried = false;
bool flush = false;
int numberofinstructions = 0;

char* onescomplement(char* stringtobeconverted,char* stringtoberturned){
  strcpy(stringtoberturned,stringtobeconverted);
  int i=0;
  while(stringtobeconverted[i] != '\0'){
    if(stringtobeconverted[i] == '1'){
      stringtoberturned[i] = '0';
    }else{
      stringtoberturned[i] = '1';
    }
    i++;
  }
}

char* toBinaryString(int n,int numberofbits) {
  int num_bits = numberofbits;
  char *string = malloc(num_bits + 1);
  if (!string) {
    return NULL;
  }
  for (int i = num_bits - 1; i >= 0; i--) {
    string[i] = (n & 1) + '0';
    n >>= 1;
  }
  string[num_bits] = '\0';
  return string;
}
void  getinstruction(char* ins,int opcodei,int op1i,int op2i,int op3i,int shamti,int immediatei,int addressi){
    if(opcodei == 8 || opcodei == 9){
    op3i = 0;
  }
  char* opcode = toBinaryString(opcodei,4);
  char* op1 = toBinaryString(op1i,5);
  char* op2 = toBinaryString(op2i,5);
  char* op3 = toBinaryString(op3i,5);
  char* shamt = toBinaryString(shamti,13);
  char* immediate = toBinaryString(immediatei,18);
  char* adress = toBinaryString(addressi,28);
  if(opcodei == 0 || opcodei == 1 || opcodei == 8 || opcodei == 9){
  snprintf(ins, sizeof(char)*33, "%s%s%s%s%s", opcode,op1,op2,op3,shamt); // puts string into buffer
  }else if(opcodei == 7){
    snprintf(ins, sizeof(char)*33, "%s%s", opcode,adress); // puts string into buffer
  }else{
    snprintf(ins, sizeof(char)*33, "%s%s%s%s", opcode,op1,op2,immediate); // puts string into buffer
  }

}
int readInstructions(){
  char file_name[] = "test.txt";
  FILE *input = fopen(file_name, "r");
  char operation[5]; // Assuming operations are maximum 3 characters long (ADD, SUB, etc.)
  char operand1[4]; // Assuming operands are maximum 2 characters long (R1, R2, etc.)
  char operand2[4];
  char operand3[5];
  char address[6];
  if( !input ) {
    err(EXIT_FAILURE, "could not open '%s'", file_name);
  }

  static char line[128];
  int n;
  int i = 0;
  int operationi;
  int op1i;
  int op2i;
  int op3i;
  int shamt;
  int immediate;
  int addressi;

  while( fgets(line, sizeof(line), input) != NULL ) {


    if( 4 == sscanf(line, "%s %s %s %s", &operation, &operand1,&operand2,&operand3) ) {
      //printf( "%s %s %s %s\n",operation, operand1, operand2,operand3 );
    } else if( 2 == sscanf(line, "%s %s", &operation, &address) ) {
      //printf( "%s %s\n",operation, address );
    } else {
      //printf( ">>> %s\n", line );
    }
            if(strcmp(operation,"ADD") == 0){
            operationi = 0;
        }else if (strcmp(operation,"SUB") == 0){
            operationi = 1;
        }else if (strcmp(operation,"MULI") == 0){
            operationi = 2;
        }else if (strcmp(operation,"ADDI") == 0){
            operationi = 3;
        }else if (strcmp(operation,"BNE") == 0){
            operationi = 4;
        }else if (strcmp(operation,"ANDI") == 0){
            operationi = 5;
        }else if (strcmp(operation,"ORI") == 0){
            operationi = 6;
        }else if (strcmp(operation,"J") == 0){
            operationi = 7;
        }else if (strcmp(operation,"SLL") == 0){
            operationi = 8;
        }else if (strcmp(operation,"SRL") == 0){
            operationi = 9;
        }else if (strcmp(operation,"LW") == 0){
            operationi = 10;
        }else if (strcmp(operation,"SW") == 0){
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
        
        shamt = atoi(operand3);
        immediate = atoi(operand3);
        addressi = atoi(address);
        if(strlen(operand3)==2){
          op3i = operand3[1] - '0';
        }else{
          char operand3s[3];
          sprintf(operand3s,"%c%c",operand3[1],operand3[2]);
          op3i = atoi(operand3s);

        }

         char ins[sizeof(char)*33];
         getinstruction(ins,operationi,op1i,op2i,op3i,shamt,immediate,addressi);

         strcpy(mem[i],ins);
         i++;

  }
  numberofinstructions = i;

  if( !feof(input) ) {
    err(EXIT_FAILURE, "error reading %s", file_name);
  }

    return 0;

}
void FetchInstruction(char* ins){
    int pc = regFile[32];
    //printf("%i \n",pc);
    strcpy(ins,mem[pc]);
    if(flush){
      strcpy(ins,"00000000000000000000000000000000");
    }
    regFile[32] += 1;
    
}
 void DecodeInstruction(char* ins,long* decoded){
    char operation[sizeof(char)*5];
    char op1[sizeof(char)*6];
    char op2[sizeof(char)*6];
    char op3[sizeof(char)*6];
    char shamt[sizeof(char)*14];
    char immediate[sizeof(char)*19];
    char immediaten[sizeof(char)*19];
    char address[sizeof(char)*29];
    operation[4] = '\0';
    op1[5] = '\0';
    op2[5] = '\0';
    op3[5] = '\0';
    shamt[13] = '\0';
    immediate[18] = '\0';
    immediaten[18] = '\0';
    address[28] = '\0';
    //char shamt[sizeof(char)*4];
    strncpy(operation,ins,4);
    strncpy(op1,ins+4,5);
    strncpy(op2,ins+9,5);
    strncpy(op3,ins+14,5);
    strncpy(shamt,ins+19,13);
    strncpy(immediate,ins+14,18);
    strncpy(address,ins+4,28);
    char *endptr;
    long int intimmediate;
    long int intoperation = strtol(operation, &endptr, 2);
    long int intop1 = strtol(op1, &endptr, 2);
    long int intop2 = strtol(op2, &endptr, 2);
    long int intop3 = strtol(op3, &endptr, 2);
    long int intshamt = strtol(shamt, &endptr, 2);
    if(immediate[0] == '1'){
      onescomplement(immediate,immediaten);
      intimmediate = -(strtol(immediaten, &endptr, 2))-1;
    }else{
     intimmediate = strtol(immediate, &endptr, 2);
    }
    long int intadress = strtol(address, &endptr, 2);
    decoded[0] = intoperation;
    if(decoded[0] == 4){
      decoded[1] = regFile[intop1];
    }else{
      decoded[1] = intop1;
    }
    decoded[2] = regFile[intop2];
    decoded[3] = regFile[intop3];
    decoded[4] = intshamt;
    decoded[5] = intimmediate;
    decoded[6] = intadress;
    if(flush){
      for(int f=0;f<7;f++){
        decoded[f] = 0;
      }
      intop1 = 0;
    }
    printf("decoded fields: \n");
    printf("opcode: %i, operand 1: R%i, operand 2: R%i, operand 3: R%i, shift: %i, immediate: %i, address: %i\n",decoded[0],intop1,intop2,intop3,decoded[4],decoded[5],decoded[6]);
    

 }
void JandBranchexecute(int finalresult){
  regFile[32] = finalresult;
  flush = false;
}
long ExecuteInstruction(long decodedins[],int PC){
  long finalresult;
  printf("execution parameters and final result: \n");
  if(decodedins[0] == 0){
    finalresult = decodedins[2] + decodedins[3];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, operand 3 value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[3],finalresult);
  }else if(decodedins[0] == 1){
    finalresult = decodedins[2] - decodedins[3];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, operand 3 value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[3],finalresult);
  }else if(decodedins[0] == 2){
    finalresult = decodedins[2] * decodedins[5];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, immediate value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[5],finalresult);
  }else if(decodedins[0] == 3 || decodedins[0] == 10 || decodedins[0] == 11){
    finalresult = decodedins[2] + decodedins[5];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, immediate value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[5],finalresult);
  }else if(decodedins[0] == 4){
    if(decodedins[1] != decodedins[2]){
      finalresult = PC + 1 + decodedins[5];
      //regFile[32] = finalresult;
      printf("address = %i \n",finalresult);
      flush = true;
      printf("opcode: %i, operand 1 value: %i, operand 2 value: %i, immediate value: %i, branch to %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[5],finalresult);
    }else{
      printf("opcode: %i, operand 1 value: %i, operand 2 value: %i, immediate value: %i, no branching \n",decodedins[0],decodedins[1],decodedins[2],decodedins[5]);
    }
  }else if(decodedins[0] == 5){
    finalresult = decodedins[2] & decodedins[5];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, immediate value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[5],finalresult);
  }else if(decodedins[0] == 6){
    finalresult = decodedins[2] | decodedins[5];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, immediate value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[5],finalresult);

  }else if(decodedins[0] == 7){
    char* adressimmediate = toBinaryString(decodedins[6],28);
    char* pcvalue = toBinaryString(regFile[32],32);
    char* finalresultst[sizeof(char)*33];
    char pcpart[sizeof(char)*5];
    pcpart[4] = '\0';
    strncpy(pcpart,pcvalue,4);
    snprintf(finalresultst, sizeof(char)*33, "%s%s", pcpart,adressimmediate);
    char* endptr;
    finalresult = strtol(finalresultst, &endptr, 2);
    //regFile[32] = finalresult;
    flush = true;
    printf("opcode: %i, jump to address: %i \n",decodedins[0],finalresult);


  }else if(decodedins[0] == 8){
    unsigned int numbertobeshifted = decodedins[2];
    finalresult = numbertobeshifted << decodedins[4];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, shift value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[4],finalresult);

  }else if(decodedins[0] == 9){
    unsigned int numbertobeshifted = decodedins[2];
    finalresult = numbertobeshifted >> decodedins[4];
    printf("opcode: %i, operand 1 destination: R%i, operand 2 value: %i, shift value: %i, final result: %i \n",decodedins[0],decodedins[1],decodedins[2],decodedins[4],finalresult);

  }
  return finalresult;
}
void  Writeback(long decodedins[],long finalresult,int memdata){
  //printf("decoded = %i \n",decodedins[0]);
  if(decodedins[0] != 4 && decodedins[0] != 7 && decodedins[0] != 11){
  if(decodedins[1] == 0){
    finalresult =0;
  }if(decodedins[0] == 10){
    finalresult = memdata;
  }
  regFile[decodedins[1]] = finalresult;
  printf("register changed : \n");
  printf("R%i ====> %i \n",decodedins[1],finalresult);
    //printf("regnumber = %i \n",decodedins[1]);
  }
}
int Memory(long decodedins[],long finalresult){
  int memdata =0;
  if(decodedins[0] == 10){
    char* endptr;
    memdata = strtol(mem[finalresult],&endptr,2);
    printf("memory loaded value : %i \n",memdata);
  }else if(decodedins[0] == 11){
    char* binaryval = toBinaryString(regFile[decodedins[1]],32);
    strcpy(mem[finalresult],binaryval);
    printf("Memory data changed : \n");
    printf("Mem[%i] ====> %i \n",finalresult,regFile[decodedins[1]]);
  }
  return memdata;
}
int main(){
// regFile[33] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0},{10,0},{11,0},{12,0},{13,0},{14,0},{15,0},{16,0},{17,0},{18,0},{19,0},{20,0},{21,0},{22,0},{23,0},{24,0},{25,0},{26,0},{27,0},{28,0},{29,0},{30,0},{31,0},{32,0}}
readInstructions();
int totalnumberofcycles = 7+((numberofinstructions-1)*2);
char ins[sizeof(char)*33];
long decodedin[7];
long currexec[7];
long currmemwb[7];
long finalresult;
long currresult;
int memdata = 0;
int dco = 0;
int eco = 0;
int clkcycle = 1;
int fetched = 0;
int decoded = 0;
int executed = 0;
int memoried = 0;
int writtenback = 0;
int tobeexec = regFile[32];
char currdec[sizeof(char)*33];
bool readyfordecode = false;
bool readyforexecute = false;
bool finishedprogram = false;
bool terminatecode = false;
bool readyforMem = false;
bool readyforWB = false;
while(!terminatecode){
  finishedprogram = strcmp(mem[regFile[32]],"") == 0;
  printf("clock cycle: %i \n",clkcycle);
    if(readyforWB){
    writtenback = memoried;
    printf("instruction %i in WriteBack \n",writtenback);
    Writeback(currmemwb,currresult,memdata);
    readyforWB = false;
    if(writtenback == fetched){
        terminatecode = true;
    }
    
  }
  if(readyforMem  && clkcycle %2 ==0){
    for (int i = 0; i < 7; i++) { 
        currmemwb[i] = currexec[i]; 
    } 
    currresult = finalresult;
    memoried = executed;
    printf("instruction %i in memory stage \n",memoried);
    memdata = Memory(currmemwb,currresult);
    readyforMem = false;
    readyforWB = true;
    
  }
  if(readyforexecute){
    if(eco ==0){
    for (int i = 0; i < 7; i++) { 
        currexec[i] = decodedin[i]; 
    } 
    executed = decoded;
    printf("executing instruction %i \n",executed);
    }else{
      printf("executed instruction %i:  \n",executed);
      finalresult = ExecuteInstruction(currexec,executed-1);
    }
    eco++;
    if(eco == 2){
      eco =0;
      readyforMem = true;
      readyforexecute = false;
    }
    
  }
  if(readyfordecode){
    if(dco ==0){
    strcpy(currdec,ins);
    decoded = fetched;
    printf("decoding instruction %i \n",decoded);
    }else{
      printf("decoded instruction %i:  \n",decoded);
      DecodeInstruction(currdec,decodedin);
    }
    dco++;
    if(dco == 2){
      dco =0;
      readyforexecute = true;
      readyfordecode = false;
    }
    
  }
  if(clkcycle %2 != 0 && !finishedprogram){
  FetchInstruction(ins);
  fetched = regFile[32];
  printf("fetched instruction %i: %s \n",fetched,ins);
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
for(int i=0;i<33;i++){
  if(i == 32){
    printf("R%i (PC): %i \n",i,regFile[i]);
  }else{
    printf("R%i: %i \n",i,regFile[i]);
  }
  
}





}

// char ins[sizeof(char)*33];
// FetchInstruction(ins);
// long decodedin[7];
// DecodeInstruction(ins,decodedin);
// long result = ExecuteInstruction(decodedin);
// Memory(decodedin,result);
// Writeback(decodedin,result);