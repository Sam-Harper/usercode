#include "SHarper/TrigTools/interface/TrigResultTreeMaker.h"

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include <iostream>
#include <fstream>

TrigResultTreeMaker::TrigResultTreeMaker(const edm::ParameterSet& iPara)
{
  trigResultsTag_ = iPara.getParameter<edm::InputTag>("trigResultsTag");
  outputFilename_ = iPara.getParameter<std::string>("outputFilename"); 
  std::cout <<"int maker "<<std::endl;
  
  trigBits_ = new TBits;
  file_=new TFile(outputFilename_.c_str(),"RECREATE");
  tree_=new TTree("trigResultTree","tree");
  tree_->Branch("evtInfo",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
  tree_->Branch("trigBits",&trigBits_);
  
  std::cout <<"leaving maker "<<std::endl;

}


void TrigResultTreeMaker::beginRun(const edm::Run& run,const edm::EventSetup& setup)
{
  std::cout <<"begining run "<<std::endl;
  bool changed=false;
  hltConfig_.init(run,setup,trigResultsTag_.process(),changed); //as we need the orginal HLT config...
  std::cout <<"table name "<<hltConfig_.tableName()<<std::endl;
  
  if(changed && !pathNames_.empty()){ 
    std::cout <<"TrigResultTreeMaker::beginRun warning HLT menu has changed "<<std::endl;
  }
  if(pathNames_.empty()){ //initialising hlt paths names
    pathNames_ = hltConfig_.triggerNames();
    TList* trigNameList = new TList;
    for(size_t pathNr=0;pathNr<pathNames_.size();pathNr++){
      trigNameList->Add(new TObjString(pathNames_[pathNr].c_str())); //sigh, I have no clue if root makes a copy fo the object or not...
    }
    tree_->GetUserInfo()->Add(trigNameList); //well its only a little memory leak..
      
  }

  
}

void TrigResultTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  edm::Handle<edm::TriggerResults> trigResultsHandle;
  iEvent.getByLabel(trigResultsTag_,trigResultsHandle);
  
  const edm::TriggerResults& trigResults = *trigResultsHandle;
  const edm::TriggerNames& trigNames = iEvent.triggerNames(trigResults);  
  
  evtInfo_.fill(iEvent);
  trigBits_->ResetAllBits();
  for(size_t pathNr=0;pathNr<pathNames_.size();pathNr++){
    size_t pathIndex = trigNames.triggerIndex(pathNames_[pathNr]);
    if(pathIndex<trigResults.size() &&  trigResults.accept(pathIndex)) trigBits_->SetBitNumber(pathNr);
  }
  tree_->Fill();
 
}

void TrigResultTreeMaker::endJob()
{
  file_->Write();
  delete file_;
  file_=0;
}



//define this as a plug-in
DEFINE_FWK_MODULE(TrigResultTreeMaker);
