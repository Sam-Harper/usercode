#include "SHarper/SHNtupliser/interface/SHEventHelper.h"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"

#include "SHarper/SHNtupliser/interface/HackedFuncs.h"

#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"
#include "SHarper/SHNtupliser/interface/SHEleCMSSWStructs.hh"

#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "DataFormats/MuonReco/interface/MuonCocktails.h"

#include "TrackingTools/GsfTools/interface/MultiTrajectoryStateTransform.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include "TVector3.h"
#include "TBits.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"

#include "DataFormats/METReco/interface/PFMET.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Scalers/interface/DcsStatus.h"

//horrible hack of a function to fix annoying things
void fixClusterShape(const reco::CaloCluster& seedCluster,const heep::Event& heepEvent,SHElectron& ele);

#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
SHEventHelper::SHEventHelper(int datasetCode,float eventWeight):
  datasetCode_(datasetCode),
  eventWeight_(eventWeight),
  isMC_(true),
  nrGenPartToStore_(0)
{
  initEcalHitVec_();
  initHcalHitVec_();
}
void SHEventHelper::setup(const edm::ParameterSet& conf)
{ 
  minEtToPromoteSC_ = conf.getParameter<double>("minEtToPromoteSC");
  addMet_ = conf.getParameter<bool>("addMet");
  addJets_ = conf.getParameter<bool>("addJets");
  addMuons_ = conf.getParameter<bool>("addMuons");
  applyMuonId_ = conf.getParameter<bool>("applyMuonId");
  addTrigs_ = conf.getParameter<bool>("addTrigs");
  addCaloTowers_ = conf.getParameter<bool>("addCaloTowers");
  addIsolTrks_ = conf.getParameter<bool>("addIsolTrks");
  fillFromGsfEle_ = conf.getParameter<bool>("fillFromGsfEle");
 
  hltDebugFiltersToSave_ = conf.getParameter<std::vector<std::string> >("hltDebugFiltersToSave");
  useHLTDebug_ = conf.getParameter<bool>("useHLTDebug");  
  hltTag_ = conf.getParameter<std::string>("hltProcName");
  
  nrGenPartToStore_ = conf.getParameter<int>("nrGenPartToStore");
  noFracShowerShape_ = conf.getParameter<bool>("noFracShowerShape");
  
  std::cout <<"warning, disabling use of HLT debug"<<std::endl;
  //useHLTDebug_=false;
  eleMVA_.reset(new ElectronMVAEstimator(edm::FileInPath ( conf.getParameter<std::string>("eleIsolMVAWeightFile").c_str() ).fullPath()));

  tracklessEleMaker_.setup(conf);
}



void SHEventHelper::makeSHEvent(const heep::Event & heepEvent, SHEvent& shEvent)const

{   
  const bool debug=false;
  if(debug) std::cout <<"making event "<<std::endl;
  shEvent.clear(); //reseting the event 
  //it is currently critical that calo hits are filled first as they are used in constructing the basic clusters
  if(debug) std::cout <<"adding calo hits"<<std::endl;
  addCaloHits(heepEvent,shEvent);
  if(debug) std::cout <<"adding calo towers "<<addCaloTowers_<<std::endl;
  if(addCaloTowers_) addCaloTowers(heepEvent,shEvent);
  if(debug) std::cout <<"adding event para "<<std::endl;
  addEventPara(heepEvent,shEvent); //this must be filled second (ele + mu need beam spot info)
  if(debug) std::cout <<"adding superclusters"<<std::endl;
  addSuperClusters(heepEvent,shEvent);
  if(debug) std::cout <<"adding electrons "<<std::endl;
  addElectrons(heepEvent,shEvent);
  if(debug) std::cout <<"adding preshower "<<std::endl; 
  // addPreShowerClusters(heepEvent,shEvent);
  if(debug) std::cout <<"adding muons "<<std::endl;
  if(addMuons_) addMuons(heepEvent,shEvent); 
 
  
  if(addTrigs_ && !useHLTDebug_) addTrigInfo(heepEvent,shEvent);
  else if(addTrigs_){
    edm::Handle<trigger::TriggerEventWithRefs> trigEventWithRefs;
    edm::InputTag trigTag("hltTriggerSummaryRAW","",hltTag_);
    heepEvent.event().getByLabel(trigTag,trigEventWithRefs);
    if(debug) std::cout <<"adding triggers"<<std::endl;
    addTrigDebugInfo(heepEvent,shEvent,*trigEventWithRefs,hltDebugFiltersToSave_,hltTag_);
  }
  // addL1Info(heepEvent,shEvent); //due to a bug l1 info is not stored in summer 09 samples
  if(debug)std::cout <<"adding jets"<<std::endl;
  if(addJets_) addJets(heepEvent,shEvent);
  if(debug) std::cout <<"adding met "<<std::endl;
  if(addMet_) addMet(heepEvent,shEvent);
  if(debug) std::cout <<"adding mc particles "<<std::endl;
  addMCParticles(heepEvent,shEvent);  
  if(debug) std::cout <<"adding isol tracks"<<std::endl;
  if(addIsolTrks_) addIsolTrks(heepEvent,shEvent);
  if(debug) std::cout <<"made event "<<std::endl;
}

