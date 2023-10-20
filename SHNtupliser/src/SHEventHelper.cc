#include "SHarper/SHNtupliser/interface/SHEventHelper.h"
#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/HEEPAnalyzer/interface/HEEPEvent.h"
#include "SHarper/SHNtupliser/interface/SHEleCMSSWStructs.hh"
#include "SHarper/SHNtupliser/interface/GeomFuncs.hh"
#include "SHarper/SHNtupliser/interface/SHTrigSumMaker.h"
#include "SHarper/SHNtupliser/interface/PFFuncs.h"
#include "SHarper/SHNtupliser/interface/GenFuncs.h"
#include "SHarper/SHNtupliser/interface/LogErr.hh"

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
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "DataFormats/METReco/interface/PFMET.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Scalers/interface/DcsStatus.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"

SHEventHelper::SHEventHelper():
  isMC_(false),
  minEtToPromoteSC_(20),
  fillFromGsfEle_(true),
  applyMuonId_(true)
{
  initEcalHitVec_();
  initHcalHitVec_();
}

void SHEventHelper::setup(const edm::ParameterSet& conf,edm::ConsumesCollector && cc)
{ 
  minEtToPromoteSC_ = conf.getParameter<double>("minEtToPromoteSC");
  eventWeight_ = conf.getParameter<double>("sampleWeight");
  datasetCode_ = conf.getParameter<int>("datasetCode");    
  applyMuonId_ = conf.getParameter<bool>("applyMuonId");
  fillFromGsfEle_ = conf.getParameter<bool>("fillFromGsfEle");
  trigSumMaker_.setup(cc);

  isMC_=datasetCode_!=0;
  branches_.setup(conf);
  if(branches_.addHLTDebug) cc.consumesMany<reco::RecoEcalCandidateIsolationMap>();
  
}



void SHEventHelper::makeSHEvent(const heep::Event & heepEvent, SHEvent& shEvent)const

{   
 
  shEvent.clear(); //reseting the event 

  addEventPara(heepEvent,shEvent); //this must be filled before (ele + mu need beam spot info)
 
  if(branches_.addSuperClus) addSuperClusters(heepEvent,shEvent);
  if(branches_.addEles) addElectrons(heepEvent,shEvent);
  if(branches_.addPreShowerClusters) addPreShowerClusters(heepEvent,shEvent);
  if(branches_.addMuons) addMuons(heepEvent,shEvent); 
  if(branches_.addTrigSum) addTrigInfo(heepEvent,shEvent);
  if(branches_.addJets) addJets(heepEvent,shEvent);
  if(branches_.addMet) addMet(heepEvent,shEvent);
  if(branches_.addIsolTrks) addIsolTrks(heepEvent,shEvent);
  if(branches_.addGenInfo) addGenInfo(heepEvent,shEvent);
  if(branches_.addPUInfo) addPUInfo(heepEvent,shEvent);
  if(branches_.addPFCands) addPFCands(heepEvent,shEvent);
  if(branches_.addPFClusters) addPFClusters(heepEvent,shEvent);
  if(branches_.addCaloHits) addCaloHits(heepEvent,shEvent);
  if(branches_.addCaloHits) addEcalWeightsHits(heepEvent,shEvent);
  if(branches_.addCaloTowers) addCaloTowers(heepEvent,shEvent);
  if(branches_.addGainSwitchInfo) addGSFixInfo(heepEvent,shEvent);
  if(branches_.addJetMETExtra) addJetMETExtra(heepEvent,shEvent);

}

void SHEventHelper::addEventPara(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  shEvent.setRunnr(heepEvent.runnr());
  shEvent.setEventnr(heepEvent.eventnr());
  shEvent.setIsMC(isMC_);
  shEvent.setDatasetCode(datasetCode_);
  shEvent.setWeight(eventWeight_);
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

    
}

//this now handles the sc promotion via photons
//photons have just a H/E<0.5 cut which is fine for our needs
void SHEventHelper::addElectrons(const heep::Event& heepEvent, SHEvent& shEvent)const
{  
  auto& eleHandle = heepEvent.handles().gsfEle;
  if(!eleHandle.isValid()) return; //protection when the colleciton doesnt exist

  const auto& photons = heepEvent.recoPhos();
  //  std::cout <<"start event "<<std::endl;
  std::vector<int> usedSCSeedIds;
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    edm::Ptr<reco::GsfElectron> elePtr(eleHandle,eleNr);
    if(elePtr->superCluster().isNonnull()) usedSCSeedIds.push_back(elePtr->superCluster()->seed()->seed().rawId());  
    //  std::cout <<"adding electron "<<eleNr<<" / "<<eleHandle->size()<<" ele "<<elePtr->energy()<<" "<<elePtr->eta()<<" "<<elePtr->phi()<<" hadem "<<elePtr->hcalOverEcal()<<" ecal driven "<<elePtr->ecalDriven()<<" sc energy " <<elePtr->superCluster()->energy()<<std::endl;

    addElectron(heepEvent,shEvent,elePtr);
  }
  
  for(size_t phoNr=0;phoNr<photons.size();phoNr++){
    //const reco::Photon& pho = photons[phoNr];
    //   std::cout <<"photon "<<phoNr<<" / "<<photons.size()<<" pho "<<pho.energy()<<" "<<pho.eta()<<" "<<pho.phi()<<" sc energy " <<pho.superCluster()->energy()<<std::endl;

    const reco::SuperCluster& sc = *photons[phoNr].superCluster();
    if(std::find(usedSCSeedIds.begin(),usedSCSeedIds.end(),sc.seed()->seed().rawId())==usedSCSeedIds.end()){
      //did not add if by electrons, so add it now
      if(sc.energy()*sin(sc.position().theta())>minEtToPromoteSC_ && minEtToPromoteSC_<10000){
	//	std::cout <<"adding photon as electron "<<std::endl;
	addElectron(heepEvent,shEvent,photons[phoNr]);
      }
    }
  }
}

