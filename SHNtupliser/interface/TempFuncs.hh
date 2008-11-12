#ifndef TEMPFUNCS
#define TEMPFUNCS

//I appreciate this is an unforunate name. This is a collection of TEMPLATE functions not TEMPORARY functions which I find usefull

#include "TClonesArray.h"

namespace TempFuncs {

  template<class T> void copyTClonesArray(TClonesArray& lhs,const TClonesArray& rhs);
 

}

template<class T> void TempFuncs::copyTClonesArray(TClonesArray& lhs,const TClonesArray& rhs)
{
  lhs.Delete();
  for(int i=0;i<rhs.GetLast()+1;i++){
    T* obj = (T*) rhs[i];
    new(lhs[i]) T(*obj);
  }
}

#endif
