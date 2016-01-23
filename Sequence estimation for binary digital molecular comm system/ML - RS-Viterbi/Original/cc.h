/*
 * Rate 1/2 convolution encoder and decoder
 * The 1st elements of arrays always store the array size!!
 * ----------------------------------------
 * ViterbiDecoder() is made to accept LLR.
 * Write CC as a class.
 * Date: 2008/1/5
 * ----------------------------------------
 * Improve the efficiencies.
 * Rewrite the table.
 * Write butterfly.
 * Date: 2008/1/11
 * ----------------------------------------
 * ... Correct the code rate...
 * Date: 2008/11/24
 */

#ifndef CC_H
#define CC_H
   
class CC
{
   public:
      CC(int FL);
      ~CC(void);
      
      // Return the coded bits array.
      int *encode(int *uncodedBits);

      // Return the decoded bits array.
      int *viterbiDecode(float *softValuesToBeDecoded);
      
	  float getCodeRate(void) { return _codeRate; }
      
   private:
      void _buildTable(int (*_outputBitEachState)[4]);
      
      // This function will only check if the FL has been changed and 
      // reallocate memory for trellis if necessary.
      void _allocateMemoryForTrellis(int FL);
      
      int (*_outputBitEachState)[4];
      struct TrellisNode **_trellisNode;
      int _previousFL;
      int _G11;
      int _G12;
      int _m;
      int _nState;
      float _codeRate;
};

struct TrellisNode
{
   struct TrellisNode *pPreviousState;
   int stateNum;
};

#endif
