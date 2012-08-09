/* 
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 *
 * Date: 2009/12/15
 */

 
#ifndef STBC_H
#define STBC_H

#include <complex>
using namespace std;

class STBC
{
   public:

      STBC();
      ~STBC();
      
      // Alamouti by defn. is 2XN. Structured as a double-sized 1-d array.
      complex<float> *AlamoutiEncode(complex<float> *symSequenceToBeEncoded);

      complex<float> *AlamoutiDecode(complex<float> *symSequenceToBeDecoded,
                                     complex<float> *channelGain);

   private:

};

#endif
