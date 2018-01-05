

#include "SHarper/TrigTools/interface/HLTElectronTrackMatchFilter.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/AssociationMap.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"

#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"

#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

//temp for debugging
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"

//
// constructors and destructor
//
HLTElectronTrackMatchFilter::HLTElectronTrackMatchFilter(const edm::ParameterSet& iConfig)  : HLTFilter(iConfig), tree_(0), file_(0)
{
  candTag_            = iConfig.getParameter< edm::InputTag > ("candTag");
  
  l1IsoGsfTracksTag_  = iConfig.getParameter< edm::InputTag > ("l1IsoGsfTracks");
  l1NonIsoGsfTracksTag_  = iConfig.getParameter< edm::InputTag > ("l1NonIsoGsfTracks");

  ncandcut_           = iConfig.getParameter<int> ("ncandcut");

  doIsolated_    = iConfig.getParameter<bool> ("doIsolated");
  debugDEtaInputTag_ = iConfig.getParameter< edm::InputTag >("debugDEta");
  debugDPhiInputTag_ = iConfig.getParameter< edm::InputTag >("debugDPhi");
  
  
  l1IsoCollTag_= iConfig.getParameter< edm::InputTag > ("L1IsoCand"); 
  l1NonIsoCollTag_= iConfig.getParameter< edm::InputTag > ("L1NonIsoCand"); 
  beamSpotTag_ = iConfig.getParameter<edm::InputTag>("beamSpot");
  
  dPhiMaxEB_ = iConfig.getParameter<double>("dPhiMaxEB");// 0.2;
  dEtaMaxEB_ = iConfig.getParameter<double>("dEtaMaxEB");//0.2;
  dPhiMaxEE_ = iConfig.getParameter<double>("dPhiMaxEE");// 0.2;
  dEtaMaxEE_ = iConfig.getParameter<double>("dEtaMaxEE");//0.2;

  std::string outFilename = iConfig.getParameter<std::string>("outFilename");
  if(!outFilename.empty()){
    file_= new TFile(outFilename.c_str(),"RECREATE");
    tree_ = new TTree("hltEleTree","tree");
    tree_->Branch("evt",&evtInfo_,EvtInfoStruct::contents().c_str());
    tree_->Branch("hltEle",&eleData_,EleDataStruct::contents().c_str());
    tree_->Branch("trkNr",&trkNr_,"trkNr/I");
    tree_->Branch("bs",&beamSpotPos_,PosStruct::contents().c_str());
  }
  
}


HLTElectronTrackMatchFilter::~HLTElectronTrackMatchFilter()
{
  if(file_) {
    file_->Write();
    delete file_;
  
  }
}


