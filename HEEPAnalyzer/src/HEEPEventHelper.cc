
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
#include "FWCore/Utilities/interface/InputTag.h"

void heep::EventHelper::setup_(const edm::ParameterSet& conf,edm::ConsumesCollector & cc)
{
  cuts_.setup(conf);
  gsfEleExtraFiller_.setup(conf,cc);
  getToken_(eleTag_,conf.getUntrackedParameter<edm::InputTag>("electronTag"),cc);
  getToken_(muonTag_ , conf.getUntrackedParameter<edm::InputTag>("muonTag"),cc);
  getToken_(jetTag_ , conf.getUntrackedParameter<edm::InputTag>("jetTag"),cc);
  getToken_(tauTag_ , conf.getUntrackedParameter<edm::InputTag>("tauTag"),cc);
  getToken_(metTag_ , conf.getUntrackedParameter<edm::InputTag>("metTag"),cc);
  getToken_(phoTag_ , conf.getUntrackedParameter<edm::InputTag>("photonTag"),cc);
  getToken_(ecalRecHitsEBTag_ , conf.getParameter<edm::InputTag>("barrelRecHitTag"),cc);
  getToken_(ecalRecHitsEETag_ , conf.getParameter<edm::InputTag>("endcapRecHitTag"),cc);
  getToken_(ecalReducedRecHitsEBTag_  ,conf.getParameter<edm::InputTag>("reducedBarrelRecHitTag"),cc);
  getToken_(ecalReducedRecHitsEETag_  ,conf.getParameter<edm::InputTag>("reducedEndcapRecHitTag"),cc);
  getToken_(hcalRecHitsTag_ , conf.getParameter<edm::InputTag>("hbheRecHitsTag"),cc);
  getToken_(superClusterEBTag_ ,conf.getParameter<edm::InputTag>("superClusterEBTag"),cc);
  getToken_(superClusterEETag_ ,conf.getParameter<edm::InputTag>("superClusterEETag"),cc);
  getToken_(preShowerClusterXTag_ ,conf.getParameter<edm::InputTag>("preShowerClusterXTag"),cc);
  getToken_(preShowerClusterYTag_ ,conf.getParameter<edm::InputTag>("preShowerClusterYTag"),cc);
  getToken_(gsfEleTag_ , conf.getParameter<edm::InputTag>("gsfEleTag"),cc);
  getToken_(recoPhoTag_ , conf.getParameter<edm::InputTag>("recoPhoTag"),cc);
  getToken_(pfCandidateTag_ , conf.getParameter<edm::InputTag>("pfCandidateTag"),cc);
  getToken_(ctfTrackTag_ , conf.getParameter<edm::InputTag>("ctfTrackTag"),cc);
  getToken_(genParticleTag_ , conf.getParameter<edm::InputTag>("genParticleTag"),cc);
  getToken_(trigEventTag_ , conf.getParameter<edm::InputTag>("trigEventTag"),cc);
  getToken_(trigResultsTag_ ,conf.getParameter<edm::InputTag>("trigResultsTag"),cc);
  getToken_(genEventInfoTag_ , conf.getParameter<edm::InputTag>("genEventInfoTag"),cc);
  getToken_(pileUpMCInfoTag_ , conf.getParameter<edm::InputTag>("pileUpMCInfoTag"),cc);
  getToken_(l1RecordTag_ , conf.getParameter<edm::InputTag>("l1RecordTag"),cc);
  getToken_(l1EmNonIsoTag_ , conf.getParameter<edm::InputTag>("l1EmNonIsoTag"),cc);
  getToken_(l1EmIsoTag_ , conf.getParameter<edm::InputTag>("l1EmIsoTag"),cc);
  getToken_(verticesTag_ , conf.getParameter<edm::InputTag>("verticesTag"),cc);
  getToken_(caloTowersTag_ , conf.getParameter<edm::InputTag>("caloTowersTag"),cc);
  getToken_(eleRhoCorrTag_ , conf.getParameter<edm::InputTag>("eleRhoCorrTag"),cc);
  getToken_(eleRhoCorr2012Tag_ , conf.getParameter<edm::InputTag>("eleRhoCorr2012Tag"),cc);
  getToken_(ecalLaserFilterTag_ , conf.getParameter<edm::InputTag>("ecalLaserFilter"),cc);
  getToken_(pfChargedIsoValEleMapTag_ , conf.getParameter<edm::InputTag>("pfChargedIsoValEleMapTag"),cc);
  getToken_(pfPhotonIsoValEleMapTag_ , conf.getParameter<edm::InputTag>("pfPhotonIsoValEleMapTag"),cc); 
  getToken_(pfNeutralIsoValEleMapTag_ , conf.getParameter<edm::InputTag>("pfNeutralIsoValEleMapTag"),cc); 
  getToken_(pfClustersECALTag_ , conf.getParameter<edm::InputTag>("pfClustersECALTag"),cc);
  getToken_(pfClustersHCALTag_ , conf.getParameter<edm::InputTag>("pfClustersHCALTag"),cc);
  getToken_(gsfEleToPFCandMapTag_ , conf.getParameter<edm::InputTag>("gsfEleToPFCandMapTag"),cc);
  getToken_(heepIDVIDTag_ ,conf.getParameter<edm::InputTag>("heepIDVID"),cc);
  getToken_(lheEventTag_ ,conf.getParameter<edm::InputTag>("lheEventTag"),cc);  
  getToken_(genEvtInfoTag_ ,conf.getParameter<edm::InputTag>("genEvtInfoTag"),cc); 
  getToken_(egHLTCandsTag_ ,conf.getParameter<edm::InputTag>("egHLTCandsTag"),cc); 
  getToken_(egHLTCandsUnseededTag_ ,conf.getParameter<edm::InputTag>("egHLTCandsUnseededTag"),cc); 
  

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

  
}

