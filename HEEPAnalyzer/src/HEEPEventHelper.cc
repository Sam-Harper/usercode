
#include "SHarper/HEEPAnalyzer/interface/HEEPEventHelper.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEvtHandles.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPTrigTools.h"

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
 
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

void heep::EventHelper::setup(const edm::ParameterSet& conf)
{
  cuts_.setup(conf);
  eleLabel_ = conf.getUntrackedParameter<edm::InputTag>("electronTag");
  muoLabel_ = conf.getUntrackedParameter<edm::InputTag>("muonTag");
  jetLabel_ = conf.getUntrackedParameter<edm::InputTag>("jetTag");
  tauLabel_ = conf.getUntrackedParameter<edm::InputTag>("tauTag");
  metLabel_ = conf.getUntrackedParameter<edm::InputTag>("metTag");
  phoLabel_ = conf.getUntrackedParameter<edm::InputTag>("photonTag");
  ecalRecHitsEBTag_ = conf.getParameter<edm::InputTag>("barrelRecHitTag");
  ecalRecHitsEETag_ = conf.getParameter<edm::InputTag>("endcapRecHitTag");
  ecalReducedRecHitsEBTag_  =conf.getParameter<edm::InputTag>("reducedBarrelRecHitTag");
  ecalReducedRecHitsEETag_  =conf.getParameter<edm::InputTag>("reducedEndcapRecHitTag");
  hcalRecHitsTag_ = conf.getParameter<edm::InputTag>("hbheRecHitsTag");
  superClusterEBTag_ =conf.getParameter<edm::InputTag>("superClusterEBTag");
  superClusterEETag_ =conf.getParameter<edm::InputTag>("superClusterEETag");
  gsfEleTag_ = conf.getParameter<edm::InputTag>("gsfEleTag");
  recoPhoTag_ = conf.getParameter<edm::InputTag>("recoPhoTag");
  pfCandidateTag_ = conf.getParameter<edm::InputTag>("pfCandidateTag");
  ctfTrackTag_ = conf.getParameter<edm::InputTag>("ctfTrackTag");
  genParticleTag_ = conf.getParameter<edm::InputTag>("genParticleTag");
  trigEventTag_ = conf.getParameter<edm::InputTag>("trigEventTag");
  trigResultsTag_ =conf.getParameter<edm::InputTag>("trigResultsTag");
  genEventInfoTag_ = conf.getParameter<edm::InputTag>("genEventInfoTag");
  pileUpMCInfoTag_ = conf.getParameter<edm::InputTag>("pileUpMCInfoTag");
  l1RecordTag_ = conf.getParameter<edm::InputTag>("l1RecordTag");
  l1EmNonIsoTag_ = conf.getParameter<edm::InputTag>("l1EmNonIsoTag");
  l1EmIsoTag_ = conf.getParameter<edm::InputTag>("l1EmIsoTag");
  verticesTag_ = conf.getParameter<edm::InputTag>("verticesTag");
  caloTowersTag_ = conf.getParameter<edm::InputTag>("caloTowersTag");
  eleRhoCorrTag_ = conf.getParameter<edm::InputTag>("eleRhoCorrTag");
  eleRhoCorr2012Tag_ = conf.getParameter<edm::InputTag>("eleRhoCorr2012Tag");
  ecalLaserFilterTag_ = conf.getParameter<edm::InputTag>("ecalLaserFilter");
  pfChargedIsoValEleMapTag_ = conf.getParameter<edm::InputTag>("pfChargedIsoValEleMapTag");
  pfPhotonIsoValEleMapTag_ = conf.getParameter<edm::InputTag>("pfPhotonIsoValEleMapTag"); 
  pfNeutralIsoValEleMapTag_ = conf.getParameter<edm::InputTag>("pfNeutralIsoValEleMapTag"); 

  //trig matching parameters
  hltProcName_ = conf.getParameter<std::string>("hltProcName");
  maxDRTrigMatch_ = conf.getParameter<double>("maxDRTrigMatch");
  maxPtRelDiffTrigMatch_ = conf.getParameter<double>("maxPtRelDiffTrigMatch");
 
  hltFiltersToCheck_ =conf.getParameter<std::vector<std::string> >("hltFiltersToCheck");
  //now get the trigger names, however we also need the number of objects each filter requires, which we read from the provenace
  hltFiltersToCheckWithNrCands_.clear();
  for(size_t filterNr=0;filterNr<hltFiltersToCheck_.size();filterNr++){
    const std::string& filterName = hltFiltersToCheck_[filterNr];
    hltFiltersToCheckWithNrCands_.push_back(std::make_pair(filterName,heep::trigtools::getMinNrObjsRequiredByFilter(filterName)));
  }
  heep::TrigCodes::setCodes(hltFiltersToCheck_); //this assigns a unique bit to each trigger name, without this no trigger codes are defined. If you recieve a trigger not found error, its because it wasnt in the vector passed to this function

  onlyAddEcalDriven_ = conf.getParameter<bool>("onlyAddEcalDriven");
  heepEleSource_ = conf.getParameter<int>("heepEleSource");

  applyRhoCorrToEleIsol_ = conf.getParameter<bool>("applyRhoCorrToEleIsol");
  eleIsolEffectiveAreas_ = conf.getParameter<edm::ParameterSet>("eleIsolEffectiveAreas");
}