void SHEventHelper::addEventPara(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  // std::cout <<"atart"<<std::endl;
  //std::cout <<" adding event "<<&(heepEvent.event())<<std::endl;
  //std::cout <<"run nr "<< heepEvent.runnr()<<std::endl;
  //std::cout <<"getting run nr "<<std::endl;
  shEvent.setRunnr(heepEvent.runnr());
  //std::cout <<"eventnr "<<std::endl;
  shEvent.setEventnr(heepEvent.eventnr());
  //std::cout <<"mv "<<std::endl;
  shEvent.setIsMC(isMC_);
  //std::cout <<"dataset "<<std::endl;
  shEvent.setDatasetCode(datasetCode_);
  //std::cout <<"weight "<<std::endl;
  shEvent.setWeight(eventWeight_);
  //std::cout <<"gen pt hat "<<std::endl;
  shEvent.setGenEventPtHat(heepEvent.genEventPtHat());
  shEvent.setBX(heepEvent.bx());
  shEvent.setLumiSec(heepEvent.lumiSec());
  shEvent.setTime(heepEvent.time());
  shEvent.setOrbitNumber(heepEvent.orbitNumber());
  shEvent.setNrVertices(-1);
  shEvent.setPreScaleCol(heepEvent.preScaleColumn());
   TVector3 vtxPos;
   if(heepEvent.handles().vertices.isValid() && heepEvent.handles().vertices->size()>0){  
     const reco::Vertex& vertex = heepEvent.handles().vertices->front();
     vtxPos.SetXYZ(vertex.x(),vertex.y(),vertex.z()); 
     shEvent.setNrVertices(heepEvent.handles().vertices->size());
     const std::vector<reco::Vertex>& vertices = *(heepEvent.handles().vertices);
     for(size_t vtxNr=0;vtxNr<vertices.size();vtxNr++){
       shEvent.addVertex(vertices[vtxNr]);
     }

   }
   shEvent.setVertex(vtxPos);
   TVector3 bs;
   if(heepEvent.handles().beamSpot.isValid()){
     math::XYZPoint bsCMSSW = heepEvent.handles().beamSpot->position();
     bs.SetXYZ(bsCMSSW.x(),bsCMSSW.y(),bsCMSSW.z());
   }
   shEvent.setBeamSpot(bs);
  
   if(heepEvent.handles().eleRhoCorr.isValid()) shEvent.setEleRhoCorr(heepEvent.eleRhoCorr());
   else shEvent.setEleRhoCorr(-999);
   if(heepEvent.handles().eleRhoCorr2012.isValid()) shEvent.setRhoCorr(heepEvent.eleRhoCorr2012());
   else shEvent.setRhoCorr(-999);

   if(heepEvent.passEcalLaserFilter()) shEvent.setFlags(1);
   else shEvent.setFlags(0);

     // std::cout <<" rho "<<heepEvent.eleRhoCorr()<<std::endl;
     // edm::Handle<double> pfHand;
     // edm::InputTag pfTag("kt6PFJetsPFlow","rho");
     // heepEvent.event().getByLabel(pfTag,pfHand);
     //  std::cout <<" pf rho "<<*pfHand<<std::endl;
     //std::cout <<" i "<<i<<"/"<<heepEvent.eleRhoCorr().size()<<" rho "<<heepEvent.eleRhoCorr()[i]<<std::endl;
       
   // }
 
  //std::cout <<"done "<<std::endl;
}

//this now handles the sc promotion via photons
//photons have just a H/E<0.5 cut which is fine for our needs
void SHEventHelper::addElectrons(const heep::Event& heepEvent, SHEvent& shEvent)const
{  

  if(!heepEvent.handles().gsfEle.isValid()) return; //protection when the colleciton doesnt exist

  // const std::vector<heep::Ele>& electrons = heepEvent.heepEles();
  const std::vector<reco::GsfElectron>& electrons = heepEvent.gsfEles();
  const std::vector<reco::Photon>& photons = heepEvent.recoPhos();
  //std::cout <<"nr electrons "<<electrons.size()<<std::endl;
  // std::cout <<"nr photons "<<photons.size()<<std::endl;
  for(size_t phoNr=0;phoNr<photons.size();phoNr++){
    const reco::SuperCluster& sc = *photons[phoNr].superCluster();
    size_t eleNr = matchToEle(sc,electrons);
    if(eleNr<electrons.size()) addElectron(heepEvent,shEvent,electrons[eleNr]);
    
    else if(sc.energy()*sin(sc.position().theta())>minEtToPromoteSC_ && minEtToPromoteSC_<10000) addElectron(heepEvent,shEvent,photons[phoNr]);
  }
  
}

void SHEventHelper::addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const reco::GsfElectron& gsfEle)const
{
  MultiTrajectoryStateTransform trajStateTransform(heepEvent.handles().trackGeom.product(),heepEvent.handles().bField.product());
  shEvent.addElectron(gsfEle,shEvent.getCaloHits());
  SHElectron* shEle = shEvent.getElectron(shEvent.nrElectrons()-1);
  //and here come the hacks, things that are not correctly filled yet....
  shEle->setIsolMVA(eleMVA_->mva(gsfEle,heepEvent.handles().vertices->size()));
  shEle->setPassMVAPreSel(shEle->isolMVA()>=-0.1);
  shEle->setPassPFlowPreSel(gsfEle.mvaOutput().status==3); 
  //redoing shower shape variables (only necessary for photons now )
  //  if(noFracShowerShape_) fixClusterShape(*(gsfEle.superCluster()->seed()),heepEvent,*shEle);
  

  if(shEle->seedId()!=0 && false){
    
    const TVector3& seedPos = GeomFuncs::getCell(shEle->seedId()).pos();
    GlobalPoint posGP(seedPos.X(),seedPos.Y(),seedPos.Z());  
    
    //  std::cout <<" pt "<<gsfEle.gsfTrack()->pt()<<std::endl;
    TrajectoryStateOnSurface outTSOS = trajStateTransform.outerStateOnSurface(*gsfEle.gsfTrack());
    TrajectoryStateOnSurface innTSOS = trajStateTransform.innerStateOnSurface(*gsfEle.gsfTrack());
    TrajectoryStateOnSurface outToSeedTSOS = trajStateTransform.extrapolatedState(outTSOS,posGP);
    TrajectoryStateOnSurface innToSeedTSOS = trajStateTransform.extrapolatedState(innTSOS,posGP);
    
    TVector3 dummy; 
    dummy.SetPtEtaPhi(0.00001,-10,0);
    if(outToSeedTSOS.isValid()){
      TVector3 outToSeedPos(outToSeedTSOS.globalPosition().x(),outToSeedTSOS.globalPosition().y(),outToSeedTSOS.globalPosition().z()); 
      shEle->setPosTrackOutToSeed(outToSeedPos); 
    }else shEle->setPosTrackOutToSeed(dummy);
    
    if(innToSeedTSOS.isValid()){
      TVector3 innToSeedPos(innToSeedTSOS.globalPosition().x(),innToSeedTSOS.globalPosition().y(),innToSeedTSOS.globalPosition().z());
      shEle->setPosTrackInnToSeed(innToSeedPos);  
    }else shEle->setPosTrackInnToSeed(dummy);
    //  std::cout <<"set state"<<std::endl;
  }


  //std::cout <<"ele "<<gsfEle.et()<<" eta "<<gsfEle.eta()<<" miss hits "<<gsfEle.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits()<<" covDist "<<gsfEle.convDist()<<" dcot "<<gsfEle.convDcot()<<" radius "<<gsfEle.convRadius()<<std::endl;

 //  double bField=0;
//   if(heepEvent.runnr()>100000){ //hack to id data
//     edm::Handle<DcsStatusCollection> dcsHandle;
//     heepEvent.event().getByLabel("scalersRawToDigi",dcsHandle);
//     float currentToBFieldScaleFactor = 2.09237036221512717e-04;
//     float current = (*dcsHandle)[0].magnetCurrent();
//     bField = current*currentToBFieldScaleFactor;
//   }else{
//     bField = heepEvent.handles().bField->inTesla(GlobalPoint(0,0,0)).z();
//   }
      
//   ConversionFinder conFind;
//   ConversionInfo convInfo = conFind.getConversionInfo(gsfEle,heepEvent.handles().ctfTrack,bField);
//   shEle->setConvInfo(convInfo.dist(),convInfo.dcot());
  

}