void heep::EventHelper::makeHeepEvent(const edm::Event& edmEvent,const edm::EventSetup& setup,heep::Event& heepEvent)const
{
  heepEvent.setHLTPSProvider(hltPSProvider_);
  setHandles(edmEvent,setup,heepEvent.handles()); 
  if(heepEleSource_==0) fillHEEPElesFromGsfEles(heepEvent.handles(),heepEvent.heepEles());
  else if(heepEleSource_==1) fillHEEPElesFromPat(heepEvent.handles(),heepEvent.heepEles());
  heepEvent.setEvent(edmEvent); 
  heepEvent.setEventSetup(setup);
  heep::TrigCodes::TrigBitSet bits = heep::trigtools::getHLTFiltersPassed(hltFiltersToCheckWithNrCands_,heepEvent.handles().trigEvent,hltProcName_);
  heepEvent.setTrigBits(bits);
}

void heep::EventHelper::makeHeepEvent(const edm::Run& run,const edm::EventSetup& setup,heep::Event& heepEvent)const
{
  heepEvent.setHLTPSProvider(hltPSProvider_);
  heepEvent.initHLTConfig(run,setup,hltProcName_);
}

void heep::EventHelper::setHandles(const edm::Event& event,const edm::EventSetup& setup,heep::EvtHandles& handles)const
{ 
  event.getByToken(muonTag_,handles.muon);
  event.getByToken(jetTag_,handles.jet);
  event.getByToken(eleTag_,handles.electron);
  event.getByToken(metTag_,handles.met);
  event.getByToken(phoTag_,handles.pho);
  event.getByToken(tauTag_,handles.tau);
  event.getByToken(ecalRecHitsEBTag_,handles.ebRecHits);
  event.getByToken(ecalRecHitsEETag_,handles.eeRecHits);
  event.getByToken(ecalReducedRecHitsEBTag_,handles.ebReducedRecHits);
  event.getByToken(ecalReducedRecHitsEETag_,handles.eeReducedRecHits);
  event.getByToken(hcalRecHitsTag_,handles.hbheRecHits);
  event.getByToken(gsfEleTag_,handles.gsfEle);
  event.getByToken(recoPhoTag_,handles.recoPho);
  event.getByToken(pfCandidateTag_,handles.pfCandidate);
  event.getByToken(superClusterEBTag_,handles.superClusEB);
  event.getByToken(superClusterEETag_,handles.superClusEE); 
  event.getByToken(preShowerClusterXTag_,handles.preShowerClusX);
  event.getByToken(preShowerClusterYTag_,handles.preShowerClusY); 
  event.getByToken(ctfTrackTag_,handles.ctfTrack);
  event.getByToken(genParticleTag_,handles.genParticle);
  event.getByToken(trigEventTag_,handles.trigEvent);
  event.getByToken(trigResultsTag_,handles.trigResults);
  event.getByToken(genEventInfoTag_,handles.genEventInfo);  
  event.getByToken(pileUpMCInfoTag_,handles.pileUpMCInfo);
  event.getByToken(l1RecordTag_,handles.l1Record);
  event.getByToken(l1EmNonIsoTag_,handles.l1EmNonIso);
  event.getByToken(l1EmIsoTag_,handles.l1EmIso);
  event.getByToken(verticesTag_,handles.vertices);
  event.getByToken(caloTowersTag_,handles.caloTowers);
  event.getByToken(eleRhoCorrTag_,handles.eleRhoCorr);
  event.getByToken(eleRhoCorr2012Tag_,handles.eleRhoCorr2012);
  event.getByToken(ecalLaserFilterTag_,handles.ecalLaserFilter);
  event.getByToken(pfChargedIsoValEleMapTag_,handles.pfChargedIsoValEleMap);
  event.getByToken(pfPhotonIsoValEleMapTag_,handles.pfPhotonIsoValEleMap);
  event.getByToken(pfNeutralIsoValEleMapTag_,handles.pfNeutralIsoValEleMap);
  event.getByToken(pfClustersECALTag_,handles.pfClustersECAL);
  event.getByToken(pfClustersHCALTag_,handles.pfClustersHCAL);
  event.getByToken(gsfEleToPFCandMapTag_,handles.gsfEleToPFCandMap);
  event.getByToken(heepIDVIDTag_,handles.heepIDVID);
  event.getByToken(lheEventTag_,handles.lheEvent);
  event.getByToken(genEvtInfoTag_,handles.genEvtInfo);
  event.getByToken(egHLTCandsTag_,handles.egHLTCands);
  event.getByToken(egHLTCandsUnseededTag_,handles.egHLTCandsUnseeded);
  //event.getByType(handles.beamSpot);

  setup.get<CaloGeometryRecord>().get(handles.caloGeom);
  setup.get<CaloTopologyRecord>().get(handles.caloTopology);
  //setup.get<L1GtTriggerMenuRcd>().get(handles.l1Menu);
  //setup.get<TrackerDigiGeometryRecord>().get(handles.trackGeom);
  setup.get<IdealMagneticFieldRecord>().get(handles.bField);
  
  gsfEleExtraFiller_.getEvtContent(event,setup);
}

