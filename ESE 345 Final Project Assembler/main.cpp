//
//  main.cpp
//  ESE 345 Final Project Assembler
//
//  Created by Aaron Knoll on 11/22/18.
//  Copyright © 2018 Aaron Knoll. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <vector>
using namespace std;

void assemble(string[]);
string getInstrFormat (string);
bool doErrorsExistInSource (const char *, vector<vector<string>>&);
bool doErrorsExistInLineOfSource (vector<string>, int);
bool isStrANumber(string);
bool isRegNumInRange(int);
bool isLoadImmFieldPosInRange(int);
bool is16BitSignedImmInRange(int);
bool is4BitUnsignedImmInRange(int);
//void writeBinaryFile(string[]);
void trim(string&);

int main(int argc, const char * argv[]) {
      vector<vector<string>> parsedAssembly;
//      string linesOfAssemblyCode[32];
      if (doErrorsExistInSource (argv[1], parsedAssembly)) {
            return 1;
      }
//      cout << linesOfAssemblyCode[0] << endl;
//      cout << parsedAssembly.size() << endl;
      for (int i = 0; i < parsedAssembly.size(); i++) {
            for (int j = 0; j < parsedAssembly.at(i).size(); j++) {
                  cout << parsedAssembly.at(i).at(j) << " ";
            }
            cout << endl;
//            cout << parsedAssembly.at(0).at(i) << " ";
      }
      
      cout << "No errors found. Assembling...\n";
//      assemble(linesOfAssemblyCode);
      //      cout << inputLine << " " << inputLine.length() << "\n";
      return 0;
}

bool doErrorsExistInSource (const char * filePath, vector<vector<string>> &parsedAssembly){
      cout << filePath << endl;
      bool errorExists = false;
      ifstream inputFileStream(filePath);
      string inputLine;
      int currentSourceCodeLineIndex = 1;
      
//      string linesOfAssemblyCode[32];
      int validAssemblyCodeLineIndex = 0;
      while (getline(inputFileStream, inputLine) && validAssemblyCodeLineIndex < 32) {
            trim(inputLine);
            stringstream ss1(inputLine);
            
            vector<string> assemblyArgs;
            
            string tempArgStorage;
            ss1 >> tempArgStorage;
            assemblyArgs.push_back(tempArgStorage);
            while (getline(ss1, tempArgStorage, ',')) {
                  trim(tempArgStorage);
                  //                  cout << assemblyArgs.size() << endl;
                  //                  if (tempArgStorage.length() > 0) {
                  assemblyArgs.push_back(tempArgStorage);
                  //                  }
            }
      
            if (inputLine.length() > 0) {
                  errorExists = doErrorsExistInLineOfSource (assemblyArgs, currentSourceCodeLineIndex);
//                  linesOfAssemblyCode[validAssemblyCodeLineIndex] = inputLine;
                  vector<string> buildStrVector;
                  for (int i = 0; i < assemblyArgs.size(); i++) {
                        buildStrVector.push_back(assemblyArgs.at(i));
//                        cout << assemblyArgs.at(0) << " ";
                  }
//                  cout << endl;
                  parsedAssembly.push_back(buildStrVector);
//                  cout << "Parsed Assembly Size: " << parsedAssembly.size() << endl;
                  validAssemblyCodeLineIndex++;
            }
            
            currentSourceCodeLineIndex++;
            //            cout << inputLine << " " << inputLine.length() << "\n";
      }
      inputFileStream.close();
      return errorExists;
}