void SHEventHelper::addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const edm::Ptr<reco::GsfElectron>& gsfEle)const
{
  shEvent.addElectron(*gsfEle);
  SHElectron* shEle = shEvent.getElectron(shEvent.nrElectrons()-1); 
  fixTrkIsols_(heepEvent,gsfEle,*shEle);
  setNrSatCrysIn5x5_(heepEvent,*shEle);
  setCutCode_(heepEvent,gsfEle,*shEle);
  if(branches_.addEleUserData) addUserData_(&*gsfEle,*shEle);
  //shEle- >setPassMVAPreSel(shEle->isolMVA()>=-0.1);
  //shEle->setPassPFlowPreSel(gsfEle.mvaOutput().status==3); 
  fillRecHitClusterMap(*gsfEle->superCluster(),shEvent);
  const reco::Photon* phoMatch = getPhoMatch_(gsfEle,heepEvent);
  const reco::GsfElectron* oldEleMatch = getOldEleMatch_(gsfEle,heepEvent);
  const reco::Photon* oldPhoMatch = getOldPhoMatch_(gsfEle,heepEvent);
  const float phoNrgy = phoMatch ? phoMatch->energy() : 0.;
  const float phoNrgyAlt = oldPhoMatch ? oldPhoMatch->energy() : 0.;  
  if(oldEleMatch) shEle->setNrgyExtra(oldEleMatch->ecalEnergy(),oldEleMatch->ecalEnergyError(),oldEleMatch->energy(),phoNrgy,phoNrgyAlt);//oldEleMatch->superCluster()->energy());
  else shEle->setNrgyExtra(0.,0.,0.,phoNrgy,phoNrgyAlt);
		    
  //MultiTrajectoryStateTransform trajStateTransform(heepEvent.handles().trackGeom.product(),heepEvent.handles().bField.product());
 
  // if(shEle->seedId()!=0 && false){
    
  //   const TVector3& seedPos = GeomFuncs::getCell(shEle->seedId()).pos();
  //   GlobalPoint posGP(seedPos.X(),seedPos.Y(),seedPos.Z());  
    
  //   //  std::cout <<" pt "<<gsfEle.gsfTrack()->pt()<<std::endl;
  //   TrajectoryStateOnSurface outTSOS = trajStateTransform.outerStateOnSurface(*gsfEle.gsfTrack());
  //   TrajectoryStateOnSurface innTSOS = trajStateTransform.innerStateOnSurface(*gsfEle.gsfTrack());
  //   TrajectoryStateOnSurface outToSeedTSOS = trajStateTransform.extrapolatedState(outTSOS,posGP);
  //   TrajectoryStateOnSurface innToSeedTSOS = trajStateTransform.extrapolatedState(innTSOS,posGP);
    
  //   TVector3 dummy; 
  //   dummy.SetPtEtaPhi(0.00001,-10,0);
  //   if(outToSeedTSOS.isValid()){
  //     TVector3 outToSeedPos(outToSeedTSOS.globalPosition().x(),outToSeedTSOS.globalPosition().y(),outToSeedTSOS.globalPosition().z()); 
  //     shEle->setPosTrackOutToSeed(outToSeedPos); 
  //   }else shEle->setPosTrackOutToSeed(dummy);
    
  //   if(innToSeedTSOS.isValid()){
  //     TVector3 innToSeedPos(innToSeedTSOS.globalPosition().x(),innToSeedTSOS.globalPosition().y(),innToSeedTSOS.globalPosition().z());
  //     shEle->setPosTrackInnToSeed(innToSeedPos);  
  //   }else shEle->setPosTrackInnToSeed(dummy);
   
  // }  
 
}


void SHEventHelper::addElectron(const heep::Event& heepEvent,SHEvent& shEvent,const reco::Photon& photon)const
{
  shEvent.addElectron(photon);  
  SHElectron* shEle = shEvent.getElectron(shEvent.nrElectrons()-1);   
  const reco::Photon* oldPhoMatch = getOldPhoMatch_(photon,heepEvent);
  const float phoNrgyAlt = oldPhoMatch ? oldPhoMatch->energy() : 0.;
  shEle->setNrgyExtra(0.,0.,0.,photon.energy(),phoNrgyAlt);
  setNrSatCrysIn5x5_(heepEvent,*shEle);
  addUserData_(&photon,*shEle);
  fillRecHitClusterMap(*photon.superCluster(),shEvent);
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
  const reco::Vertex& vertex = heepEvent.handles().vertices->front();
  return muon::isHighPtMuon(muon,vertex);

  // if(muon.isGlobalMuon() && 
  //    muon.globalTrack()->hitPattern().numberOfValidMuonHits()>0 &&
  //    muon.numberOfMatchedStations()>1 &&
  //    muon.globalTrack()->hitPattern().numberOfValidPixelHits()>0 && 
  //    muon.globalTrack()->hitPattern().trackerLayersWithMeasurement() > 5 ){
  //   // reco::TrackRef cktTrackRef = (muon::tevOptimized(muon, 200, 17., 40., 0.25)).first; //53X
  //   reco::TrackRef cktTrackRef = (muon::tevOptimized(muon)).first; //60X
  //   const reco::Track& cktTrack = *cktTrackRef;
  //   const reco::Vertex& vertex = heepEvent.handles().vertices->front();
  //   if(cktTrack.ptError()/cktTrack.pt()<0.3 && 
  //      fabs(cktTrack.dxy(vertex.position())) < 0.2 &&
  //      fabs(cktTrack.dz(vertex.position())) < 0.5) return true;
    
  // }
  //return false;
}

