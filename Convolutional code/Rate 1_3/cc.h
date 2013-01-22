/*
 * Rate 1/3 convolution encoder and viterbi decoder.
 *
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 * Date: 2009/12/26
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
      void _buildTable(int (*_outputBitEachState)[6]);
      
      // This function will only check if the FL has been changed and 
      // reallocate memory for trellis if necessary.
      void _allocateMemoryForTrellis(int FL);
      
      int (*_outputBitEachState)[6];
      struct TrellisNode **_trellisNode;
      int _previousFL;
      int _G11;
      int _G12;
      int _G13;
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