// ------------ method called to produce the data  ------------
bool
HLTElectronTrackMatchFilter::hltFilter(edm::Event& iEvent, const edm::EventSetup& iSetup,trigger::TriggerFilterObjectWithRefs & filterproduct)
{
  // The filter object
 
  
  if( saveTags() ){filterproduct.addCollectionTag(l1IsoCollTag_);}
  if( saveTags() && !doIsolated_){filterproduct.addCollectionTag(l1NonIsoCollTag_);}
  

  edm::Handle<trigger::TriggerFilterObjectWithRefs> PrevFilterOutput;

  iEvent.getByLabel (candTag_,PrevFilterOutput);

  std::vector<edm::Ref<reco::RecoEcalCandidateCollection> > recoecalcands;
  PrevFilterOutput->getObjects(trigger::TriggerCluster, recoecalcands);

  //get hold of the pixel seed - supercluster association map
  edm::Handle<reco::GsfTrackCollection> l1IsoGsfTracks;
  iEvent.getByLabel (l1IsoGsfTracksTag_,l1IsoGsfTracks);

  edm::Handle<reco::GsfTrackCollection> l1NonIsoGsfTracks;
  if(!doIsolated_) iEvent.getByLabel (l1NonIsoGsfTracksTag_,l1NonIsoGsfTracks);
  
  edm::Handle<reco::BeamSpot> beamSpotHandle ;
  iEvent.getByLabel(beamSpotTag_,beamSpotHandle) ;


   //debugging 
 
  //edm::InputTag dEtaInputTag("hltL1GsfTrackVars","Deta");
  //edm::InputTag dPhiInputTag("hltL1GsfTrackVars","Dphi"); 
  edm::Handle<reco::RecoEcalCandidateIsolationMap> dEtaHandle;
  iEvent.getByLabel(debugDEtaInputTag_,dEtaHandle);
  edm::Handle<reco::RecoEcalCandidateIsolationMap> dPhiHandle;
  iEvent.getByLabel(debugDPhiInputTag_,dPhiHandle); 

  eventSetupData_.setup(iSetup);
  

  //debugging
  beamSpotPos_.fill(beamSpotHandle->position());
  evtInfo_.fill(iEvent);
 
  int nrPass = 0;

  // std::cout <<"start of event "<<iEvent.id().event()<<std::endl;
  for (unsigned int i=0; i<recoecalcands.size(); i++) {

    edm::Ref<reco::RecoEcalCandidateCollection> ref = recoecalcands[i];
    reco::SuperClusterRef superClusRef = ref->superCluster();
 
    //naughty, these temp debugging vars are not filled by eleData_.fill() so 
    //if we set them up here, they wont be overidden
    if(dEtaHandle.isValid()&& dPhiHandle.isValid()){
      eleData_.matteoDEta = dEtaHandle->find(ref)->val;
      eleData_.matteoDPhi = dPhiHandle->find(ref)->val;
    }else {
      eleData_.matteoDEta =999;
      eleData_.matteoDPhi =999;
    }
    //std::cout <<"eventnr: "<<iEvent.id().event()<<" ele "<<i<<" et "<<ref->et()<<" dEta "<<eleData_.matteoDEta<<std::endl;
    
    bool match = matchedTrack(*l1IsoGsfTracks,superClusRef,*beamSpotHandle);
    if(!doIsolated_ && !match) match=matchedTrack(*l1NonIsoGsfTracks,superClusRef,*beamSpotHandle);
    
    if (match) {
      nrPass++;
      filterproduct.addObject(trigger::TriggerCluster, ref);
    }
    
  }//end loop over candidates
  bool accept(nrPass>=ncandcut_);
  
  
  return accept;
}


