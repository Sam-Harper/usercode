#include "SHarper/SHNtupliser/interface/SHCaloTower.hh"

ClassImp(SHCaloTower)

//iEta = input eta not iEta..
SHCaloTower::SHCaloTower(int iId,float iEMNrgy,float iHadD1Nrgy,float iHadD2Nrgy,float iEta,float iPhi):
  towerId_(iId),emNrgy_(iEMNrgy),hadD1Nrgy_(iHadD1Nrgy),hadD2Nrgy_(iHadD2Nrgy),
  eta_(iEta),phi_(iPhi){}  