bool doErrorsExistInLineOfSource (vector<string> assemblyArgs, int currentSourceCodeLineIndex) {
      
      bool errorExists = false;
      if (!getInstrFormat(assemblyArgs.at(0)).compare("li")) {
            if (assemblyArgs.size() == 4) {
                  if (assemblyArgs.at(1).at(0) == '$' && isStrANumber(assemblyArgs.at(1).substr(1)) &&
                      isStrANumber(assemblyArgs.at(2)) && isStrANumber(assemblyArgs.at(3))) {
                        
                        int regNum = stoi(assemblyArgs.at(1).substr(1));
                        if (!isRegNumInRange(regNum)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register specified\n";
                              errorExists = true;
                        }
                        
                        int immValue = stoi(assemblyArgs.at(2));
                        if (!is16BitSignedImmInRange(immValue)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": immediate value is out of range\n";
                              errorExists = true;
                        }
                        
                        int loadImmFieldPos = stoi(assemblyArgs.at(3));
                        if (!isLoadImmFieldPosInRange(loadImmFieldPos)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": immediate value field position is out of range\n";
                              errorExists = true;
                        }
                  } else {
                        cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid argument syntax\n";
                        errorExists = true;
                  }
            } else {
                  cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid number of arguments\n";
                  errorExists = true;
            }
      } else if (!getInstrFormat(assemblyArgs.at(0)).compare("r4")) {
            if (assemblyArgs.size() == 5) {
                  if (assemblyArgs.at(1).at(0) == '$' && isStrANumber(assemblyArgs.at(1).substr(1)) &&
                      assemblyArgs.at(2).at(0) == '$' && isStrANumber(assemblyArgs.at(2).substr(1)) &&
                      assemblyArgs.at(3).at(0) == '$' && isStrANumber(assemblyArgs.at(3).substr(1)) &&
                      assemblyArgs.at(4).at(0) == '$' && isStrANumber(assemblyArgs.at(4).substr(1))) {
                        
                        int regNum = stoi(assemblyArgs.at(1).substr(1));
                        if (!isRegNumInRange(regNum)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rd specified\n";
                              errorExists = true;
                        }
                        
                        for (int i = 2; i < 5; i++) {
                              regNum = stoi(assemblyArgs.at(i).substr(1));
                              if (!isRegNumInRange(regNum)) {
                                    cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rs" <<
                                    (i - 1) << " specified\n";
                                    errorExists = true;
                              }
                        }
                  } else {
                        cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid argument syntax\n";
                        errorExists = true;
                  }
            } else {
                  cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid number of arguments\n";
                  errorExists = true;
            }
      } else if (!getInstrFormat(assemblyArgs.at(0)).compare("r3")) {
            if (assemblyArgs.size() == 4) {
                  if (assemblyArgs.at(1).at(0) == '$' && isStrANumber(assemblyArgs.at(1).substr(1)) &&
                      assemblyArgs.at(2).at(0) == '$' && isStrANumber(assemblyArgs.at(2).substr(1)) &&
                      assemblyArgs.at(3).at(0) == '$' && isStrANumber(assemblyArgs.at(3).substr(1))) {
                        
                        int regNum = stoi(assemblyArgs.at(1).substr(1));
                        if (!isRegNumInRange(regNum)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rd specified\n";
                              errorExists = true;
                        }
                        
                        for (int i = 2; i < 4; i++) {
                              regNum = stoi(assemblyArgs.at(i).substr(1));
                              if (!isRegNumInRange(regNum)) {
                                    cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rs" <<
                                    (i - 1) << " specified\n";
                                    errorExists = true;
                              }
                        }
                  }
            } else {
                  cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid number of arguments\n";
                  errorExists = true;
            }
      } else if (!getInstrFormat(assemblyArgs.at(0)).compare("r3-2")) {
            if (assemblyArgs.size() == 3) {
                  if (assemblyArgs.at(1).at(0) == '$' && isStrANumber(assemblyArgs.at(1).substr(1)) &&
                      assemblyArgs.at(2).at(0) == '$' && isStrANumber(assemblyArgs.at(2).substr(1))) {
                        
                        int regNum = stoi(assemblyArgs.at(1).substr(1));
                        if (!isRegNumInRange(regNum)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rd specified\n";
                              errorExists = true;
                        }
                        
                        regNum = stoi(assemblyArgs.at(2).substr(1));
                        if (!isRegNumInRange(regNum)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rs1 specified\n";
                              errorExists = true;
                        }
                  }
            } else {
                  cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid number of arguments\n";
                  errorExists = true;
            }
      } else if (!getInstrFormat(assemblyArgs.at(0)).compare("r3-0")) {
            if (assemblyArgs.size() != 1) {
                  cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid number of arguments\n";
                  errorExists = true;
            }
      } else if (!getInstrFormat(assemblyArgs.at(0)).compare("r3-i")) {
            if (assemblyArgs.size() == 4) {
                  if (assemblyArgs.at(1).at(0) == '$' && isStrANumber(assemblyArgs.at(1).substr(1)) &&
                      assemblyArgs.at(2).at(0) == '$' && isStrANumber(assemblyArgs.at(2).substr(1)) &&
                      isStrANumber(assemblyArgs.at(3))) {
                        
                        int regNum = stoi(assemblyArgs.at(1).substr(1));
                        if (!isRegNumInRange(regNum)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rd specified\n";
                              errorExists = true;
                        }
                        
                        regNum = stoi(assemblyArgs.at(2).substr(1));
                        if (!isRegNumInRange(regNum)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid register rs1 specified\n";
                              errorExists = true;
                        }
                        
                        int immValue = stoi(assemblyArgs.at(3));
                        if (!is4BitUnsignedImmInRange(immValue)) {
                              cerr << "Error: Line #" << currentSourceCodeLineIndex << ": immediate value is out of range\n";
                              errorExists = true;
                        }
                  }
            } else {
                  cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid number of arguments\n";
                  errorExists = true;
            }
      } else {
            cerr << "Error: Line #" << currentSourceCodeLineIndex << ": invalid operation\n";
            errorExists = true;
      }
      
      return errorExists;
}

