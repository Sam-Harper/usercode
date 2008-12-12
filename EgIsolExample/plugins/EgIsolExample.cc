#include "SHarper/EgIsolExample/interface/EgIsolExample.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h" 


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
  ecalIsolFromDepsTag_ = iPara.getParameter<edm::InputTag>("ecalIsolFromDepsTag");
  hcalIsolFromDepsTag_ = iPara.getParameter<edm::InputTag>("hcalIsolFromDepsTag"); 
  trackIsolFromDepsTag_ = iPara.getParameter<edm::InputTag>("trackIsolFromDepsTag"); 
  hcalDepth1IsolFromDepsTag_ = iPara.getParameter<edm::InputTag>("hcalDepth1IsolFromDepsTag"); 
  hcalDepth2IsolFromDepsTag_ = iPara.getParameter<edm::InputTag>("hcalDepth2IsolFromDepsTag");
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

  //now do the isolation from isol deposits
  ecalIsolFromDepsEBHist_ = fs->make<TH1D>("ecalIsolFromDepsEBHist","Ecal Isolation;Ecal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalIsolFromDepsEBHist_ = fs->make<TH1D>("hcalIsolFromDepsEBHist","Hcal Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.);  
  trackIsolFromDepsEBHist_ = fs->make<TH1D>("trackIsolFromDepsEBHist","Track Isolation;Hcal Isolation (GeV/c);# Events / 0.5 GeV/c",84,-1.,20.); 
  hcalDepth1IsolFromDepsEBHist_ = fs->make<TH1D>("hcalDepth1IsolFromDepsEBHist","Hcal Depth 1 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalDepth2IsolFromDepsEBHist_ = fs->make<TH1D>("hcalDepth2IsolFromDepsEBHist","Hcal Depth 2 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  //endcap hists
  ecalIsolFromDepsEEHist_ = fs->make<TH1D>("ecalIsolFromDepsEEHist","Ecal Isolation;Ecal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalIsolFromDepsEEHist_ = fs->make<TH1D>("hcalIsolFromDepsEEHist","Hcal Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.);  
  trackIsolFromDepsEEHist_ = fs->make<TH1D>("trackIsolFromDepsEEHist","Track Isolation;Track Isolation (GeV/c);# Events / 0.5 GeV/c",84,-1.,20.); 
  hcalDepth1IsolFromDepsEEHist_ = fs->make<TH1D>("hcalDepth1IsolFromDepsEEHist","Hcal Depth 1 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  hcalDepth2IsolFromDepsEEHist_ = fs->make<TH1D>("hcalDepth2IsolFromDepsEEHist","Hcal Depth 2 Isolation;Hcal Isolation (GeV);# Events / 0.5 GeV",84,-1.,20.); 
  
}

void EgIsolExample::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{  

  
  //get our electrons
  edm::Handle<reco::GsfElectronCollection> eleHandle;
  iEvent.getByLabel(electronTag_,eleHandle);
  
  fillIsolHists(iEvent,eleHandle);
  fillIsolFromDepsHists(iEvent,eleHandle);
  
}

void EgIsolExample::fillIsolHists(const edm::Event& iEvent,const edm::Handle<reco::GsfElectronCollection>& eleHandle)
{
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
      hcalDepth2IsolEBHist_->Fill(hcalDepth2IsolMap[eleRef]);
    }else{ 
      ecalIsolEEHist_->Fill(ecalIsolMap[eleRef]);
      hcalIsolEEHist_->Fill(hcalIsolMap[eleRef]);
      trackIsolEEHist_->Fill(trackIsolMap[eleRef]);
      hcalDepth1IsolEEHist_->Fill(hcalDepth1IsolMap[eleRef]);
      hcalDepth2IsolEEHist_->Fill(hcalDepth2IsolMap[eleRef]);

    }//end barrel/endcap check
  }//end loop over all electrons
}

void EgIsolExample::fillIsolFromDepsHists(const edm::Event& iEvent,const edm::Handle<reco::GsfElectronCollection>& eleHandle)
{
  //get our value maps
  const edm::ValueMap<double>&  ecalIsolFromDepsMap = getValueMap(iEvent,ecalIsolFromDepsTag_);
  const edm::ValueMap<double>&  hcalIsolFromDepsMap = getValueMap(iEvent,hcalIsolFromDepsTag_);
  const edm::ValueMap<double>&  trackIsolFromDepsMap = getValueMap(iEvent,trackIsolFromDepsTag_);
  const edm::ValueMap<double>&  hcalDepth1IsolFromDepsMap= getValueMap(iEvent,hcalDepth1IsolFromDepsTag_);
  const edm::ValueMap<double>&  hcalDepth2IsolFromDepsMap= getValueMap(iEvent,hcalDepth2IsolFromDepsTag_);

  //loop over all the electrons and fill the hists
  //in practice one would place cuts and do duplicate removal
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    const reco::GsfElectronRef eleRef(eleHandle,eleNr); //we create an electron ref so we can use the valuemap
    if(eleRef->classification()<100){//barrel
      ecalIsolFromDepsEBHist_->Fill(ecalIsolFromDepsMap[eleRef]);
      hcalIsolFromDepsEBHist_->Fill(hcalIsolFromDepsMap[eleRef]);
      trackIsolFromDepsEBHist_->Fill(trackIsolFromDepsMap[eleRef]);
      hcalDepth1IsolFromDepsEBHist_->Fill(hcalDepth1IsolFromDepsMap[eleRef]);
      hcalDepth2IsolFromDepsEBHist_->Fill(hcalDepth2IsolFromDepsMap[eleRef]);
    }else{ 
      ecalIsolFromDepsEEHist_->Fill(ecalIsolFromDepsMap[eleRef]);
      hcalIsolFromDepsEEHist_->Fill(hcalIsolFromDepsMap[eleRef]);
      trackIsolFromDepsEEHist_->Fill(trackIsolFromDepsMap[eleRef]);
      hcalDepth1IsolFromDepsEEHist_->Fill(hcalDepth1IsolFromDepsMap[eleRef]);
      hcalDepth2IsolFromDepsEEHist_->Fill(hcalDepth2IsolFromDepsMap[eleRef]);
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
