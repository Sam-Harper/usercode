#ifndef SHEVENT_SHBITSDEF_HH
#define SHEVENT_SHBITSDEF_HH

#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

#include "TBits.h"

#include <vector>
#include <string>
#include <set>

/* Class Description:    
**  maps std::strings to bit numbers
**  useful for HLT work (what it was orginally designed for)
**  class is intended to be written once and read many times
*/

class SHBitsDef {

private:
  std::vector<std::pair<std::string,size_t> > strToBitNr_;
  mutable std::vector<std::string> bitNrToStr_; //! a cache which is constructed from the first one
  
  typedef TempFuncs::PairSortBy1st<std::string,int,std::less<std::string> > KeyComp;

public:
  SHBitsDef(){}
  virtual ~SHBitsDef(){}

  size_t getBitNr(const std::string& bitName)const; //if not found, returns max value (this may change to max-value -1)
  TBits getBits(const std::string& bitNames)const;
  TBits getBits(const std::vector<std::string>& bitNames)const;
  std::string getBitName(size_t bitNr)const{//toying with a reference or not, problem is that it'll go out of scope on the next change and you might keep the name around
    for(auto& bitDef : strToBitNr_){
      if(bitDef.second==bitNr) return bitDef.first;
    }
    return "";
    //if(cacheOutOfDate_()) buildBitNrToStrMap_();
    //return bitNr<bitNrToStr_.size() ? bitNrToStr_[bitNr] : "";
  }
  std::vector<std::string> getBitNames(const TBits& bits)const{
    std::vector<size_t> bitNrs;
    for(size_t bitNr=bits.FirstSetBit();bitNr<=bits.LastSetBit();bitNr++){
      if(bits.TestBitNumber(bitNr)) bitNrs.push_back(bitNr);
    }
    return getBitNames(bitNrs);
  }
  
  std::vector<std::string> getBitNames(const std::vector<size_t>& bitNrs)const{
    std::vector<std::string> names;
    for(size_t bitNr : bitNrs) names.push_back(getBitName(bitNr));
    return names;
  }
      
  const std::vector<std::pair<std::string,size_t> >& data()const{return strToBitNr_;}
  void setBitsDef(const std::set<std::string>& bitNames);
  void clear(){strToBitNr_.clear();bitNrToStr_.clear();}

private:
  void buildBitNrToStrMap_()const;
  bool cacheOutOfDate_()const{return bitNrToStr_.empty();}
  
  ClassDef(SHBitsDef,1)
};



#endif
