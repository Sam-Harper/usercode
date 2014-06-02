#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"

//I detest resorting to this but its the easiest option
#define SLHCBUILD 0
#if SLHCBUILD
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1TCalorimeter/interface/CaloCluster.h"

SHL1Cluster::SHL1Cluster(const l1t::EGamma& egamma,const l1t::CaloCluster& clus):
  etTS_(clus.hwPt()),
  emEtOverTotEt_(0),
  iEta_(clus.hwEta()),iPhi_(clus.hwPhi()),
  isolEmEtTS_(clus.hwIso()),isolHadEtTS_(0),
  trigScale_(2), //best guess
  fgVeto_(0),
  isIsolated_(0),
  p4_(),
  leadTower_(-1),
  constituents_(0),
  hademTS_(clus.hOverE()),
  clusterFlags_(clus.clusterFlags()),
  etSeedTS_(clus.hwSeedPt())
{
  //p4_.SetPtEtaPhiM(clus.p4().Et(),clus.p4().Eta(),clus.p4().Phi(),0);
  p4_.SetPtEtaPhiM(0.0001,0,0,0);

}

SHL1Cluster::SHL1Cluster(const l1t::CaloCluster& clus):
  etTS_(clus.hwPt()),
  emEtOverTotEt_(0),
  iEta_(clus.hwEta()),iPhi_(clus.hwPhi()),
  isolEmEtTS_(-1),isolHadEtTS_(-1),
  trigScale_(2), //best guess
  fgVeto_(0),
  isIsolated_(0),
  p4_(),
  leadTower_(-1),
  constituents_(0),
  hademTS_(clus.hOverE()),
  clusterFlags_(clus.clusterFlags()),
  etSeedTS_(clus.hwSeedPt())
{
  //  p4_.SetPtEtaPhiM(clus.p4().Et(),clus.p4().Eta(),clus.p4().Phi(),0);
  p4_.SetPtEtaPhiM(0.0001,0,0,0);
}

#else

namespace l1t{
  class EGamma{};
}

SHL1Cluster::SHL1Cluster(const l1t::EGamma& egamma,const l1t::CaloCluster& clus)
{
  std::cout <<"Error, disabled for this build, clusters are not being filled"<<std::endl;
}
SHL1Cluster::SHL1Cluster(const l1t::CaloCluster& clus)
{
 std::cout <<"Error, disabled for this build, clusters are not being filled"<<std::endl;

}
#endif
