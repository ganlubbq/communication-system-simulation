#include "fileIO.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

float getValue(char *itemName, char *filename)
{
   char str[50]= "";
   // Open the configuration file for read operation.
   ifstream param(filename);

   while (!param.eof())
   {
      // Read one line of the file.
      param.getline(str,50);
      string temp = str;
      // Search for the item in the line just read.
      int position = temp.find(itemName);
    
      if (position != -1)
      {
         position = temp.find("=");
      
         if (position != -1)      
         {
            char *fp = (str+position+1);

            param.close();
            return atof(fp);
         }
      }
   }
 
   cout << "File read error for \"" << itemName << "\"" << endl;
   return -1;
}

void outputMessage(char *str, char *filename)
{
     ofstream _messageLog;

     _messageLog.open(filename,ios::app);
     _messageLog << str << endl;
     _messageLog.close();
}
