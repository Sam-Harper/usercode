#include "SHarper/SHNtupliser/interface/SHHLTMenu.hh"

#include "SHarper/SHNtupliser/interface/LogErr.hh"

ClassImp(SHHLTMenu)

const SHHLTMenu::Path SHHLTMenu::nullPath_;

size_t SHHLTMenu::pathBitNr(const std::string& pathName)const
{
  if(cacheOutOfDate_()) buildPathNameToNrMap_();
  
  auto res = std::equal_range(pathNameToNrs_.begin(),pathNameToNrs_.end(),pathName,KeyComp());
  int nrFound = std::distance(res.first,res.second);
  if(nrFound==0) return paths_.size();
  else{
    if(nrFound>1) LogErr<<" warning, "<<nrFound<<" entries found for "<<pathName<<std::endl;
    return res.first->second;
  }
}

void SHHLTMenu::buildPathNameToNrMap_()const
{
  pathNameToNrs_.clear();
  pathNameToNrs_.reserve(paths_.size());
  for(size_t bitNr=0;bitNr<paths_.size();bitNr++){
    if(paths_[bitNr].pathNr()!=bitNr){
      LogErr <<" path at index "<<bitNr<<" ("<<paths_[bitNr].name()<<") has pathNr "<<paths_[bitNr].pathNr()<<", this is broken"<<std::endl;
    }
    pathNameToNrs_.push_back({paths_[bitNr].name(),bitNr}); 
  }
  std::sort(pathNameToNrs_.begin(),pathNameToNrs_.end(),KeyComp());
}
 
