#ifndef TEMPFUNCS
#define TEMPFUNCS

//I appreciate this is an unforunate name. This is a collection of TEMPLATE functions not TEMPORARY functions which I find usefull


#include "TClonesArray.h"

namespace TempFuncs {

  template<class T> void copyTClonesArray(TClonesArray& lhs,const TClonesArray& rhs);
  template<class T> bool convertStringToVec(const std::string& string,std::vector<T>& vec);

  template<class T,class Y,class CompType> struct PairComp : public std::binary_function<std::pair<T,Y>,std::pair<T,Y>,bool> {
    bool operator()(const std::pair<T,Y>& lhs,const std::pair<T,Y>& rhs,const CompType& compObj = CompType()){return compObj(lhs.first,rhs.first);} 
    bool operator()(const std::pair<T,Y>& lhs,const T& rhs,const CompType& compObj = CompType()){return compObj(lhs.first,rhs);}
    bool operator()(const T& lhs,const  std::pair<T,Y>& rhs,const CompType& compObj = CompType()){return compObj(lhs,rhs.first);}
  
  };

  template <class T1,class T2,typename Comp=std::less<T1> > struct PairSortBy1st : public std::binary_function<std::pair<T1,T2>,std::pair<T1,T2>,bool> { 
    Comp comp;
    bool operator()(const std::pair<T1,T2>& lhs,const std::pair<T1,T2>&rhs){return comp(lhs.first,rhs.first);}
    bool operator()(const T1& lhs,const std::pair<T1,T2>&rhs){return comp(lhs,rhs.first);}
    bool operator()(const std::pair<T1,T2>& lhs,const T1 &rhs){return comp(lhs.first,rhs);}
    bool operator()(const T1& lhs,const T1 &rhs){return comp(lhs,rhs);}
   

  };



  template <class T1,class T2,typename Comp=std::less<T2> > struct PairSortBy2nd : public std::binary_function<std::pair<T1,T2>,std::pair<T1,T2>,bool>  {
    Comp comp;
    bool operator()(const std::pair<T1,T2>& lhs,const std::pair<T1,T2>&rhs){return comp(lhs.second,rhs.second);}
    bool operator()(const T2& lhs,const std::pair<T1,T2>&rhs){return comp(lhs,rhs.second);}
    bool operator()(const std::pair<T1,T2>& lhs,const T2 &rhs){return comp(lhs.second,rhs);}
    bool operator()(const T2& lhs,const T2 &rhs){return comp(lhs,rhs);}
   
  };

   template <class T1,class T2> struct PairEq1st : public std::unary_function<std::pair<T1,T2>,bool> { 
     
     T1 val;
     PairEq1st(const T1& iVal):val(iVal){}
     bool operator()(const std::pair<T1,T2>& pair){return pair.first==val;}
     bool operator==(const std::pair<T1,T2>& pair){return pair.first==val;}
  };

   template <class T1,class T2> struct PairEq2nd : public std::unary_function<std::pair<T1,T2>,bool> { 
     T2 val;
     PairEq2nd(const T2& iVal):val(iVal){}
     bool operator()(const std::pair<T1,T2>& pair){return pair.second==val;}
     bool operator==(const std::pair<T1,T2>& pair){return pair.second==val;}
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
