#include "SHarper/SHNtupliser/interface/SHGainSwitchInfo.hh"
ClassImp(SHGainSwitchInfo)

void SHGainSwitchInfo::setFixInfo(bool dupSC,bool hasGSElePho,std::vector<int> gsHitsUnfixed)
{
  dupSC_=dupSC;
  hasGSElePho_=hasGSElePho;
  gsHitsUnfixed_=gsHitsUnfixed;
}

void SHGainSwitchInfo::clear()
{
  dupSC_=false;
  gsHitsUnfixed_.clear();
  hasGSElePho_=false;
  metOrg_=-999.0;
  metPhiOrg_=0.;
  metJustEGFix_=-999.0;
  metPhiJustEGFix_=0.;
  
}
