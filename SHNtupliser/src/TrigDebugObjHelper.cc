#include "SHarper/SHNtupliser/interface/TrigDebugObjHelper.h"

#include "SHarper/SHNtupliser/interface/SHTrigObjContainer.hh"


#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"

#include <boost/algorithm/string.hpp>

TrigDebugObjHelper::TrigDebugObjHelper(const edm::ParameterSet& iConfig)
{
  hltIsoEleProducer_ = iConfig.getParameter<edm::InputTag>("hltIsoEleProducer");
  hltNonIsoEleProducer_ = iConfig.getParameter<edm::InputTag>("hltNonIsoEleProducer");
  hltIsoEcalCandProducer_ = iConfig.getParameter<edm::InputTag>("hltIsoEcalCandProducer");
  hltNonIsoEcalCandProducer_ = iConfig.getParameter<edm::InputTag>("hltNonIsoEcalCandProducer");
  hltTag_ = iConfig.getParameter<std::string>("hltProcName");
  std::vector<std::string>  ecalCandValuesToSave = iConfig.getParameter<std::vector<std::string> >("ecalCandValuesToSave");
  std::vector<std::string>  eleValuesToSave = iConfig.getParameter<std::vector<std::string> >("eleValuesToSave");
  processInputVecOfProdNames(ecalCandValuesToSave,ecalCandVarNames_,ecalCandProdNames_,hltTag_);
  processInputVecOfProdNames(eleValuesToSave,eleVarNames_,eleProdNames_,hltTag_);
}

void TrigDebugObjHelper::fillDebugTrigObjs(const edm::Event& iEvent,SHTrigObjContainer* shTrigObjs)const
{
  shTrigObjs->clear();
  edm::Handle<reco::ElectronCollection> hltIsoElesHandle;
  iEvent.getByLabel(edm::InputTag(hltIsoEleProducer_.label(),"",hltTag_),hltIsoElesHandle);
  edm::Handle<reco::ElectronCollection> hltNonIsoElesHandle;
  iEvent.getByLabel(edm::InputTag(hltNonIsoEleProducer_.label(),"",hltTag_),hltNonIsoElesHandle);
  
  edm::Handle<reco::RecoEcalCandidateCollection> hltIsoEcalCandsHandle;
  iEvent.getByLabel(edm::InputTag(hltIsoEcalCandProducer_.label(),"",hltTag_),hltIsoEcalCandsHandle);
  edm::Handle<reco::RecoEcalCandidateCollection> hltNonIsoEcalCandsHandle;
  iEvent.getByLabel(edm::InputTag(hltNonIsoEcalCandProducer_.label(),"",hltTag_),hltNonIsoEcalCandsHandle);
  
  // std::cout <<"hlt tag "<<hltIsoEcalCandProducer_.label()<<" "<<hltTag_<<std::endl;
  
  typedef std::pair<reco::RecoEcalCandidateRef,reco::ElectronRef> EcalCandElePair;
  std::vector<EcalCandElePair> ecalCandElesIso;
  std::vector<EcalCandElePair> ecalCandElesNonIso;
  
  fillEcalCandEles(hltIsoEcalCandsHandle,hltIsoElesHandle,ecalCandElesIso,true);
  fillEcalCandEles(hltNonIsoEcalCandsHandle,hltNonIsoElesHandle,ecalCandElesNonIso,false);

  fillDebugTrigObjs(iEvent,ecalCandElesIso,shTrigObjs);
  fillDebugTrigObjs(iEvent,ecalCandElesNonIso,shTrigObjs);
}


void TrigDebugObjHelper::fillDebugTrigObjs(const edm::Event& event,std::vector<std::pair<reco::RecoEcalCandidateRef,reco::ElectronRef> > ecalCandEles, SHTrigObjContainer* shTrigObjs,bool isIso)const
{
  // std::cout <<"nr ecal cands "<<ecalCandEles.size()<<std::endl;
  for(size_t candNr=0;candNr<ecalCandEles.size();candNr++){
    std::vector<float> ecalCandValues;
    std::vector<float> eleValues; 

   
    getHLTProdValues(ecalCandEles[candNr].first,event,ecalCandProdNames_,ecalCandValues);
    getHLTProdValues(ecalCandEles[candNr].second,event,eleProdNames_,eleValues); //can pass a null reference to function, it will deal with it

    std::vector<std::pair<std::string,float> > values;
    mergeValues(ecalCandValues,eleValues,values);


    const reco::RecoEcalCandidateRef& obj = ecalCandEles[candNr].first;
    TLorentzVector p4;
    p4.SetPtEtaPhiM(obj->pt(),obj->eta(),obj->phi(),obj->mass());
    TVector3 caloPos;
    const reco::SuperCluster& sc = *obj->superCluster();
    caloPos.SetXYZ(sc.x(),sc.y(),sc.z());
    
    //  std::cout <<"cand nr "<<candNr<<" sc "<<sc.eta()<<" "<<sc.phi()<<" energy "<<sc.energy()<<std::endl;

    SHTrigEcalCand ecalCand(p4,sc.energy(),sc.preshowerEnergy(),caloPos,isIso);
    ecalCand.setVars(values);

    //   if(ecalCand.var("hadNrgy")<0) std::cout <<" bad candidate, event nr "<<event.id().run()<<" "<<event.id().event()<<std::endl;

    shTrigObjs->addEcalCand(ecalCand);
  }//end loop over cand ele pairs
}


