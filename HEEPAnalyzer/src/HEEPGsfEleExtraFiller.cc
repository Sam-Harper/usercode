#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtraFiller.h"

heep::GsfEleExtraFiller::GsfEleExtraFiller(const edm::ParameterSet& iPara,edm::ConsumesCollector&& iC)
{
  setup(iPara,iC);
}

void heep::GsfEleExtraFiller::setup(const edm::ParameterSet& iPara,edm::ConsumesCollector& iC)
{
  setToken(verticesToken_,iC,iPara,"verticesLabel");
  setToken(rhoToken_,iC,iPara,"eleRhoCorrLabel");
  setToken(trkIsoNoJetCoreToken_,iC,iPara,"trkIsoNoJetCoreLabel");
  setToken(nrSatCrysIn5x5Token_,iC,iPara,"nrSatCrysIn5x5Label");
}

void heep::GsfEleExtraFiller::getEvtContent(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  iEvent.getByToken(verticesToken_,verticesHandle_);
  iEvent.getByToken(rhoToken_,rhoHandle_);
  iEvent.getByToken(trkIsoNoJetCoreToken_,trkIsoNoJetCoreHandle_);
  iEvent.getByToken(nrSatCrysIn5x5Token_,nrSatCrysIn5x5Handle_);
}
  
void heep::GsfEleExtraFiller::operator()(const edm::Ptr<reco::GsfElectron>& ele,heep::GsfEleExtra& eleExtra)
{
  
  eleExtra.rho = *rhoHandle_;
  eleExtra.primaryVertex.SetXYZ(0.,0.,0.);
  if(!verticesHandle_->empty()) eleExtra.primaryVertex = verticesHandle_->front().position();
  eleExtra.trkIsoNoJetCore = (*trkIsoNoJetCoreHandle_)[ele];
  eleExtra.nrSatCrysIn5x5 = (*nrSatCrysIn5x5Handle_)[ele];
}
heep::GsfEleExtra heep::GsfEleExtraFiller::operator()(const edm::Ptr<reco::GsfElectron>& ele)
{
  heep::GsfEleExtra eleExtra;
  (*this)(ele,eleExtra);
  return eleExtra;
}