void SHEventHelper::addSuperClusters(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  if(heepEvent.handles().superClusEB.isValid()){
    const std::vector<reco::SuperCluster>& superClusEB = heepEvent.superClustersEB(); 
    for(size_t superClusNr=0;superClusNr<superClusEB.size();superClusNr++){
      if(isEcalBarrel_(superClusEB[superClusNr])){
	shEvent.addSuperCluster(superClusEB[superClusNr]);
      }
    }
  }
  if(heepEvent.handles().superClusEB.isValid()){
  //now endcap
    const std::vector<reco::SuperCluster>& superClusEE = heepEvent.superClustersEE(); 
    for(size_t superClusNr=0;superClusNr<superClusEE.size();superClusNr++){
      if(isEcalEndcap_(superClusEE[superClusNr])){
	shEvent.addSuperCluster(superClusEE[superClusNr]);
      }
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
  shEvent.getCaloHits().setADCToGeVEB(heepEvent.handles().ecalADCToGeV->getEBValue());
  shEvent.getCaloHits().setADCToGeVEE(heepEvent.handles().ecalADCToGeV->getEEValue());
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
  
  if(ebHits) fillEcalHitVec_(heepEvent,*ebHits,shEvent);
  if(eeHits) fillEcalHitVec_(heepEvent,*eeHits,shEvent);

  shEvent.addEcalHits(ecalHitVec_);

}


void SHEventHelper::addEcalWeightsHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{

  if(heepEvent.handles().ebWeightsRecHits.isValid()){
    for(auto& hit : *heepEvent.handles().ebWeightsRecHits){
      SHCaloHit shHit;
      setRecHit(shHit,hit,heepEvent);
      shEvent.getCaloHits().addExtraHit("ecalWeights",shHit);
    }
  }
  if(heepEvent.handles().eeWeightsRecHits.isValid()){
    for(auto& hit : *heepEvent.handles().eeWeightsRecHits){
      SHCaloHit shHit;
      setRecHit(shHit,hit,heepEvent);
      shEvent.getCaloHits().addExtraHit("ecalWeights",shHit);
    }
  }
}

    

void SHEventHelper::addHcalHits(const heep::Event& heepEvent, SHEvent& shEvent)const
{
  for(size_t i=0;i<hcalHitVec_.size();i++) hcalHitVec_[i].setNrgy(-999.);
  
  const HBHERecHitCollection* hcalHits = heepEvent.handles().hbheRecHits.isValid() ? heepEvent.hbheHits() : NULL;
  
  if(hcalHits!=NULL){
    for(HBHERecHitCollection::const_iterator hitIt = hcalHits->begin();
	hitIt!=hcalHits->end();++hitIt){
      if(!branches_.filterHcalHits || 
	 passCaloHitFilter_(hitIt->detid(),shEvent,kMaxDRHcalHits_) ){
	 
	hcalHitVec_[hcalHitHash_(hitIt->detid())].setNrgy(hitIt->energy());
      }
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
	if(!branches_.filterCaloTowers ||
	   isNearEle_(towerIt->eta(),towerIt->phi(),shEvent,kMaxDRCaloTowers_) ){
	  SHCaloTower caloTower(towerIt->id(),towerIt->emEnergy(),
				towerIt->hadEnergy()-towerIt->hadEnergyHeOuterLayer(),
				towerIt->hadEnergyHeOuterLayer(),
				towerIt->eta(),towerIt->phi());
	  shEvent.addCaloTower(caloTower);
	}
      }
    }
  }

}

void SHEventHelper::addGenInfo(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  GenFuncs::fillGenInfo(heepEvent,shEvent.getGenInfo(),branches_.addMCParts,branches_.addPDFWeights);
}

void SHEventHelper::addPFCands(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().vertices.isValid() && 
     heepEvent.handles().pfCandidate.isValid() &&
     heepEvent.handles().gsfEle.isValid() && 
     heepEvent.handles().gsfEleToPFCandMap.isValid()){
    reco::VertexRef mainVtx(heepEvent.handles().vertices,0); 
    
    PFFuncs::fillPFCands(&shEvent,kMaxDRPFCands_,shEvent.getPFCands(),heepEvent.handles().pfCandidate,
			 mainVtx,heepEvent.handles().vertices,
			   *(heepEvent.handles().gsfEleToPFCandMap.product()),
			 heepEvent.handles().gsfEle);
  }else if(heepEvent.handles().packedPFCand.isValid() &&
	   heepEvent.handles().gsfEle.isValid()){
    PFFuncs::fillPFCands(&shEvent,kMaxDRPFCands_,shEvent.getPFCands(),heepEvent.handles().packedPFCand,
			 heepEvent.handles().gsfEle);
    
	   

  }
}

void SHEventHelper::addPFClusters(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().pfClustersECAL.isValid() && heepEvent.handles().pfClustersHCAL.isValid() &&
     heepEvent.handles().superClusEB.isValid() && heepEvent.handles().superClusEE.isValid()){

    fillPFClustersECAL_(&shEvent,kMaxDRPFClusts_,shEvent.getPFClusters(),heepEvent.pfClustersECAL(),heepEvent.superClustersEB(),heepEvent.superClustersEE());
    fillPFClustersHCAL_(&shEvent,kMaxDRPFClusts_,shEvent.getPFClusters(),heepEvent.pfClustersHCAL());
  }
}

void SHEventHelper::addPUInfo(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().pileUpMCInfo.isValid()){
    for(auto& puInfo : *heepEvent.handles().pileUpMCInfo){
      shEvent.getPUSum().addPUInfo( puInfo.getBunchCrossing(),puInfo.getPU_NumInteractions(),puInfo.getTrueNumInteractions());
    }
  }
}

void SHEventHelper::addTrigInfo(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  trigSumMaker_.makeSHTrigSum(heepEvent,shEvent.getTrigSum());
  if(branches_.addHLTDebug) SHTrigSumMaker::associateEgHLTDebug(heepEvent,shEvent.getTrigSum());
  
}

void SHEventHelper::addJets(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().jet.isValid()){
    for(size_t jetNr=0;jetNr<heepEvent.jets().size();jetNr++){
      if(heepEvent.jets()[jetNr].pt()>30) shEvent.addJet(heepEvent.jets()[jetNr]);
    }
  }
}


void SHEventHelper::addGSFixInfo(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  SHGainSwitchInfo& gsInfo = shEvent.getGSInfo();
  gsInfo.clear();
  auto handles = heepEvent.handles();
  
  //first get general event 
  if(handles.gsFixDupECALClusters.isValid() && handles.gsFixHitsNotReplaced.isValid()){
    
    std::vector<int> detIdsNotFixed;
    for(const DetId& id : *handles.gsFixHitsNotReplaced){
      detIdsNotFixed.push_back(id.rawId());
    }
    bool hasEBGS=false;
    if(handles.ebReducedRecHits.isValid()){
      hasEBGS |= hasGSIn5x5_(handles.recoPho,*handles.ebReducedRecHits,*handles.caloTopology);
      hasEBGS |= hasGSIn5x5_(handles.gsfEle,*handles.ebReducedRecHits,*handles.caloTopology);
    }
    bool dupSC = *handles.gsFixDupECALClusters;
    gsInfo.setFixInfo(dupSC,hasEBGS,detIdsNotFixed);
  } 
  
  //add the orginal multi fit hits
  if(handles.gsFixOrgReducedEGEBHits.isValid()){
    auto gsHits = getGainSwitchedHits(*handles.gsFixOrgReducedEGEBHits);
    for(const EcalRecHit* hit : gsHits){
      SHCaloHit shHit;
      setRecHit(shHit,*hit,heepEvent);
      shEvent.getCaloHits().addExtraHit("ebGSMultiFit",shHit);
    }
  }

  //now mets
  if(handles.gsFixMETOrg.isValid() && !handles.gsFixMETOrg->empty()){
    auto& met = handles.gsFixMETOrg->front();
    gsInfo.setMETOrg(met.et(),met.phi());
  }
  if(handles.gsFixMETEGClean.isValid() && !handles.gsFixMETEGClean->empty()){
    auto& met = handles.gsFixMETEGClean->front();
    gsInfo.setMETJustEGFix(met.et(),met.phi());
  }
    
}

