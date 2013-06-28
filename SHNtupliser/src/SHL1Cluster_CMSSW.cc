#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"

namespace l1slhc{
  class L1CaloCluster{};
}

SHL1Cluster::SHL1Cluster(const l1slhc::L1CaloCluster& clus){}
// #include "SimDataFormats/SLHC/interface/L1CaloCluster.h"

// SHL1Cluster::SHL1Cluster(const l1slhc::L1CaloCluster& clus):
//   etTS_(clus.E()),
//   emEtOverTotEt_(clus.eGammaValue()),
//   iEta_(clus.iEta()),iPhi_(clus.iPhi()),
//   isolEmEtTS_(clus.isoEnergyEG()),isolHadEtTS_(clus.isoEnergyTau()),
//   trigScale_(2), //best guess
//   fgVeto_(clus.fg()),
//   isIsolated_(clus.isoEG()),
//   p4_(),
//   leadTower_(-1),
//   constituents_(0)
// {
//   p4_.SetPtEtaPhiM(clus.p4().Et(),clus.p4().Eta(),clus.p4().Phi(),0);

//   int leadTowerLEta=0; //LEta  = local eta, ie an offset from seed tower
//   int leadTowerLPhi=0;
//   int highestEmEt = 0;
//   int bit=0x1;
//   for(int lEta=0;lEta<=1;lEta++){
//     for(int lPhi=0;lPhi<=1;lPhi++){
//       int index=clus.hasConstituent(lEta,lPhi);
//       int towerEmEt = index>=0 ? clus.getConstituent(index)->E() : 0;
//       if(towerEmEt>highestEmEt){ //logic here (tie for same et towers) mimics isolation algo for lead tower
// 	highestEmEt = towerEmEt;
// 	leadTowerLEta=lEta;
// 	leadTowerLPhi=lPhi;
//       }
//       if(index>0) constituents_|=bit;
//       bit = bit<<1;

//     }
//   }
//   leadTower_=leadTowerLEta;
//   leadTower_|= (leadTowerLPhi<<1);
  
// }

