#include "SHarper/SHNtupliser/interface/TrigMenuMgr.hh"

#include "TList.h"

const SHL1Menu TrigMenuMgr::dummyL1Menu_;
const SHHLTMenu TrigMenuMgr::dummyHLTMenu_;

const SHL1Menu& TrigMenuMgr::getL1(const std::string& menuName)const
{
  auto res=l1Menus_.find(menuName);
  if(res!=l1Menus_.end()) return res->second;
  else return dummyL1Menu_;
}

const SHHLTMenu& TrigMenuMgr::getHLT(const std::string& menuName,const std::string& procName)const
{
  auto res=hltMenus_.find({menuName,procName});
  if(res!=hltMenus_.end()) return res->second;
  else return dummyHLTMenu_;
}

bool TrigMenuMgr::hasL1Menu(const std::string& menuName)const
{
  return l1Menus_.find(menuName)!=l1Menus_.end();
}

bool TrigMenuMgr::hasHLTMenu(const std::string& menuName,const std::string& procName)const
{
  return hltMenus_.find({menuName,procName})!=hltMenus_.end();
}

void TrigMenuMgr::read(TTree* tree)
{
  
  TList* userInfo = tree->GetUserInfo();
  TIter next(userInfo);
  while(TObject *obj = next()){
    if(obj->ClassName()==std::string("SHL1Menu")){
      auto shObj = static_cast<SHL1Menu*>(obj);
      shObj->clearCache(); //just in case of root shenanigans
      l1Menus_.insert({*shObj,*shObj});
    }
    if(obj->ClassName()==std::string("SHHLTMenu")){
      auto shObj = static_cast<SHHLTMenu*>(obj);
      shObj->clearCache(); //just in case of root shenanigans
      hltMenus_.insert({*shObj,*shObj});
    }
  }
}

void TrigMenuMgr::write(TTree* tree)const
{
  TList* userInfo = tree->GetUserInfo();
  auto alreadyStoredL1Keys=getStoredKeys_<SHL1Menu,L1Key,L1Hasher>(tree,"SHL1Menu");
  for(auto& data : l1Menus_){
    if(alreadyStoredL1Keys.find(data.first)==alreadyStoredL1Keys.end()){
      userInfo->Add(new SHL1Menu(data.second));
    }
  }
  auto alreadyStoredHLTKeys=getStoredKeys_<SHHLTMenu,HLTKey,HLTHasher>(tree,"SHHLTMenu");
  for(auto& data : hltMenus_){
    if(alreadyStoredHLTKeys.find(data.first)==alreadyStoredHLTKeys.end()){
      userInfo->Add(new SHHLTMenu(data.second));
    }
  } 
}
