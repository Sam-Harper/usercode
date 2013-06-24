#include "SHarper/SHNtupliser/interface/SHL1CaloTowerContainer.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

ClassImp(SHL1CaloTowerContainer)

const SHL1CaloTower SHL1CaloTowerContainer::nullTower_;

SHL1CaloTowerContainer::SHL1CaloTowerContainer():
  caloTowerArray_("SHL1CaloTower",1100){}

SHL1CaloTowerContainer::SHL1CaloTowerContainer(const SHL1CaloTowerContainer& rhs):
  caloTowerArray_(rhs.caloTowerArray_){}

SHL1CaloTowerContainer& SHL1CaloTowerContainer::operator=(const SHL1CaloTowerContainer& rhs)
{
  if(&rhs!=this){
    TempFuncs::copyTClonesArray<SHL1CaloTower>(caloTowerArray_,rhs.caloTowerArray_);
  }
  return *this;
}

void SHL1CaloTowerContainer::addTower(const SHL1CaloTower& tower)
{
  new(caloTowerArray_[nrCaloTowersStored()]) SHL1CaloTower(tower);

}  


//this function fills the temporary vector with a size equal to all crystals in the calorimeter
//then for each hit stored, we hash the detid to get the location it would have in the vector
//then we set that location in the vector with the index of the crystal
//this allows us to do fast constant time lookups via detid with only a moderate
//performance hit to the first loop up
void SHL1CaloTowerContainer::createTowerIndxTable_()const
{
  //first we use the swap trick to make the vector is the right size
  // and all initialised to -1
  std::vector<int> tempIndx(DetIdTools::kNrL1CaloTowers,-1);
  towerIndxTable_.swap(tempIndx);
  
 
  for(int towerNr=0;towerNr<=caloTowerArray_.GetLast();towerNr++){
    const SHL1CaloTower& tower = getCaloTowerByIndx(towerNr);
   
    int towerIndex = DetIdTools::calHashL1Calo(tower.iEta(),tower.iPhi());
    if(towerIndex<0 || towerIndex>=DetIdTools::kNrL1CaloTowers) LogErr <<" Warning: towerIndex "<<towerIndex <<" max "<< DetIdTools::kNrCaloTowers<<" iEta "<<tower.iEta()<<" iPhi "<<tower.iPhi()<<std::endl;
    else towerIndxTable_[towerIndex] = towerNr;
  }  
}

const SHL1CaloTower& SHL1CaloTowerContainer::getCaloTower(int iEta,int iPhi)const
{
  
  int hash = DetIdTools::calHashL1Calo(iEta,iPhi);
  if(hash<0) return nullTower_;
  
  if(towerIndxTable_.empty()) createTowerIndxTable_();
  if(static_cast<size_t>(hash)>=towerIndxTable_.size()) return nullTower_;
  
  int indx = towerIndxTable_[hash];
  if(indx<0) return nullTower_;
  else return getCaloTowerByIndx(indx);

}
