#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"


#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/PreshowerCluster.h"

//note the seed cluster must be the first entry of the shape map
SHSuperCluster::SHSuperCluster(const reco::SuperCluster& superClus):
  nrgy_(superClus.energy()),
  preShowerNrgy_(superClus.preshowerEnergy()),
  rho_(superClus.position().rho()),
  eta_(superClus.position().eta()),
  phi_(superClus.position().phi()),
  nrCrys_(superClus.hitsAndFractions().size()),
  clusterArray_("SHBasicCluster",4),
  flags_(superClus.flags())
  //preShowerArray_("SHPreShowerCluster",10)
{
  if(superClus.clusters().isAvailable()){
    for(auto clusIt  = superClus.clustersBegin();clusIt!=superClus.clustersEnd();++clusIt){
      new(clusterArray_[nrClus()]) SHBasicCluster(**clusIt);
    }
  }else{ //just the seed was stored in the CMSSW dataformat
    new(clusterArray_[nrClus()]) SHBasicCluster(*superClus.seed());
  }
    

}