void SHEventHelper::addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const reco::Photon& photon)const
{
  //std::cout <<"adding photon"<<std::endl;
  shEvent.addElectron(photon,shEvent.getCaloHits());
  //if(noFracShowerShape_){
  //  SHElectron* shEle = shEvent.getElectron(shEvent.nrElectrons()-1);
  //  fixClusterShape(*(photon.superCluster()->seed()),heepEvent,*shEle);
  // }
//   double bField=0;
//   if(heepEvent.runnr()>100000){ //hack to id data
//     edm::Handle<DcsStatusCollection> dcsHandle;
//     heepEvent.event().getByLabel("scalersRawToDigi",dcsHandle);
//     float currentToBFieldScaleFactor = 2.09237036221512717e-04;
//     float current = (*dcsHandle)[0].magnetCurrent();
//     bField = current*currentToBFieldScaleFactor;
//   }else{
//     bField = heepEvent.handles().bField->inTesla(GlobalPoint(0,0,0)).z();
//   }
      
//   ConversionFinder conFind;
//   ConversionInfo convInfo = conFind.getConversionInfo(photon,heepEvent.handles().ctfTrack,bField);
//   shEle->setConvInfo(convInfo.dist(),convInfo.dcot());

}


void SHEventHelper::addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const reco::SuperCluster& superClus)const
{

  cmssw::IsolationVariables isol03 = tracklessEleMaker_.getIsol(superClus,heepEvent,0.3);
  cmssw::IsolationVariables isol04 = tracklessEleMaker_.getIsol(superClus,heepEvent,0.4);
  cmssw::FiducialFlags fid = tracklessEleMaker_.getFid(superClus,heepEvent);
  cmssw::ShowerShape shape = tracklessEleMaker_.getShowerShape(superClus,heepEvent,fid.isEB); 
  TLorentzVector p4 = tracklessEleMaker_.getP4(superClus,heepEvent);
  
  shEvent.addElectron(p4,superClus,fid,shape,isol03,isol04,shEvent.getCaloHits());
  
}

void SHEventHelper::addMuons(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().muon.isValid()){
    for(size_t muNr=0;muNr<heepEvent.muons().size();muNr++){
      const reco::Muon& muon = heepEvent.muons()[muNr];
      
      if(muon.isGlobalMuon() && (!applyMuonId_ || passMuonId(muon,heepEvent))) shEvent.addMuon(muon);
    }
  }
}

bool SHEventHelper::passMuonId(const reco::Muon& muon,const heep::Event& heepEvent)
{
  if(muon.isGlobalMuon() && 
     muon.globalTrack()->hitPattern().numberOfValidMuonHits()>0 &&
     muon.numberOfMatchedStations()>1 &&
     muon.globalTrack()->hitPattern().numberOfValidPixelHits()>0 && 
     muon.globalTrack()->hitPattern().trackerLayersWithMeasurement() > 5 ){
    // reco::TrackRef cktTrackRef = (muon::tevOptimized(muon, 200, 17., 40., 0.25)).first; //53X
    reco::TrackRef cktTrackRef = (muon::tevOptimized(muon)).first; //60X
    const reco::Track& cktTrack = *cktTrackRef;
    const reco::Vertex& vertex = heepEvent.handles().vertices->front();
    if(cktTrack.ptError()/cktTrack.pt()<0.3 && 
       fabs(cktTrack.dxy(vertex.position())) < 0.2 &&
       fabs(cktTrack.dz(vertex.position())) < 0.5) return true;
    
  }
  return false;
}

size_t SHEventHelper::matchToEle(const reco::SuperCluster& superClus,const std::vector<reco::GsfElectron> eles)const
{
  //  std::cout <<"photon sc "<<superClus<<std::endl;
 
  //  std::cout <<"start "<<std::endl;
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    const reco::GsfElectron& ele = eles[eleNr];
    
    //std::cout <<"eleNr "<<eleNr<<std::endl;//" "<<(*ele.superCluster())<<std::endl;
    //if(math::deltaR2(ele.superCluster->eta(),ele.superCluster->phi(),superClus.eta(),superClus.phi())<0.05*0.05) return eleNr;

    if(ele.superCluster()->seed()->hitsAndFractions()[0].first==superClus.seed()->hitsAndFractions()[0].first) return eleNr;
  //   const reco::SuperClusterRef sc = ele.superCluster();
//     //std::cout <<"nr basic clusters "<<sc->clustersSize()<<std::endl;
//     for(auto it=sc->clustersBegin();it!=sc->clustersEnd();++it){
//       const std::vector< std::pair<DetId, float> > & hits = (*it)->hitsAndFractions();
//       //std::cout <<"clus energy "<<(*it)->energy()<<" nr hits"<<hits.size()<<std::endl;
//       for(size_t hitNr=0;hitNr<hits.size();hitNr++){
// 	if(hits[hitNr].first.subdetId()==1){
// 	  EBDetId ebDetId(hits[hitNr].first);
// 	  std::cout <<"iEta "<<ebDetId.ieta()<<" iPhi "<<ebDetId.iphi()<<" frac "<<hits[hitNr].second<<std::endl;
// 	}else{
// 	  EEDetId eeDetId(hits[hitNr].first);
// 	  std::cout <<"ix "<<eeDetId.ix()<<" iy "<<eeDetId.iy()<<" frac "<<hits[hitNr].second<<std::endl;
// 	}
//       }
//     }

//   }
//   std::cout <<"end "<<std::endl;
  }
  return eles.size();
}

