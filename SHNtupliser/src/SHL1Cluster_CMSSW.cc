#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"

//I detest resorting to this but its the easiest option
#define SLHCBUILD 1
#if SLHCBUILD
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1TCalorimeter/interface/CaloCluster.h"

SHL1Cluster::SHL1Cluster(const l1t::EGamma& clus):
  etTS_(clus.hwPt()),
  emEtOverTotEt_(0),
  iEta_(clus.hwEta()),iPhi_(clus.hwPhi()),
  isolEmEtTS_(clus.hwIso()),isolHadEtTS_(0),
  trigScale_(2), //best guess
  isIsolated_(clus.hwIso()),
  p4_(),
  hademTS_(0),
  clusterFlags_(0),
  etSeedTS_(0),
  quality_(clus.hwQual())
{
  p4_.SetPtEtaPhiM(clus.p4().Et(),clus.p4().Eta(),clus.p4().Phi(),0);

}

SHL1Cluster::SHL1Cluster(const l1t::EGamma& egamma,const l1t::CaloCluster& clus):
  etTS_(clus.hwPt()),
  emEtOverTotEt_(0),
  iEta_(clus.hwEta()),iPhi_(clus.hwPhi()),
  isolEmEtTS_(egamma.hwIso()),isolHadEtTS_(0),
  trigScale_(2), //best guess
  isIsolated_(egamma.hwIso()),
  p4_(),
  hademTS_(clus.hOverE()),
  clusterFlags_(clus.clusterFlags()),
  etSeedTS_(clus.hwSeedPt()),
  quality_(egamma.hwQual())
{
  p4_.SetPtEtaPhiM(egamma.p4().Et(),egamma.p4().Eta(),egamma.p4().Phi(),0);
 
}


#else

namespace l1t{
  class EGamma{};
}

SHL1Cluster::SHL1Cluster(const l1t::EGamma& egamma,const l1t::CaloCluster& clus)
{
  std::cout <<"Error, disabled for this build, clusters are not being filled"<<std::endl;
}
SHL1Cluster::SHL1Cluster(const l1t::EGamma& egamma)
{
 std::cout <<"Error, disabled for this build, clusters are not being filled"<<std::endl;

}
#endif

