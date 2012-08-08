#include "SHarper/TrigTools/interface/TrigResultsComparer.h"

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <fstream>

//little naughty helper function, many of these probably exist in CMSSW
//just checks that all the bits in bits to check are set in word
//reason for function is that due to opperator presidence, this can easily be bugged through missing brackets
bool checkAllBits(int word,int bitsToCheck)
{
  return (word&bitsToCheck)==bitsToCheck;
}

TrigResultsComparer::TrigResultsComparer(const edm::ParameterSet& iPara)
{
  prevTrigResultsTag_ = iPara.getParameter<edm::InputTag>("prevTrigResultsTag");
  newTrigResultsTag_ =  iPara.getParameter<edm::InputTag>("newTrigResultsTag");
}


void TrigResultsComparer::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  bool changed=false;
  prevHLTConfig_.init(run,setup,prevTrigResultsTag_.process(),changed); 
  std::cout <<"prev table name "<<prevHLTConfig_.tableName()<<" process "<<prevTrigResultsTag_.process()<<std::endl;
  
  if(changed && !pathCompData_.empty()){ 
    std::cout <<"TrigResultsComparer::beginRun warning prev HLT menu has changed, results are undefined "<<std::endl;
  }

  newHLTConfig_.init(run,setup,newTrigResultsTag_.process(),changed); 
  std::cout <<"new table name "<<newHLTConfig_.tableName()<<" process "<<newTrigResultsTag_.process()<<std::endl;
  
  if(changed && !pathCompData_.empty()){ 
    std::cout <<"TrigResultsComparer::beginRun warning new HLT menu has changed, results are undefined "<<std::endl;
  }
  
  //so the plan is to 
  //make copies of the list of path names of new and old menu so we can sort them
  //then we loop over the prev menu path names and see which are also present in the new menu, adding them accordingly
  //then we repeat for the new menu path names, adding any which are just in that meun
  if(pathCompData_.empty()){ //initialising hlt paths names
    std::vector<std::string> prevPathNames = prevHLTConfig_.triggerNames();
    std::vector<std::string> newPathNames = newHLTConfig_.triggerNames();
    std::sort(prevPathNames.begin(),prevPathNames.end());
    std::sort(newPathNames.begin(),newPathNames.end());
    pathCompData_.reserve(std::max(prevPathNames.size(),newPathNames.size()));
    for(size_t prevPathNr=0;prevPathNr<prevPathNames.size();prevPathNr++){
      if(std::binary_search(newPathNames.begin(),newPathNames.end(),prevPathNames[prevPathNr])){ //both have it
	pathCompData_.push_back(TrigCompData(prevPathNames[prevPathNr],0x3));
      }else{ //just in prev menu
	pathCompData_.push_back(TrigCompData(prevPathNames[prevPathNr],0x1));
      }
    }//end loop of prev menus paths

    for(size_t newPathNr=0;newPathNr<newPathNames.size();newPathNr++){
      if(!std::binary_search(prevPathNames.begin(),prevPathNames.end(),newPathNames[newPathNr])){ //only in new menu
	pathCompData_.push_back(TrigCompData(newPathNames[newPathNr],0x2));
      }// if was in both, already added
    }//end loop of new menus paths

  }//end check that pathCompData was empty
  
}

void TrigResultsComparer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  edm::Handle<edm::TriggerResults> prevTrigResultsHandle;
  iEvent.getByLabel(prevTrigResultsTag_,prevTrigResultsHandle);
  
  edm::Handle<edm::TriggerResults> newTrigResultsHandle;
  iEvent.getByLabel(newTrigResultsTag_,newTrigResultsHandle);
  
  
  const edm::TriggerNames& prevTrigNames = iEvent.triggerNames(*prevTrigResultsHandle);  
  const edm::TriggerNames& newTrigNames = iEvent.triggerNames(*newTrigResultsHandle); 
  
  //the idea is that because we cant reproduce pre-scales easily (unless something has changed in the service)
  //so we ignore triggers for the lumi sections in which either are prescaled
  for(size_t pathNr=0;pathNr<pathCompData_.size();pathNr++){
  
    int prevTrigCode = getTrigCode(pathCompData_[pathNr],*prevTrigResultsHandle,prevTrigNames,prevHLTConfig_);
    int newTrigCode = getTrigCode(pathCompData_[pathNr],*newTrigResultsHandle,newTrigNames,newHLTConfig_);
    if(checkAllBits(prevTrigCode,kPassed) && checkAllBits(newTrigCode,kPassed)) pathCompData_[pathNr].nrBothPass++;
    else if(!checkAllBits(prevTrigCode,kFailedPreScale) && !checkAllBits(prevTrigCode,kFailedPreScale)){
      if(checkAllBits(prevTrigCode,kPassed)) pathCompData_[pathNr].nrPrevOnlyPass++;
      if(checkAllBits(newTrigCode,kPassed)) pathCompData_[pathNr].nrNewOnlyPass++;
    }
  }
}

