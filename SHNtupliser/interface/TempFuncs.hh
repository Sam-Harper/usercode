#ifndef TEMPFUNCS
#define TEMPFUNCS

//I appreciate this is an unforunate name. This is a collection of TEMPLATE functions not TEMPORARY functions which I find usefull


#include "TClonesArray.h"

namespace TempFuncs {

  template<class T> void copyTClonesArray(TClonesArray& lhs,const TClonesArray& rhs);
 
  template<class T,class Y,class CompType> struct PairComp : public std::binary_function<std::pair<T,Y>,std::pair<T,Y>,bool> {
    bool operator()(const std::pair<T,Y>& lhs,const std::pair<T,Y>& rhs,const CompType& compObj = CompType()){return compObj(lhs.first,rhs.first);}
    bool operator()(const std::pair<T,Y>& lhs,const T& rhs,const CompType& compObj = CompType()){return compObj(lhs.first,rhs);}
    bool operator()(const T& lhs,const  std::pair<T,Y>& rhs,const CompType& compObj = CompType()){return compObj(lhs,rhs.first);}
  
  };
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