bool HLTElectronTrackMatchFilter::matchedTrack(const reco::GsfTrackCollection& gsfTracks,const reco::SuperClusterRef superClusRef,const reco::BeamSpot& beamSpot)
{  
  bool returnVal=false;
  trkNr_=0; //debug var
  //bool matched=false;
  //std::cout <<" sc "<<superClusRef->energy()<<" eta "<<superClusRef->eta()<<" phi "<<superClusRef->phi()<<std::endl;
  GlobalPoint scPos(superClusRef->x(),superClusRef->y(),superClusRef->z());
  GlobalPoint bsPos(beamSpot.position().x(),beamSpot.position().y(),beamSpot.position().z());
  int nrTracks = 0;

  eleData_.minDEta=999;
  eleData_.minDPhi=999;

  std::vector<GsfTrkEpPair> trkEpPairs;

  for(reco::GsfTrackCollection::const_iterator trkIt =gsfTracks.begin();trkIt!=gsfTracks.end();++trkIt){
    edm::RefToBase<TrajectorySeed> seed = trkIt->extra()->seedRef() ;
    reco::ElectronSeedRef elseed = seed.castTo<reco::ElectronSeedRef>() ;
    edm::RefToBase<reco::CaloCluster> caloCluster = elseed->caloCluster() ;
    reco::SuperClusterRef scRef = caloCluster.castTo<reco::SuperClusterRef>() ;
    if(&*scRef==&*superClusRef){
      GlobalVector momAtVtx = eventSetupData_.extrapolateTrackMomToPoint(*trkIt,bsPos);
      float eOverP = momAtVtx.mag()==0 ? -1 : scRef->energy()/momAtVtx.mag(); 
      // std::cout <<" normal p "<<trkIt->p()<<" vtx p "<<momAtVtx.mag()<<std::endl;
      trkEpPairs.push_back(GsfTrkEpPair(&*trkIt,eOverP));
    }
  }
  nrTracks = trkEpPairs.size();
  std::sort(trkEpPairs.begin(),trkEpPairs.end(),isInnerMost(eventSetupData_.trackerGeomHandle()));

  for(size_t trkNr=0;trkNr<trkEpPairs.size() && trkNr<999;trkNr++){
  
    const reco::GsfTrack* trk=trkEpPairs[trkNr].first; //this is the best one...
    
    GlobalPoint trackExtrapToSC = eventSetupData_.extrapolateTrackPosToPoint(*trk,scPos);
    EleRelPointPair scAtVtx(superClusRef->position(),trackExtrapToSC,beamSpot.position());
   
    if(fabs(scAtVtx.dEta())<fabs(eleData_.minDEta)) eleData_.minDEta = scAtVtx.dEta();
    if(fabs(scAtVtx.dPhi())<fabs(eleData_.minDPhi)) eleData_.minDPhi = scAtVtx.dPhi();
  
  
  }
  float dEtaMax = superClusRef->eta() < 1.479 ? dEtaMaxEB_ : dEtaMaxEE_;
  float dPhiMax = superClusRef->eta() < 1.479 ? dPhiMaxEB_ : dPhiMaxEE_;

  if( (fabs(eleData_.minDPhi)<dPhiMax && fabs(eleData_.minDEta)<dEtaMax) || trkEpPairs.size()>1) returnVal=true;
  eleData_.pass=returnVal;

  //if(!trkEpPairs.empty()){ //we found a match
  for(size_t trkNr=0;trkNr<trkEpPairs.size() && trkNr<999;trkNr++){
    
    const reco::GsfTrack* trk=trkEpPairs[trkNr].first; //this is the best one...
    
    GlobalPoint trackExtrapToSC = eventSetupData_.extrapolateTrackPosToPoint(*trk,scPos);
    EleRelPointPair scAtVtx(superClusRef->position(),trackExtrapToSC,beamSpot.position());
    EleRelPointPair scAtVtxNoBS(superClusRef->position(),trackExtrapToSC,math::XYZPoint(beamSpot.position().x(),beamSpot.position().y(),0));
    // std::cout <<"dPhi "<<scAtVtx.dPhi()<<" dEta "<<scAtVtx.dEta()<<std::endl<<"dPhiNoBS "<<scAtVtxNoBS.dPhi()<<" dEtaNoBS "<<scAtVtxNoBS.dEta()<<std::endl<<std::endl;
    //   std::cout <<"nr miss hits "<<trkIt->trackerExpectedHitsInner().numberOfLostHits()<<std::endl;
    //  std::cout <<"seed ref "<<&*scRef<<" sc ref "<<&*superClusRef<<std::endl;
    //std::cout <<"              trk "<<trkNr<<" dEta "<<scAtVtx.dEta()<<std::endl;
    trkNr_++;
    eleData_.fill(*superClusRef,*trk,trackExtrapToSC,scAtVtx.dEta(),scAtVtx.dPhi(),nrTracks);
    
    if(tree_) tree_->Fill();
    
    //if((fabs(scAtVtx.dPhi())<dPhiMax_ && fabs(scAtVtx.dEta())<dEtaMax_) || nrTracks>1) returnVal=true; 
    // if(fabs(scAtVtx.dPhi())<dPhiMax_ && fabs(scAtVtx.dEta())<dEtaMax_) returnVal=true;
  }
  if(trkEpPairs.empty()){ //we did not find a match

    eleData_.fill(*superClusRef);
    if(tree_) tree_->Fill();
  }
  return  returnVal;
}



