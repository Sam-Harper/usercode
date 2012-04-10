#include "SHarper/SHNtupliser/interface/SHBasicCluster.hh"

#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"

#include <algorithm>

SHBasicCluster::SHBasicCluster(const reco::BasicCluster& clus,const SHCaloHitContainer& hits):
  totNrgy_(clus.energy()),
  nrCrys_(clus.hitsAndFractions().size()),
  eMaxId_(0),
  e2ndId_(0),
  eta_(clus.position().eta()),
  phi_(clus.position().phi()), 
  hitDetIds_(),
  seedId_(clus.seed()),//if its barrel, it doesnt have a seed unless its the seed cluster...
  hitInfo_(0) 
{
  
  std::pair<float,int> nrgyIdMax(-1.,0);
  std::pair<float,int> nrgyId2nd(-1.,0);

  const std::vector<std::pair<DetId,float> >& clusDetIds = clus.hitsAndFractions();
  std::vector<int> tempHitIds;
  tempHitIds.reserve(clusDetIds.size());
  //hitDetIds_.reserve(clusDetIds.size());
  std::vector<std::pair<int,float> > dominoNrgys;
  std::vector<int> iEtas;

  for(size_t i=0;i<clusDetIds.size();i++){
    // hitDetIds_.push_back(clusDetIds[i].first.rawId());
    tempHitIds.push_back(clusDetIds[i].first);
    float hitNrgy = hits.getHitNrgy(clusDetIds[i].first.rawId());
    
    //here we are attempting to figure out the "seed" crystal
    if(seedId_==0 && DetIdTools::isEcalBarrel(clusDetIds[i].first.rawId())){
      int iPhi = DetIdTools::iPhiBarrel(clusDetIds[i].first.rawId());
      std::vector<std::pair<int,float> >::iterator it = std::find_if(dominoNrgys.begin(),dominoNrgys.end(),TempFuncs::PairEq1st<int,float>(iPhi));
      
      if(it!=dominoNrgys.end()) it->second+=hitNrgy;
      else dominoNrgys.push_back(std::pair<int,float>(iPhi,hitNrgy));
      
      int iEta = DetIdTools::iEtaBarrel(clusDetIds[i].first.rawId());
      std::vector<int>::const_iterator iEtaIt =  std::find(iEtas.begin(),iEtas.end(),iEta);
      if(iEtaIt==iEtas.end()) iEtas.push_back(iEta);
    }
      

    if(hitNrgy > nrgyIdMax.first) {
      nrgyId2nd = nrgyIdMax;
      nrgyIdMax.first = hitNrgy;
      nrgyIdMax.second = clusDetIds[i].first.rawId();
    }else if(hitNrgy > nrgyId2nd.first){
      nrgyId2nd.first = hitNrgy;
      nrgyId2nd.second = clusDetIds[i].first.rawId();
    }
  }
  
  if(!dominoNrgys.empty()){
    std::sort(dominoNrgys.begin(),dominoNrgys.end(),TempFuncs::PairSortBy2nd<int,float,std::greater<float> >());
    int seedIPhi = dominoNrgys[0].first;
    std::sort(iEtas.begin(),iEtas.end());
    int seedIEta = iEtas.size()==5 ? iEtas[2] : 0;
    seedId_ = DetIdTools::makeEcalBarrelId(seedIEta,seedIPhi);
  }
  
  
  hitInfo_ = packHits_(seedId_,tempHitIds);
  
  // eMaxId_=nrgyIdMax.second;
  e2ndId_=nrgyId2nd.second;
}