size_t SHEventHelper::matchToEle(const reco::SuperCluster& superClus,const std::vector<heep::Ele> eles)const
{
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    const reco::GsfElectron& ele = eles[eleNr].gsfEle();
    if(ele.superCluster()->seed()->hitsAndFractions()[0].first==superClus.seed()->hitsAndFractions()[0].first) return eleNr;
  }
  return eles.size();
}

void SHEventHelper::addSuperClusters(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  if(heepEvent.handles().superClusEB.isValid()){
    const std::vector<reco::SuperCluster>& superClusEB = heepEvent.superClustersEB(); 
    for(size_t superClusNr=0;superClusNr<superClusEB.size();superClusNr++){
      shEvent.addSuperCluster(superClusEB[superClusNr],shEvent.getCaloHits());
    }
  }
  if(heepEvent.handles().superClusEB.isValid()){
  //now endcap
    const std::vector<reco::SuperCluster>& superClusEE = heepEvent.superClustersEE(); 
    for(size_t superClusNr=0;superClusNr<superClusEE.size();superClusNr++){
      shEvent.addSuperCluster(superClusEE[superClusNr],shEvent.getCaloHits());
    }
  }
}

void SHEventHelper::addPreShowerClusters(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  if(heepEvent.handles().preShowerClusX.isValid()){
    const std::vector<reco::PreshowerCluster>& preShowerClus = *(heepEvent.handles().preShowerClusX);
    for(size_t clusNr=0;clusNr<preShowerClus.size();clusNr++){
      shEvent.addPreShowerCluster(preShowerClus[clusNr]);
    }
  }
  if(heepEvent.handles().preShowerClusY.isValid()){
    const std::vector<reco::PreshowerCluster>& preShowerClus = *(heepEvent.handles().preShowerClusY); 
    for(size_t clusNr=0;clusNr<preShowerClus.size();clusNr++){
      shEvent.addPreShowerCluster(preShowerClus[clusNr]);
    }
  }
}

void SHEventHelper::addCaloHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  addEcalHits(heepEvent,shEvent);
  addHcalHits(heepEvent,shEvent);
}

//we first zero the vector of the ecal hits (note we keep this persistant because we dont want to keep allocating memory)
//we then loop over the barrel and endcap hits and add them to the vector
//we then add the vector to the event
void SHEventHelper::addEcalHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  for(size_t i=0;i<ecalHitVec_.size();i++) ecalHitVec_[i].setNrgy(-999.);
  
  const EcalRecHitCollection* ebHits =heepEvent.handles().ebRecHits.isValid() ? heepEvent.ebHitsFull() : 
    heepEvent.handles().ebReducedRecHits.isValid() ? &(*heepEvent.handles().ebReducedRecHits) : NULL;
  const EcalRecHitCollection* eeHits =heepEvent.handles().eeRecHits.isValid() ? heepEvent.eeHitsFull() : 
    heepEvent.handles().eeReducedRecHits.isValid() ? &(*heepEvent.handles().eeReducedRecHits) : NULL;
  


  if(ebHits!=NULL){
    for(EcalRecHitCollection::const_iterator hitIt = ebHits->begin();
	hitIt!=ebHits->end();++hitIt){
      SHCaloHit& shHit = ecalHitVec_[ecalHitHash_(hitIt->detid())];
      shHit.setNrgy(hitIt->energy());
      shHit.setTime(hitIt->time());
      //shHit.setFlag(hitIt->flags());
      shHit.setFlag(0);//temp fix
      shHit.setFlagBits(getEcalFlagBits_(*hitIt));
      
    }
  }//end of null check on ebHits

  if(eeHits!=NULL){
    for(EcalRecHitCollection::const_iterator hitIt = eeHits->begin();
	hitIt!=eeHits->end();++hitIt){
      SHCaloHit& shHit = ecalHitVec_[ecalHitHash_(hitIt->detid())];
      shHit.setNrgy(hitIt->energy());
      shHit.setTime(hitIt->time());  
      //shHit.setFlag(hitIt->flags()); 
      shHit.setFlag(0),
      shHit.setFlagBits(getEcalFlagBits_(*hitIt));
      
    }
  }//end of null check on eeHits

  shEvent.addEcalHits(ecalHitVec_);

}

void SHEventHelper::addHcalHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  for(size_t i=0;i<hcalHitVec_.size();i++) hcalHitVec_[i].setNrgy(-999.);
  
  const HBHERecHitCollection* hcalHits = heepEvent.handles().hbheRecHits.isValid() ? heepEvent.hbheHits() : NULL;
  
  if(hcalHits!=NULL){
    for(HBHERecHitCollection::const_iterator hitIt = hcalHits->begin();
	hitIt!=hcalHits->end();++hitIt){
      hcalHitVec_[hcalHitHash_(hitIt->detid())].setNrgy(hitIt->energy());
    }
  }//end of null check on hcal hits

  shEvent.addHcalHits(hcalHitVec_);
}

void SHEventHelper::addCaloTowers(const heep::Event& heepEvent, SHEvent& shEvent)const
{

  const CaloTowerCollection* caloTowers = heepEvent.handles().caloTowers.isValid() ? &(*heepEvent.handles().caloTowers) : NULL;
  if(caloTowers!=NULL){
    for(CaloTowerCollection::const_iterator towerIt=caloTowers->begin();
	towerIt!=caloTowers->end();++towerIt){
      if(towerIt->id().ietaAbs()<=29){
	SHCaloTower caloTower(towerIt->id(),towerIt->emEnergy(),
			      towerIt->hadEnergy()-towerIt->hadEnergyHeOuterLayer(),
			      towerIt->hadEnergyHeOuterLayer(),
			      towerIt->eta(),towerIt->phi());
	shEvent.addCaloTower(caloTower);
      }
    }
  }

}

void SHEventHelper::addTrigInfo(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().trigEvent.isValid()){
    const trigger::TriggerEvent& trigEvt = heepEvent.triggerEvent();
    
    const edm::TriggerResults& trigResults = *heepEvent.handles().trigResults;
    const edm::TriggerNames& trigNames = heepEvent.event().triggerNames(trigResults);  
    addTrigInfo(trigEvt,trigResults,trigNames,shEvent,&heepEvent);
  }
}

