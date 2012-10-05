#include "SHarper/TrigTools/interface/TriggerRateNtupMaker.h"

#include "SHarper/TrigTools/interface/TrigToolsFuncs.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/METReco/interface/PFMET.h"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>



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
  trigTree_->Branch("pfMET",&pfMET_,"pfMET/F");
  trigTree_->Branch("evt",&evtInfo_,trigtools::EvtInfoStruct::contents().c_str());
}
  
void TriggerRateNtupMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  evtInfo_.fill(iEvent);
  nrVerts_ = -1;  
  zVtx_ = -1;
  
  edm::Handle<trigger::TriggerEvent> trigEvent; 
  iEvent.getByLabel(trigEventTag_,trigEvent);
  

  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByLabel("offlinePrimaryVerticesWithBS",vertices);
  if(vertices.isValid()){
    nrVerts_ = std::count_if(vertices->begin(),vertices->end(),std::not1(std::mem_fun_ref(&reco::Vertex::isFake)));

  }

  edm::Handle<edm::View<reco::PFMET> > pfMET;
  iEvent.getByLabel("pfMet",pfMET);
  if(pfMET.isValid()){
    pfMET_=pfMET->front().et();
  }
  //usefule for dumping everything to screen
  //trigtools::dumpTriggerEvent(*trigEvent);
 
  
  std::vector<TLorentzVector> trigP4s;
  for(size_t trigNr=0;trigNr<paths_.size();trigNr++){
    trigtools::getP4sOfObsPassingFilter(trigP4s,*trigEvent,paths_[trigNr].second,trigEventTag_.process());
    paths_[trigNr].first.fill(trigP4s);
  }
  trigTree_->Fill();
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
