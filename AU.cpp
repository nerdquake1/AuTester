#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <sstream>
#include <cstring>
#include <fstream>
#include <cstring>

using namespace std;

int GccCommander(std::string fileName)
{
    int result = std::system(("g++ " + fileName + "").c_str());
    if (result == 1)
    {
        throw std::invalid_argument("The program failed to compile.");
    }
    return result;
}

void parseFunctions(vector<string>& lines, const string& fileName)
{
    ifstream input(fileName);
    if (!input.is_open())
    {
        return;
    }
    string line;
    bool foundComment = false;
    bool foundStart = false;
    bool doneStoring = false;
    int parCount = 0;
    while (getline(input, line))
    {
        if (!foundComment)
        {
            // Look for line containing "*/"
            if (line.find("/* @AuTEST") != string::npos)
            {
                foundComment = true;
            }
            continue;
        }
        if (!foundStart)
        {
            // Look for line containing "*/"
            if (line.find("*/") != string::npos)
            {
                foundStart = true;
            }
            continue;
        }
        
        // Look for line containing "*/"
        if (line.find("{") != string::npos)
        {
            parCount++;
        }
        if (line.find("}") != string::npos)
        {
            parCount--;
        }
        lines.push_back(line);
        
        if (parCount < 1)
        {
            foundComment = false;
            foundStart = false;
            continue;
        }
        
        
    }
}

void parseTestCase(vector<string>& lines, vector<vector<string>>& TCases, const string& fileName)
{
    ifstream input(fileName);
    if (!input.is_open())
    {
        return;
    }
    string line;
    bool foundComment = false;
    while (getline(input, line))
    {
        if (!foundComment)
        {
            // Look for line containing "*/"
            if (line.find("/* @AuTEST") != string::npos)
            {
                lines.push_back(line);
                foundComment = true;
            }
            continue;
        }
        lines.push_back(line);
        // Stop reading if line contains "}"
        if (line.find("{") != string::npos) {
            TCases.push_back(lines);
            foundComment = false;
            lines.clear();
        }
    }  
}

void GenerateMainMethod(std::vector<string> GenVector, const string& outputFile)
{
    std::ofstream file;
    file.open(outputFile, std::ios::app); // open file in append mode
    file << "int main() {" << std::endl;
        for (int i = 0; i < GenVector.size(); i++) {
            file << GenVector[i]; // replacing with vector
        }
    file << "    return 0;" << std::endl;
    file << "}" << std::endl;
    file.close();
}

void GenerateIncludes(const string& outputFile)
{
    std::ofstream file;
    file.open(outputFile, std::ios::out); // open file in append mode
    file << "#include <stdio.h>" << std::endl;
    file << "#include <stdbool.h>" << std::endl;
    file.close();
}

