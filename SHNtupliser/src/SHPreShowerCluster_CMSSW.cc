#include "SHarper/SHNtupliser/interface/SHPreShowerCluster.hh"

#include "DataFormats/EgammaReco/interface/PreshowerCluster.h"

SHPreShowerCluster::SHPreShowerCluster(const reco::PreshowerCluster& clus):
  nrgy_(clus.energy()),
  pos_(clus.position().X(),clus.position().Y(),clus.position().Z()), 
  eta_(pos_.Eta()),
  plane_(clus.plane())
{
  const std::vector<std::pair<DetId,float> >& clusDetIds = clus.hitsAndFractions();
  hits_.reserve(clusDetIds.size());
  for(size_t hitNr=0;hitNr<clusDetIds.size();hitNr++){
    hits_.push_back(clusDetIds[hitNr].first.rawId());
  }
}
  