void trim(string& str) {
      size_t charIndex = str.find_first_not_of(" \t");
      str.erase(0, charIndex);
      
      charIndex = str.find_first_of("#");
      if (string::npos != charIndex){
            str.erase(charIndex);
      }
      
      charIndex = str.find_last_not_of(" \t");
      if (string::npos != charIndex){
            str.erase(charIndex + 1);
      }
}

bool isStrANumber(string str) {
      if (!isdigit(str.at(0)) && str.at(0) != '-') {
            return false;
      }
      for (int i = 1; i < str.length(); i++) {
            if (!isdigit(str.at(i))) {
                  return false;
            }
      }
      return true;
}

bool isRegNumInRange(int num) {
      if (num >= 0 && num < 32) {
            return true;
      }
      return false;
}

bool isLoadImmFieldPosInRange(int num) {
      if (num >= 0 && num < 16) {
            return true;
      }
      return false;
}

bool is16BitSignedImmInRange(int num) {
      if (num >= -32768 && num < 32768) {
            return true;
      }
      return false;
}

bool is4BitUnsignedImmInRange(int num) {
      if (num >= 0 && num < 16) {
            return true;
      }
      return false;
}

void assemble(string assemblyLangCode[]) {
      //      ifstream inputFileStream(argv[1]);
      //      string inputLine;
      for (int i = 0; i < 32; i++) {
            
            unordered_map<string, string> assemblyLangMap = {
                  // load immediate instruction format
                  {"li",     "0"},
                  // R4 instruction format
                  {"mal",    "10000"},
                  {"mah",    "10001"},
                  {"msl",    "10010"},
                  {"msh",    "10011"},
                  // R3 instruction format
                  {"nop",    "1100000000"},
                  {"bcw",    "1100000001"},
                  {"and",    "1100000010"},
                  {"or",     "1100000011"},
                  {"popcnth","1100000100"},
                  {"clz",    "1100000101"},
                  {"rot",    "1100000110"},
                  {"shlhi",  "1100000111"},
                  {"a",      "1100001000"},
                  {"sfw",    "1100001001"},
                  {"ah",     "1100001010"},
                  {"sfh",    "1100001011"},
                  {"ahs",    "1100001100"},
                  {"sfhs",   "1100001101"},
                  {"mpyu",   "1100001110"},
                  {"absdb",  "1100001111"}
            };
            
            string assembledBinaryLine = "";
//            if (!instrFormatMap.at(assemblyLangCode[i]).compare("li")) {
//                  assembledBinaryLine += "0";
//                  cout << "It's alive!\n";
//            } else if (!instrFormatMap.at(assemblyLangCode[i]).compare("r4")) {
//                  assembledBinaryLine += "100";
//            } else if (!instrFormatMap.at(assemblyLangCode[i]).compare("r3")) {
//                  assembledBinaryLine += "110000";
//            } else {
//
//            }
      }
}

string getInstrFormat (string key) {
      
      unordered_map<string, string> instrFormatMap = {
            // load immediate instruction format
            {"li",     "li"},
            // R4 instruction format
            {"mal",    "r4"},
            {"mah",    "r4"},
            {"msl",    "r4"},
            {"msh",    "r4"},
            // R3 instruction format
            {"nop",    "r3-0"},     // no args; special case
            {"bcw",    "r3-2"},     // rs2 not used; special case
            {"and",    "r3"},
            {"or",     "r3"},
            {"popcnth","r3-2"},     // rs2 not used; special case
            {"clz",    "r3-2"},     // rs2 not used; special case
            {"rot",    "r3"},
            {"shlhi",  "r3-i"},     // right-most 4-bit imm in rs2 instr field; special case
            {"a",      "r3"},
            {"sfw",    "r3"},
            {"ah",     "r3"},
            {"sfh",    "r3"},
            {"ahs",    "r3"},
            {"sfhs",   "r3"},
            {"mpyu",   "r3"},
            {"absdb",  "r3"}
      };
      if (instrFormatMap.find(key) != instrFormatMap.end()) {
            return instrFormatMap.at(key);
      }
      return "NULL";
}