void heep::EventHelper::makeHeepEvent(const edm::Event& edmEvent,const edm::EventSetup& setup,heep::Event& heepEvent)const
{
  setHandles(edmEvent,setup,heepEvent.handles()); 
  if(heepEleSource_==0) fillHEEPElesFromGsfEles(heepEvent.handles(),heepEvent.heepEles());
  else if(heepEleSource_==1) fillHEEPElesFromPat(heepEvent.handles(),heepEvent.heepEles());
  heepEvent.setEvent(edmEvent); 
  heepEvent.setEventSetup(setup);
  heep::TrigCodes::TrigBitSet bits = heep::trigtools::getHLTFiltersPassed(hltFiltersToCheckWithNrCands_,heepEvent.handles().trigEvent,hltProcName_);
  heepEvent.setTrigBits(bits);
}


void heep::EventHelper::setHandles(const edm::Event& event,const edm::EventSetup& setup,heep::EvtHandles& handles)const
{ 
  event.getByLabel(muoLabel_,handles.muon);
  event.getByLabel(jetLabel_,handles.jet);
  event.getByLabel(eleLabel_,handles.electron);
  event.getByLabel(metLabel_,handles.met);
  event.getByLabel(phoLabel_,handles.pho);
  event.getByLabel(tauLabel_,handles.tau);
  event.getByLabel(ecalRecHitsEBTag_,handles.ebRecHits);
  event.getByLabel(ecalRecHitsEETag_,handles.eeRecHits);
  event.getByLabel(ecalReducedRecHitsEBTag_,handles.ebReducedRecHits);
  event.getByLabel(ecalReducedRecHitsEETag_,handles.eeReducedRecHits);
  event.getByLabel(hcalRecHitsTag_,handles.hbheRecHits);
  event.getByLabel(gsfEleTag_,handles.gsfEle);
  event.getByLabel(recoPhoTag_,handles.recoPho);
  event.getByLabel(pfCandidateTag_,handles.pfCandidate);
  event.getByLabel(superClusterEBTag_,handles.superClusEB);
  event.getByLabel(superClusterEETag_,handles.superClusEE); 
  event.getByLabel(ctfTrackTag_,handles.ctfTrack);
  event.getByLabel(genParticleTag_,handles.genParticle);
  event.getByLabel(trigEventTag_,handles.trigEvent);
  event.getByLabel(trigResultsTag_,handles.trigResults);
  event.getByLabel(genEventInfoTag_,handles.genEventInfo);  
  event.getByLabel(pileUpMCInfoTag_,handles.pileUpMCInfo);
  event.getByLabel(l1RecordTag_,handles.l1Record);
  event.getByLabel(l1EmNonIsoTag_,handles.l1EmNonIso);
  event.getByLabel(l1EmIsoTag_,handles.l1EmIso);
  event.getByLabel(verticesTag_,handles.vertices);
  event.getByLabel(caloTowersTag_,handles.caloTowers);
  event.getByLabel(eleRhoCorrTag_,handles.eleRhoCorr);
  event.getByLabel(eleRhoCorr2012Tag_,handles.eleRhoCorr2012);
  event.getByLabel(ecalLaserFilterTag_,handles.ecalLaserFilter);
  event.getByLabel(pfChargedIsoValEleMapTag_,handles.pfChargedIsoValEleMap);
  event.getByLabel(pfPhotonIsoValEleMapTag_,handles.pfPhotonIsoValEleMap);
  event.getByLabel(pfNeutralIsoValEleMapTag_,handles.pfNeutralIsoValEleMap);
  event.getByType(handles.beamSpot);

  setup.get<CaloGeometryRecord>().get(handles.caloGeom);
  setup.get<CaloTopologyRecord>().get(handles.caloTopology);
  //setup.get<L1GtTriggerMenuRcd>().get(handles.l1Menu);
  //setup.get<TrackerDigiGeometryRecord>().get(handles.trackGeom);
  setup.get<IdealMagneticFieldRecord>().get(handles.bField);
}