void SHEventHelper::addTrigInfo(const trigger::TriggerEvent& trigEvt,
				const edm::TriggerResults& trigResults,
				const edm::TriggerNames& trigNames,SHEvent& shEvent,const heep::Event* heepEvent)const //heepEvent may be null...
{
 
  //  const trigger::TriggerObjectCollection& trigObjs = heepEvent.trigObjColl();
  for(size_t filterNr=0;filterNr<trigEvt.sizeFilters();filterNr++){
    SHTrigInfo trigInfo;
    trigInfo.setTrigId(-1);
    trigInfo.setTrigName(trigEvt.filterTag(filterNr).label());
    const trigger::Keys& trigKeys = trigEvt.filterKeys(filterNr);  //trigger::Keys is actually a vector<uint16_t> holding the position of trigger objects in the trigger collection passing the filter
    const trigger::TriggerObjectCollection & trigObjColl(trigEvt.getObjects());
    for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt){
      const trigger::TriggerObject& obj = trigObjColl[*keyIt];
      //  std::cout <<" filter "<<trigEvt.filterTag(filterNr).label()<<" obj id "<<obj.id()<<std::endl;
      TLorentzVector p4;
      //note I call this function as its probably the fastest way to get info out of  TriggerObject in 22X (look at how it calculates et, its impressive, it might be possible to do it slower but I doubt it)
      // if(trigEvt.filterTag(filterNr).process()=="HLT" && trigEvt.filterTag(filterNr).label()=="hltSingleMu15L3Filtered15") std::cout <<"filter: "<<trigEvt.filterTag(filterNr).label()<<" prod "<<trigEvt.filterTag(filterNr).process()<<" obj.pt "<<obj.pt()<<std::endl;
      p4.SetPtEtaPhiM(obj.pt(),obj.eta(),obj.phi(),obj.mass());
      trigInfo.addObj(p4);
    }
    if(!trigKeys.empty()) shEvent.addTrigInfo(trigInfo); //only adding triggers which actually have objects passing
  } 
  
  for(size_t pathNr=0;pathNr<trigResults.size();pathNr++){
    SHTrigInfo trigInfo;
    trigInfo.setTrigId(1);
    if(pathNr<trigNames.size()){
      const std::string& pathName = trigNames.triggerName(pathNr);
      trigInfo.setTrigName(pathName);
      trigInfo.setPreScale(heepEvent ? heepEvent->hltPreScale(pathName) : -1);
      trigInfo.setPass(trigResults.accept(pathNr));
    }else{
      trigInfo.setTrigName("NullName");
      trigInfo.setPass(false);
      trigInfo.setPreScale(-1);
    } 
   
    shEvent.addTrigInfo(trigInfo);
  }


}


void SHEventHelper::addTrigDebugInfo(const heep::Event& heepEvent,SHEvent& shEvent,const trigger::TriggerEventWithRefs& trigEvt,const std::vector<std::string>& filterNames,const std::string& hltTag)const
{
  //const edm::TriggerResults& trigResults = *heepEvent.handles().trigResults;
  // const edm::TriggerNames& trigNames = heepEvent.event().triggerNames(trigResults);
  
  
//   for(size_t pathNr=0;pathNr<trigResults.size();pathNr++){
//     SHTrigInfo trigInfo;
//     trigInfo.setTrigId(1);
//     if(pathNr<trigNames.size()){
//        trigInfo.setTrigName(trigNames.triggerName(pathNr));
//     }else trigInfo.setTrigName("NullName");
//     trigInfo.setPass(trigResults.accept(pathNr)); 
//     shEvent.addTrigInfo(trigInfo);
//   }
  
 
  for(size_t filterNr=0;filterNr<filterNames.size();filterNr++){ 
    SHTrigInfo trigInfo;
    trigInfo.setTrigId(2);
    trigInfo.setTrigName(filterNames[filterNr]);
    
    std::vector<reco::RecoEcalCandidateRef> ecalTrigObjs;
    std::vector<reco::ElectronRef> eleTrigObjs;
    std::vector<l1extra::L1EmParticleRef> l1IsoTrigObjs; 
    std::vector<l1extra::L1EmParticleRef> l1NonIsoTrigObjs;
    int filterNrInEvt = trigEvt.filterIndex(edm::InputTag(filterNames[filterNr],"",hltTag).encode());
    if(filterNrInEvt<trigEvt.size()){
      trigEvt.getObjects(filterNrInEvt,trigger::TriggerCluster,ecalTrigObjs);
      trigEvt.getObjects(filterNrInEvt,trigger::TriggerElectron,eleTrigObjs);
      trigEvt.getObjects(filterNrInEvt,trigger::TriggerL1IsoEG,l1IsoTrigObjs) ;
      trigEvt.getObjects(filterNrInEvt,trigger::TriggerL1NoIsoEG,l1NonIsoTrigObjs);
      //  std::cout <<"filter "<<filterNames[filterNr]<<" nr pass "<<ecalTrigObjs.size()<<" "<<l1IsoTrigObjs.size()<<" "<<l1NonIsoTrigObjs.size()<<std::endl;
      for(size_t candNr=0;candNr<ecalTrigObjs.size();candNr++){
	TLorentzVector p4;
	reco::RecoEcalCandidateRef& obj = ecalTrigObjs[candNr];
	
	p4.SetPtEtaPhiM(obj->pt(),obj->eta(),obj->phi(),obj->mass());
	trigInfo.addObj(p4);

      }
      for(size_t candNr=0;candNr<eleTrigObjs.size();candNr++){
	TLorentzVector p4;
	reco::ElectronRef& obj = eleTrigObjs[candNr];
	
	p4.SetPtEtaPhiM(obj->pt(),obj->eta(),obj->phi(),obj->mass());
	trigInfo.addObj(p4);

      }
      for(size_t candNr=0;candNr<l1IsoTrigObjs.size();candNr++){
	TLorentzVector p4;
	l1extra::L1EmParticleRef& obj = l1IsoTrigObjs[candNr];
	p4.SetPtEtaPhiM(obj->pt(),obj->eta(),obj->phi(),obj->mass());
	trigInfo.addObj(p4);

      }
      for(size_t candNr=0;candNr<l1NonIsoTrigObjs.size();candNr++){
	TLorentzVector p4;
	l1extra::L1EmParticleRef& obj = l1NonIsoTrigObjs[candNr];
	p4.SetPtEtaPhiM(obj->pt(),obj->eta(),obj->phi(),obj->mass());
	trigInfo.addObj(p4);

      }

    }//end filter present check
    //   std::cout <<"trig info "<<trigInfo.nrPass()<<std::endl;

    if(trigInfo.nrPass()>0) trigInfo.setPass(true);

    shEvent.addTrigInfo(trigInfo);
  }//end filter loop
 
}