void TrigResultsComparer::endJob()
{
  std::cout <<" name & which menu & nr passing both & nr passing only prev & nr passing only new "<<std::endl;
  for(size_t pathNr=0;pathNr<pathCompData_.size();pathNr++){
    std::cout <<pathCompData_[pathNr]<<std::endl;
  }
}



int TrigResultsComparer::getTrigCode(const TrigResultsComparer::TrigCompData& data,const edm::TriggerResults& trigResults,const edm::TriggerNames& trigNames,const HLTConfigProvider& hltConfig)
{
  int trigCode =0x0;
  size_t pathIndex = trigNames.triggerIndex(data.name);
  
  if(pathIndex<trigResults.size()){
    trigCode |=kInMenu;
    if(trigResults.wasrun(pathIndex)) trigCode|=kWasRun;
    if(trigResults.accept(pathIndex)) trigCode|=kPassed;
    else{ //it potentially failed due to pre-scale so we will now check this
      size_t pathIndexInConfig = hltConfig.triggerIndex(data.name);
      size_t lastModuleRun = trigResults.index(pathIndex); //this is the module which made the design of the path, if its the prescale module, then it failed the prescale
      
      //a little bit of debug just to check things
      if(pathIndexInConfig>=hltConfig.size()) std::cout <<"TrigResultsComparer::getTrigCode : Warning pathIndex out of bounds, path index "<<pathIndexInConfig<<", max="<<hltConfig.size()<<std::endl;
      //else if(lastModuleRun>=hltConfig.size(pathIndexInConfig)) std::cout <<"TrigResultsComparer::getTrigCode : Warning moduleIndex out of bounds, module index "<<lastModuleRun<<", max="<<hltConfig.size(pathIndexInConfig)<<std::endl;
      
      
      if(lastModuleRun<hltConfig.size(pathIndexInConfig) && hltConfig.moduleType(hltConfig.moduleLabel(pathIndexInConfig,lastModuleRun))=="HLTPrescaler") trigCode|=kFailedPreScale; 
    }
  }
  return trigCode;//will be zero if its not in the menu and if its not in the menu, it cant be run or pass either...
}

std::ostream& operator<<(std::ostream& os,const TrigResultsComparer::TrigCompData& data)
{
  return data.print(os);
}

std::ostream& TrigResultsComparer::TrigCompData::print(std::ostream& os)const
{
  
  //os<<"path name "<<name;
  //if(menuExistsIn==0x3){
  //  if(nrPrevOnlyPass==0 && nrNewOnlyPass ==0) os <<" perfect match ( nr passing = "<<nrBothPass<<")";
  //  else os <<" nr both pass "<<nrBothPass<<" nr prev only pass "<<nrPrevOnlyPass<<" nr new only pass "<<nrNewOnlyPass;
  //}else if(menuExistsIn==0x1) os<< " only exists in prev menu (nr passing = "<<nrPrevOnlyPass<<")";
  //else if(menuExistsIn==0x2) os<< "only exists in new menu (nr passing = "<<nrNewOnlyPass<<")";
  //return os;

  os <<name<<" & "<<menuExistsIn<<" & "<<nrBothPass<<" & "<<nrPrevOnlyPass<<" & "<<nrNewOnlyPass;
  return os;
  
}


//define this as a plug-in
DEFINE_FWK_MODULE(TrigResultsComparer);