//fills the heepEles vector using pat electrons as starting point
void heep::EventHelper::fillHEEPElesFromPat(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const
{
  heepEles.clear();
  const edm::View<pat::Electron>& eles = *handles.electron;
  for(edm::View<pat::Electron>::const_iterator eleIt = eles.begin(); eleIt!=eles.end(); ++eleIt){ 
    if(!onlyAddEcalDriven_ || eleIt->ecalDrivenSeed()){
      addHEEPEle_(*eleIt,handles,heepEles);
    }
  }
  //the electrons are now filled, lets add trigger info
  if(handles.trigEvent.isValid() && handles.trigResults.isValid()){
    heep::trigtools::setHLTFiltersObjPasses(heepEles,hltFiltersToCheck_,handles.trigEvent,hltProcName_,
					    maxDRTrigMatch_,maxPtRelDiffTrigMatch_);
  }
}

//fills the heepEles vector using GsfElectrons as starting point
void heep::EventHelper::fillHEEPElesFromGsfEles(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const
{
  heepEles.clear();
  const std::vector<reco::GsfElectron>& eles = *handles.gsfEle;
  for(std::vector<reco::GsfElectron>::const_iterator eleIt = eles.begin(); eleIt!=eles.end(); ++eleIt){ 
    if(!onlyAddEcalDriven_ || eleIt->ecalDrivenSeed()){
      addHEEPEle_(*eleIt,handles,heepEles);
    }
  }
  //the electrons are now filled, lets add trigger info
  if(handles.trigEvent.isValid() && handles.trigResults.isValid()){
    heep::trigtools::setHLTFiltersObjPasses(heepEles,hltFiltersToCheck_,handles.trigEvent,hltProcName_,
					    maxDRTrigMatch_,maxPtRelDiffTrigMatch_);
  }
}

//this converts the pat::Electron / reco::GsfElectron into a heep::Electron
void heep::EventHelper::addHEEPEle_(const reco::GsfElectron& gsfEle,const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const
{
  heepEles.push_back(heep::Ele(gsfEle));
  heep::Ele& ele =  heepEles.back();  
  //now we need to set isolation rho corrections
  ele.setApplyRhoIsolCorr(applyRhoCorrToEleIsol_);
  ele.setIsolEffectiveAreas(eleIsolEffectiveAreas_);
  
  math::XYZPoint pvPos(0,0,0);
  if(handles.vertices.isValid() && !handles.vertices->empty()) pvPos = handles.vertices->front().position();
  ele.setEvtPrimVertexPos(pvPos);
  if(handles.eleRhoCorr.isValid()) ele.setRhoForIsolCorr(*handles.eleRhoCorr);
  //now we would like to set the cut results, this has to come after setting isolation parameters
  ele.setCutCode(cuts_.getCutCode(ele)); 
}