// void SHEventHelper::addL1Info(const heep::Event& heepEvent,SHEvent& shEvent)const
// {
 
//   const std::vector<bool>& l1Word = heepEvent.l1Decision(); 
//   TBits myL1Bits(l1Word.size()); 
//   for(size_t bitNr=0;bitNr<l1Word.size();bitNr++){
//     if(l1Word[bitNr]) myL1Bits.SetBitNumber(bitNr);
//   }
//   shEvent.setL1Bits(myL1Bits);
//   for(size_t candNr=0;candNr<heepEvent.l1EmNonIso().size();candNr++){
//     const l1extra::L1EmParticle& cand = heepEvent.l1EmNonIso()[candNr];
//     TLorentzVector p4;
//     p4.SetPxPyPzE(cand.px(),cand.py(),cand.pz(),cand.energy());
//     shEvent.addL1Cand(p4,cand.type());
//   }
//   for(size_t candNr=0;candNr<heepEvent.l1EmIso().size();candNr++){
//     const l1extra::L1EmParticle& cand = heepEvent.l1EmIso()[candNr];
//     TLorentzVector p4;
//     p4.SetPxPyPzE(cand.px(),cand.py(),cand.pz(),cand.energy());
//     shEvent.addL1Cand(p4,cand.type());
//   }
// }
  

void SHEventHelper::addJets(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().jet.isValid()){
    for(size_t jetNr=0;jetNr<heepEvent.jets().size();jetNr++){
      if(heepEvent.jets()[jetNr].et()>15) shEvent.addJet(heepEvent.jets()[jetNr]);
    }
  }
}

void SHEventHelper::addIsolTrks(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  TVector3 trkP3,trkVtxPos;
  const float minPtCut=-1;

  if(heepEvent.handles().ctfTrack.isValid()){
    const std::vector<reco::Track>& tracks = heepEvent.ctfTracks();
    
    for(size_t trkNr=0;trkNr<tracks.size();trkNr++){
      const reco::Track& trk = tracks[trkNr];
      trkP3.SetXYZ(trk.px(),trk.py(),trk.pz());
      trkVtxPos.SetXYZ(trk.vx(),trk.vy(),trk.vz());
      int vertexNr=-1;
      if(heepEvent.handles().vertices.isValid()) vertexNr = getVertexNr(trk,*heepEvent.handles().vertices);
      //std::cout <<"trk chi2 "<<trk.chi2()<<" trk err "<<trk.ptError()<<std::endl;
      if(trkP3.Pt()>minPtCut) shEvent.addIsolTrk(trkP3,trkVtxPos,trk.charge()>0,vertexNr,trk.chi2(),trk.ndof());
    }
  } 
}

void SHEventHelper::addMet(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  //why yes I might be throwing away the patty goodness, whoops
  
  SHMet shMet;
  SHMet shPFMet;
  if(heepEvent.handles().met.isValid() && !heepEvent.mets().empty()){
    const pat::MET& met = heepEvent.mets()[0];
    if(met.isPFMET()){
      shPFMet.setMet(met.et()*cos(met.phi()),met.et()*sin(met.phi()));
      shPFMet.setSumEmEt(0,0,0);
      shPFMet.setSumHadEt(0,0,0,0);
    }else{
      shMet.setMet(met.mEtCorr()[0].mex,met.mEtCorr()[0].mey);
      shMet.setSumEmEt(met.emEtInEB(),met.emEtInEE(),met.emEtInHF());
      shMet.setSumHadEt(met.hadEtInHB(),met.hadEtInHE(),met.hadEtInHO(),met.hadEtInHF());
    }
  }
  shEvent.setMet(shMet); 
  shEvent.setPFMet(shPFMet);
 
//   edm::Handle<edm::View<reco::PFMET> > pfMETHandle;
//   heepEvent.event().getByLabel("pfMet",pfMETHandle);
//   if(pfMETHandle.isValid()){
//     const reco::PFMET& pfMET = pfMETHandle->front();
//     shPFMet.setMet(pfMET.et()*cos(pfMET.phi()),pfMET.et()*sin(pfMET.phi()));
//     shPFMet.setSumEmEt(0,0,0);
//     shPFMet.setSumHadEt(0,0,0,0);
//   }
 // std::cout <<"pf met "<<pfMET.et()<<" sh met "<<shMet.mEt()<<" pf phi "<<pfMET.phi()<<" sh phi "<<shMet.phi()<<std::endl;
 

}  

void SHEventHelper::addMCParticles(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(!heepEvent.hasGenParticles()) return;
  const std::vector<reco::GenParticle>& particles = heepEvent.genParticles();
  int nrPartsToStore = nrGenPartToStore_;
  if(nrPartsToStore==-1) nrPartsToStore = particles.size();

  //okay its really just easier if I make a vector of pointers and call it a day
  //this probably could be more efficient 
  std::vector<const reco::Candidate*> candPointers(particles.size(),0x0);
  for(size_t partNr=0;partNr<particles.size();partNr++){
    candPointers[partNr]=&particles[partNr];
  }
  
  for(size_t partIndx=0;partIndx<candPointers.size() && partIndx<static_cast<unsigned>(nrPartsToStore);partIndx++){
    
    //first get all the easy quantities
    const reco::Candidate* genPart = candPointers[partIndx];
    int index = partIndx;
    int pid = genPart->pdgId();
    int status = genPart->status(); //note this is not accessible in CMSSW_1_3_5
    //int status = -1;
    int nrMo = genPart->numberOfMothers();
    int nrDa = genPart->numberOfDaughters();
    TLorentzVector p4(genPart->px(),genPart->py(),genPart->pz(),genPart->energy());
    TVector3 pos(genPart->vx(),genPart->vy(),genPart->vz());
    
    
    //now sort out the mother and daughter links
    int jmo1 = -1;
    int jmo2 = -1;
    int jda1 = -1;
    int jda2 = -1;
    std::vector<const reco::Candidate*>::const_iterator vecIt;
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->mother(0));
    if(vecIt!=candPointers.end()) jmo1 = vecIt - candPointers.begin();
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->mother(nrMo-1));
    if(vecIt!=candPointers.end()) jmo2 = vecIt - candPointers.begin();
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->daughter(0));
    if(vecIt!=candPointers.end()) jda1 = vecIt - candPointers.begin();
    vecIt = std::find(candPointers.begin(),candPointers.end(),genPart->daughter(nrDa-1));
    
    if(vecIt!=candPointers.end()) jda2 = vecIt - candPointers.begin();

    //yay, we have everything we need
    shEvent.addMCParticle(index,status,pid,jmo1,jmo2,nrMo,jda1,jda2,nrDa,p4,pos);
    
  }//end loop over all mc particles

}

