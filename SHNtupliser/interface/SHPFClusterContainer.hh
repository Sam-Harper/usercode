#ifndef SHPFCLUSTERCONTAINER_HH
#define SHPFCLUSTERCONTAINER_HH

#include "SHarper/SHNtupliser/interface/SHPFCluster.hh"

#include <vector>

class SHPFClusterContainer {
private:
  std::vector<SHPFCluster> ecalClusters_;
  std::vector<SHPFCluster> hcalClusters_;


public: 
  SHPFClusterContainer();
  virtual ~SHPFClusterContainer(){}

  void addECALCluster(const SHPFCluster& clus){ecalClusters_.push_back(clus);}
  void addHCALCluster(const SHPFCluster& clus){hcalClusters_.push_back(clus);}
 
  void clear(){ecalClusters_.clear();hcalClusters_.clear();}

  const SHPFCluster& getECALCluster(size_t index)const{return ecalClusters_[index];}
  const SHPFCluster& getHCALCluster(size_t index)const{return hcalClusters_[index];} 
  size_t nrECALClusters()const{return ecalClusters_.size();}
  size_t nrHCALClusters()const{return hcalClusters_.size();}

  const std::vector<SHPFCluster>& ecal()const{return ecalClusters_;}
  const std::vector<SHPFCluster>& hcal()const{return hcalClusters_;}
  
  ClassDef(SHPFClusterContainer,1)
};

#endif
