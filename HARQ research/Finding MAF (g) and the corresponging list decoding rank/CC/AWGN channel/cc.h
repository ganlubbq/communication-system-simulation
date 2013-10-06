/*
 * Rate 1/2 convolution encoder and decoder
 * The 1st element of arrays always stores the array size!!
 * ----------------------------------------
 * ViterbiDecoder() is made to accept LLR.
 * Write CC as a class.
 * Date: 2008/1/5
 * ----------------------------------------
 * Conquer the list Viterbi decoder
 * Date: 2008/1/8
 */

#ifndef CC_H
#define CC_H

class TRELLISNODE
{
   public:
      TRELLISNODE() 
      { _nList = 0;previousNode = 0;inputBit = 0;previousRank=0; }
      ~TRELLISNODE()
      { 
         if (previousNode != 0) { delete [] previousNode;previousNode=0; }
         if (inputBit != 0) { delete [] inputBit;inputBit=0; }
         if (previousRank != 0) 
         { delete [] previousRank;previousRank=0; }
      }
      
      void initListNum(int nList)
      {
         if (nList == _nList) {}
         else if (_nList == 0)
         {
            _nList = nList;
            previousNode = new TRELLISNODE* [nList];
            inputBit = new int [nList];
            previousRank = new int [nList];
         }
         else // List num has been altered.
         {
            delete [] previousNode;
            delete [] inputBit;
            delete [] previousRank;
            _nList = nList;
            previousNode = new TRELLISNODE* [nList];
            inputBit = new int [nList];
            previousRank = new int [nList];
         }
      }

      TRELLISNODE **previousNode;// Array of pointers, not double array
      int *previousRank;
      int *inputBit;
   private:
      int _nList;
};
   
class CC
{
   public:
      CC(void);
      ~CC(void);
      
      // Return the coded bits array.
      int *encode(int *uncodedBits);

      // Return the decoded bits array.
      int **listViterbiDecode(double *softValuesToBeDecoded, int numOfListDecodeOutput);
      
	  double getCodeRate(void) { return _codeRate; }
      
   private:
      int (*_nextoutput)[4];
      void _buildTable(int (*_nextoutput)[4]);
      
      TRELLISNODE **_trellisNode;
      int _previousFL;
      
      int _G11;
      int _G12;
      int _m;
      int _nState;
      double _codeRate;
};

#endif
