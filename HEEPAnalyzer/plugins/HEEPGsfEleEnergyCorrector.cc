
#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleEnergyCorrector.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"

HEEPGsfEleEnergyCorrector::HEEPGsfEleEnergyCorrector(const edm::ParameterSet& iPara)
{
  inputEleTag_ = iPara.getParameter<edm::InputTag>("inputEleTag");
  produces < reco::GsfElectronCollection >();
}


void HEEPGsfEleEnergyCorrector::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{
  
  edm::Handle<reco::GsfElectronCollection> eleHandle; 
  iEvent.getByLabel(inputEleTag_,eleHandle);
  const reco::GsfElectronCollection& eles = *(eleHandle.product());
  

  std::auto_ptr<reco::GsfElectronCollection> outEles(new reco::GsfElectronCollection);

  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    reco::GsfElectron ele(eles[eleNr]);
    if(ele.isEB()){
      float rawEt = ele.superCluster()->rawEnergy()*sin(ele.superCluster()->position().Theta());
      ele.setCorrectedEcalEnergy(ele.correctedEcalEnergy()*fEtCorr(rawEt,ele.classification()));
    }
    outEles->push_back(ele);
  }

  iEvent.put(outEles);
}
    
float HEEPGsfEleEnergyCorrector::fEtCorr(float et,int type)
{
  
  float par[5][5] =
     {
       { 0.974507, 1.16569, -0.000884133, 0.161423, -125.356 },
       { 0.974507, 1.16569, -0.000884133, 0.161423, -125.356 },
       { 0.96449, 0.991457, 0.000237869, 0.159983, -4.38755 },
       { 0.97956, 0.883959, 0.000782834, -0.106388, -124.394 },
       { 0.97213, 0.999528, 5.61192e-06, 0.0143269, -17.1776 }
     } ;
  if ( et > 200 )  et =200 ; 
  if ( et < 5 ) return 1. ; 
  if ( 5 <= et && et < 10 ) return par[type][0] ; 
  if ( 10 <= et && et <= 200 )  return (par[type][1]  + et*par[type][2])*(1- par[type][3]*exp(et/par[type][4])) ; 
  
  return 1.; //should never get here...

}

//define this as a plug-in
DEFINE_FWK_MODULE(HEEPGsfEleEnergyCorrector);
