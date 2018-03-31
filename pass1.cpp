/*Code for pass1*/
#include "utility.cpp" /*conatins all important headers*/
#include "tables.cpp"

using namespace std;

/*Variable to keep persisted*/
bool error_flag=false;
int program_length;

void handle_LTORG(string& litPrefix, int& lineNumberDelta,int lineNumber,int& LOCCTR, int& lastDeltaLOCCTR){
  string litAddress,litValue;
  litPrefix = "";
  for(auto const& it: LITTAB){
    litAddress = it.second.address;
    litValue = it.second.value;
    if(litAddress!="?"){
      /*Pass as already address is assigned*/
    }
    else{
      lineNumber += 5;
      lineNumberDelta += 5;
      LITTAB[it.first].address = intToStringHex(LOCCTR);
      litPrefix += "\n" + to_string(lineNumber) + "\t" + intToStringHex(LOCCTR) + "\t" + "*" + "\t" + "="+litValue + "\t" + " " + "\t" + " ";

      if(litValue[0]=='X'){
        LOCCTR += (litValue.length() -3)/2;
        lastDeltaLOCCTR += (litValue.length() -3)/2;
      }
      else if(litValue[0]=='C'){
        LOCCTR += litValue.length() -3;
        lastDeltaLOCCTR += litValue.length() -3;
      }
    }
  }
}
void pass1(){
  ifstream sourceFile;//begin
  ofstream intermediateFile, errorFile;

  sourceFile.open("test_input.txt");
  intermediateFile.open("intermediate_file.txt");
  writeToFile(intermediateFile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tComment");
  errorFile.open("error_file.txt");
  writeToFile(errorFile,"************PASS1************");

  string fileLine;
  string writeData,writeDataSuffix="",writeDataPrefix="";
  int index=0;

  bool statusCode;
  string label,opcode,operand,comment;
  string tempOperand;

  int startAddress,LOCCTR,saveLOCCTR,lineNumber,lastDeltaLOCCTR,lineNumberDelta=0;
  lineNumber = 0;
  lastDeltaLOCCTR = 0;

  getline(sourceFile,fileLine);
  lineNumber += 5;
  while(checkCommentLine(fileLine)){
    writeData = to_string(lineNumber) + "\t" + fileLine;
    writeToFile(intermediateFile,writeData);
    getline(sourceFile,fileLine);//read first input line
    lineNumber += 5;
    index = 0;
  }

  readFirstNonWhiteSpace(fileLine,index,statusCode,label);
  readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);

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
          error_flag = true;
        }
      }
      if(OPTAB[getRealOpcode(opcode)].exists=='y'){//Search for opcode in OPTAB
        if(OPTAB[getRealOpcode(opcode)].format==3){
          LOCCTR += 3;
          lastDeltaLOCCTR += 3;
          if(getFlagFormat(opcode)=='+'){
            LOCCTR += 1;
            lastDeltaLOCCTR += 1;
          }
          if(getRealOpcode(opcode)=="RSUB"){
            operand = " ";
          }
          else{
            readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
            if(operand[operand.length()-1] == ','){
              readFirstNonWhiteSpace(fileLine,index,statusCode,tempOperand);
              operand += tempOperand;
            }
          }

          if(getFlagFormat(operand)=='='){
            tempOperand = operand.substr(1,operand.length()-1);
            if(tempOperand=="*"){
              tempOperand = "X'" + intToStringHex(LOCCTR-lastDeltaLOCCTR,6) + "'";
            }
            if(LITTAB[tempOperand].exists=='n'){
              LITTAB[tempOperand].value = tempOperand;
              LITTAB[tempOperand].exists = 'y';
              LITTAB[tempOperand].address = "?";
            }
          }
        }
        else if(OPTAB[getRealOpcode(opcode)].format==1){
          operand = " ";
          LOCCTR += OPTAB[getRealOpcode(opcode)].format;
          lastDeltaLOCCTR += OPTAB[getRealOpcode(opcode)].format;
        }
        else{
          LOCCTR += OPTAB[getRealOpcode(opcode)].format;
          lastDeltaLOCCTR += OPTAB[getRealOpcode(opcode)].format;
          readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
          if(operand[operand.length()-1] == ','){
            readFirstNonWhiteSpace(fileLine,index,statusCode,tempOperand);
            operand += tempOperand;
          }
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
      else if(opcode=="BASE"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
      }
      else if(opcode=="LTORG"){
        operand = " ";
        handle_LTORG(writeDataSuffix,lineNumberDelta,lineNumber,LOCCTR,lastDeltaLOCCTR);
      }
      else if(opcode=="ORG"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        int tempVariable;
        tempVariable = saveLOCCTR;
        saveLOCCTR = LOCCTR;
        LOCCTR = tempVariable;

        if(SYMTAB[operand].exists=='y'){
          LOCCTR = stringHexToInt(SYMTAB[operand].address);
        }
      }
      else{
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        writeData = "Line: "+to_string(lineNumber)+" : Invalid OPCODE. Found " + opcode;
        writeToFile(errorFile,writeData);
        error_flag = true;
      }
      readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
      writeData = writeDataPrefix + to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
      writeDataPrefix = "";
      writeDataSuffix = "";
    }
    else{
      writeData = to_string(lineNumber) + "\t" + fileLine;
    }
    writeToFile(intermediateFile,writeData);

    getline(sourceFile,fileLine); //Read next line
    lineNumber += 5 + lineNumberDelta;
    lineNumberDelta = 0;
    index = 0;
    lastDeltaLOCCTR = 0;
    readFirstNonWhiteSpace(fileLine,index,statusCode,label); //Parse label
    readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);//Parse OPCODE
  }
  readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
  readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);

  handle_LTORG(writeDataSuffix,lineNumberDelta,lineNumber,LOCCTR,lastDeltaLOCCTR);

  writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
  writeToFile(intermediateFile,writeData);

  program_length = LOCCTR - startAddress;

  sourceFile.close();
  intermediateFile.close();
  errorFile.close();
}
