#ifndef SHEVENT_SHTRIGRESULT_HH
#define SHEVENT_SHTRIGRESULT_HH

#include "Rtypes.h"
#include <limits>

class SHTrigResult {
public:
  enum class Status {READY=0,PASS=1,FAIL=2,EXCEPTION=3,UNDEFINED=4};
private: 
  size_t bitNr_;
  Status result_;
  int preScale_;

public:
  SHTrigResult():bitNr_(std::numeric_limits<size_t>::max()),result_(Status::UNDEFINED),preScale_(-1){}
  SHTrigResult(int iBitNr,Status iResult,int iPreScale):bitNr_(iBitNr),result_(iResult),preScale_(iPreScale){}
  virtual ~SHTrigResult(){}
  
  bool accept()const{return result_==Status::PASS;}
  bool wasRun()const{return result_!=Status::READY && result_!=Status::UNDEFINED;}
  bool error()const{return result_==Status::EXCEPTION;}
  bool valid()const{return result_!=Status::UNDEFINED;}
  size_t bitNr()const{return bitNr_;}
  int preScale()const{return preScale_;}
  
  bool operator<(const SHTrigResult& rhs)const{return bitNr_<rhs.bitNr_;}

  template<typename CompFunc=std::less<size_t> >
  class Sorter {
  public:
    bool operator()(const SHTrigResult& lhs,const SHTrigResult&rhs)const{CompFunc func;return func(lhs.bitNr(),rhs.bitNr());}
    bool operator()(const SHTrigResult& lhs,const size_t rhs)const{CompFunc func;return func(lhs.bitNr(),rhs);}
    bool operator()(const size_t lhs,const SHTrigResult rhs)const{CompFunc func;return func(lhs,rhs.bitNr());}
  };
  
  ClassDef(SHTrigResult,1)

};


#endif
