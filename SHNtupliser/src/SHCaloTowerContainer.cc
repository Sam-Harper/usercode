#include "SHarper/SHNtupliser/interface/SHCaloTowerContainer.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

ClassImp(SHCaloTowerContainer)

const SHCaloTower SHCaloTowerContainer::nullTower_;

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


//this function fills the temporary vector with a size equal to all crystals in the calorimeter
//then for each hit stored, we hash the detid to get the location it would have in the vector
//then we set that location in the vector with the index of the crystal
//this allows us to do fast constant time lookups via detid with only a moderate
//performance hit to the first loop up
void SHCaloTowerContainer::createTowerIndxTable_()const
{
  //first we use the swap trick to make the vector is the right size
  // and all initialised to -1
  std::vector<int> tempIndx(DetIdTools::kNrCaloTowers,-1);
  towerIndxTable_.swap(tempIndx);
  
 
  for(int towerNr=0;towerNr<=caloTowerArray_.GetLast();towerNr++){
    const SHCaloTower& tower = getCaloTowerByIndx(towerNr);
    if(DetIdTools::iEtaAbsCalo(tower.detId())<=29){
      int towerIndex = DetIdTools::calHashCalo(tower.detId());
      if(towerIndex<0 || towerIndex>=DetIdTools::kNrCaloTowers) LogErr <<" Warning: towerIndex "<<towerIndex <<" max "<< DetIdTools::kNrCaloTowers<<" iEta "<<DetIdTools::iEtaAbsCalo(tower.detId())<<" iPhi "<<DetIdTools::iPhiCalo(tower.detId())<<" eta "<<tower.eta()<<std::endl;
      else towerIndxTable_[towerIndex] = towerNr;
    } 
  }

}

const SHCaloTower& SHCaloTowerContainer::getCaloTower(int detId)const
{
  
  int hash = DetIdTools::calHashCalo(detId);
  if(hash<0) return nullTower_;
  
  if(towerIndxTable_.empty()) createTowerIndxTable_();
  if(static_cast<size_t>(hash)>=towerIndxTable_.size()) return nullTower_;
  
  int indx = towerIndxTable_[hash];
  if(indx<0) return nullTower_;
  else return getCaloTowerByIndx(indx);


}
