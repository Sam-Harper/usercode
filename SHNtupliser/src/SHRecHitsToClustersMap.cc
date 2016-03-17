#include "SHarper/SHNtupliser/interface/SHRecHitsToClustersMap.hh"
#include "SHarper/SHNtupliser/interface/TempFuncs.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

ClassImp(SHRecHitsToClustersMap)


bool SHRecHitsToClustersMap::addCluster(int seedId,const std::vector<std::pair<int,float> >& recHitAndFracs)
{
  if(std::find_if(clusSeedIds_.begin(),clusSeedIds_.end(),
		  [&seedId](const std::pair<int,size_t>& val)
		  {return val.first==seedId;})!=clusSeedIds_.end()){
    LogErr<<" warning, seedId "<<seedId<<" has already been added"<<std::endl;
    return false;
  }else{
    isSorted_=false;
    size_t seedIndx = clusSeedIds_.size();
    clusSeedIds_.push_back(std::pair<int,size_t>(seedId,seedIndx));
    for(auto& data : recHitData_) data.second.push_back(0.);

    for(auto& recHitAndFrac : recHitAndFracs){
      auto recHitIt = std::find_if(recHitData_.begin(),recHitData_.end(),
				   [&recHitAndFrac](const std::pair<int,std::vector<float>>& val)
				   {return recHitAndFrac.first==val.first;});
      if(recHitIt==recHitData_.end()){
	recHitData_.emplace_back(std::make_pair(recHitAndFrac.first,std::vector<float>()));
	recHitData_.back().second.resize(seedIndx+1,0.);
	recHitData_.back().second[seedIndx]=recHitAndFrac.second;
      }else{
	recHitIt->second[seedIndx]=recHitAndFrac.second;
      }
    }//end loop over all rec hits
    return true;
  }//end already added seed id protection
}

void SHRecHitsToClustersMap::sort()
{
  if(!isSorted_){
    std::sort(clusSeedIds_.begin(),clusSeedIds_.end(),TempFuncs::PairSortBy1st<int,size_t>());
    std::sort(recHitData_.begin(),recHitData_.end(),TempFuncs::PairSortBy1st<int,std::vector<float>>());
    isSorted_=true;
  }
}

std::vector<std::pair<int,float>> SHRecHitsToClustersMap::getClusterHits(int seedId)const
{
  
  std::vector<std::pair<int,float>> result;
  if(!isSorted_){
    LogErr<<" error, vector was not sorted, can not look up cluster hits"<<std::endl;
    return result;
  }

  auto seedClusData = std::equal_range(clusSeedIds_.begin(),clusSeedIds_.end(),seedId,TempFuncs::PairSortBy1st<int,size_t>());
  if(seedClusData.first==seedClusData.second) return result;
  else{
    
    size_t seedClusIndx = seedClusData.first->second;    
    for(auto& recHit : recHitData_){
      if(recHit.second[seedClusIndx]>0.0001){
	result.push_back(std::pair<int,float>(recHit.first,recHit.second[seedClusIndx]));
      }
    }
    return result;
  }
}
