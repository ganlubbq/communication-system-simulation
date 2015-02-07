/* 
 * Author: Ling-San Meng
 * Email: f95942117@ntu.edu.tw
 *
 * Date: 2009/12/26
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
      complex<double> *AlamoutiEncode(complex<double> *symSequenceToBeEncoded);

      // Caution! "channelGain[]" will be remade in this function. 
      // See the correpsonding .cpp for details.
      complex<double> *AlamoutiDecode(complex<double> *symSequenceToBeDecoded,
                                      complex<double> *channelGain);

   private:

};

#endif