GlobalPoint HLTElectronTrackMatchFilter::EventSetupData::extrapolateTrackPosToPoint(const reco::GsfTrack& gsfTrack,const GlobalPoint& pointToExtrapTo)
{
  TrajectoryStateOnSurface innTSOS = mtsTransform()->innerStateOnSurface(gsfTrack);
  TrajectoryStateOnSurface posTSOS = mtsTransform()->extrapolatedState(innTSOS,pointToExtrapTo);
  GlobalPoint  extrapolatedPos;
  mtsMode()->positionFromModeCartesian(posTSOS,extrapolatedPos);
  return extrapolatedPos;
}

GlobalVector HLTElectronTrackMatchFilter::EventSetupData::extrapolateTrackMomToPoint(const reco::GsfTrack& gsfTrack,const GlobalPoint& pointToExtrapTo)
{
  TrajectoryStateOnSurface innTSOS = mtsTransform()->innerStateOnSurface(gsfTrack);
  TrajectoryStateOnSurface posTSOS = mtsTransform()->extrapolatedState(innTSOS,pointToExtrapTo);
  GlobalVector  extrapolatedMom;
  mtsMode()->momentumFromModeCartesian(posTSOS,extrapolatedMom);
  return extrapolatedMom;
}

HLTElectronTrackMatchFilter::EventSetupData::EventSetupData(const HLTElectronTrackMatchFilter::EventSetupData& rhs):
  cacheIDTDGeom_(rhs.cacheIDTDGeom_),
  cacheIDMagField_(rhs.cacheIDMagField_),
  magField_(rhs.magField_),
  trackerHandle_(rhs.trackerHandle_),
  mtsMode_(rhs.mtsMode_)
 
{
  if(rhs.mtsTransform_) mtsTransform_ = new MultiTrajectoryStateTransform(*rhs.mtsTransform_);
  else mtsTransform_ =0;
    
}  

HLTElectronTrackMatchFilter::EventSetupData* HLTElectronTrackMatchFilter::EventSetupData::operator=(const HLTElectronTrackMatchFilter::EventSetupData& rhs)
{ 
  if(this!=&rhs){ //just to ensure we're not copying ourselves
    cacheIDTDGeom_ = rhs.cacheIDTDGeom_;
    cacheIDMagField_ = rhs.cacheIDMagField_;
    magField_ = rhs.magField_;
    trackerHandle_ = rhs.trackerHandle_;
    mtsMode_ = rhs.mtsMode_;
    
    delete mtsTransform_;
    if(rhs.mtsTransform_) mtsTransform_ = new MultiTrajectoryStateTransform(*rhs.mtsTransform_);
    else mtsTransform_ =0;
  }
  return this;
}

void HLTElectronTrackMatchFilter::EventSetupData::setup(const edm::EventSetup& iSetup)
{
  bool updateField(false);
  if (cacheIDMagField_!=iSetup.get<IdealMagneticFieldRecord>().cacheIdentifier()){
    updateField = true;
    cacheIDMagField_=iSetup.get<IdealMagneticFieldRecord>().cacheIdentifier();
    iSetup.get<IdealMagneticFieldRecord>().get(magField_);
  }
  
  bool updateGeometry(false);
  if (cacheIDTDGeom_!=iSetup.get<TrackerDigiGeometryRecord>().cacheIdentifier()){
    updateGeometry = true;
    cacheIDTDGeom_=iSetup.get<TrackerDigiGeometryRecord>().cacheIdentifier();
    iSetup.get<TrackerDigiGeometryRecord>().get(trackerHandle_);
  }
  
  if ( updateField || updateGeometry || !mtsTransform_ ) {
    delete mtsTransform_;
    mtsTransform_ = new MultiTrajectoryStateTransform(trackerHandle_.product(),magField_.product());
  }
}