//fills the heepEles vector using pat electrons as starting point
void heep::EventHelper::fillHEEPElesFromPat(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const
{
  heepEles.clear();
  if(!handles.electron.isValid()) return;
  const edm::Handle<edm::View<pat::Electron> >& eles = handles.electron;
  for(edm::View<pat::Electron>::const_iterator eleIt = eles->begin(); eleIt!=eles->end(); ++eleIt){ 
    if(!onlyAddEcalDriven_ || eleIt->ecalDrivenSeed()){
      edm::Ptr<reco::GsfElectron> elePtr(eles,eleIt-eles->begin());
      addHEEPEle_(elePtr,handles,heepEles);
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
  if(!handles.gsfEle.isValid()) return;
  const edm::Handle<std::vector<reco::GsfElectron> >& eles = handles.gsfEle;
  for(std::vector<reco::GsfElectron>::const_iterator eleIt = eles->begin(); eleIt!=eles->end(); ++eleIt){ 
    if(!onlyAddEcalDriven_ || eleIt->ecalDrivenSeed()){
      edm::Ptr<reco::GsfElectron> elePtr(eles,eleIt-eles->begin());
      addHEEPEle_(elePtr,handles,heepEles);
    }
  }
  //the electrons are now filled, lets add trigger info
  if(handles.trigEvent.isValid() && handles.trigResults.isValid()){
    heep::trigtools::setHLTFiltersObjPasses(heepEles,hltFiltersToCheck_,handles.trigEvent,hltProcName_,
					    maxDRTrigMatch_,maxPtRelDiffTrigMatch_);
  }
}

//this converts the pat::Electron / reco::GsfElectron into a heep::Electron
void heep::EventHelper::addHEEPEle_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const
{
  heepEles.push_back(heep::Ele(gsfEle));
  heep::Ele& ele =  heepEles.back();  
  //all the extra stuff is set here now
  ele.setGsfEleExtra(gsfEleExtraFiller_(gsfEle));
  
  //now we would like to set the cut results, this has to come after setting isolation parameters
  ele.setCutCode(cuts_.getCutCode(ele)); 
  //int cutCode = cuts_.getCutCode(*handles.eleRhoCorr,pvPos,gsfEle);
  //if(cutCode!=ele.cutCode()) std::cout <<" cutCode "<<ele.cutCode()<<" "<<cuts_.getCutCode(*handles.eleRhoCorr,pvPos,gsfEle)<<" rho "<<*handles.eleRhoCorr<<" pv z "<<pvPos.z()<<std::endl;
}