void generateTestCase(std::vector<string>& TCases, const string& outputFile, std::vector<string>& GenVector){

    std::ofstream file;
    file.open(outputFile, std::ios::app);
    int innerIndex = 0;
    string testCaseLine;
    string testName, functionName, returnType;
    string add1, add2, addResult;
    string outputTestName;
    
    int numElements = TCases.size();

    //outer for is each test case, inner for is the pieces to be gotten
    for(int h=0; h<numElements/4; h++){
        for(int i=h*4; i<numElements; i++){
            stringstream ss(TCases[i]);
            string word;

            while(ss >> word){
                if(word=="@AuTEST" || word == "/*" || word == "*" || word == "@Test" || word == "Vector" || word == "@Result" || word == "*/"){
                    continue;
                }
                else if(i==h*4){
                    testName += word;
                }
                else if(i==h*4 + 1){
                    add1 += word;
                }
                else if(i==h*4 + 2){
                    addResult = word;
                }
                word == " ";
            }
        }

        for(int j=0; j<1; j++){
            stringstream ss2(TCases[numElements-1]);
            string word2;

            while(ss2 >> word2){
                if(j==0 && returnType == ""){
                    returnType = word2;
                }
                else if(j==0 && functionName == ""){
                    functionName = word2;
                }
                word2 == " ";
            }
        }
            //This cuts the parentheses out of the function name, so functionName string does not contain functionName(int, just funcNameCut
            string parenthesesCut = "(";
            int cutter = functionName.find(parenthesesCut);
            string funcNameCut = functionName.substr(0,cutter);

            //Writing out the initial part of the test case generation.

            testCaseLine += "/* TEST " + testName + "\n";
            testCaseLine += " * TestVector " + add1 + "\n";
            testCaseLine += " * Result " + addResult + " \n" + " */\n";

            //The beginning of the test function itself

            testCaseLine += "bool " + testName + "_" + funcNameCut + "(){ \n";
            outputTestName += testName + "_" + funcNameCut + "();\n";


            testCaseLine += ("    printf(""\"TEST " + funcNameCut + " :: " + testName + "\");\n\n");

            //split lines to account for more than two parameters (possibly put into an array and iterate through it, incrementing the name to account)
            testCaseLine += returnType + " result = " + funcNameCut + "(" + add1 + ");\n";
            testCaseLine += "bool passedTest = result == " + addResult + ";\n\n";

            testCaseLine += "    if(passedTest){\n";
            testCaseLine += ("        printf(\"Passed\\n\");\n");
            testCaseLine += "    }else{\n";

            //This line must be changed to account for data types!!
            if(returnType == "int"){
                testCaseLine += ("        printf(\"Failed - returned %d when " + addResult + " was expected\\n\", result);\n");
            }
            else if(returnType == "char"){
                testCaseLine += ("        printf(\"Failed - returned %c when " + addResult + " was expected\\n\", result);\n");
            }
            else if(returnType == "double"){
                testCaseLine += ("        printf(\"Failed - returned %.2f when " + addResult + " was expected\\n\", result);\n");
            }
            else if(returnType == "float"){
                testCaseLine += ("        printf(\"Failed - returned %.2f when " + addResult + " was expected\\n\", result);\n");
            }
            else if(returnType == "string"){
                testCaseLine += ("        printf(\"Failed - returned %s when " + addResult + " was expected\\n\", result);\n");
            }

            testCaseLine += "    }\n      return passedTest;\n}\n\n";
            //cout << testCaseLine;

            file << testCaseLine;

            //this cout line is the testcase name, separate from the rest (for Andrew)

            //resetting the stuff to account for multiple test cases
            testCaseLine = "";
            //outputTestName = "";
            add1 = "";
            testName = "";
            returnType = "";
            functionName = "";
    }
    GenVector.push_back(outputTestName);
    file.close();
    //return outputTestName;
}

void generateFunction(std::vector<string> func, string outputFile) {
    ofstream fileOUT;
    fileOUT.open(outputFile, ios::app);
    for (int i = 0; i < func.size(); i++) {
        fileOUT << func[i] << endl;
    }
    fileOUT.close();
}

int main()
{
    std::string outputFile = "AUTesting.c";
    std::string fileName = "";
    std::cout << "Please Enter Target File Directory: ";
    std::cin >> fileName;

    try
    {
        int complieResult = GccCommander(fileName);
        if (complieResult == 0)
        {
            std::cout << "The program compiled successfully!" << std::endl;
        }
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    vector<vector<string>> TCases = {{}};
    vector<string> infoLines = {};
    parseTestCase(infoLines, TCases, fileName);
    vector<string> lines = {};
    parseFunctions(lines, fileName);
    vector<string> GenVector = {};


    GenerateIncludes(outputFile);
    generateFunction(lines, outputFile);
    for(int i = 0; i < TCases.size(); i ++){
        generateTestCase(TCases[i], outputFile, GenVector);
    }
    GenerateMainMethod(GenVector, outputFile);

    try
    {
        int testcomplieResult = GccCommander(outputFile);
        if (testcomplieResult == 0)
        {
            std::cout << "The program compiled successfully!" << std::endl;
        }

    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}