void HLTElectronTrackMatchFilter::EleDataStruct::fill(const reco::SuperCluster& superClus,const reco::GsfTrack& trk,const GlobalPoint& trkPosAtSC,float iDEta,float iDPhi,float iNrTracks)
{
  nrgy = superClus.energy();
  et = superClus.energy()*sin(superClus.position().theta());
  detEta = superClus.eta();
  detPhi = superClus.phi();
  
  trkPt = trk.pt();
  trkP = trk.p();
  trkEta = trk.eta();
  trkPhi = trk.phi();
  q = trk.charge();
  
  trkEtaAtSC=trkPosAtSC.eta();
  trkPhiAtSC=trkPosAtSC.phi();
  dPhi = iDPhi;
  dEta = iDEta;

  nrMissHits =trk.hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS);
  nrTracks = iNrTracks;
}
void HLTElectronTrackMatchFilter::EleDataStruct::fill(const reco::SuperCluster& superClus)
{
  nrgy = superClus.energy();
  et = superClus.energy()*sin(superClus.position().theta());
  detEta = superClus.eta();
  detPhi = superClus.phi();
  
  trkPt = -1;
  trkP =-1;
  trkEta = -999;
  trkPhi = -999;
  q = -999;
  trkEtaAtSC=-999;
  trkPhiAtSC=-999;
  dPhi = 999;
  dEta = 999;
  
  nrMissHits = 999;
  nrTracks = -999;
}
void HLTElectronTrackMatchFilter::EvtInfoStruct::fill(const edm::Event& event)
{
  runnr = event.id().run();
  lumiSec = event.luminosityBlock();
  eventnr = event.id().event();
  bx = event.bunchCrossing();
}



bool  HLTElectronTrackMatchFilter::isInnerMost::operator()
 ( const GsfTrkEpPair& e1, const GsfTrkEpPair& e2 )
 {
  reco::HitPattern gsfHitPattern1 = e1.first->hitPattern() ;
  reco::HitPattern gsfHitPattern2 = e2.first->hitPattern() ;

  // retreive first valid hit
  int gsfHitCounter1 = 0 ;
  trackingRecHit_iterator elHitsIt1 ;
  for
   ( elHitsIt1 = e1.first->recHitsBegin() ;
     elHitsIt1 != e1.first->recHitsEnd() ;
     elHitsIt1++, gsfHitCounter1++ )
   { if (((**elHitsIt1).isValid())) break ; }

  int gsfHitCounter2 = 0 ;
  trackingRecHit_iterator elHitsIt2 ;
  for
   ( elHitsIt2 = e2.first->recHitsBegin() ;
     elHitsIt2 != e2.first->recHitsEnd() ;
     elHitsIt2++, gsfHitCounter2++ )
   { if (((**elHitsIt2).isValid())) break ; }

  uint32_t gsfHit1 = gsfHitPattern1.getHitPattern(reco::HitPattern::TRACK_HITS,gsfHitCounter1) ;
  uint32_t gsfHit2 = gsfHitPattern2.getHitPattern(reco::HitPattern::TRACK_HITS,gsfHitCounter2) ;
  if (gsfHitPattern1.getSubStructure(gsfHit1)!=gsfHitPattern2.getSubStructure(gsfHit2))
   { return (gsfHitPattern1.getSubStructure(gsfHit1)<gsfHitPattern2.getSubStructure(gsfHit2)) ; }
  else if (gsfHitPattern1.getLayer(gsfHit1)!=gsfHitPattern2.getLayer(gsfHit2))
   { return (gsfHitPattern1.getLayer(gsfHit1)<gsfHitPattern2.getLayer(gsfHit2)) ; }
  else
   { return (std::abs(e1.second-1)<std::abs(e2.second-1)) ; }

 }
