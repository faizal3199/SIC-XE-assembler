/*Code for pass1*/
#include "utility.cpp" /*conatins all important headers*/
#include "tables.cpp"

using namespace std;

void pass1(){
  ifstream sourceFile;//begin
  ofstream intermediateFile, errorFile;

  sourceFile.open("test_input.txt");
  intermediateFile.open("intermediate_file.txt");
  writeToFile(intermediateFile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tComment");
  errorFile.open("error_file.txt");

  string fileLine;
  string writeData;
  int index=0;

  getline(sourceFile,fileLine);
  while(checkCommentLine(fileLine)){
    getline(sourceFile,fileLine);//read first input line
  }

  bool statusCode;
  string label,opcode,operand,comment;

  readFirstNonWhiteSpace(fileLine,index,statusCode,label);
  readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);

  int startAddress,LOCCTR,lineNumber,lastDeltaLOCCTR;
  lineNumber = 5;
  lastDeltaLOCCTR = 0;

  if(opcode=="START"){
    readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
    readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
    startAddress = stringHexToInt(operand);
    // cout<<startAddress<<endl;
    // exit(0);
    LOCCTR = startAddress;
    writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    writeToFile(intermediateFile,writeData); // Write file to intermediate file

    getline(sourceFile,fileLine); //Read next line
    lineNumber += 5;
    index = 0;
    readFirstNonWhiteSpace(fileLine,index,statusCode,label); //Parse label
    readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);//Parse OPCODE
  }
  else{
    startAddress = 0;
    LOCCTR = 0;
  }

  while(opcode!="END"){
    if(!checkCommentLine(fileLine)){
      if(label!=""){//Label found
        if(SYMTAB[label].exists=='n'){
          SYMTAB[label].name = label;
          SYMTAB[label].address = intToStringHex(LOCCTR);
          SYMTAB[label].exists = 'y';
        }
        else{
          writeData = "Line: "+to_string(lineNumber)+" : Duplicate symbol for '"+label+"'. Previously defined at "+SYMTAB[label].address;
          writeToFile(errorFile,writeData);
        }
      }
      if(OPTAB[getRealOpcode(opcode)].exists=='y'){//Search for opcode in OPTAB
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        if(OPTAB[getRealOpcode(opcode)].format==3){
          LOCCTR += 3;
          lastDeltaLOCCTR += 3;
          if(getOpcodeFormat(opcode)=='+'){
            LOCCTR += 1;
            lastDeltaLOCCTR += 3;
          }
        }
        else{
          LOCCTR += OPTAB[getRealOpcode(opcode)].format;
          lastDeltaLOCCTR += OPTAB[getRealOpcode(opcode)].format;
        }
      }
      else if(opcode == "WORD"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += 3;
        lastDeltaLOCCTR += 3;
      }
      else if(opcode == "RESW"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += 3*stoi(operand);
        lastDeltaLOCCTR += 3*stoi(operand);
      }
      else if(opcode == "RESB"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += stoi(operand);
        lastDeltaLOCCTR += stoi(operand);
      }
      else if(opcode == "BYTE"){
        readByteOperand(fileLine,index,statusCode,operand);
        if(operand[0]=='X'){
          LOCCTR += (operand.length() -3)/2;
          lastDeltaLOCCTR += (operand.length() -3)/2;
        }
        else if(operand[0]=='C'){
          LOCCTR += operand.length() -3;
          lastDeltaLOCCTR += operand.length() -3;
        }
        // else{
        //   writeData = "Line: "+to_string(line)+" : Invalid operand for BYTE. Found " + operand;
        //   writeToFile(errorFile,writeData);
        // }
      }
      else{
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        writeData = "Line: "+to_string(lineNumber)+" : Invalid OPCODE. Found " + opcode;
        writeToFile(errorFile,writeData);
      }
      readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
      writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    }
    else{
      writeData = to_string(lineNumber) + "\t" + fileLine;
    }
    writeToFile(intermediateFile,writeData);

    getline(sourceFile,fileLine); //Read next line
    lineNumber += 5;
    index = 0;
    lastDeltaLOCCTR = 0;
    readFirstNonWhiteSpace(fileLine,index,statusCode,label); //Parse label
    readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);//Parse OPCODE
  }
  readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
  readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
  writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
  writeToFile(intermediateFile,writeData);

  int program_length = LOCCTR - startAddress;
}

int main(){
  load_OPTAB();
  pass1();
  return 0;
}
