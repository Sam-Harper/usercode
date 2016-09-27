#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtraFiller.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPElectronTkIsolation.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEcalClusterTools.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

heep::GsfEleExtraFiller::GsfEleExtraFiller(const edm::ParameterSet& iPara,edm::ConsumesCollector&& iC):
  trkIsoParam_(iPara.getParameter<edm::ParameterSet>("trkIsoConfig"))
{
  setup(iPara,iC);
}

heep::GsfEleExtraFiller::TrkIsoParam::TrkIsoParam(const edm::ParameterSet& iParam):
  extRadius(iParam.getParameter<double>("extRadius")),
  intRadiusBarrel(iParam.getParameter<double>("intRadiusBarrel")),
  intRadiusEndcap(iParam.getParameter<double>("intRadiusEndcap")),
  stripBarrel(iParam.getParameter<double>("stripBarrel")),
  stripEndcap(iParam.getParameter<double>("stripEndcap")),
  ptMin(iParam.getParameter<double>("ptMin")),
  maxVtxDist(iParam.getParameter<double>("maxVtxDist")),
  drb(iParam.getParameter<double>("drb"))
{ 

}

void heep::GsfEleExtraFiller::setup(const edm::ParameterSet& iPara,edm::ConsumesCollector& iC)
{
  setToken(verticesToken_,iC,iPara,"verticesTag");
  setToken(rhoToken_,iC,iPara,"eleRhoCorrTag");
  setToken(trkIsoNoJetCoreToken_,iC,iPara,"trkIsoNoJetCoreTag");
  setToken(nrSatCrysIn5x5Token_,iC,iPara,"nrSatCrysIn5x5Tag");
  //recalcuate tags
  setToken(ebRecHitToken_,iC,iPara,"reducedBarrelRecHitTag");
  setToken(eeRecHitToken_,iC,iPara,"reducedEndcapRecHitTag");
  setToken(beamSpotToken_,iC,iPara,"beamSpotTag");
  setToken(trkToken_,iC,iPara,"ctfTrackTag");
}

void heep::GsfEleExtraFiller::getEvtContent(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  iEvent.getByToken(verticesToken_,verticesHandle_);
  iEvent.getByToken(rhoToken_,rhoHandle_);
  iEvent.getByToken(trkIsoNoJetCoreToken_,trkIsoNoJetCoreHandle_);
  iEvent.getByToken(nrSatCrysIn5x5Token_,nrSatCrysIn5x5Handle_);
  iEvent.getByToken(ebRecHitToken_,ebRecHitHandle_);
  iEvent.getByToken(eeRecHitToken_,eeRecHitHandle_);
  iEvent.getByToken(beamSpotToken_,beamSpotHandle_);
  iEvent.getByToken(trkToken_,trkHandle_);
  iSetup.get<CaloTopologyRecord>().get(caloTopoHandle_);

}
  
void heep::GsfEleExtraFiller::operator()(const edm::Ptr<reco::GsfElectron>& ele,heep::GsfEleExtra& eleExtra)
{

  eleExtra.rho = *rhoHandle_;
  eleExtra.primaryVertex.SetXYZ(0.,0.,0.);
  if(!verticesHandle_->empty()) eleExtra.primaryVertex = verticesHandle_->front().position();

  eleExtra.trkIsoNoJetCore = getTrkIsoNoJetCore_(ele);
  eleExtra.nrSatCrysIn5x5 = getNrSatCrysIn5x5_(ele);
}
heep::GsfEleExtra heep::GsfEleExtraFiller::operator()(const edm::Ptr<reco::GsfElectron>& ele)
{
  heep::GsfEleExtra eleExtra;
  (*this)(ele,eleExtra);
  return eleExtra;
}

float heep::GsfEleExtraFiller::getTrkIsoNoJetCore_(const edm::Ptr<reco::GsfElectron>& ele)
{
  if(trkIsoNoJetCoreHandle_.isValid()) return (*trkIsoNoJetCoreHandle_)[ele];
  else if(trkHandle_.isValid()){ //we recalculate it
    heep::ElectronTkIsolation isolCorr(trkIsoParam_.extRadius,trkIsoParam_.intRadiusBarrel,trkIsoParam_.intRadiusEndcap,
				       trkIsoParam_.stripBarrel,trkIsoParam_.stripEndcap,trkIsoParam_.ptMin,
				       trkIsoParam_.maxVtxDist,trkIsoParam_.drb,
				       trkHandle_.product(),beamSpotHandle_->position());
    isolCorr.setAlgosToReject({reco::TrackBase::jetCoreRegionalStep});
    return isolCorr.getPtTracks(&*ele);
  }else{//no tracks no previously stored value, return -1
    return -1;
  }
    
    
}
float heep::GsfEleExtraFiller::getNrSatCrysIn5x5_(const edm::Ptr<reco::GsfElectron>& ele)
{
  if(nrSatCrysIn5x5Handle_.isValid()) return (*nrSatCrysIn5x5Handle_)[ele];
  else if(ebRecHitHandle_.isValid() && eeRecHitHandle_.isValid()){ //we recalculate it
   
    DetId id = ele->superCluster()->seed()->seed();
    auto recHits = id.subdetId()==EcalBarrel ? ebRecHitHandle_.product() : eeRecHitHandle_.product();
    return heep::EcalClusterTools::nrSaturatedCrysIn5x5(id,recHits,caloTopoHandle_.product());
  }else{
    return -1;
  }
}
