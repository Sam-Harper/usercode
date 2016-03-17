#ifndef SHEVENT_SHRECHITSTOCLUSTERSMAP_HH
#define SHEVENT_SHRECHITSTOCLUSTERSMAP_HH

#include "TObject.h"

#include <vector>
#include <utility>

class SHRecHitsToClustersMap {
private:

  std::vector<std::pair<int,size_t>> clusSeedIds_; //a sorted vector, first entry is seedDetId, second is the entry in the vector in recHitData
  std::vector<std::pair<int,std::vector<float> > > recHitData_; //sorted vector
  bool isSorted_;

public:
  SHRecHitsToClustersMap():isSorted_(true){}
  virtual ~SHRecHitsToClustersMap(){}
  bool addCluster(int seedId,const std::vector<std::pair<int,float> >& recHitAndFracs);
  std::vector<std::pair<int,float>> getClusterHits(int seedId)const;
  //  std::vector<std::pair<int,float>> getHitClusters(int detId);
  void sort();
  void clear(){clusSeedIds_.clear();recHitData_.clear();}
  ClassDef(SHRecHitsToClustersMap,1)
};



#endif