void SHEventHelper::addJetMETExtra(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  auto handles = heepEvent.handles();
  //  std::cout <<"adding jetMETExtra "<< handles.extraJets.size()<<std::endl;
  auto getCollName=[](const edm::Provenance* prov){
    if(prov){
      if(prov->productInstanceName().empty()) return prov->moduleLabel()+"__"+prov->processName();
      else return prov->moduleLabel()+"__"+prov->productInstanceName()+"__"+prov->processName();
    }else return std::string();
  };

  std::vector<std::vector<SHJet> > shJets;
  std::vector<std::string> shJetNames;
  for(const auto& jetColl: handles.extraJets){
    //std::cout <<"looping over jets"<<std::endl;
    if(jetColl.isValid()){
      //std::cout <<"filling jets "<<std::endl;
      shJets.push_back(std::vector<SHJet>());
      shJets.reserve(jetColl->size());
      for(const auto& jet : *jetColl){
	if(jet.pt()>15) shJets.back().emplace_back(SHJet(jet));
      }
      //	std::transform(jetColl->begin(),jetColl->end(),std::back_inserter(shJets.back()),
      //		     [](const auto& jet){return SHJet(jet);});
      
      shJetNames.push_back(getCollName(jetColl.provenance()));
    }
  }
  std::vector<SHMet> shMETs;
  std::vector<std::string> shMETNames;
  for(const auto& mets: handles.extraMETs){
    if(mets.isValid()){
      if(mets->size()!=1) throw cms::Exception("CorruptData","More than one (or zero) MET in the collection");
      const auto& met = (*mets)[0];
      SHMet shMET;
      shMET.setMet(met.et()*std::cos(met.phi()),
		   met.et()*std::sin(met.phi()));
      shMET.setSumEmEt(0,0,0);
      shMET.setSumHadEt(0,0,0,0);      
      shMETs.push_back(shMET);
      shMETNames.push_back(getCollName(mets.provenance()));
    }
  }
  auto& jetMETExtra = shEvent.getJetMETExtra();
  jetMETExtra.setJets(std::move(shJets),std::move(shJetNames));
  jetMETExtra.setMETs(std::move(shMETs),std::move(shMETNames));

}

  
std::vector<const EcalRecHit*> SHEventHelper::getGainSwitchedHits(const EcalRecHitCollection& hits)
{
  std::vector<const EcalRecHit*> gsHits;
  const std::vector<int> flags={EcalRecHit::kHasSwitchToGain6,EcalRecHit::kHasSwitchToGain1};
  for(auto& hit : hits){
    if(hit.checkFlags(flags)) gsHits.push_back(&hit);
  }
  return gsHits;
}

void SHEventHelper::addIsolTrks(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().ctfTrack.isValid()){ //we're in AOD, fill from tracks
    addIsolTrksFromTrks_(heepEvent,shEvent);
  }else if(heepEvent.handles().packedPFCand.isValid() &&
	   heepEvent.handles().lostTrack.isValid()){// &&
    //	   heepEvent.handles().muons.isValid() &&
    //	   heepEvent.handles().electrons.isValid()){ //we're in miniAOD, fill from packed cands
    addIsolTrksFromCands_(heepEvent,shEvent);
  }
  
}

void SHEventHelper::addIsolTrksFromTrks_(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  const float minPtCut=-1;

  if(heepEvent.handles().ctfTrack.isValid()){
    const std::vector<reco::Track>& tracks = heepEvent.ctfTracks();
   
    for(size_t trkNr=0;trkNr<tracks.size();trkNr++){
      const reco::Track& trk = tracks[trkNr];
      if(!branches_.filterIsolTrks || 
	 isNearEle_(trk.eta(),trk.phi(),shEvent,kMaxDRTrks_) ){
	int vertexNr=-1;
	if(heepEvent.handles().vertices.isValid()) vertexNr = getVertexNr(trk,*heepEvent.handles().vertices);
	//std::cout <<"trk chi2 "<<trk.chi2()<<" trk err "<<trk.ptError()<<std::endl;
	if(trk.pt()>minPtCut) shEvent.addIsolTrk(trk,vertexNr);
      }
    }
  } 
}


void SHEventHelper::addIsolTrksFromCands_(const heep::Event& heepEvent,SHEvent& shEvent)const
{
  if(heepEvent.handles().packedPFCand.isValid()){
    bool rejectEles = heepEvent.handles().lostEleTrack.isValid(); //if this valid, we'll add them in later
    addIsolTrksFromCands_(*heepEvent.handles().packedPFCand,heepEvent,shEvent,rejectEles);
  }
  if(heepEvent.handles().lostTrack.isValid()){
    addIsolTrksFromCands_(*heepEvent.handles().lostTrack,heepEvent,shEvent,false);
  }
  if(heepEvent.handles().lostEleTrack.isValid()){
    addIsolTrksFromCands_(*heepEvent.handles().lostEleTrack,heepEvent,shEvent,false);
  }

}

