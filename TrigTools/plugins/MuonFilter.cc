#include "SHarper/TrigTools/interface/MuonFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"

#include <functional>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
//little helper func to convert a string to whatever 


MuonFilter:: MuonFilter(const edm::ParameterSet& iPara):nrPass_(0),tree_(0),file_(0)//,p4Array_(new TClonesArray("TLorentzVector",10))
{
  muonTag_ = iPara.getParameter<edm::InputTag>("muonTag");
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
    tree_->Branch("nrMu",&nrMu_);
    
    tree_->Branch("evtInfo",&evtInfo_,"runnr/I:lumiSec:eventnr:bx");
    tree_->Branch("nrPassCut1",&nrPassCut1_,"nrPassCut1/I");
    tree_->Branch("nrPassCut2",&nrPassCut2_,"nrPassCut2/I");
  }
}

bool MuonFilter::filter(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  edm::Handle<reco::MuonCollection> muonHandle;
  iEvent.getByLabel(muonTag_,muonHandle);
  
 //  edm::Handle<reco::BeamSpot> recoBSHandle;
//   iEvent.getByLabel("offlineBeamSpot",recoBSHandle);

//   edm::Handle<reco::BeamSpot> hltBSHandle;
//   iEvent.getByLabel("hltOnlineBeamSpot",hltBSHandle);
//   edm::Handle<reco::BeamSpot> hltOffBSHandle;
//   iEvent.getByLabel("hltOfflineBeamSpot",hltOffBSHandle);
//   std::cout <<" "<<std::endl;
//   std::cout <<"reco bs "<<recoBSHandle->position().x()<<" "<<recoBSHandle->position().y()<<" "<<recoBSHandle->position().z()<<std::endl;
//   std::cout <<"hlt  bs "<<hltBSHandle->position().x()<<" "<<hltBSHandle->position().y()<<" "<<hltBSHandle->position().z()<<std::endl;
//    std::cout <<"hlt  bs "<<hltOffBSHandle->position().x()<<" "<<hltOffBSHandle->position().y()<<" "<<hltOffBSHandle->position().z()<<std::endl;
//   std::cout <<"delta "<<hltBSHandle->position().x()-recoBSHandle->position().x()<<" "<<hltBSHandle->position().y()-recoBSHandle->position().y()<<" "<<hltBSHandle->position().z()-recoBSHandle->position().z()<<std::endl;
//   std::cout <<" "<<std::endl;

  const reco::MuonCollection& muons = *muonHandle;
  bool returnVal=false;
  nrPassCut1_=0;
  nrPassCut2_=0;
  //p4Vec_.clear();
  ptVec_.clear();
  etaVec_.clear();
  phiVec_.clear();
  // p4Array_->Delete();
  nrMu_=0;
  
  for(size_t muNr=0;muNr<muons.size();muNr++){
    if(muons[muNr].innerTrack().isNonnull()){
      nrMu_++;
      if(muons[muNr].innerTrack().isNonnull() && muons[muNr].innerTrack()->pt()>ptCut1_) nrPassCut1_++;
      if(muons[muNr].innerTrack().isNonnull() && muons[muNr].innerTrack()->pt()>ptCut2_) nrPassCut2_++;
      //      TLorentzVector p4;
      //p4.SetPtEtaPhiM(muons[muNr].innerTrack()->pt(),muons[muNr].innerTrack()->eta(),muons[muNr].innerTrack()->phi(),0);
      //p4Vec_.push_back(p4);
      ptVec_.push_back(muons[muNr].innerTrack()->pt());
      etaVec_.push_back(muons[muNr].innerTrack()->eta());
      phiVec_.push_back(muons[muNr].innerTrack()->phi());
      
      ///new((*p4Array_)[p4Array_->GetLast()+1]) TLorentzVector(p4);

      //   std::cout <<"event "<<iEvent.id().event()<<" mu "<<muNr<<" pt "<<(muons[muNr].innerTrack().isNonnull() ? muons[muNr].innerTrack()->pt() : -1)<<" global "<<(muons[muNr].globalTrack().isNonnull() ? muons[muNr].globalTrack()->pt() : -1)<<std::endl;
    }
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

void MuonFilter::endJob()
{
  if(file_){
    file_->Write();
    delete file_;
    file_=0;
  }


}
  

//define this as a plug-in
DEFINE_FWK_MODULE(MuonFilter);
