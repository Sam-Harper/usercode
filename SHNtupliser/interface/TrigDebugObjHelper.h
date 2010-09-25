#ifndef SHARPER_SHNTUPLISER_TRIGDEBUGOBJHELPER
#define SHARPER_SHNTUPLISER_TRIGDEBUGOBJHELPER

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateFwd.h"

class SHTrigObjContainer;

class TrigDebugObjHelper {
private:
  edm::InputTag  hltIsoEleProducer_;
  edm::InputTag  hltNonIsoEleProducer_; 
  edm::InputTag  hltIsoEcalCandProducer_;
  edm::InputTag  hltNonIsoEcalCandProducer_;
  std::vector<std::string> ecalCandVarNames_;
  std::vector<std::pair<edm::InputTag,edm::InputTag> > ecalCandProdNames_;
  std::vector<std::string> eleVarNames_;
  std::vector<std::pair<edm::InputTag,edm::InputTag> > eleProdNames_;

  std::string hltTag_; //this is the only place the process name of hlt is picked up from, it overides all others (or should)

public:
  explicit TrigDebugObjHelper(const edm::ParameterSet& iPara);
  ~TrigDebugObjHelper(){}


  void fillDebugTrigObjs(const edm::Event& iEvent,SHTrigObjContainer* shTrigObjs)const;
  void fillDebugTrigObjs(const edm::Event& event,std::vector<std::pair<reco::RecoEcalCandidateRef,reco::ElectronRef> > ecalCandEles, SHTrigObjContainer* shTrigObjs)const;
  void mergeValues(const std::vector<float> ecalCandValues,const std::vector<float> eleValues,std::vector<std::pair<std::string,float> >& values)const;


  template<class T> void getHLTProdValues(const edm::Ref<std::vector<T> > ele,const edm::Event& iEvent,const std::vector<std::pair<edm::InputTag,edm::InputTag> >& prodNames,std::vector<float>& values)const;

  //template<class T> edm::Ref<std::vector<T> > getRefToCand_(double eta,double phi,const edm::Handle<std::vector<T> >& cands);
  
  static reco::ElectronRef getEleRef(const edm::Handle<reco::ElectronCollection>& eles,const reco::RecoEcalCandidate& ecalCand);
  static void fillEcalCandEles(const edm::Handle<reco::RecoEcalCandidateCollection>& ecalCands,
			       const edm::Handle<reco::ElectronCollection> &eles,
			       std::vector<std::pair<reco::RecoEcalCandidateRef,reco::ElectronRef> >& ecalCandEles);
  
  static void processInputVecOfProdNames(const std::vector<std::string>& input,std::vector<std::string>& varNames,std::vector<std::pair<edm::InputTag,edm::InputTag> >& prodNames,const std::string& hltTag);


  void setHLTTag(const std::string hltTag){hltTag_=hltTag;}

};


// template<class T> edm::Ref<std::vector<T> > TrigDebugObjHelper::getRefToCand_(double eta,double phi,const edm::Handle<std::vector<T> >& cands)
// {
//   edm::Ref<std::vector<T> > candRef;
 
//   if(cands.isValid()){ //check we have valid handle, may not be the case if the products werent made (as they may not be in HLT) 
//     double bestDeltaR = kMaxDeltaR_; 
//     for(typename std::vector<T>::const_iterator candIt = cands->begin();candIt!=cands->end();++candIt){     
//       double deltaR = fabs(reco::deltaR(candIt->eta(),candIt->phi(),eta,phi));
//       if(deltaR<bestDeltaR){
// 	bestDeltaR = deltaR;
// 	candRef = edm::Ref<std::vector<T> >(cands,candIt - cands ->begin());
//       }
//     }
//   }
//   return candRef;  
// }

template<class T> void TrigDebugObjHelper::getHLTProdValues(const edm::Ref<std::vector<T> > ele,const edm::Event& iEvent,const std::vector<std::pair<edm::InputTag,edm::InputTag> >& prodNames,std::vector<float>& values)const
{
  values.clear();
  edm::Handle<edm::AssociationMap<edm::OneToValue<std::vector<T>,float > > > isoMap;
  edm::Handle<edm::AssociationMap<edm::OneToValue<std::vector<T>,float > > > nonIsoMap;

  for(size_t prodNr=0;prodNr<prodNames.size();prodNr++){
    values.push_back(-999.); //replaced by correct value if found
   
    if(ele.isNull()) continue;

    //okay horrible hack alert, I want to change the HLT process on the fly
    edm::InputTag isoTag(prodNames[prodNr].first.label(),prodNames[prodNr].first.instance(),hltTag_);
    edm::InputTag nonIsoTag(prodNames[prodNr].second.label(),prodNames[prodNr].second.instance(),hltTag_);

    //iEvent.getByLabel(prodNames[prodNr].first,isoMap);
    //iEvent.getByLabel(prodNames[prodNr].second,nonIsoMap);
    iEvent.getByLabel(isoTag,isoMap);
    iEvent.getByLabel(nonIsoTag,nonIsoMap);
    
    typename edm::AssociationMap<edm::OneToValue<std::vector<T>,float > >::const_iterator mapIt;
   
   

    //lets split this into four cases, isoMap is valid / not valid x nonIsoMap is valid / not valid
    if(isoMap.isValid() && nonIsoMap.isValid()){ //both valid, first check iso map then non iso map for object
      mapIt = (*isoMap).find( ele );
      if(mapIt!=(*isoMap).end()) values[prodNr] = mapIt->val;
      else{
	mapIt = (*nonIsoMap).find( ele ); 
	if(mapIt!=(*nonIsoMap).end()) values[prodNr] = mapIt->val;
      }
    }else if(isoMap.isValid() && !nonIsoMap.isValid()){ //case 2, only iso map is valid
      mapIt = (*isoMap).find(ele);
      if(mapIt!=(*isoMap).end()) values[prodNr] = mapIt->val;
    }else if(!isoMap.isValid() && nonIsoMap.isValid()){ //case 3 only non iso map is valid
      mapIt = (*nonIsoMap).find( ele ); 
      if(mapIt!=(*nonIsoMap).end()) values[prodNr] = mapIt->val;
    }//no need for case 4 where both are invalid
   
  }
}
  


#endif
