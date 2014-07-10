#include "SHarper/SHNtupliser/interface/SHPFCluster.hh"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
SHPFCluster::SHPFCluster(const reco::PFCluster& clus,int scId):
  superClusId_(scId),
  caloId_(clus.caloID().detector()),
  flags_(clus.flags()),
  seedId_(clus.seed()),
  nrgy_(clus.energy())
{
  pos_.SetXYZ(clus.x(),clus.y(),clus.z());
  
  const std::vector<std::pair<DetId,float> >& hitsAndFracs = clus.hitsAndFractions();
  for(size_t hitNr=0;hitNr<hitsAndFracs.size();hitNr++){
    detIdAndFracs_.push_back(std::pair<int,float>(hitsAndFracs[hitNr].first.rawId(),hitsAndFracs[hitNr].second));
  }


}