void SHEventHelper::addIsolTrksFromCands_(const std::vector<pat::PackedCandidate>& cands,
					  const heep::Event& heepEvent,SHEvent& shEvent,
					  bool rejectEles)const
{
  float minPtCut = 1.; 
  for(auto& cand : cands){
    const int absPdgId = std::abs(cand.pdgId());
    if(rejectEles && absPdgId==11) continue;
    //ensure its a charged object
    //in theory charge()!=0 should work
    if(cand.charge()!=0 && cand.hasTrackDetails()){
      if(!(absPdgId==11 || absPdgId==13 || absPdgId==15 || absPdgId==211) ) std::cout <<"pid "<<cand.pdgId()<<" charged but not usual ones"<<std::endl;
      const reco::Track& trk = cand.pseudoTrack();
      if(!branches_.filterIsolTrks || 
	 isNearEle_(trk.eta(),trk.phi(),shEvent,kMaxDRTrks_) ){
	SHIsolTrack isolTrk = createSHIsolTrack_(cand,heepEvent);
	int pseudoAlgo=0;
	if(absPdgId==11) pseudoAlgo=1;
	if(absPdgId==13) pseudoAlgo=2;
	if(absPdgId==15) pseudoAlgo=3;
	if(absPdgId==211) pseudoAlgo=4;
	
	isolTrk.setAlgosAndQual(SHIsolTrack::packAlgoIDInfo(pseudoAlgo,0,isolTrk.quality()));
	//	if(trk.pt()>minPtCut) shEvent.addIsolTrk(trk,vertexNr);
	if(trk.pt()>minPtCut) shEvent.addIsolTrk(isolTrk);
	
      }
    }
  }
}
SHIsolTrack SHEventHelper::createSHIsolTrack_(const pat::PackedCandidate& cand,
					    const heep::Event& heepEvent)const
{
  const reco::Track& trk = cand.pseudoTrack();
  
  int vertexNr=-1;
  if(heepEvent.handles().vertices.isValid()) vertexNr = getVertexNr(trk,*heepEvent.handles().vertices);

  // bool debug = std::abs(trk.eta()-1.32041)<0.001;
  // if(debug){
  //   std::cout <<"cand "<<cand.pt()<<" "<<cand.eta()<<" "<<cand.phi()<<" id "<<cand.pdgId()<<" ";
  //   std::cout <<"trk "<<trk.pt()<<" "<<trk.eta()<<" "<<trk.phi()<<" err "<<trk.ptError()<<std::endl;
  // }
    
  //not necessary with new lost tracks
  // if(std::abs(cand.pdgId())==11){
  //   const reco::GsfElectron* ele = matchTrkToEle_(trk,heepEvent);
  //   if(ele && ele->gsfTrack().isNonnull()){
  //     SHIsolTrack isoTrk(cand.pseudoTrack(),vertexNr);
  //     // if(debug) std::cout <<"   overrighting trk pt "<<ele->gsfTrack()->pt()<<std::endl;
  //     isoTrk.setPt(ele->gsfTrack()->pt());
      
  //     return isoTrk;
  //   }
  // }
  return SHIsolTrack(cand.pseudoTrack(),vertexNr);
  
}
const reco::GsfElectron* SHEventHelper::matchTrkToEle_(const reco::TrackBase& trk,
						       const heep::Event& heepEvent)const
{
  if(heepEvent.handles().gsfEle.isValid()){
    
    auto match=[](const reco::TrackBase& trk,const reco::GsfElectron& ele){
      return std::abs(trk.eta()-ele.gsfTrack()->eta())<0.001 &&
      std::abs(trk.phi()-ele.gsfTrack()->phi())<0.001;// && 
    };
    for(auto& ele : *heepEvent.handles().gsfEle){ 
      if(ele.gsfTrack().isNonnull() && match(trk,ele)) { 
	return &ele;
      }
    }
  }
  return nullptr;
}

void SHEventHelper::fillRecHitClusterMap(const reco::SuperCluster& superClus,SHEvent& shEvent)
{
  if(superClus.clusters().isAvailable()){
    for(auto clusIt=superClus.clustersBegin();clusIt!=superClus.clustersEnd();++clusIt){
      fillRecHitClusterMap_(**clusIt,shEvent);
    }
  }else{
    fillRecHitClusterMap_(*superClus.seed(),shEvent);
  } 
}

void SHEventHelper::fillRecHitClusterMap_(const reco::CaloCluster& clus,SHEvent& shEvent)
{
  std::vector<std::pair<int,float>> hitsAndFracs(clus.hitsAndFractions().size());
  std::transform(clus.hitsAndFractions().begin(),clus.hitsAndFractions().end(),
		 hitsAndFracs.begin(),
		     [](const std::pair<DetId,float>& val)
		 {return std::pair<int,float>(val.first.rawId(),val.second);});
  if(!shEvent.getRecHitClusMap().addCluster(clus.seed().rawId(),hitsAndFracs)){
    LogErr <<"bad fill "<<shEvent.runnr()<<" "<<shEvent.lumiSec()<<" "<<shEvent.eventnr()<<std::endl;
  }
  
}

