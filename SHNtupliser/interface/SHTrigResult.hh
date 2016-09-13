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
  int hltPreScale_;
  std::vector<std::pair<size_t,int>> l1PreScales_; //L1 bit, prescale

public:
  SHTrigResult():bitNr_(std::numeric_limits<size_t>::max()),result_(Status::UNDEFINED),hltPreScale_(-1){}
  SHTrigResult(int iBitNr,Status iResult,int iHLTPreScale):bitNr_(iBitNr),result_(iResult),hltPreScale_(iHLTPreScale){}
  SHTrigResult(int iBitNr,Status iResult,int iHLTPreScale,const std::vector<std::pair<size_t,int>>& iL1PreScales):bitNr_(iBitNr),result_(iResult),hltPreScale_(iHLTPreScale),l1PreScales_(iL1PreScales){}
  virtual ~SHTrigResult(){}
  
  bool accept()const{return result_==Status::PASS;}
  bool wasRun()const{return result_!=Status::READY && result_!=Status::UNDEFINED;}
  bool error()const{return result_==Status::EXCEPTION;}
  bool valid()const{return result_!=Status::UNDEFINED;}
  size_t bitNr()const{return bitNr_;}
  int hltPreScale()const{return hltPreScale_;}
  int preScale()const{return hltPreScale_;}
  const std::vector<std::pair<size_t,int>>& l1PreScales()const{return l1PreScales_;}
  
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