int SHEventHelper::ecalHitHash_(const DetId detId)const
{
  if(detId.det()==DetId::Ecal){
    if(detId.subdetId()==EcalBarrel){
      EBDetId ebDetId(detId);
      return ebDetId.hashedIndex();
    }else if(detId.subdetId()==EcalEndcap){
      EEDetId eeDetId(detId);
      return eeDetId.hashedIndex()+EBDetId::MAX_HASH+1;
    }
  } 
  return -1; //not in ecal barrel or endcap  
}


int SHEventHelper::hcalHitHash_(const DetId detId)const
{
  return DetIdTools::calHashHcal(detId.rawId());
  //HcalDetId hcalDetId(detId);
  //return DetIhashed_index();
}


void SHEventHelper::initEcalHitVec_()const
{
  //first make sure the vector is the approprate size and has all null entries
  SHCaloHit nullHit(0,-999.);
  std::vector<SHCaloHit> tempVector(EBDetId::MAX_HASH+1+(EEDetId::kSizeForDenseIndexing),nullHit);
  ecalHitVec_.swap(tempVector);

  
  //now init barrel, place det ids at the correct positions
  for(int barrelIEta=-85;barrelIEta<=85;barrelIEta++){
    if(barrelIEta==0) continue;
    for(int barrelIPhi=1;barrelIPhi<=360;barrelIPhi++){
      EBDetId detId(barrelIEta,barrelIPhi);
      ecalHitVec_[detId.hashedIndex()].setDetId(detId.rawId());
    }
  }

  //now init endcap
  for(int xNr=1;xNr<=100;xNr++){
    for(int yNr=1;yNr<=100;yNr++){
      for(int zSide=-1;zSide<=1;zSide+=2){
	if(EEDetId::validDetId(xNr,yNr,zSide)){
	  EEDetId detId(xNr,yNr,zSide);
	  ecalHitVec_[detId.hashedIndex()+EBDetId::MAX_HASH+1].setDetId(detId.rawId());
	}//end valid det id check
      }//z side loop
    }//end phi loop
  }//end eta loop

  //std::cout <<"checking indexes"<<std::endl;
  //for(size_t i=0;i<ecalHitVec_.size();i++){
    //if(ecalHitVec_[i].detId()==0) std::cout <<"ecal index "<<i<< " is "<<ecalHitVec_[i].detId()<<std::endl;
    // }

}


void SHEventHelper::initHcalHitVec_()const
{
  SHCaloHit nullHit(0,-999.);
  std::vector<SHCaloHit> tempVector(kNrHcalBarrelCrys_+kNrHcalEndcapCrys_,nullHit);
  hcalHitVec_.swap(tempVector);

  //fill barrel and endcap seperately
  //first barrel
  for(int iEtaAbs=1;iEtaAbs<=16;iEtaAbs++){
    for(int side=0;side<=1;side++){
       int iEta = iEtaAbs*(2*side-1);
       for(int iPhi=1;iPhi<=72;iPhi++){
	 int maxDepth = 1;
	 if(iEtaAbs>=15) maxDepth=2;
	 for(int depth=1;depth<=maxDepth;depth++){
	   if(HackedFuncs::validHcalDetId(HcalBarrel,iEta,iPhi,depth)){
	     HcalDetId detId(HcalBarrel,iEta,iPhi,depth);
	     int indx = DetIdTools::calHashHcal(detId.rawId());
	   
	    //std::cout <<"iEta "<<iEta<<" iPhi "<<iPhi<<" depth "<<depth<< " pos phi "<<pos.Phi()<<" eta "<<pos.Eta()<<" index "<<indx;//<<std::endl; 
	     

	    hcalHitVec_[indx].setDetId(detId.rawId());
	   
	   }//end vaild det id
	 }//end depth loop 
       }//end iphi loop
    }//end side loop
  }//end barrel ieta loop

  for(int iEtaAbs=16;iEtaAbs<=29;iEtaAbs++){
    for(int side=0;side<=1;side++){
      int iEta = iEtaAbs*(2*side-1);
      for(int iPhi=1;iPhi<=72;iPhi++){
	for(int depth=1;depth<=3;depth++){
	  if(HackedFuncs::validHcalDetId(HcalEndcap,iEta,iPhi,depth)){
	    HcalDetId detId(HcalEndcap,iEta,iPhi,depth);
	    int indx = DetIdTools::calHashHcal(detId.rawId());
	   
	    // std::cout <<"iEta "<<iEta<<" iPhi "<<iPhi<<" depth "<<depth<< " pos phi "<<pos.Phi()<<" eta "<<pos.Eta()<<" index "<<indx;//<<std::endl; 
	  
	    hcalHitVec_[indx].setDetId(detId.rawId());
	    
	    // std::cout <<" detId "<<hcalHitVec_[indx].detId()<<std::endl;
	  }
	}//end depth loop
      }//end iphi loop
    }//end side loop
  }//end iEtaAbs loop
  std::cout <<"checking indexes"<<std::endl;
  for(size_t i=0;i<hcalHitVec_.size();i++){
    if(hcalHitVec_[i].detId()==0) std::cout <<"hcal index "<<i<< " is "<<hcalHitVec_[i].detId()<<std::endl;
  }
}

//because they dont have an accessor :(
uint32_t SHEventHelper::getEcalFlagBits_(const EcalRecHit& hit)
{
  uint32_t bits=0x0;
  for(int bitNr=0;bitNr<32;bitNr++){
    bits |= hit.checkFlag(bitNr)<<bitNr;
  }
  return bits;

}

int SHEventHelper::getVertexNr(const reco::TrackBase& track,const std::vector<reco::Vertex>& vertices)
{ //code inspired by Florian's PFNoPU class
  int returnVal = -1;
 
  float bestweight=0;
  
  for(size_t vertexNr=0;vertexNr<vertices.size();vertexNr++){
    const reco::Vertex& vtx = vertices[vertexNr];
    
    // loop on tracks in vertices
    for(reco::Vertex::trackRef_iterator trkIt=vtx.tracks_begin();trkIt!=vtx.tracks_end(); ++trkIt) {

      const reco::TrackBaseRef& vertexTrkRef = *trkIt;

      // one of the tracks in the vertex is the same as 
      // the track considered in the function
      if(vertexTrkRef.get() ==  &track) {
        float weight = vtx.trackWeight(vertexTrkRef);
        //select the vertex for which the track has the highest weight
        if (weight > bestweight){
          bestweight=weight;
          returnVal = vertexNr;
        }
      }
    }//end loop over vertex tracks
    
  }//end loop over vertices
 
  if(returnVal<0) returnVal = SHEventHelper::getVertexNrClosestZ(track,vertices);
  return returnVal;
}

