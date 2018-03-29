/*Code for pass2*/

#include "pass1.cpp"

using namespace std;

/*Declaring variables in global space*/
ifstream intermediateFile;
ofstream errorFile,objectFile,ListingFile;

bool isComment;
string label,opcode,operand,comment;
string operand1,operand2;

int lineNumber,address;
string objectCode, writeData, currentRecord, modificationRecord;

int program_counter, base_register_value, currentTextRecordLength;
bool nobase;
/*Declaration ends*/

string readTillTab(string data,int& index){
  string tempBuffer = "";

  while(fileLine[index] != '\t'){
    tempBuffer += fileLine[index];
    index++;
  }
  index++;
  return tempBuffer;
}
void readIntermediateFile(ifstream& readFile,bool& isComment, int& lineNumber, int& address, string& label, string& opcode, string& operand, string& comment){
  string fileLine="";
  string tempBuffer="";
  bool tempStatus;
  int index=0;
  getline(readFile, fileLine);

  lineNumber = stoi(readTillTab(fileLine,index));

  isComment = (fileLine[index]=='.')?true:false;
  if(isComment){
    readFirstNonWhiteSpace(fileLine,index,tempStatus,comment,true);
    return;
  }

  lineNumber = stoi(readTillTab(fileLine,index));
  address = stringHexToInt(readTillTab(fileLine,index));
  label = readTillTab(fileLine,index);
  opcode = readTillTab(fileLine,index);
  if(opcode=='BYTE'){
    readByteOperand(fileLine,index,tempStatus,opcode);
  }
  else{
    operand = readTillTab(fileLine,index);
    if(operand==" "){
      operand = "";
    }
  }
  readFirstNonWhiteSpace(fileLine,index,tempStatus,comment,true)
}

string createObjectCodeFormat34(){
  string objcode;
  int halfBytes;
  halfBytes = (getFlagFormat(opcode)=='+')?5:3;

  if(getFlagFormat(operand)=='#'){//Immediate
    if(operand.substr(operand.length()-2,2)==",X"){//Error handling for Immediate with index based
      writeData = "Line: "+to_string(lineNumber)+" Index based addressing not supported with Indirect addressing";
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }
    int immediateValue = stringHexToInt(operand.substr(1,operand.length()-1));
    if(immediateValue>=(1<<4*halfBytes)){
      writeData = "Line: "+to_string(lineNumber)+" Immediate value exceeds format limit";
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
      objcode += (halfBytes==5)?"100000":"0000";
    }
    else{
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
      objcode += (halfBytes==5)?'1':'0';
      objcode += intToStringHex(immediateValue,halfBytes);
    }
    return objcode;
  }
  else if(getFlagFormat(operand)=='@'){
    string tempOperand = stringHexToInt(operand.substr(1,operand.length()-1));
    if(tempOperand.substr(tempOperand.length()-2,2)==",X" || SYMTAB[tempOperand].exists=='n'){//Error handling for Indirect with index based
      writeData = "Line: "+to_string(lineNumber)
      writeData += (SYMTAB[tempOperand].exists=='n')?" Symbol doesn't exists":" Index based addressing not supported with Indirect addressing";
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }

    int operandAddress = stringHexToInt(SYMTAB[tempOperand].address);
    program_counter = stringHexToInt(address);
    program_counter += (halfBytes==5)4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
        objcode += '2';
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
          objcode += '4';
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }
    }
    else{//No base or pc based addressing in format 4
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
      objcode += '1';
      objcode += intToStringHex(relativeAddress,halfBytes);
      return objcode;
    }

    writeData = "Line: "+to_string(lineNumber)
    writeData += "Can't fit into program counter based or base register based addressing."
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
    objcode += (halfBytes==5)?"100000":"0000";

    return objcode;
  }
  else{
    int xbpe=0;
    string tempOperand = operand;
    if(operand.substr(operand.length()-2,2)==",X"){
      tempOperand = operand.substr(0,operand.length()-2);
      xbpe = 8;
    }

    if(SYMTAB[tempOperand].exists=='n'){
      writeData = "Line: "+to_string(lineNumber)
      writeData += "Symbol doesn't exists."
      writeToFile(errorFile,writeData);

      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
      objcode += (halfBytes==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
      objcode += "000";
      return objcode;
    }

    int operandAddress = stringHexToInt(SYMTAB[tempOperand].address);
    program_counter = stringHexToInt(address);
    program_counter += (halfBytes==5)4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
        objcode += intToStringHex(xbpe+2,1);
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
          objcode += intToStringHex(xbpe+4,1);
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }
    }
    else{//No base or pc based addressing in format 4
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
      objcode += intToStringHex(xbpe+1,1);
      objcode += intToStringHex(relativeAddress,halfBytes);
      return objcode;
    }

    writeData = "Line: "+to_string(lineNumber)
    writeData += "Can't fit into program counter based or base register based addressing."
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
    objcode += (halfBytes==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
    objcode += "000";

    return objcode;
  }
}

