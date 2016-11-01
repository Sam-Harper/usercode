#ifndef SHEVENT_SHTRIGRESULT_HH
#define SHEVENT_SHTRIGRESULT_HH

#include "SHarper/SHNtupliser/interface/SHL1Result.hh"

#include "Rtypes.h"
#include <limits>

//this has evolved from being a ntuple stored object to a more heavy object created from stored info

class SHTrigResult {
public:
  enum class Status {READY=0,PASS=1,FAIL=2,EXCEPTION=3,UNDEFINED=4};
private: 
  size_t bitNr_;
  std::string name_;
  Status result_;
  int hltPreScale_;
  std::vector<SHL1Result> l1Seeds_;

public:
  SHTrigResult():bitNr_(std::numeric_limits<size_t>::max()),result_(Status::UNDEFINED),hltPreScale_(-1){}
  SHTrigResult(int iBitNr,std::string iName,Status iResult,int iHLTPreScale):
    bitNr_(iBitNr),name_(std::move(iName)),result_(iResult),hltPreScale_(iHLTPreScale){}
  SHTrigResult(int iBitNr,std::string iName,Status iResult,int iHLTPreScale,std::vector<SHL1Result> iL1Seeds):
    bitNr_(iBitNr),name_(std::move(iName)),result_(iResult),hltPreScale_(iHLTPreScale),
    l1Seeds_(std::move(iL1Seeds)){}
  virtual ~SHTrigResult(){}
  
  bool accept()const{return result_==Status::PASS;}
  bool wasRun()const{return result_!=Status::READY && result_!=Status::UNDEFINED;}
  bool error()const{return result_==Status::EXCEPTION;}
  bool valid()const{return result_!=Status::UNDEFINED;}
  size_t bitNr()const{return bitNr_;}
  int hltPreScale()const{return hltPreScale_;}
  int preScale()const{return hltPreScale_;}
  const std::vector<SHL1Result>& l1Seeds()const{return l1Seeds_;}
  
  bool operator<(const SHTrigResult& rhs)const{return bitNr_<rhs.bitNr_;}

  template<typename CompFunc=std::less<size_t> >
  class Sorter {
  public:
    bool operator()(const SHTrigResult& lhs,const SHTrigResult&rhs)const{CompFunc func;return func(lhs.bitNr(),rhs.bitNr());}
    bool operator()(const SHTrigResult& lhs,const size_t rhs)const{CompFunc func;return func(lhs.bitNr(),rhs);}
    bool operator()(const size_t lhs,const SHTrigResult rhs)const{CompFunc func;return func(lhs,rhs.bitNr());}
  };
  
  ClassDef(SHTrigResult,2)

};


#endif
