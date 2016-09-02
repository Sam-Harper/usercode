#include "SHarper/SHNtupliser/interface/TrigMenuDataMgr.hh"

#include "TList.h"

const SHTrigMenuData& TrigMenuDataMgr::get(const std::string& menuName,const std::string& procName)const
{
  auto res=menuData_.find({menuName,procName});
  if(res!=menuData_.end()) return res->second;
  else return dummyData_;
}

void TrigMenuDataMgr::read(TTree* tree)
{
  
  TList* userInfo = tree->GetUserInfo();
  TIter next(userInfo);
  while(TObject *obj = next()){
    if(obj->ClassName()==std::string("SHTrigMenuData")){
      auto shObj = static_cast<SHTrigMenuData*>(obj);
      menuData_.insert({*shObj,*shObj});
    }
  }
}

void TrigMenuDataMgr::write(TTree* tree)const
{
  TList* userInfo = tree->GetUserInfo();
  auto alreadyStoredKeys=getStoredKeys_(tree);
  for(auto& data : menuData_){
    if(alreadyStoredKeys.find(data.first)==alreadyStoredKeys.end()){
      userInfo->Add(new SHTrigMenuData(data.second));
    }
  } 
}

std::unordered_set<TrigMenuDataMgr::Key,TrigMenuDataMgr::Hasher> 
TrigMenuDataMgr::getStoredKeys_(TTree* tree)const
{
  std::unordered_set<Key,Hasher> keys;
  TList* userInfo = tree->GetUserInfo();

  TIter next(userInfo);
  while(TObject *obj = next()){
    if(obj->ClassName()==std::string("SHTrigMenuData")){
      auto shObj = static_cast<SHTrigMenuData*>(obj);
      keys.insert(*shObj);
    }
  }
  return keys;
}
