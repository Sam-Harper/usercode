#include "SHarper/SHNtupliser/interface/SHL1CaloTower.hh"

ClassImp(SHL1CaloTower)

//iEta = input eta not iEta..
  SHL1CaloTower::SHL1CaloTower(int iIEta,int iIPhi,int iEMEtTS,int iHadEtTS,bool iECALFG,bool iHCALFG,int iTrigScale):
    iEta_(iIEta),
    iPhi_(iIPhi),
    emEtTS_(iEMEtTS),
    hadEtTS_(iHadEtTS),
    ecalFG_(iECALFG),
    hcalFG_(iHCALFG),
    trigScale_(iTrigScale)
{

}