int SHEventHelper::getVertexNrClosestZ(const reco::TrackBase& track,const std::vector<reco::Vertex>& vertices)
{
  
  double dzMin = 999.;
  int bestVertexNr = -1;
  for(size_t vertexNr=0;vertexNr<vertices.size();vertexNr++){
    double dz = fabs(track.vz() - vertices[vertexNr].z());
    if(dz<dzMin) {
      dzMin = dz;
      bestVertexNr=vertexNr;
    }
  }
  return bestVertexNr;
}
//dummy function for releases without this function
//void fixClusterShape(const reco::CaloCluster& seedCluster,const heep::Event& heepEvent,SHElectron& ele){}

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgoRcd.h"
#include "CommonTools/Utils/interface/StringToEnumValue.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
void fixClusterShape(const reco::CaloCluster& seedCluster,const heep::Event& heepEvent,SHElectron& ele)
{
  
  const EcalRecHitCollection* ebHits =heepEvent.handles().ebRecHits.isValid() ? heepEvent.ebHitsFull() : 
    heepEvent.handles().ebReducedRecHits.isValid() ? &(*heepEvent.handles().ebReducedRecHits) : NULL;
  const EcalRecHitCollection* eeHits =heepEvent.handles().eeRecHits.isValid() ? heepEvent.eeHitsFull() : 
    heepEvent.handles().eeReducedRecHits.isValid() ? &(*heepEvent.handles().eeReducedRecHits) : NULL;
  
  float sigmaEtaEta=0;
  float sigmaIEtaIEta=0;
  float e2x5Max=0;
  float e5x5=0;
  float e1x5=0;
  
  if(ebHits && eeHits){
    
    DetId seedXtalId = seedCluster.hitsAndFractions()[0].first ;
    int detector = seedXtalId.subdetId() ;
    const EcalRecHitCollection* recHits = detector==EcalBarrel ? ebHits : eeHits; 
    edm::ESHandle<CaloTopology> topology;
    edm::ESHandle<CaloGeometry> geom;
//     edm::ESHandle<EcalSeverityLevelAlgo> severityLevelAlgo;
//     heepEvent.eventSetup().get<CaloTopologyRecord>().get(topology);
//     heepEvent.eventSetup().get<EcalSeverityLevelAlgoRcd>().get(severityLevelAlgo);
//     heepEvent.eventSetup().get<CaloGeometryRecord>().get(geom);
//     std::vector<std::string> recHitFlagsToBeExcludedBarrelStr = {
//       "kFaultyHardware",
//       "kPoorCalib",
//       "kTowerRecovered",
//       "kDead"
//     };
//     std::vector<std::string> recHitSeverityToBeExcludedStr = {
//       "kWeird",
//       "kBad",
//       "kTime"
//     };
    
//     std::vector<std::string> recHitFlagsToBeExcludedEndcapStr = {
//       "kFaultyHardware",
//       "kPoorCalib",
//       "kSaturated",
//       "kLeadingEdgeRecovered",
//       "kNeighboursRecovered",
//       "kTowerRecovered",
//       "kDead",
//       "kWeird"
//     };
//     std::vector<int> recHitSeverityToBeExcluded = StringToEnumValue<EcalSeverityLevel::SeverityLevel>(recHitSeverityToBeExcludedStr);
//     std::vector<int> recHitFlagsToBeExcludedBarrel= StringToEnumValue<EcalRecHit::Flags>(recHitFlagsToBeExcludedBarrelStr);  
//     std::vector<int> recHitFlagsToBeExcludedEndcap= StringToEnumValue<EcalRecHit::Flags>(recHitFlagsToBeExcludedEndcapStr);
//     std::vector<int>& recHitFlagsToBeExcluded = detector==EcalBarrel ? recHitFlagsToBeExcludedBarrel : recHitFlagsToBeExcludedEndcap;
     
    // std::vector<float> covariances = noZS::EcalClusterTools::covariances(seedCluster,recHits,topology.product(),geom.product(),recHitFlagsToBeExcluded,recHitSeverityToBeExcluded,severityLevelAlgo.product()) ;
    //   sigmaEtaEta = sqrt(covariances[0]) ;
    // std::vector<float> localCovariances = noZS::EcalClusterTools::localCovariances(seedCluster,recHits,topology.product(),recHitFlagsToBeExcluded,recHitSeverityToBeExcluded,severityLevelAlgo.product()) ;
      std::vector<float> covariances = noZS::EcalClusterTools::covariances(seedCluster,recHits,topology.product(),geom.product()) ;
    sigmaEtaEta = sqrt(covariances[0]) ;
    std::vector<float> localCovariances = noZS::EcalClusterTools::localCovariances(seedCluster,recHits,topology.product()) ;
  
    sigmaIEtaIEta = sqrt(localCovariances[0]) ;
    //  e1x5 = noZS::EcalClusterTools::e1x5(seedCluster,recHits,topology.product(),recHitFlagsToBeExcluded,recHitSeverityToBeExcluded,severityLevelAlgo.product());
    //    e2x5Max = noZS::EcalClusterTools::e2x5Max(seedCluster,recHits,topology.product(),recHitFlagsToBeExcluded,recHitSeverityToBeExcluded,severityLevelAlgo.product());
    //  e5x5 = noZS::EcalClusterTools::e5x5(seedCluster,recHits,topology.product(),recHitFlagsToBeExcluded,recHitSeverityToBeExcluded,severityLevelAlgo.product());
    e1x5 = noZS::EcalClusterTools::e1x5(seedCluster,recHits,topology.product());
    e2x5Max = noZS::EcalClusterTools::e2x5Max(seedCluster,recHits,topology.product());
    e5x5 = noZS::EcalClusterTools::e5x5(seedCluster,recHits,topology.product());
  }

  ele.setShowerShape(sigmaEtaEta,sigmaIEtaIEta,e1x5,e2x5Max,e5x5);
  
}


