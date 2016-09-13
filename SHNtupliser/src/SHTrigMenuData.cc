#include "SHarper/SHNtupliser/interface/SHTrigMenuData.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSummary.hh"

ClassImp(SHTrigMenuData)

SHTrigMenuData::SHTrigMenuData(const SHTrigSummary& trigSum):
  menuName_(trigSum.menuName()),
  processName_(trigSum.processName()),
  globalTag_(trigSum.globalTag()),
  pathBitsDef_(trigSum.pathBitsDef()),
  filterBitsDef_(trigSum.filterBitsDef()),
  l1Names_(trigSum.l1Names())
{

}

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

