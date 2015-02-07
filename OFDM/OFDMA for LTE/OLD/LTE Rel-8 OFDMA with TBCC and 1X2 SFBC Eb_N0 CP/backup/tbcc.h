/* TBCC
 * Rate 1/3 convolution encoder and viterbi decoder.
 * Date: 2009/12/22
 * -------------------------------------------------
 * Rearrange the coded bits as
 * systematic part | parity 1 | parity 2
 * Date: 2009/12/27
 * 
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 */

#ifndef TBCC_H
#define TBCC_H
   
class TBCC
{
   public:
      TBCC();
      ~TBCC();
      
      // Return the coded bits array.
      int *encode(int *uncodedBits);

      // Return the decoded bits array.
      int *viterbiDecode(double *softValuesToBeDecoded);
      
	   double getCodeRate(void) { return _codeRate; }
      
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
      double _codeRate;
      int _trellisLen;
};

struct TrellisNode
{
   struct TrellisNode *pPreviousState;
   int stateNum;
};

#endif
