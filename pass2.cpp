/*Code for pass2*/

#include "pass1.cpp"

using namespace std;

/*Declaring variables in global space*/
ifstream intermediateFile;
ofstream errorFile,objectFile,ListingFile;

bool isComment;
string label,opcode,operand,comment;
string operand1,operand2;

int lineNumber,blockNumber,address,startAddress;
string objectCode, writeData, currentRecord, modificationRecord, endRecord;

int program_counter, base_register_value, currentTextRecordLength;
bool nobase;
/*Declaration ends*/

string readTillTab(string data,int& index){
  string tempBuffer = "";

  while(index<data.length() && data[index] != '\t'){
    tempBuffer += data[index];
    index++;
  }
  index++;
  return tempBuffer;
}
bool readIntermediateFile(ifstream& readFile,bool& isComment, int& lineNumber, int& address, int& blockNumber, string& label, string& opcode, string& operand, string& comment){
  string fileLine="";
  string tempBuffer="";
  bool tempStatus;
  int index=0;
  if(!getline(readFile, fileLine)){
    return false;
  }
  lineNumber = stoi(readTillTab(fileLine,index));

  isComment = (fileLine[index]=='.')?true:false;
  if(isComment){
    readFirstNonWhiteSpace(fileLine,index,tempStatus,comment,true);
    return true;
  }
  address = stringHexToInt(readTillTab(fileLine,index));
  tempBuffer = readTillTab(fileLine,index);
  if(tempBuffer == " "){
    blockNumber = -1;
  }
  else{
    blockNumber = stoi(tempBuffer);
  }
  label = readTillTab(fileLine,index);
  opcode = readTillTab(fileLine,index);
  if(opcode=="BYTE"){
    readByteOperand(fileLine,index,tempStatus,operand);
  }
  else{
    operand = readTillTab(fileLine,index);
  }
  readFirstNonWhiteSpace(fileLine,index,tempStatus,comment,true);
  return true;
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

    string tempOperand = operand.substr(1,operand.length()-1);
    if(if_all_num(tempOperand)||((SYMTAB[tempOperand].exists=='y')&&SYMTAB[tempOperand].relative==0)){//Imidiate integer value
      int immediateValue;

      if(if_all_num(tempOperand)){
        immediateValue = stoi(tempOperand);
      }
      else{
        immediateValue = stringHexToInt(SYMTAB[tempOperand].address);
      }
      /*Process Immediate value*/
      if(immediateValue>=(1<<4*halfBytes)){//Can't fit it
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
    else if(SYMTAB[tempOperand].exists=='n') {
      writeData = "Line "+to_string(lineNumber);
      writeData += " : Symbol doesn't exists. Found " + tempOperand;
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }
    else{
      int operandAddress = stringHexToInt(SYMTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOperand].blockNumber]].startAddress);

      /*Process Immediate symbol value*/
      if(halfBytes==5){/*If format 4*/
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
        objcode += '1';
        objcode += intToStringHex(operandAddress,halfBytes);

        /*add modifacation record here*/
        modificationRecord += "M^" + intToStringHex(address+1,6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }

      /*Handle format 3*/
      program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
      program_counter += (halfBytes==5)?4:3;
      int relativeAddress = operandAddress - program_counter;

      /*Try PC based*/
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
        objcode += '2';
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      /*Try base based*/
      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
          objcode += '4';
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }

      /*Use direct addressing with no PC or base*/
      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+1,2);
        objcode += '0';
        objcode += intToStringHex(operandAddress,halfBytes);

        /*add modifacation record here*/
        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
  }
  else if(getFlagFormat(operand)=='@'){
    string tempOperand = operand.substr(1,operand.length()-1);
    if(tempOperand.substr(tempOperand.length()-2,2)==",X" || SYMTAB[tempOperand].exists=='n'){//Error handling for Indirect with index based
      writeData = "Line "+to_string(lineNumber);
      writeData += (SYMTAB[tempOperand].exists=='n')?": Symbol doesn't exists":" Index based addressing not supported with Indirect addressing";
      writeToFile(errorFile,writeData);
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
      objcode += (halfBytes==5)?"100000":"0000";
      return objcode;
    }

    int operandAddress = stringHexToInt(SYMTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOperand].blockNumber]].startAddress);
    program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

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

      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
        objcode += '0';
        objcode += intToStringHex(operandAddress,halfBytes);

        /*add modifacation record here*/
        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{//No base or pc based addressing in format 4
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
      objcode += '1';
      objcode += intToStringHex(operandAddress,halfBytes);

      /*add modifacation record here*/
      modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: "+to_string(lineNumber);
    writeData += "Can't fit into program counter based or base register based addressing.";
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+2,2);
    objcode += (halfBytes==5)?"100000":"0000";

    return objcode;
  }
  else if(getFlagFormat(operand)=='='){//Literals
    string tempOperand = operand.substr(1,operand.length()-1);

    if(tempOperand=="*"){
      tempOperand = "X'" + intToStringHex(address,6) + "'";
      /*Add modification record for value created by operand `*` */
      modificationRecord += "M^" + intToStringHex(stringHexToInt(LITTAB[tempOperand].address)+stringHexToInt(BLOCKS[BLocksNumToName[LITTAB[tempOperand].blockNumber]].startAddress),6) + '^';
      modificationRecord += intToStringHex(6,2);
      modificationRecord += '\n';
    }

    if(LITTAB[tempOperand].exists=='n'){
      writeData = "Line "+to_string(lineNumber)+" : Symbol doesn't exists. Found " + tempOperand;
      writeToFile(errorFile,writeData);

      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
      objcode += (halfBytes==5)?"000":"0";
      objcode += "000";
      return objcode;
    }

    int operandAddress = stringHexToInt(LITTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[LITTAB[tempOperand].blockNumber]].startAddress);
    program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

    if(halfBytes==3){
      int relativeAddress = operandAddress - program_counter;
      if(relativeAddress>=(-2048) && relativeAddress<=2047){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
        objcode += '2';
        objcode += intToStringHex(relativeAddress,halfBytes);
        return objcode;
      }

      if(!nobase){
        relativeAddress = operandAddress - base_register_value;
        if(relativeAddress>=0 && relativeAddress<=4095){
          objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
          objcode += '4';
          objcode += intToStringHex(relativeAddress,halfBytes);
          return objcode;
        }
      }

      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
        objcode += '0';
        objcode += intToStringHex(operandAddress,halfBytes);

        /*add modifacation record here*/
        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{//No base or pc based addressing in format 4
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
      objcode += '1';
      objcode += intToStringHex(operandAddress,halfBytes);

      /*add modifacation record here*/
      modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: "+to_string(lineNumber);
    writeData += "Can't fit into program counter based or base register based addressing.";
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
    objcode += (halfBytes==5)?"100":"0";
    objcode += "000";

    return objcode;
  }
  else{/*Handle direct addressing*/
    int xbpe=0;
    string tempOperand = operand;
    if(operand.substr(operand.length()-2,2)==",X"){
      tempOperand = operand.substr(0,operand.length()-2);
      xbpe = 8;
    }

    if(SYMTAB[tempOperand].exists=='n'){
      writeData = "Line "+to_string(lineNumber)+" : Symbol doesn't exists. Found " + tempOperand;
      writeToFile(errorFile,writeData);

      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
      objcode += (halfBytes==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
      objcode += "000";
      return objcode;
    }

    int operandAddress = stringHexToInt(SYMTAB[tempOperand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOperand].blockNumber]].startAddress);
    program_counter = address + stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress);
    program_counter += (halfBytes==5)?4:3;

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

      if(operandAddress<=4095){
        objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
        objcode += intToStringHex(xbpe,1);
        objcode += intToStringHex(operandAddress,halfBytes);

        /*add modifacation record here*/
        modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
        modificationRecord += (halfBytes==5)?"05":"03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else{//No base or pc based addressing in format 4
      objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
      objcode += intToStringHex(xbpe+1,1);
      objcode += intToStringHex(operandAddress,halfBytes);

      /*add modifacation record here*/
      modificationRecord += "M^" + intToStringHex(address+1+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      modificationRecord += (halfBytes==5)?"05":"03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: "+to_string(lineNumber);
    writeData += "Can't fit into program counter based or base register based addressing.";
    writeToFile(errorFile,writeData);
    objcode = intToStringHex(stringHexToInt(OPTAB[getRealOpcode(opcode)].opcode)+3,2);
    objcode += (halfBytes==5)?(intToStringHex(xbpe+1,1)+"00"):intToStringHex(xbpe,1);
    objcode += "000";

    return objcode;
  }
}

