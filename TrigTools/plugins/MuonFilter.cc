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


MuonFilter:: MuonFilter(const edm::ParameterSet& iPara):nrPass_(0)
{
  muonTag_ = iPara.getParameter<edm::InputTag>("muonTag");
  ptCut1_ = iPara.getParameter<double>("ptCut1");
  ptCut2_ = iPara.getParameter<double>("ptCut2");
  nrToPassCut1_ = iPara.getParameter<int>("nrToPassCut1");
  nrToPassCut2_ = iPara.getParameter<int>("nrToPassCut2");
  reverseCuts_ = iPara.getParameter<bool>("reverseCuts");
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
  int nrPassPt1=0;
  int nrPassPt2=0;
  
  for(size_t muNr=0;muNr<muons.size();muNr++){
    if(muons[muNr].innerTrack().isNonnull() && muons[muNr].innerTrack()->pt()>ptCut1_) nrPassPt1++;
    if(muons[muNr].innerTrack().isNonnull() && muons[muNr].innerTrack()->pt()>ptCut2_) nrPassPt2++;
    
      std::cout <<"event "<<iEvent.id().event()<<" mu "<<muNr<<" pt "<<(muons[muNr].innerTrack().isNonnull() ? muons[muNr].innerTrack()->pt() : -1)<<" global "<<(muons[muNr].globalTrack().isNonnull() ? muons[muNr].globalTrack()->pt() : -1)<<std::endl;
  }
  if(nrPassPt2>=nrToPassCut2_ && nrPassPt1>=nrToPassCut1_) returnVal=true;

  if(reserveCuts_) returnVal= !returnVal;

  if(returnVal) nrPass_++;
  return returnVal;
}

  

//define this as a plug-in
DEFINE_FWK_MODULE(MuonFilter);
