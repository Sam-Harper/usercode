
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
  ctfTrackTag_ = conf.getParameter<edm::InputTag>("ctfTrackTag");
  genParticleTag_ = conf.getParameter<edm::InputTag>("genParticleTag");
  trigEventTag_ = conf.getParameter<edm::InputTag>("trigEventTag");
  genEventPtHatTag_ = conf.getParameter<edm::InputTag>("genEventPtHatTag");

  //trig matching parameters
  hltProcName_ = conf.getParameter<std::string>("hltProcName");
  maxDRTrigMatch_ = conf.getParameter<double>("maxDRTrigMatch");
  maxPtRelDiffTrigMatch_ = conf.getParameter<double>("maxPtRelDiffTrigMatch");
  hltFiltersToCheck_ = conf.getParameter<std::vector<std::string> >("hltFiltersToCheck");
  heep::TrigCodes::setCodes(hltFiltersToCheck_); //this assigns a unique bit to each trigger name, without this no trigger codes are defined. If you recieve a trigger not found error, its because it wasnt in the vector passed to this function
}

void heep::EventHelper::makeHeepEvent(const edm::Event& edmEvent,const edm::EventSetup& setup,heep::Event& heepEvent)const
{
  setHandles(edmEvent,setup,heepEvent.handles());
  fillHEEPElesFromPat(heepEvent.handles(),heepEvent.heepElectrons());
  heepEvent.setEvent(edmEvent);
}


void heep::EventHelper::setHandles(const edm::Event& event,const edm::EventSetup& setup,heep::EvtHandles& handles)const
{ 
  //yay, now in 2_1 we dont have to program by exception
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
  event.getByLabel(superClusterEBTag_,handles.superClusEB);
  event.getByLabel(superClusterEETag_,handles.superClusEE); 
  event.getByLabel(ctfTrackTag_,handles.ctfTrack);
  event.getByLabel(genParticleTag_,handles.genParticle);
  event.getByLabel(trigEventTag_,handles.trigEvent);
  event.getByLabel(genEventPtHatTag_,handles.genEventPtHat);
  
  setup.get<CaloGeometryRecord>().get(handles.caloGeom);
  setup.get<CaloTopologyRecord>().get(handles.caloTopology);
 
}