void writeTextRecord(bool lastRecord=false){
  if(lastRecord){
    if(currentRecord.length()>0){//Write last text record
      writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
      writeToFile(objectFile,writeData);
      currentRecord = "";
    }
    return;
  }
  if(objectCode != ""){
    if(currentRecord.length()==0){
      writeData = "T^" + intToStringHex(address+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      writeToFile(objectFile,writeData,false);
    }
    //What is objectCode length > 60
    if((currentRecord + objectCode).length()>60){
      //Write current record
      writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
      writeToFile(objectFile,writeData);

      //Initialize new text currentRecord
      currentRecord = "";
      writeData = "T^" + intToStringHex(address+stringHexToInt(BLOCKS[BLocksNumToName[blockNumber]].startAddress),6) + '^';
      writeToFile(objectFile,writeData,false);
    }

    currentRecord += objectCode;
  }
  else{
    /*Assembler directive which doesn't result in address genrenation*/
    if(opcode=="START"||opcode=="END"||opcode=="BASE"||opcode=="NOBASE"||opcode=="LTORG"||opcode=="ORG"||opcode=="EQU"){
      /*DO nothing*/
    }
    else{
      //Write current record if exists
      if(currentRecord.length()>0){
        writeData = intToStringHex(currentRecord.length()/2,2) + '^' + currentRecord;
        writeToFile(objectFile,writeData);
      }
      currentRecord = "";
    }
  }
}

void writeEndRecord(bool write=true){
  if(write){
    if(endRecord.length()>0){
      writeToFile(objectFile,endRecord);
    }
    else{
      writeEndRecord(false);
    }
  }
  if(operand==""||operand==" "){//If no operand of END
    endRecord = "E^" + intToStringHex(startAddress,6);
  }
  else{//Make operand on end firstExecutableAddress
    int firstExecutableAddress;
    if(SYMTAB[operand].exists=='n'){
      firstExecutableAddress = startAddress;
      writeData = "Line "+to_string(lineNumber)+" : Symbol doesn't exists. Found " + operand;
      writeToFile(errorFile,writeData);
    }
    else{
      firstExecutableAddress = stringHexToInt(SYMTAB[operand].address);
    }
    endRecord = "E^" + intToStringHex(firstExecutableAddress,6);
  }
}

void pass2(){
  string tempBuffer;
  intermediateFile.open("intermediate_"+fileName);//begin
  if(!intermediateFile){
    cout<<"Unable to open file: intermediate_"<<fileName<<endl;
    exit(1);
  }
  getline(intermediateFile, tempBuffer); // Discard heading line

  objectFile.open("object_"+fileName);
  if(!objectFile){
    cout<<"Unable to open file: object_"<<fileName<<endl;
    exit(1);
  }

  ListingFile.open("listing_"+fileName);
  if(!ListingFile){
    cout<<"Unable to open file: listing_"<<fileName<<endl;
    exit(1);
  }
  writeToFile(ListingFile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tObjectCode\tComment");

  errorFile.open("error_"+fileName,fstream::app);
  if(!errorFile){
    cout<<"Unable to open file: error_"<<fileName<<endl;
    exit(1);
  }
  writeToFile(errorFile,"\n\n************PASS2************");

  /*Intialize global variables*/
  objectCode = "";
  currentTextRecordLength=0;
  currentRecord = "";
  modificationRecord = "";
  blockNumber = 0;
  nobase = true;

  readIntermediateFile(intermediateFile,isComment,lineNumber,address,blockNumber,label,opcode,operand,comment);
  while(isComment){//Handle with previous comments
    writeData = to_string(lineNumber) + "\t" + comment;
    writeToFile(ListingFile,writeData);
    readIntermediateFile(intermediateFile,isComment,lineNumber,address,blockNumber,label,opcode,operand,comment);
  }

  if(opcode=="START"){
    startAddress = address;
    writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + to_string(blockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
    writeToFile(ListingFile,writeData);
  }
  else{
    label = "";
    startAddress = 0;
    address = 0;
  }

  writeData = "H^"+expandString(label,6,' ',true)+'^'+intToStringHex(address,6)+'^'+intToStringHex(program_length,6);
  writeToFile(objectFile,writeData);

  readIntermediateFile(intermediateFile,isComment,lineNumber,address,blockNumber,label,opcode,operand,comment);
  currentTextRecordLength  = 0;

  while(opcode!="END"){
    if(!isComment){
      if(OPTAB[getRealOpcode(opcode)].exists=='y'){
        if(OPTAB[getRealOpcode(opcode)].format==1){
          objectCode = OPTAB[getRealOpcode(opcode)].opcode;
        }
        else if(OPTAB[getRealOpcode(opcode)].format==2){
          operand1 = operand.substr(0,operand.find(','));
          operand2 = operand.substr(operand.find(',')+1,operand.length()-operand.find(',') -1 );

          if(operand2==operand){//If not two operand i.e. a
            if(getRealOpcode(opcode)=="SVC"){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + intToStringHex(stoi(operand1),1) + '0';
            }
            else if(REGTAB[operand1].exists=='y'){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + REGTAB[operand1].num + '0';
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
            }
            else if(getRealOpcode(opcode)=="SHIFTR" || getRealOpcode(opcode)=="SHIFTL"){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + REGTAB[operand1].num + intToStringHex(stoi(operand2),1);
            }
            else if(REGTAB[operand2].exists=='n'){
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + "00";
              writeData = "Line: "+to_string(lineNumber)+" Invalid Register name";
              writeToFile(errorFile,writeData);
            }
            else{
              objectCode = OPTAB[getRealOpcode(opcode)].opcode + REGTAB[operand1].num + REGTAB[operand2].num;
            }
          }
        }
        else if(OPTAB[getRealOpcode(opcode)].format==3){
          if(getRealOpcode(opcode)=="RSUB"){
            objectCode = OPTAB[getRealOpcode(opcode)].opcode;
            objectCode += (getFlagFormat(opcode)=='+')?"000000":"0000";
          }
          else{
            objectCode = createObjectCodeFormat34();
          }
        }
      }//If opcode in optab
      else if(opcode=="BYTE"){
        if(operand[0]=='X'){
          objectCode = operand.substr(2,operand.length()-3);
        }
        else if(operand[0]=='C'){
          objectCode = stringToHexString(operand.substr(2,operand.length()-3));
        }
      }
      else if(label=="*"){
        if(opcode[1]=='C'){
          objectCode = stringToHexString(opcode.substr(3,opcode.length()-4));
        }
        else if(opcode[1]=='X'){
          objectCode = opcode.substr(3,opcode.length()-4);
        }
      }
      else if(opcode=="WORD"){
        objectCode = intToStringHex(stoi(operand),6);
      }
      else if(opcode=="BASE"){
        if(SYMTAB[operand].exists=='y'){
          base_register_value = stringHexToInt(SYMTAB[operand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[operand].blockNumber]].startAddress);
          nobase = false;
        }
        else{
          writeData = "Line "+to_string(lineNumber)+" : Symbol doesn't exists. Found " + operand;
          writeToFile(errorFile,writeData);
        }
        objectCode = "";
      }
      else if(opcode=="NOBASE"){
        if(nobase){//check if assembler was using base addressing
          writeData = "Line "+to_string(lineNumber)+": Assembler wasn't using base addressing";
          writeToFile(errorFile,writeData);
        }
        else{
          nobase = true;
        }
        objectCode = "";
      }
      else{
        objectCode = "";
      }
      //Write to text record if any generated
      writeTextRecord();

      if(blockNumber==-1){
        writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
      }
      else{
        writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + to_string(blockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
      }
    }//if not comment
    else{
      writeData = to_string(lineNumber) + "\t" + comment;
    }
    writeToFile(ListingFile,writeData);//Write listing line
    readIntermediateFile(intermediateFile,isComment,lineNumber,address,blockNumber,label,opcode,operand,comment);//Read next line
    objectCode = "";
  }//while opcode not end
  writeTextRecord();

  //Save end record
  writeEndRecord(false);
  writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + "" +"\t" + comment;
  writeToFile(ListingFile,writeData);

  while(readIntermediateFile(intermediateFile,isComment,lineNumber,address,blockNumber,label,opcode,operand,comment)){
    if(label=="*"){
      if(opcode[1]=='C'){
        objectCode = stringToHexString(opcode.substr(3,opcode.length()-4));
      }
      else if(opcode[1]=='X'){
        objectCode = opcode.substr(3,opcode.length()-4);
      }
      writeTextRecord();
    }
    writeData = to_string(lineNumber) + "\t" + intToStringHex(address) + "\t" + to_string(blockNumber) + label + "\t" + opcode + "\t" + operand + "\t" + objectCode +"\t" + comment;
    writeToFile(ListingFile,writeData);
  }
  writeTextRecord(true);
  writeToFile(objectFile,modificationRecord,false);//Write modification record
  writeEndRecord(true);//Write end record
}//Function end

/*TODO
1)LTORG with *
2)EQU
3)Expressions
*/

int main(){
  cout<<"****Input file and executable(assembler.out) should be in same folder****"<<endl<<endl;
  cout<<"Enter name of input file:";
  cin>>fileName;

  cout<<"\nLoading OPTAB"<<endl;
  load_tables();

  cout<<"\nPerforming PASS1"<<endl;
  cout<<"Writing intermediate file to 'intermediate_"<<fileName<<"'"<<endl;
  cout<<"Writing error file to 'error_"<<fileName<<"'"<<endl;
  pass1();

  cout<<"\nPerforming PASS2"<<endl;
  cout<<"Writing object file to 'object_"<<fileName<<"'"<<endl;
  cout<<"Writing listing file to 'listing_"<<fileName<<"'"<<endl;
  pass2();

}
