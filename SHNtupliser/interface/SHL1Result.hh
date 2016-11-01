#ifndef SHEVENT_SHL1RESULT_HH
#define SHEVENT_SHL1RESULT_HH

#include "Rtypes.h"
#include <limits>

//this has evolved from being a ntuple stored object to a more heavy object created from stored info

class SHL1Result {
private: 
  size_t bitNr_;
  std::string name_;
  bool result_;
  bool masked_;
  int preScale_;

public:
  SHL1Result():bitNr_(std::numeric_limits<size_t>::max()),result_(false),masked_(-1),preScale_(-1){}
  SHL1Result(size_t iBitNr,std::string iName,bool iResult,bool iMask,int iPreScale):
    bitNr_(iBitNr),name_(std::move(iName)),result_(iResult),masked_(iMask),preScale_(iPreScale){}

  virtual ~SHL1Result(){}
  
  const std::string& name()const{return name_;}
  bool result()const{return result_;}
  bool masked()const{return masked_;}
  size_t bitNr()const{return bitNr_;}
  int preScale()const{return preScale_;}
  bool valid()const{return bitNr_!=std::numeric_limits<size_t>::max();}
  
  ClassDef(SHL1Result,2)

};


#endif
