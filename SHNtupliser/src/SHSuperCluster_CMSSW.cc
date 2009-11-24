#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"


#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"


//note the seed cluster must be the first entry of the shape map
SHSuperCluster::SHSuperCluster(const reco::SuperCluster& superClus,const SHCaloHitContainer& hits):
  nrgy_(superClus.rawEnergy()),
  preShowerNrgy_(superClus.preshowerEnergy()),
  pos_(superClus.position().X(),superClus.position().Y(),superClus.position().Z()), 
  eta_(pos_.Eta()),
  nrCrys_(superClus.hitsAndFractions().size()),
  clusterArray_("SHBasicCluster",4)
{
  
  for(reco::CaloCluster_iterator clusIt  = superClus.clustersBegin();clusIt!=superClus.clustersEnd();++clusIt){
    new(clusterArray_[nrClus()]) SHBasicCluster(**clusIt,hits);
  }
  
  

}
