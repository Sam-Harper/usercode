#include "SHarper/SHNtupliser/interface/SHTrigMenuData.hh"

ClassImp(SHTrigMenuData)

bool SHTrigMenuData::operator==(const SHTrigMenuData& rhs)const
{
  return menuName_==rhs.menuName_ && processName_==rhs.processName_;
}

bool SHTrigMenuData::operator<(const SHTrigMenuData& rhs)const
{
  if(menuName_<rhs.menuName_) return true;
  else if(menuName_>rhs.menuName_) return false;
  else return processName_<rhs.processName_;
}

