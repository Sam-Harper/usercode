#ifndef SHEVENT_SHL1RESULT_HH
#define SHEVENT_SHL1RESULT_HH

#include "Rtypes.h"
#include <limits>

class SHL1Result {
private: 
  size_t bitNr_;
  bool result_;
  bool masked_;
  int preScale_;

public:
  SHL1Result():bitNr_(std::numeric_limits<size_t>::max()),result_(false),masked_(-1),preScale_(-1){}
  SHL1Result(size_t iBitNr,bool iResult,bool iMask,int iPreScale):bitNr_(iBitNr),result_(iResult),masked_(iMask),preScale_(iPreScale){}
  virtual ~SHL1Result(){}
  

  bool result()const{return result_;}
  bool masked()const{return masked_;}
  size_t bitNr()const{return bitNr_;}
  int preScale()const{return preScale_;}
  bool valid()const{return bitNr_!=std::numeric_limits<size_t>::max();}
  
  ClassDef(SHL1Result,1)

};


#endif
