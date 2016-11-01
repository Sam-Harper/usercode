#include "SHarper/SHNtupliser/interface/SHL1Menu.hh"

#include "SHarper/SHNtupliser/interface/LogErr.hh"

ClassImp(SHL1Menu)

const SHL1Menu::Seed SHL1Menu::nullSeed_;

size_t SHL1Menu::l1BitNr(const std::string& l1Name)const
{
  if(cacheOutOfDate_()) buildPathNameToNrMap_();
  
  auto res = std::equal_range(l1NamesToBits_.begin(),l1NamesToBits_.end(),l1Name,KeyComp());
  int nrFound = std::distance(res.first,res.second);
  if(nrFound==0) return l1Seeds_.size();
  else{
    if(nrFound>1) LogErr<<" warning, "<<nrFound<<" entries found for "<<l1Name<<std::endl;
    return res.first->second;
  }
}

void SHL1Menu::clear()
{
  clearCache();
  menuName_.clear();
  l1Seeds_.clear();
}

void SHL1Menu::buildPathNameToNrMap_()const
{
  l1NamesToBits_.clear();
  l1NamesToBits_.reserve(l1Seeds_.size());
  for(size_t bitNr=0;bitNr<l1Seeds_.size();bitNr++){
    if(l1Seeds_[bitNr].bitNr()!=bitNr){
      LogErr <<" seed at index "<<bitNr<<" ("<<l1Seeds_[bitNr].name()<<") has bit nr "<<l1Seeds_[bitNr].bitNr()<<", this is broken"<<std::endl;
   }
    l1NamesToBits_.push_back({l1Seeds_[bitNr].name(),bitNr});
  }
  std::sort(l1NamesToBits_.begin(),l1NamesToBits_.end(),KeyComp());
}
