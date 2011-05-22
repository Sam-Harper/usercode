#include "SHarper/TrigTools/interface/TriggerRateNtupMaker.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>




TriggerRateNtupMaker::TriggerRateNtupMaker(const edm::ParameterSet& iPara):
  trigTree_(0),outputFile_(0)
{
 
  trigEventTag_ = iPara.getParameter<edm::InputTag>("trigEventTag");

  outputFilename_ = iPara.getParameter<std::string>("outputFilename");
  std::vector<std::string> filterNames = iPara.getParameter<std::vector<std::string> >("filterNames");
  

  for(size_t filterNr=0;filterNr<filterNames.size();filterNr++){ 
    typedef std::pair<trigtools::PathData,std::string> PathPair;
    paths_.push_back(PathPair(trigtools::PathData(),filterNames[filterNr]));
  }

}

TriggerRateNtupMaker::~TriggerRateNtupMaker()
{
  if(outputFile_) delete outputFile_; //the trigTree is owned by outputFile so deleting that file should be enough
}

void TriggerRateNtupMaker::beginJob()
{
 
  outputFile_ = new TFile(outputFilename_.c_str(),"RECREATE");
  
  trigTree_ = new TTree("trigRateTree","trig tree");
  
  for(size_t trigNr=0;trigNr<paths_.size();trigNr++){
    std::ostringstream name;
    name <<"trig"<<trigNr;
    
    trigTree_->Branch(name.str().c_str(),&paths_[trigNr].first,trigtools::PathData::contents().c_str());
  }
  trigTree_->Branch("nrVerts",&nrVerts_,"nrVerts/I");
  trigTree_->Branch("zVtx",&zVtx_,"zVtx/F");
  trigTree_->Branch("evt",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
}
  
void TriggerRateNtupMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  evtInfo_.fill(iEvent);
  nrVerts_ = -1;  
  zVtx_ = -1;
  
  edm::Handle<trigger::TriggerEvent> trigEvent; 
  iEvent.getByLabel(trigEventTag_,trigEvent);
  
  
  std::vector<TLorentzVector> trigP4s;
  for(size_t trigNr=0;trigNr<paths_.size();trigNr++){
    makeVecOfTrigP4s(trigP4s,*trigEvent,edm::InputTag(paths_[trigNr].second,"",trigEventTag_.process()));
    paths_[trigNr].first.fill(trigP4s);
  }
  trigTree_->Fill();
}


void TriggerRateNtupMaker::makeVecOfTrigP4s(std::vector<TLorentzVector>& trigP4s,const trigger::TriggerEvent& trigEvent,const edm::InputTag& filterName)
{
  trigP4s.clear();
  trigger::size_type filterIndex = trigEvent.filterIndex(filterName); 
  if(filterIndex<trigEvent.sizeFilters()){ 
    const trigger::Keys& trigKeys = trigEvent.filterKeys(filterIndex); 
    const trigger::TriggerObjectCollection & trigObjColl(trigEvent.getObjects());
    for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){ 
      const trigger::TriggerObject& obj = trigObjColl[*keyIt];
      TLorentzVector p4;
      p4.SetPtEtaPhiM(obj.pt(),obj.eta(),obj.phi(),obj.mass());
      trigP4s.push_back(p4);
    }
  }
}
 
void TriggerRateNtupMaker::endJob()
{
  
  std::vector<std::string> filterDefs;
  for(size_t i=0;i<paths_.size();i++) {
    filterDefs.push_back(paths_[i].second);
    std::cout <<"i "<<i<<" "<<paths_[i].second<<std::endl;
  }
  
  outputFile_->Write();
  outputFile_->WriteObject(&filterDefs,"filterDefs");
}

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerRateNtupMaker);
