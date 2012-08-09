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
      
      complex<float> *AlamoutiEncode(complex<float> *symSequenceToBeAlamoutiEncoded);

      complex<float> *AlamoutiDecode(complex<float> *symSequenceToBeAlamoutiDecoded, complex<float> *channelGain);

   private:

};

#endif