void pass2(){
  intermediateFile.open("intermediate_file.txt");//begin
  getline(readFile, fileLine); // Discard heading line
  objectFile.open('object_file.txt');
  ListingFile.open("listing_file.txt");
  writeToFile(listing_file,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tObject Code\tComment");
  errorFile.open("error_file.txt",fstream::app);
  writeToFile(errorFile,"\n\n************PASS2************");

  /*Intialize global variables*/
  objectCode = "";
  currentTextRecordLength=0;
  nobase = true;

  readIntermediateFile(intermediateFile,isComment,lineNumber,address,label,opcode,operand,comment);
  while(isComment){//Handle with previous comments
    readIntermediateFile(intermediateFile,isComment,lineNumber,address,label,opcode,operand,comment);
    writeData = to_string(lineNumber) + "\t" + comment;
    writeToFile(ListingFile,writeData);
  }

  if(opcode=="START"){
    writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment +"\t" + objectCode;
    writeToFile(ListingFile,writeData);
  }
  else{
    label = "";
    address = 0;
  }

  writeData = "H^"+expandString(label,6,' ',true)+'^'+intToStringHex(address,6)+'^'+intToStringHex(program_length,6);
  writeToFile(objectFile,writeData);

  readIntermediateFile(intermediateFile,isComment,lineNumber,address,label,opcode,operand,comment);
  writeToFile(objectFile,"T^",false);
  currentTextRecordLength  = 0;

  while(opcode!="END"){
    if(!isComment){
      if(OPTAB[getRealOpcode(opcode)].exists=='y'){
        if(OPTAB[getRealOpcode(opcode)].format==1){
          objectCode = OPTAB[getRealOpcode(opcode)].opcode;
        }
        else if(OPTAB[getRealOpcode(opcode)].format==2){
          operand1 = operand1.substr(0,s.find(','));
          operand2 = operand2.substr(s.find(',')+1,s.length()-s.find(',') -1 );

          if(operand2==operand){//If not two operand i.e. a
            if(getRealOpcode(opcode)=="SVC"){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + intToStringHex(stoi(operand1),1) + '0';
            }
            else if(REGTAB[operand1].exists=='y'){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + operand1 + '0';
            }
            else{
              objectCode = getRealOpcode(opcode) + '0' + '0';
              writeData = "Line: "+to_string(lineNumber)+" Invalid Register name";
              writeToFile(errorFile,writeData);
            }
          }
          else{//Two operands i.e. a,b
            if(REGTAB[operand1].exists=='n'){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + "00";
              writeData = "Line: "+to_string(lineNumber)+" Invalid Register name";
              writeToFile(errorFile,writeData);
              break;
            }
            if(getRealOpcode(opcode)=="SHIFTR" || getRealOpcode(opcode)=="SHIFTL"){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + operand1 + intToStringHex(stoi(operand2),1);
            }
            else if(REGTAB[operand2].exists=='n'){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + "00";
              writeData = "Line: "+to_string(lineNumber)+" Invalid Register name";
              writeToFile(errorFile,writeData);
              break;
            }
            else{
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + operand1 + operand2;
            }
          }
        }
        else if(OPTAB[getRealOpcode(opcode)].format==3){
          if(getRealOpcode(opcode)=="RSUB"){
            if(getFlagFormat(opcode)=='+'){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + "000000";
            }
            else{
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + "0000";
            }
          }
          else{
            objectCode = createObjectCodeFormat34();
          }
        }
      }//If opcode in optab
    }//if not comment
  }//while not end
}//Function end
