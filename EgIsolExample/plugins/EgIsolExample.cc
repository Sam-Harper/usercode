#include "SHarper/EgIsolExample/interface/EgIsolExample.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h" 
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h" 

#include "TH1D.h"

//here we set up our tags to get the ValueMaps
EgIsolExample::EgIsolExample(const edm::ParameterSet& iPara)
{
  electronTag_ = iPara.getParameter<edm::InputTag>("electronTag");
  ecalIsolTag_ = iPara.getParameter<edm::InputTag>("ecalIsolTag");
  hcalIsolTag_ = iPara.getParameter<edm::InputTag>("hcalIsolTag"); 
  trackIsolTag_ = iPara.getParameter<edm::InputTag>("trackIsolTag"); 
  hcalDepth1IsolTag_ = iPara.getParameter<edm::InputTag>("hcalDepth1IsolTag"); 
  hcalDepth2IsolTag_ = iPara.getParameter<edm::InputTag>("hcalDepth2IsolTag");  
}

//here we will define our histograms
void EgIsolExample::beginJob(const edm::EventSetup& iSetup)
{
  edm::Service<TFileService> fs;
  //barrel hists
  ecalIsolEBHist_ = fs->make<TH1D>("ecalIsolEBHist","Ecal Isolation;Ecal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalIsolEBHist_ = fs->make<TH1D>("hcalIsolEBHist","Hcal Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.);  
  trackIsolEBHist_ = fs->make<TH1D>("trackIsolEBHist","Track Isolation;Hcal Isolation (GeV/c);# Events / 0.5 GeV/c",84,-1.,20.); 
  hcalDepth1IsolEBHist_ = fs->make<TH1D>("hcalDepth1IsolEBHist","Hcal Depth 1 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalDepth2IsolEBHist_ = fs->make<TH1D>("hcalDepth2IsolEBHist","Hcal Depth 2 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  //endcap hists
  ecalIsolEEHist_ = fs->make<TH1D>("ecalIsolEEHist","Ecal Isolation;Ecal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalIsolEEHist_ = fs->make<TH1D>("hcalIsolEEHist","Hcal Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.);  
  trackIsolEEHist_ = fs->make<TH1D>("trackIsolEEHist","Track Isolation;Track Isolation (GeV/c);# Events / 0.5 GeV/c",84,-1.,20.); 
  hcalDepth1IsolEEHist_ = fs->make<TH1D>("hcalDepth1IsolEEHist","Hcal Depth 1 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalDepth2IsolEEHist_ = fs->make<TH1D>("hcalDepth2IsolEEHist","Hcal Depth 2 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  
}

void EgIsolExample::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{  

  
  //get our electrons
  edm::Handle<reco::GsfElectronCollection> eleHandle;
  iEvent.getByLabel(electronTag_,eleHandle);
  
  //get our value maps
  const edm::ValueMap<double>&  ecalIsolMap = getValueMap(iEvent,ecalIsolTag_);
  const edm::ValueMap<double>&  hcalIsolMap = getValueMap(iEvent,hcalIsolTag_);
  const edm::ValueMap<double>&  trackIsolMap = getValueMap(iEvent,trackIsolTag_);
  const edm::ValueMap<double>&  hcalDepth1IsolMap= getValueMap(iEvent,hcalDepth1IsolTag_);
  const edm::ValueMap<double>&  hcalDepth2IsolMap= getValueMap(iEvent,hcalDepth2IsolTag_);

  //loop over all the electrons and fill the hists
  //in practice one would place cuts and do duplicate removal
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    const reco::GsfElectronRef eleRef(eleHandle,eleNr); //we create an electron ref so we can use the valuemap
    if(eleRef->classification()<100){//barrel
      ecalIsolEBHist_->Fill(ecalIsolMap[eleRef]);
      hcalIsolEBHist_->Fill(hcalIsolMap[eleRef]);
      trackIsolEBHist_->Fill(trackIsolMap[eleRef]);
      hcalDepth1IsolEBHist_->Fill(hcalDepth1IsolMap[eleRef]);
      hcalDepth2IsolEBHist_->Fill(hcalDepth1IsolMap[eleRef]);
    }else{ 
      ecalIsolEEHist_->Fill(ecalIsolMap[eleRef]);
      hcalIsolEEHist_->Fill(hcalIsolMap[eleRef]);
      trackIsolEEHist_->Fill(trackIsolMap[eleRef]);
      hcalDepth1IsolEEHist_->Fill(hcalDepth1IsolMap[eleRef]);
      hcalDepth2IsolEEHist_->Fill(hcalDepth2IsolMap[eleRef]);
    }//end barrel/endcap check
  }//end loop over all electrons
}



void EgIsolExample::endJob()
{

}

//little labour saving function to get the reference to the ValueMap
const edm::ValueMap<double>& EgIsolExample::getValueMap(const edm::Event& iEvent,edm::InputTag& inputTag)
{
  edm::Handle<edm::ValueMap<double> > handle;
  iEvent.getByLabel(inputTag,handle);
  return *(handle.product());
}

//define this as a plug-in
DEFINE_FWK_MODULE(EgIsolExample);
