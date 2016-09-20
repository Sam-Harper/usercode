#include "SHarper/SHNtupliser/interface/SHSuperCluster.hh"
#include <cmath>
#include <algorithm>
ClassImp(SHSuperCluster)

SHSuperCluster::SHSuperCluster():
  nrgy_(0.),preShowerNrgy_(0.),
  pos_(0.,0.,0.),eta_(0.),
  nrCrys_(0),
  clusterArray_("SHBasicCluster",4),
  flags_(0)
			      //preShowerArray_("SHPreShowerCluster",10)
{

}

SHSuperCluster::SHSuperCluster(const SHSuperCluster& rhs):
  nrgy_(rhs.nrgy_),preShowerNrgy_(rhs.preShowerNrgy_),
  pos_(rhs.pos_),eta_(rhs.eta_),
  nrCrys_(rhs.nrCrys_),
  clusterArray_("SHBasicCluster",4),
  flags_(rhs.flags_)
			      //preShowerArray_("SHPreShowerCluster",10)
{
  //for some unknown reason I cant auto copy the array across, something to do with the clone function
  for(int clusNr=0;clusNr<rhs.nrClus();clusNr++){
    new(clusterArray_[nrClus()]) SHBasicCluster(*rhs.getClus(clusNr));
  }
 //  for(int clusNr=0;clusNr<rhs.nrPreShowerClus();clusNr++){
//     new(preShowerArray_[nrPreShowerClus()]) SHPreShowerCluster(*rhs.getPreShowerClus(clusNr));
//   }
}

SHSuperCluster::SHSuperCluster(const std::vector<SHBasicCluster>& basicClusters):
  nrgy_(0.),preShowerNrgy_(0.),
  pos_(0.,0.,0.),eta_(0.),
  nrCrys_(0),
  clusterArray_("SHBasicCluster",4),
  flags_(0)
			      // preShowerArray_("SHPreShowerCluster",10)
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
  //  preShowerArray_.Delete();
}

const SHBasicCluster* SHSuperCluster::getClus(int clusNr)const
{
  SHBasicCluster* clus = (SHBasicCluster*) clusterArray_[clusNr];
  return clus;
}

// const SHPreShowerCluster* SHSuperCluster::getPreShowerClus(int clusNr)const
// {
//   SHPreShowerCluster* clus = (SHPreShowerCluster*) preShowerArray_[clusNr];
//   return clus;
// }


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

std::ostream& SHSuperCluster::print(std::ostream& output)const
{
  output<<"supercluster nrgy: "<<nrgy()<<" preshow nrgy "<<preShowerNrgy()<<" et "<<et()<<" eta "<<eta()<<" phi "<<phi()<<std::endl;
  output<<"nr basic clusters "<<nrClus()<<" nr crys "<<nrCrys()<<std::endl;//<<" nr ps clusters "<<nrPreShowerClus()<<std::endl;
  //  for(int clusNr=0;clusNr<nrPreShowerClus();clusNr++){
    //   output <<" ps clus "<<clusNr<<" "<<*(getPreShowerClus(clusNr))<<std::endl;
  //}
  return output;


}
std::ostream &operator <<(std::ostream& output,const SHSuperCluster& clus)
{
  return clus.print(output);
}
