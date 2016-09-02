#include "SHarper/SHNtupliser/interface/SHBitsDef.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

#include <limits>
#include <boost/algorithm/string.hpp>

ClassImp(SHBitsDef)

size_t SHBitsDef::getBitNr(const std::string& bitName)const
{
  auto result = std::equal_range(strToBitNr_.begin(),strToBitNr_.end(),bitName,KeyComp());
  if(result.first!=result.second){
    if(std::distance(result.first,result.second)!=1){
      LogErr<<" Warning, multiple entries found for bit "<<bitName<<std::endl;
    }
    return result.first->second;
  }else return std::numeric_limits<size_t>::max();
}

TBits SHBitsDef::getBits(const std::string& bitNames)const
{  
  std::vector<std::string> bitNamesVec;
  boost::split(bitNamesVec,bitNames,boost::is_any_of(":"));
  return getBits(bitNamesVec); 
}

TBits SHBitsDef::getBits(const std::vector<std::string>& bitNames)const
{ 
  TBits bits;
  for(auto& bitName : bitNames){
    size_t bitNr=getBitNr(bitName);
    if(bitNr<std::numeric_limits<size_t>::max()){
      bits.SetBitNumber(bitNr);
    }
  }
  return bits;
}

//so i decided to let the caller ensure they had a unique set of
//names and enforced this by having them pass a std::set
//otherwise passing them in one at time would have been messy
void SHBitsDef::setBitsDef(const std::set<std::string>& bitNames)
{
  clear();
  size_t bitNr=0;
  for(auto& bitName : bitNames){
    strToBitNr_.push_back(std::pair<std::string,int>(bitName,bitNr));
    bitNr++;
  }
  //not strictly necessary as the set should be sorted but just in case we end up sorting differently for
  //some werid reason
  std::sort(strToBitNr_.begin(),strToBitNr_.end(),KeyComp());
  
}


void SHBitsDef::setBitsDef(std::vector<std::string> bitNames)
{
  clear();

  //sort and remove dups from our names
  std::sort(bitNames.begin(),bitNames.end(),StrComp());
  auto lastNonDup = std::unique(bitNames.begin(),bitNames.end());
  bitNames.erase(lastNonDup,bitNames.end()); //not necessary in theory as could just loop to this point but a mild pain...

  size_t bitNr=0;
  for(auto& bitName : bitNames){
    strToBitNr_.push_back(std::pair<std::string,int>(bitName,bitNr));
    bitNr++;
  }
  //not strictly necessary as the set should be sorted but just in case we end up sorting differently for
  //some werid reason
  std::sort(strToBitNr_.begin(),strToBitNr_.end(),KeyComp());
  
}

void SHBitsDef::buildBitNrToStrMap_()const
{
  bitNrToStr_.clear(); 
  bitNrToStr_.resize(strToBitNr_.size());
  for(auto& entry : strToBitNr_){
    if(entry.second<bitNrToStr_.size()) bitNrToStr_.resize(entry.second+1);
    bitNrToStr_[entry.second]=entry.first;
  }
				      
}