//fills the heepEles vector using pat electrons as starting point
void heep::EventHelper::fillHEEPElesFromPat(const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const
{
  heepEles.clear();
  const edm::View<pat::Electron>& eles = *handles.electron;
  for(edm::View<pat::Electron>::const_iterator eleIt = eles.begin(); eleIt!=eles.end(); ++eleIt){
    addHEEPEle(*eleIt,handles,heepEles);
  }
  //the electrons are now filled, lets add trigger info
  heep::trigtools::setHLTFiltersObjPasses(heepEles,hltFiltersToCheck_,handles.trigEvent,hltProcName_,
					  maxDRTrigMatch_,maxPtRelDiffTrigMatch_);
}

//this converts the pat::Electron into a heep::Electron
void heep::EventHelper::addHEEPEle(const pat::Electron& patEle,const heep::EvtHandles& handles,std::vector<heep::Ele>& heepEles)const
{
  heep::Ele::IsolData isolData;
  fillIsolData(patEle,isolData);
  heep::Ele::ClusShapeData clusShapeData;
  fillClusShapeData(patEle,clusShapeData); 
  heepEles.push_back(heep::Ele(patEle,clusShapeData,isolData));
  
  //now we would like to set the cut results
  heep::Ele& ele =  heepEles.back();
  ele.setCutCode(cuts_.getCutCode(ele)); 

}

void heep::EventHelper::fillIsolData(const pat::Electron &patEle,heep::Ele::IsolData& isolData)const
{ 
  isolData.em = patEle.ecalIso();
  isolData.hadDepth1 = patEle.userIso(0);
  isolData.hadDepth2 = patEle.userIso(1);
  isolData.ptTrks = patEle.trackIso(); 
  isolData.nrTrks=999; //not defined
}

//fills it directly from the PAT electron
void heep::EventHelper::fillClusShapeData(const pat::Electron &patEle,heep::Ele::ClusShapeData& clusShapeData)const
{
  clusShapeData.sigmaEtaEta=patEle.scSigmaEtaEta();
  clusShapeData.sigmaIEtaIEta=patEle.scSigmaIEtaIEta();
  clusShapeData.e2x5MaxOver5x5=patEle.scE2x5Max()/patEle.scE5x5();
  clusShapeData.e1x5Over5x5=patEle.scE1x5()/patEle.scE5x5();
  clusShapeData.e5x5=patEle.scE5x5();
}

//recalculates cluster shape variables from scratch
void heep::EventHelper::fillClusShapeData(const reco::BasicCluster& seedClus,const heep::EvtHandles& handles,heep::Ele::ClusShapeData& clusShapeData)const
{
  clusShapeData.sigmaEtaEta=999.;
  clusShapeData.sigmaIEtaIEta=999.;
  clusShapeData.e2x5MaxOver5x5=-1.; 
  clusShapeData.e1x5Over5x5=-1.;
  clusShapeData.e5x5=-999.;
  
  const DetId firstDetId = seedClus.getHitsByDetId()[0]; //note this  not actually be the seed hit but it doesnt matter because all hits will be in the barrel OR endcap (it is also incredably inefficient as it getHitsByDetId passes the vector by value not reference)

  const CaloGeometry* caloGeom = handles.caloGeom.product();
  const CaloTopology* caloTopology = handles.caloTopology.product();
  const EcalRecHitCollection* ebRecHits = handles.ebReducedRecHits.product();
  const EcalRecHitCollection* eeRecHits = handles.eeReducedRecHits.product();

  if(firstDetId.subdetId()==EcalBarrel){
    std::vector<float> stdCov = EcalClusterTools::covariances(seedClus,ebRecHits,caloTopology,caloGeom);
    std::vector<float> crysCov = EcalClusterTools::localCovariances(seedClus,ebRecHits,caloTopology);
    clusShapeData.sigmaEtaEta = sqrt(stdCov[0]);
    clusShapeData.sigmaIEtaIEta =  sqrt(crysCov[0]); 
    float e5x5 =  EcalClusterTools::e5x5(seedClus,ebRecHits,caloTopology);
    clusShapeData.e5x5=e5x5;
    if(e5x5!=0.) {
      clusShapeData.e2x5MaxOver5x5 = EcalClusterTools::e2x5Max(seedClus,ebRecHits,caloTopology)/e5x5;
      clusShapeData.e1x5Over5x5 = EcalClusterTools::e1x5(seedClus,ebRecHits,caloTopology)/e5x5; //from V00-05-19 of RecoEcal/EgammaCoreTools e1x5 now gives e1x5
    }
  }else if(firstDetId.subdetId()==EcalEndcap){ 

 
    std::vector<float> stdCov = EcalClusterTools::covariances(seedClus,eeRecHits,caloTopology,caloGeom); 
    std::vector<float> crysCov = EcalClusterTools::localCovariances(seedClus,eeRecHits,caloTopology);
    clusShapeData.sigmaEtaEta = sqrt(stdCov[0]);
    clusShapeData.sigmaIEtaIEta =  sqrt(crysCov[0]); 
    float e5x5 =  EcalClusterTools::e5x5(seedClus,eeRecHits,caloTopology);
    clusShapeData.e5x5=e5x5;
    if(e5x5!=0.) {
      clusShapeData.e2x5MaxOver5x5 = EcalClusterTools::e2x5Max(seedClus,eeRecHits,caloTopology)/e5x5;
      clusShapeData.e1x5Over5x5 = EcalClusterTools::e1x5(seedClus,eeRecHits,caloTopology)/e5x5; //from V00-05-19 of RecoEcal/EgammaCoreTools e1x5 now gives e1x5
    }
  }
}