void TrigDebugObjHelper::mergeValues(const std::vector<float> ecalCandValues,const std::vector<float> eleValues,std::vector<std::pair<std::string,float> >& values)const
{
  values.clear();
  
  for(size_t candNr=0;candNr<ecalCandValues.size();candNr++){
    values.push_back(std::make_pair(ecalCandVarNames_[candNr],ecalCandValues[candNr]));
  }
  for(size_t candNr=0;candNr<eleValues.size();candNr++){
    values.push_back(std::make_pair(eleVarNames_[candNr],eleValues[candNr]));
  }

}


void TrigDebugObjHelper::fillEcalCandEles(const edm::Handle<reco::RecoEcalCandidateCollection>& ecalCands,
					  const edm::Handle<reco::ElectronCollection> &eles,
					  std::vector<std::pair<reco::RecoEcalCandidateRef,reco::ElectronRef> >& ecalCandEles)
{
  if(ecalCands.isValid()){

    //  std::cout <<"is valid "<<std::endl;
    typedef reco::RecoEcalCandidateCollection::const_iterator ConstIt;
    for(ConstIt candIt = ecalCands->begin();candIt!=ecalCands->end();++candIt){
      reco::RecoEcalCandidateRef ecalCandRef(ecalCands,candIt- ecalCands->begin());
      ecalCandEles.push_back(std::make_pair(ecalCandRef,getEleRef(eles,*candIt)));
    }
  }
}

reco::ElectronRef TrigDebugObjHelper::getEleRef(const edm::Handle<reco::ElectronCollection>& eles,const reco::RecoEcalCandidate& ecalCand)
{
  if(eles.isValid()){
    typedef reco::ElectronCollection::const_iterator ConstIt;
    for(ConstIt candIt = eles->begin();candIt!=eles->end();++candIt){
      if(candIt->superCluster()==ecalCand.superCluster()){
	reco::ElectronRef eleRef(eles,candIt - eles->begin());
	return eleRef;
      }
    }
  }
  reco::ElectronRef nullRef;
  return nullRef;
}




//input has string of form a:b:c
//varNames is vector of a
//prodNames has pair of edm::InputTags of form pair(b,c)
//optionally b and c can be of format b1@b2 where b1 is the first part of the input tag and b2 is the second
void TrigDebugObjHelper::processInputVecOfProdNames(const std::vector<std::string>& input,std::vector<std::string>& varNames,std::vector<std::pair<edm::InputTag,edm::InputTag> >& prodNames,const std::string& hltTag)
{
  varNames.clear();
  prodNames.clear();
 
  for(size_t entryNr=0;entryNr<input.size();entryNr++){
    std::vector<std::string> splitEntry;
    boost::split(splitEntry,input[entryNr],boost::is_any_of(":"));
    if(splitEntry.size()==3){
      varNames.push_back(splitEntry[0]);
      
      std::vector<std::string> splitE1;
      boost::split(splitE1,splitEntry[1],boost::is_any_of("@"));
      std::vector<std::string> splitE2;
      boost::split(splitE2,splitEntry[2],boost::is_any_of("@"));
      
      std::string proc1;
      if(splitE1.size()==2) proc1=splitE1[1];
      std::string proc2;
      if(splitE2.size()==2) proc2=splitE2[1];
      edm::InputTag tag1(splitE1[0],proc1,hltTag);
      edm::InputTag tag2(splitE2[0],proc2,hltTag);
      // std::cout <<splitEntry[1]<<" "<<splitEntry[2]<<std::endl;
      prodNames.push_back(std::make_pair(tag1,tag2));
    }else{
      std::cout <<"TrigDebugObjHelper::makeVecOfProductNames_ Error entry "<<input[entryNr]<<" has "<<splitEntry.size()<<" fields, not 3 "<<std::endl;
      varNames.push_back(std::string(""));
      prodNames.push_back(std::make_pair(edm::InputTag(),edm::InputTag()));      
    }
  }//end loop over input
  
  
}
