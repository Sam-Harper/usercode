#include "SHarper/SHNtupliser/interface/SHCaloTowerContainer.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

ClassImp(SHCaloTowerContainer)

SHCaloTowerContainer::SHCaloTowerContainer():
  caloTowerArray_("SHCaloTower",1100){}

SHCaloTowerContainer::SHCaloTowerContainer(const SHCaloTowerContainer& rhs):
  caloTowerArray_(rhs.caloTowerArray_){}

SHCaloTowerContainer& SHCaloTowerContainer::operator=(const SHCaloTowerContainer& rhs)
{
  if(&rhs!=this){
    TempFuncs::copyTClonesArray<SHCaloTower>(caloTowerArray_,rhs.caloTowerArray_);
  }
  return *this;
}

void SHCaloTowerContainer::addTower(int id,float emNrgy,float hadD1Nrgy,float hadD2Nrgy,float eta,float phi)
{
  new(caloTowerArray_[nrCaloTowersStored()]) SHCaloTower(id,emNrgy,hadD1Nrgy,hadD2Nrgy,eta,phi);
}

void SHCaloTowerContainer::addTower(const SHCaloTower& tower)
{
  new(caloTowerArray_[nrCaloTowersStored()]) SHCaloTower(tower);

}  
