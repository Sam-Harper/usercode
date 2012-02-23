#include "SHarper/TrigTools/interface/JetFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include <functional>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>


//little helper func to convert a string to whatever 


JetFilter:: JetFilter(const edm::ParameterSet& iPara):nrPass_(0),tree_(0),file_(0)//,p4Array_(new TClonesArray("TLorentzVector",10))
{
  jetTag_ = iPara.getParameter<edm::InputTag>("jetTag");
  ptCut1_ = iPara.getParameter<double>("ptCut1");
  ptCut2_ = iPara.getParameter<double>("ptCut2");
  nrToPassCut1_ = iPara.getParameter<int>("nrToPassCut1");
  nrToPassCut2_ = iPara.getParameter<int>("nrToPassCut2");
  reverseCuts_ = iPara.getParameter<bool>("reverseCuts");
  outputFilename_ = iPara.getParameter<std::string>("outputFilename");

  if(!outputFilename_.empty()){
    file_ = new TFile(outputFilename_.c_str(),"RECREATE");
    tree_ = new TTree("p4Tree","p4Tree");
    // tree_->Branch("p4s",&p4Vec_);
    tree_->Branch("pt",&ptVec_);
    tree_->Branch("eta",&etaVec_);
    tree_->Branch("phi",&phiVec_);
    tree_->Branch("mass",&massVec_);
    tree_->Branch("nrJet",&nrJet_);
    
    tree_->Branch("evtInfo",&evtInfo_,"runnr/I:lumiSec:eventnr:bx");
    tree_->Branch("nrPassCut1",&nrPassCut1_,"nrPassCut1/I");
    tree_->Branch("nrPassCut2",&nrPassCut2_,"nrPassCut2/I");
  }
}

bool JetFilter::filter(edm::Event& iEvent,const edm::EventSetup& iSetup)
{


  edm::Handle<std::vector<pat::Jet> > jetsHandle;                       
  iEvent.getByLabel (jetTag_, jetsHandle);              
  const std::vector<pat::Jet>& jets = *jetsHandle;




 
  bool returnVal=false;
  nrPassCut1_=0;
  nrPassCut2_=0;
  //p4Vec_.clear();
  ptVec_.clear();
  etaVec_.clear();
  phiVec_.clear();
  massVec_.clear();
  // p4Array_->Delete();
  nrJet_=0;
  
  for(size_t jetNr=0;jetNr<jets.size();jetNr++){
    nrJet_++;
    
    ptVec_.push_back(jets[jetNr].pt());
    etaVec_.push_back(jets[jetNr].eta());
    phiVec_.push_back(jets[jetNr].phi());
    massVec_.push_back(jets[jetNr].mass());
      
  }
  
  evtInfo_.runnr=iEvent.id().run();
  evtInfo_.eventNr=iEvent.id().event();
  evtInfo_.lumiSec=iEvent.luminosityBlock();
  evtInfo_.bx = iEvent.bunchCrossing();
  
  if(tree_) tree_->Fill();

  if(nrPassCut2_>=nrToPassCut2_ && nrPassCut1_>=nrToPassCut1_) returnVal=true;
  if(reverseCuts_) returnVal= !returnVal;

  if(returnVal) nrPass_++;
  return returnVal;
}

void JetFilter::endJob()
{
  if(file_){
    file_->Write();
    delete file_;
    file_=0;
  }


}
  

//define this as a plug-in
DEFINE_FWK_MODULE(JetFilter);
