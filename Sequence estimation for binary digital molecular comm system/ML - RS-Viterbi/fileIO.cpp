#include "fileIO.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

float getValue(const char *itemName, const char *filename, const char *option)
{
   // Open the configuration file for read operation.
   ifstream file(filename);
      
   while (!file.eof())
   {
      char lineChar[50]= "";
      // Read one line of the file.
      file.getline(lineChar,50,'=');
      
      // Format conversion
      string lineStr = lineChar;      
      
      // Search for the item in the line just read.
      int position = lineStr.find(itemName);
    
      if (position != -1)
      {
         float value;
            
         if (strcmp(option,"dec") == 0) { file >> dec >> value; }
         else if (strcmp(option,"oct") == 0)
         {
            int intValue;
            file >> oct >> intValue;
            value = (float)intValue;
         }
         else if (strcmp(option,"hex") == 0)
         {
            int intValue;
            file >> hex >> intValue;
            value = (float)intValue;
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
   return -1;
}

void outputMessage(const char *str, const char *filename)
{
     ofstream _messageLog;

     _messageLog.open(filename,ios::app);
     _messageLog << str << endl;
     _messageLog.close();
}
