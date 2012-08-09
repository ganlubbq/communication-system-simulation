#include "fileIO.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define BUFFER_SIZE 100

double getValue(char *itemName, char *fileName, char *option)
{
   // Open the configuration file for read operation.
   ifstream file(fileName);
      
   while (!file.eof())
   {
      char charBuff[BUFFER_SIZE]= "";

      // Read text until '='
      file.getline(charBuff,BUFFER_SIZE,'=');    
      string lineStr = charBuff;
       
      // Cut the string from the last enter
      lineStr.erase(0,lineStr.find_last_of("\n")+1);
      // Trim off the whitespaces before and after the string      
      lineStr.erase(0,lineStr.find_first_not_of(" "));
      lineStr.erase(lineStr.find_last_not_of(" ")+1);
      
      if (lineStr == itemName)
      {
         double value;
         
         if (strcmp(option,"dec") == 0) { file >> dec >> value; }
         else if (strcmp(option,"oct") == 0)
         {
            int intValue;
            file >> oct >> intValue;
            value = (double)intValue;
         }
         else if (strcmp(option,"hex") == 0)
         {
            int intValue;
            file >> hex >> intValue;
            value = (double)intValue;
         }
         else { break; }

         if (file.fail() == 1) { break; }
            
         file.close();

         return value;
      }
   }
 
   cout << "File read error for \"" << itemName << "\"" << endl;
   file.close();
   getchar();
   exit(1);
   return -1;
}

double *getArray(char *itemName, int arraySize, char *fileName)
{
   // Open the configuration file for read operation.
   ifstream file(fileName);
      
   while (!file.eof())
   {
      char charBuff[BUFFER_SIZE]= "";

      // Read text until '='
      file.getline(charBuff,BUFFER_SIZE,'=');    
      string lineStr = charBuff;
      
      // Cut the string from the last enter
      lineStr.erase(0,lineStr.find_last_of("\n")+1);
      // Trim off the whitespaces before and after the string      
      lineStr.erase(0,lineStr.find_first_not_of(" "));
      lineStr.erase(lineStr.find_last_not_of(" ")+1);

      if (lineStr == itemName)
      {
         file.getline(charBuff,BUFFER_SIZE);
         string lineStr = charBuff;
         
         // Trim off the whitespaces before and after the string
         lineStr.erase(0,lineStr.find_first_not_of(" "));
         lineStr.erase(lineStr.find_last_not_of(" ") + 1);       

         double *value = new double [arraySize];         
         int idx = 0;
         
         while (1)
         {
            // Find white space
            int spacePosition = lineStr.find(' ');
                   
            if (spacePosition!=-1)
            {
               // Copy the string before the first space
               string clone = lineStr;
               clone.erase(spacePosition);
               
               // Convert the string to double
               value[idx] = atof(clone.c_str()); 
               idx++;
               
               // Cut the string from the first space
               lineStr = lineStr.substr(spacePosition);
               lineStr.erase(0,lineStr.find_first_not_of(" "));
            }
            else // means there is only one left
            {
               // Get the last value
               value[idx] = atof(lineStr.c_str());
               idx++;
                      
               break;
            }  
         }
       
         // Check if we have read the correct number of values
         if (idx != arraySize) { break; }
         
         file.close();

         return value;
      }
   }
 
   cout << "File read error for \"" << itemName << "\"" << endl;
   file.close();
   getchar();
   exit(1);
   return NULL;
}

char *getString(char *itemName, char *fileName)
{
   // Open the configuration file for read operation.
   ifstream file(fileName);
      
   while (!file.eof())
   {
      char charBuff[BUFFER_SIZE]= "";

      // Read text until '='
      file.getline(charBuff,BUFFER_SIZE,'=');    
      string lineStr = charBuff;
      
      // Cut the string from the last enter
      lineStr.erase(0,lineStr.find_last_of("\n")+1);
      // Trim off the whitespaces before and after the string      
      lineStr.erase(0,lineStr.find_first_not_of(" "));
      lineStr.erase(lineStr.find_last_not_of(" ")+1);
      
      if (lineStr == itemName)
      {
         // Read text until '\n'
         file.getline(charBuff,BUFFER_SIZE,'\n');
         string _lineStr = charBuff;

         // Trim off the whitespaces before and after the string      
         _lineStr.erase(0,_lineStr.find_first_not_of(" "));
         _lineStr.erase(_lineStr.find_last_not_of(" ")+1);

         char *returnStr = new char [_lineStr.size()+1];
         strcpy (returnStr, _lineStr.c_str());

         if (file.fail() == 1) { break; }          
         file.close();

         return returnStr;
      }
   }
 
   cout << "File read error for \"" << itemName << "\"" << endl;
   file.close();
   getchar();
   exit(1);
   return NULL;
}

void outputMessage(char *str, char *fileName)
{
     ofstream _messageLog;

     _messageLog.open(fileName,ios::app);
     _messageLog << str << endl;
     _messageLog.close();
}
