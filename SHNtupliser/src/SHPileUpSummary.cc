#include "SHarper/SHNtupliser/interface/SHPileUpSummary.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

ClassImp(SHPileUpSummary)


SHPileUpSummary::SHPileUpSummary():
  puInfos_("SHPileUpInfo",5)
{


}
SHPileUpSummary::SHPileUpSummary(const SHPileUpSummary& rhs):
  puInfos_("SHPileUpInfo",5)
{
  TempFuncs::copyTClonesArray<SHPileUpInfo>(puInfos_,rhs.puInfos_);
}

SHPileUpSummary& SHPileUpSummary::operator=(const SHPileUpSummary& rhs)
{
  if(this!=&rhs){
    TempFuncs::copyTClonesArray<SHPileUpInfo>(puInfos_,rhs.puInfos_);
  }
  return *this;
}

void SHPileUpSummary::addPUInfo(int iBx,int iNrInt,int iNrTrueInt)
{
  new(puInfos_[nrPUInfos()]) SHPileUpInfo(iBx,iNrInt,iNrTrueInt);
}

