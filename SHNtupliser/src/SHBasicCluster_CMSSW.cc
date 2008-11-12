#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"
 

#include "DataFormats/EgammaReco/interface/BasicCluster.h"

SHBasicCluster::SHBasicCluster(const reco::BasicCluster& clus,const SHCaloHitContainer& hits):
  totNrgy_(clus.energy()),
  nrCrys_(clus.getHitsByDetId().size()),
  eMaxId_(0),
  e2ndId_(0),
  eta_(clus.position().eta()),
  phi_(clus.position().phi()), 
  hitDetIds_()
{
  
  std::pair<float,int> nrgyIdMax(-1.,0);
  std::pair<float,int> nrgyId2nd(-1.,0);

  std::vector<DetId> clusDetIds(clus.getHitsByDetId());
  hitDetIds_.reserve(clusDetIds.size());
  for(size_t i=0;i<clusDetIds.size();i++){
    hitDetIds_.push_back(clusDetIds[i].rawId());
    float hitNrgy = hits.getHitNrgy(clusDetIds[i].rawId());
    if(hitNrgy > nrgyIdMax.first) {
      nrgyId2nd = nrgyIdMax;
      nrgyIdMax.first = hitNrgy;
      nrgyIdMax.second = clusDetIds[i].rawId();
    }else if(hitNrgy > nrgyId2nd.first){
      nrgyId2nd.first = hitNrgy;
      nrgyId2nd.second = clusDetIds[i].rawId();
    }
  }

  eMaxId_=nrgyIdMax.second;
  e2ndId_=nrgyId2nd.second;
}