void SHEventHelper::addMet(const heep::Event& heepEvent,SHEvent& shEvent)const
{ 
  SHMet shMet;
  SHMet shPFMet;
  if(heepEvent.handles().met.isValid() && !heepEvent.mets().empty()){
    
    if(heepEvent.mets().size()!=1) throw cms::Exception("CorruptData","More than one (or zero) MET in the collection");
    
    const pat::MET& met = heepEvent.mets().front();
    if(met.isPFMET()){
      shPFMet.setMet(met.et()*std::cos(met.phi()),
		     met.et()*std::sin(met.phi()));
      shPFMet.setSumEmEt(0,0,0);
      shPFMet.setSumHadEt(0,0,0,0);
      shMet.setMet(met.caloMETPt()*std::cos(met.caloMETPhi()),
		   met.caloMETPt()*std::sin(met.caloMETPhi()));
      shMet.setSumEmEt(0,0,0);
      shMet.setSumHadEt(0,0,0,0);
      
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
	   if(DetIdTools::isValidHcalBarrelId(iEta,iPhi,depth)){
	     HcalDetId detId(HcalBarrel,iEta,iPhi,depth);
	     size_t indx = DetIdTools::calHashHcal(detId.rawId());
	     if(indx>=hcalHitVec_.size()){
	       LogErr<<" error for HCAL barrel with eta "<<iEta<<" "<<iPhi<<" "<<depth<<" has a bad hash "<<indx<<" (max="<<hcalHitVec_.size()<<std::endl;
	     }else{
	       if(hcalHitVec_[indx].detId()!=0){
		 LogErr<<" error for HCAL barrel with eta "<<iEta<<" "<<iPhi<<" "<<depth<<", index "<<indx<<" already used "<<std::endl;
	       }else hcalHitVec_[indx].setDetId(detId.rawId());
	     }	  
	   }//end vaild det id
	 }//end depth loop 
       }//end iphi loop
    }//end side loop
  }//end barrel ieta loop

  for(int iEtaAbs=16;iEtaAbs<=29;iEtaAbs++){
    for(int side=0;side<=1;side++){
      int iEta = iEtaAbs*(2*side-1);
      for(int iPhi=1;iPhi<=72;iPhi++){
	for(int depth=1;depth<=8;depth++){
	  if(DetIdTools::isValidHcalEndcapId(iEta,iPhi,depth)){
	    HcalDetId detId(HcalEndcap,iEta,iPhi,depth);
	    size_t indx = DetIdTools::calHashHcal(detId.rawId());
	    if(indx>=hcalHitVec_.size()){
	       LogErr<<" error for HCAL endcap with eta "<<iEta<<" "<<iPhi<<" "<<depth<<" has a bad hash "<<indx<<" (max="<<hcalHitVec_.size()<<std::endl;
	     }else{
	       if(hcalHitVec_[indx].detId()!=0){
		 LogErr<<" error for HCAL endcap with eta "<<iEta<<" "<<iPhi<<" "<<depth<<", index "<<indx<<" already used "<<std::endl;
	       }else hcalHitVec_[indx].setDetId(detId.rawId());
	     }	      
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


//grr, this should be member func
int SHEventHelper::getTrkQuality_(const reco::Track& trk)
{
  int trkQual=0;
  for(int qualBit = 0;qualBit<reco::TrackBase::qualitySize;qualBit++){
    if(trk.quality(static_cast<reco::TrackBase::TrackQuality>(qualBit))) trkQual|=(0x1<<qualBit);
  }
  //now we deal with the invalid state
  if(trkQual==0) trkQual|=(0x1<<reco::TrackBase::qualitySize);
  return trkQual;
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


void SHEventHelper::fillPFClustersECAL_(const SHEvent* event,double maxDR,SHPFClusterContainer& shPFClusters,
		    const std::vector<reco::PFCluster>& pfClusters,
		    const std::vector<reco::SuperCluster>& superClustersEB,const std::vector<reco::SuperCluster>& superClustersEE)const
{
  //  std::cout <<"filling candidates "<<std::endl;

  const double maxDR2 = maxDR*maxDR;
  std::vector<std::pair<float,float> > eleEtaPhi;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    if(ele->et()>20){
      eleEtaPhi.push_back(std::make_pair(ele->detEta(),ele->detPhi()));
    }
  }

  for(size_t clusNr=0;clusNr<pfClusters.size();clusNr++){
    const reco::PFCluster& pfCluster = pfClusters[clusNr];
   
    bool accept =false;
    for(size_t eleNr=0;eleNr<eleEtaPhi.size();eleNr++){
      if(MathFuncs::calDeltaR2(eleEtaPhi[eleNr].first,eleEtaPhi[eleNr].second,
			       pfCluster.eta(),pfCluster.phi())<maxDR2){
	accept=true;
	break;
      }
    }//end ele loop

    if(accept){
      const std::vector<reco::SuperCluster>& superClusters = pfCluster.caloID().detector(reco::CaloID::DET_ECAL_BARREL) ? superClustersEB : superClustersEE;
      //std::cout <<" pf clus E "<<pfCluster.energy()<<" eta "<<pfCluster.eta()<<" phi "<<pfCluster.phi()<<" seed "<<pfCluster.seed().rawId()<<" address "<<&pfCluster<<std::endl;
      //int scSeedCrysId=0;
      int scSeedCrysId=getSCSeedCrysId_(pfCluster.seed().rawId(),superClusters);
      shPFClusters.addECALCluster(SHPFCluster(pfCluster,scSeedCrysId));
    } 
  }
}

void SHEventHelper::fillPFClustersHCAL_(const SHEvent* event,double maxDR,SHPFClusterContainer& shPFClusters,const std::vector<reco::PFCluster>& pfClusters)const
{
  //  std::cout <<"filling candidates "<<std::endl;

  const double maxDR2 = maxDR*maxDR;
  std::vector<std::pair<float,float> > eleEtaPhi;
  for(int eleNr=0;eleNr<event->nrElectrons();eleNr++){
    const SHElectron* ele = event->getElectron(eleNr);
    if(ele->et()>20){
      eleEtaPhi.push_back(std::make_pair(ele->detEta(),ele->detPhi()));
    }
  }

  for(size_t clusNr=0;clusNr<pfClusters.size();clusNr++){
    const reco::PFCluster& pfCluster = pfClusters[clusNr];

    bool accept =false;
    for(size_t eleNr=0;eleNr<eleEtaPhi.size();eleNr++){
      if(MathFuncs::calDeltaR2(eleEtaPhi[eleNr].first,eleEtaPhi[eleNr].second,
			       pfCluster.eta(),pfCluster.phi())<maxDR2){
	accept=true;
	break;
      }
    }//end ele loop

    if(accept){
      shPFClusters.addHCALCluster(SHPFCluster(pfCluster,0));
    } 
  }
}

int SHEventHelper::getSCSeedCrysId_(uint pfSeedId,const std::vector<reco::SuperCluster>& superClusters)const
{
  // std::cout <<"getting sc seed id "<<std::endl;
  for(size_t scNr=0;scNr<superClusters.size();scNr++){
    // std::cout <<"super clust "<<scNr<<" / "<<superClusters.size()<<std::endl;
    const reco::SuperCluster& sc = superClusters[scNr];
    for(reco::CaloCluster_iterator clusIt  = sc.clustersBegin();clusIt!=sc.clustersEnd();++clusIt){
      //  std::cout <<"clus seed E "<<(*clusIt)->energy()<<" eta "<<(*clusIt)->eta()<<" phi "<<(*clusIt)->phi()<<" "<<(*clusIt)->seed().rawId()<<" pf seed "<<pfSeedId<<" address "<<(&**clusIt)<<std::endl;
      if((*clusIt)->seed().rawId()==pfSeedId) return sc.seed()->seed().rawId();
    }
  }
  return 0;
}

const std::vector<int> makeSortedVec(std::vector<int> vec)
{
  
  std::vector<int> sorted(std::move(vec));
  std::sort(sorted.begin(),sorted.end());
  return sorted;
}

void SHEventHelper::fixTrkIsols_(const heep::Event& heepEvent,const edm::Ptr<reco::GsfElectron>& gsfEle,SHElectron& shEle)const
{
  if(branches_.fillTrkIsolFromUserData){
    const pat::Electron* patEle = dynamic_cast<const pat::Electron*>(&*gsfEle);
    if(patEle){
      shEle.setTrkIsol(shEle.isolPtTrks(),shEle.isolPtTrksDR04(),patEle->userFloat(branches_.trkIsolUserDataName));
      return;
    }
      
  }else if(heepEvent.handles().eleIsolPtTrksValueMap.isValid()){
    float trkIso = (*heepEvent.handles().eleIsolPtTrksValueMap)[gsfEle];
    shEle.setTrkIsol(shEle.isolPtTrks(),shEle.isolPtTrksDR04(),trkIso);
    return;
  }else{
    shEle.setTrkIsol(shEle.isolPtTrks(),shEle.isolPtTrksDR04(),-1);
    return;
  }
   
  if(!heepEvent.handles().ctfTrack.isValid()){
    shEle.setTrkIsol(-1.,-1.,-1);
    return;
  }
  const float minDR2=0.015*0.015;
  const float minAbsEta = 0.015;
  const float minTrkPt = 0.7;
  const float maxTrkDZ = 0.2;
  static const std::vector<int>algosToReject = makeSortedVec({reco::TrackBase::jetCoreRegionalStep});
  
  float isolPtTrks03=0.;
  float isolPtTrks04=0.;
  int isolNrTrks=0;
  
  const float gsfTrkEta = gsfEle->gsfTrack()->eta();
  const float gsfTrkPhi = gsfEle->gsfTrack()->phi();
  const float gsfTrkVZ = gsfEle->gsfTrack()->vz();
  
  for (auto& trk  : heepEvent.ctfTracks()){
    
    if(std::abs(trk.vz() - gsfTrkVZ)<maxTrkDZ && 
       trk.pt()>minTrkPt &&
       !std::binary_search(algosToReject.begin(),algosToReject.end(),trk.algo())){
      const float trkEta = trk.eta();
      const float trkPhi = trk.phi();
      float dR2 = reco::deltaR2(gsfTrkEta,gsfTrkPhi,trkEta,trkPhi);
      if(dR2>minDR2 && std::abs(trkEta-gsfTrkEta)>minAbsEta){
	if(dR2<0.4*0.4) isolPtTrks04+=trk.pt();
	if(dR2<0.3*0.3){
	     isolPtTrks03+=trk.pt();
	     isolNrTrks++;
	}
      }
    }
  }
  shEle.setTrkIsol(isolPtTrks03,isolPtTrks04,isolNrTrks);
}


void SHEventHelper::setCutCode_(const heep::Event& heepEvent,const edm::Ptr<reco::GsfElectron>& gsfEle,SHElectron& shEle)const
{
  const heep::Ele* heepEle=nullptr;
  for(auto& ele : heepEvent.heepEles()){
    if(&ele.gsfEle()==&*gsfEle){
      heepEle=&ele;
      break;
    }
  }
  if(heepEle) shEle.setCutCode(heepEle->cutCode());    

  if(heepEvent.handles().heepIDVIDBits.isValid()){
    shEle.setCutCodeVID((*heepEvent.handles().heepIDVIDBits)[gsfEle]);
  }
  std::vector<int> vidBits;
  for(const auto& idBits : heepEvent.handles().vidBits){
    if(idBits.isValid()){
      vidBits.push_back((*idBits)[gsfEle]);
    }else{
      vidBits.push_back(-1);
    } 
  } 
  shEle.setIDs(std::move(vidBits));
}

void SHEventHelper::addUserData_(const reco::Photon* pho,SHElectron& ele)const
{
  const pat::Photon* patPho = dynamic_cast<const pat::Photon*>(pho);
  if(patPho){
    //ele.setUserIDs(patPho->electronIDs());
    std::vector<std::pair<std::string,float> > userIDs;
    std::vector<std::pair<std::string,float> > userFloats;
    std::vector<std::pair<std::string,int> > userInts;
    for(auto& name : patPho->userFloatNames()) userFloats.push_back({name,patPho->userFloat(name)});  
    for(auto& name : patPho->userIntNames()) userInts.push_back({name,patPho->userInt(name)});
    for(auto& id : patPho->photonIDs()) userIDs.push_back({id.first,id.second});
    
    ele.setUserInts(std::move(userInts));
    ele.setUserFloats(std::move(userFloats));
    ele.setUserIDs(std::move(userIDs));

  }
}

void SHEventHelper::addUserData_(const reco::GsfElectron* gsfEle,SHElectron& ele)const
{
  const pat::Electron* patEle = dynamic_cast<const pat::Electron*>(gsfEle);
  if(patEle){
    ele.setUserIDs(patEle->electronIDs());
    std::vector<std::pair<std::string,float> > userFloats;
    std::vector<std::pair<std::string,int> > userInts;
    for(auto& name : patEle->userFloatNames()) userFloats.push_back({name,patEle->userFloat(name)});  
    for(auto& name : patEle->userIntNames()) userInts.push_back({name,patEle->userInt(name)});
    ele.setUserInts(std::move(userInts));
    ele.setUserFloats(std::move(userFloats));

  }
}
 
#include "SHarper/HEEPAnalyzer/interface/HEEPEcalClusterTools.h"
void SHEventHelper::setNrSatCrysIn5x5_(const heep::Event& heepEvent,SHElectron& shEle)const
{
  DetId id = shEle.superClus()->seedClus()->seedId();
  const EcalRecHitCollection* ebHits =heepEvent.handles().ebRecHits.isValid() ? heepEvent.ebHitsFull() : 
    heepEvent.handles().ebReducedRecHits.isValid() ? &(*heepEvent.handles().ebReducedRecHits) : nullptr;
  const EcalRecHitCollection* eeHits =heepEvent.handles().eeRecHits.isValid() ? heepEvent.eeHitsFull() : 
    heepEvent.handles().eeReducedRecHits.isValid() ? &(*heepEvent.handles().eeReducedRecHits) : nullptr;
  auto recHits = id.subdetId()==EcalBarrel ? ebHits : eeHits;

  shEle.setNrSatCrysIn5x5(heep::EcalClusterTools::nrSaturatedCrysIn5x5(id,recHits,heepEvent.handles().caloTopology.product()));
  

}

bool SHEventHelper::isNearEle_(float eta,float phi,const SHEvent& shEvent,const float maxDR)const
{
  const float maxDR2=maxDR*maxDR;
  for(int eleNr=0;eleNr<shEvent.nrElectrons();eleNr++){
    const SHElectron& ele = *shEvent.getElectron(eleNr);
    if(ele.dEtaIn()>900) continue;
    if(MathFuncs::calDeltaR2(ele.detEta(),ele.detPhi(),eta,phi)<maxDR2) return true;
    if(MathFuncs::calDeltaR2(ele.eta(),ele.phi(),eta,phi)<maxDR2) return true;
    
  }
  return false;

}
void SHEventHelper::fillEcalHitVec_(const heep::Event& heepEvt,const EcalRecHitCollection& hitColl,const SHEvent& shEvent)const 
{
  for(auto& hit : hitColl){
    if(!branches_.filterEcalHits || 
       passCaloHitFilter_(hit.detid(),shEvent,kMaxDREcalHits_) ){
      
      SHCaloHit& shHit = ecalHitVec_[ecalHitHash_(hit.detid())];
      setRecHit(shHit,hit,heepEvt);
    }
  }
}

float SHEventHelper::getEcalRecHitAmplitude(const heep::Event& heepEvent,int detId,float calibNrgy)const
{
  float calibConst = heepEvent.getEcalRecHitCalibConst(detId);
  float adcToGeVConst = heepEvent.getADCToGeVConst(detId);
  
  

  if(calibConst*adcToGeVConst!=0) return calibNrgy/(calibConst*adcToGeVConst);
  else{
    std::cout <<"error calib is zero for "<<detId<<" "<<calibNrgy<<" calib "<<calibConst<<" adc "<<adcToGeVConst<<std::endl;

    return std::numeric_limits<float>::max();
  }
}

bool SHEventHelper::passCaloHitFilter_(int hitId,const SHEvent& shEvent,const float maxDR)const
{
  double cellEta=0,cellPhi=0;
  GeomFuncs::getCellEtaPhi(hitId,cellEta,cellPhi);
  
  const float maxDR2=maxDR;
  for(int eleNr=0;eleNr<shEvent.nrElectrons();eleNr++){
    const SHElectron& ele = *shEvent.getElectron(eleNr);
    if(MathFuncs::calDeltaR2(ele.detEta(),ele.detPhi(),cellEta,cellPhi)<maxDR2) return true;    
  }
  return false;

}


void SHEventHelper::setRecHit(SHCaloHit& shHit,const EcalRecHit& hit,const heep::Event& heepEvent)const
{
  shHit.setDetId(hit.detid());
  shHit.setNrgy(hit.energy());
  shHit.setTime(hit.time());
  shHit.setFlagBits(getEcalFlagBits_(hit));     
  shHit.setTimeErr(hit.timeError());
  shHit.setNrgyErr(hit.energyError());
  shHit.setChi2(hit.chi2());
  shHit.setAmplitude(getEcalRecHitAmplitude(heepEvent,hit.detid(),hit.energy()));
  
  if(shHit.nrgy()!=hit.energy() || shHit.chi2()!=hit.chi2() || shHit.timeErr()!=hit.timeError() ||
     shHit.nrgyErr()!=hit.energyError() || shHit.time()!=hit.time()){
    std::cout <<"miss match nrgy : "<<shHit.nrgy()<<" "<<hit.energy()<<" chi2 "<<shHit.chi2()<<" "<<hit.chi2()<<" timeErr "<<shHit.timeErr()<<" "<<hit.timeError()<<" nrgyErr "<<shHit.nrgyErr()<<" "<<hit.energyError()<<" time "<<shHit.time()<<" "<<hit.time()<<std::endl;
  }

}

bool SHEventHelper::isEcalBarrel_(const DetId& id)
{
  return id.det()==DetId::Ecal && id.subdetId()==EcalBarrel;
}

bool SHEventHelper::isEcalEndcap_(const DetId& id)
{
  return id.det()==DetId::Ecal && id.subdetId()==EcalEndcap;
}

const reco::Photon* 
SHEventHelper::getPhoMatch_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::Event& heepEvent)
{
  const reco::Photon* bestMatch = nullptr;
  if(heepEvent.handles().recoPho.isValid()) {
    for(auto& pho : *heepEvent.handles().recoPho){
      if(pho.superCluster()->seed()->seed() == gsfEle->superCluster()->seed()->seed()){
	if(bestMatch) std::cout <<"already matched photon "<<bestMatch->energy()<<" "<<pho.energy()<<std::endl;
	bestMatch = &pho;
      }
    }
  }
  return bestMatch;
}



namespace{
  int calDIEta(int lhs,int rhs)
  {
    int retVal = lhs - rhs;
    if(lhs*rhs<0){ //crossing zero
    if(retVal<0) retVal++;
    else retVal--;
    }
    return retVal;
  }

  int calDIPhi(int lhs,int rhs)
  {
    int retVal = lhs-rhs;
    while(retVal>180) retVal-=360;
    while(retVal<-180) retVal+=360;
    return retVal;
  }

  int calDIR2(const DetId lhs,const DetId rhs)
  {
    if(lhs.subdetId()!=rhs.subdetId()) return std::numeric_limits<int>::max();
    else if(lhs.subdetId()==EcalBarrel){
      
      EBDetId lhsEB(lhs);
      EBDetId rhsEB(rhs); 
      int dIEta = calDIEta(lhsEB.ieta(),rhsEB.ieta());
      int dIPhi = calDIPhi(lhsEB.iphi(),rhsEB.iphi());
      int dIR2 = dIEta*dIEta+dIPhi*dIPhi;
      return dIR2;
    }else if(lhs.subdetId()==EcalEndcap){
      EEDetId lhsEE(lhs);
      EEDetId rhsEE(rhs); 
      if(lhsEE.zside()!=rhsEE.zside()){
	return std::numeric_limits<int>::max();
      }else{
	int dIX = std::abs(lhsEE.ix()-rhsEE.ix());
	int dIY = std::abs(lhsEE.iy()-rhsEE.iy());
	int dIR2 = dIX*dIX+dIY*dIY;
	return dIR2;
      }
    }else return std::numeric_limits<int>::max();
    
  }
}
  
const reco::GsfElectron* 
SHEventHelper::getOldEleMatch_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::Event& heepEvent)
{
  const reco::GsfElectron* bestMatch = nullptr;
  int bestDIR2=gsfEle->isEB() ?  2  : 0; //seed in 3x3 barrel, exact match endcap
  if(heepEvent.handles().oldGsfEle.isValid()) {
    for(auto& ele : *heepEvent.handles().oldGsfEle){
      int dIR2 = calDIR2(ele.superCluster()->seed()->seed(),gsfEle->superCluster()->seed()->seed());
      if(dIR2<=bestDIR2){
	bestMatch = &ele;
	bestDIR2 = dIR2;
      }
    }
  }
  return bestMatch;
}
const reco::Photon* 
SHEventHelper::getOldPhoMatch_(const edm::Ptr<reco::GsfElectron>& gsfEle,const heep::Event& heepEvent)
{
  const reco::Photon* bestMatch = nullptr;
  int bestDIR2=gsfEle->isEB() ?  2  : 0; //seed in 3x3 barrel, exact match endcap
  if(heepEvent.handles().oldPho.isValid()) {
    for(auto& pho : *heepEvent.handles().oldPho){
      int dIR2 = calDIR2(pho.superCluster()->seed()->seed(),gsfEle->superCluster()->seed()->seed());
      if(dIR2<=bestDIR2){
	bestMatch = &pho;
	bestDIR2 = dIR2;
      }
    }
  }
  return bestMatch;
}
const reco::Photon* 
SHEventHelper::getOldPhoMatch_(const reco::Photon& pho,const heep::Event& heepEvent)
{
  const reco::Photon* bestMatch = nullptr;
  int bestDIR2=pho.isEB() ?  2  : 0; //seed in 3x3 barrel, exact match endcap
  if(heepEvent.handles().oldPho.isValid()) {
    for(auto& oldPho : *heepEvent.handles().oldPho){
      int dIR2 = calDIR2(oldPho.superCluster()->seed()->seed(),pho.superCluster()->seed()->seed());
      if(dIR2<=bestDIR2){
	bestMatch = &oldPho;
	bestDIR2 = dIR2;
      }
    }
  }
  return bestMatch;
}

		  
