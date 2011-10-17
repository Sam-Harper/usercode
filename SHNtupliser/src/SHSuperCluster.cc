#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"
#include <cmath>
#include <algorithm>
ClassImp(SHSuperCluster)

SHSuperCluster::SHSuperCluster():
  nrgy_(0.),preShowerNrgy_(0.),
  pos_(0.,0.,0.),eta_(0.),
  nrCrys_(0),
  clusterArray_("SHBasicCluster",4)
{

}

SHSuperCluster::SHSuperCluster(const SHSuperCluster& rhs):
  nrgy_(rhs.nrgy_),preShowerNrgy_(rhs.preShowerNrgy_),
  pos_(rhs.pos_),eta_(rhs.eta_),
  nrCrys_(rhs.nrCrys_),
  clusterArray_("SHBasicCluster",4)
{
  //for some unknown reason I cant auto copy the array across, something to do with the clone function
  for(int clusNr=0;clusNr<rhs.nrClus();clusNr++){
    new(clusterArray_[nrClus()]) SHBasicCluster(*rhs.getClus(clusNr));
  }
  
}

SHSuperCluster::SHSuperCluster(const std::vector<SHBasicCluster>& basicClusters):
  nrgy_(0.),preShowerNrgy_(0.),
  pos_(0.,0.,0.),eta_(0.),
  nrCrys_(0),
  clusterArray_("SHBasicCluster",4)
{
  //float etaSum=0;
  // float phiSum=0;
  //std::cout <<"start sc "<<std::endl;
  for(size_t clusNr=0;clusNr<basicClusters.size();clusNr++){
    new(clusterArray_[nrClus()]) SHBasicCluster(basicClusters[clusNr]);
    nrgy_+= basicClusters[clusNr].nrgy();
    TVector3 nrgyWeightedPos = basicClusters[clusNr].pos();
    //  std::cout <<"eta "<<nrgyWeightedPos.Eta()<<" phi "<<nrgyWeightedPos.Eta()<<" nrgy "<<basicClusters[clusNr].nrgy()<<std::endl;
    nrgyWeightedPos*=basicClusters[clusNr].nrgy();
    pos_+=nrgyWeightedPos;
    nrCrys_+=basicClusters[clusNr].nrCrys();
  }
  pos_*=1./nrgy_;
  eta_=pos_.Eta();
  // pos_.SetPtEtaPhi(1,etaSum,phiSum);
  
}

//because roots memory ownership doc is woefully lacking, I'm deleting here to make sure
SHSuperCluster::~SHSuperCluster()
{
  clusterArray_.Delete();
 
}

const SHBasicCluster* SHSuperCluster::getClus(int clusNr)const
{
  SHBasicCluster* clus = (SHBasicCluster*) clusterArray_[clusNr];
  return clus;
}


const SHBasicCluster* SHSuperCluster::seedClus()const
{
  double maxNrgy=0.;
  const SHBasicCluster* seedClus = NULL;
  for(int clusNr=0;clusNr<nrClus();clusNr++){
    const SHBasicCluster* clus = getClus(clusNr);
    if(clus->nrgy()>maxNrgy){
      maxNrgy = clus->nrgy();
      seedClus = clus;
    }
  }
  return seedClus;

}



float SHSuperCluster::rawNrgy()const
{
  float rawNrgy=0.;
  for(int clusNr=0;clusNr<nrClus();clusNr++){
    rawNrgy +=getClus(clusNr)->totNrgy();
  }
  return rawNrgy;
}




//loop over all the clusters of the supercluster and add the hits to the vector
void SHSuperCluster::getHitsByDetId(std::vector<int>& hitDetIds)const
{
  hitDetIds.clear();
  for(int clusNr=0;clusNr<nrClus();clusNr++){
    const SHBasicCluster* clus = getClus(clusNr);
    std::vector<int> clusHits = clus->getHitsByDetId();
    //I know, I know i should do this via stl algorthims
    for(size_t i=0;i<clusHits.size();i++) hitDetIds.push_back(clusHits[i]);
  }
  std::sort(hitDetIds.begin(),hitDetIds.end());
}
