
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaTowerIsolation.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "TTree.h"


class EGammaFilter : public edm::one::EDFilter<> {

private:
  edm::EDGetTokenT<edm::View<reco::Photon> > phoTag_;
  int nrPhosRequired_;
  float phoEtCut_;
  edm::EDGetTokenT<edm::View<reco::GsfElectron> > eleTag_;
  edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoTag_;
  int nrElesRequired_;
  float eleEtCut_;
  bool requireEcalDriven_;

  int nrSCsRequired_;
  float scEtCut_;

  edm::EDGetTokenT<reco::SuperClusterCollection> superClusEBTag_;
  edm::EDGetTokenT<reco::SuperClusterCollection> superClusEETag_;
  edm::EDGetTokenT<CaloTowerCollection> caloTowerTag_;
  int nrPass_;
  int nrTot_;

  std::pair<float,float> nrPassWeights_;
  std::pair<float,float> nrTotWeights_;
  
public:
  explicit EGammaFilter(const edm::ParameterSet& para);
  ~EGammaFilter(){}
  
  virtual bool filter(edm::Event& event,const edm::EventSetup& setup);
  virtual void endJob();
  bool passPho(edm::Event& event)const;
  bool passEle(edm::Event& event)const; 
  bool passSC(edm::Event& event)const;
};

EGammaFilter::EGammaFilter(const edm::ParameterSet& para):nrPass_(0),nrTot_(0)

{
  eleTag_=consumes<edm::View<reco::GsfElectron> >(para.getParameter<edm::InputTag>("eleTag"));
  phoTag_=consumes<edm::View<reco::Photon> >(para.getParameter<edm::InputTag>("phoTag"));
  superClusEBTag_=consumes<reco::SuperClusterCollection>(para.getParameter<edm::InputTag>("superClusEBTag"));
  superClusEETag_=consumes<reco::SuperClusterCollection>(para.getParameter<edm::InputTag>("superClusEETag"));
  caloTowerTag_=consumes<CaloTowerCollection>(para.getParameter<edm::InputTag>("caloTowerTag"));
  genEvtInfoTag_=consumes<GenEventInfoProduct>(para.getParameter<edm::InputTag>("genEvtInfoTag"));
  nrElesRequired_=para.getParameter<int>("nrElesRequired");
  nrPhosRequired_=para.getParameter<int>("nrPhosRequired"); 
  nrSCsRequired_=para.getParameter<int>("nrSCsRequired");
  phoEtCut_ = para.getParameter<double>("phoEtCut");
  eleEtCut_ = para.getParameter<double>("eleEtCut");
  scEtCut_ = para.getParameter<double>("scEtCut");
  requireEcalDriven_ = para.getParameter<bool>("requireEcalDriven");

}

bool EGammaFilter::filter(edm::Event& event,const edm::EventSetup& setup)
{   
  edm::Handle<GenEventInfoProduct> genEvtInfo;
  event.getByToken(genEvtInfoTag_,genEvtInfo);
  
  float weight = 1;
  if(genEvtInfo.isValid()) weight=genEvtInfo->weight();
  nrTot_++;

  nrTotWeights_.first+=weight;
  nrTotWeights_.second+=weight*weight;

  if(passPho(event) && passEle(event) && passSC(event)){
    //   std::cout <<"passed "<<std::endl;
    nrPass_++;
    nrPassWeights_.first+=weight;
    nrPassWeights_.second+=weight*weight;
    return true;
  }else return false;
  
}
void EGammaFilter::endJob()
{ 
  edm::Service<TFileService> fs;
  if(fs.isAvailable()){
    fs->file().cd();
    //quick and dirty hack as writing ints directly isnt working
    TTree* tree = new TTree("preFilterEventCountTree","Event count");
    tree->Branch("nrPass",&nrPass_,"nrPass/I");
    tree->Branch("nrTot",&nrTot_,"nrTot/I");
    tree->Branch("nrTotWeights",&nrTotWeights_.first,"nrTotWeights/F");
    tree->Branch("nrTotWeightsSQ",&nrTotWeights_.second,"nrTotWeightsSQ/F");
    tree->Branch("nrPassWeights",&nrPassWeights_.first,"nrPassWeights/F");
    tree->Branch("nrPassWeightsSQ",&nrPassWeights_.second,"nrPassWeightsSQ/F");
    
    tree->Fill();
  }
  
}  
bool EGammaFilter::passPho(edm::Event& event)const
{
  if(nrPhosRequired_<=0) return true; //automatically passes
  
  edm::Handle<edm::View<reco::Photon> > phoHandle;
  event.getByToken(phoTag_,phoHandle);
  
  int nrPhos=0;
  for(size_t phoNr=0;phoNr<phoHandle->size();phoNr++){
    const reco::Photon& pho = (*phoHandle)[phoNr];
    if(pho.et()>phoEtCut_) nrPhos++;
  }
  
  return nrPhos>=nrPhosRequired_;
}

bool EGammaFilter::passEle(edm::Event& event)const
{
  if(nrElesRequired_<=0) return true; //automatically passes
  
  edm::Handle<edm::View<reco::GsfElectron> > eleHandle;
  event.getByToken(eleTag_,eleHandle);
  
  int nrEles=0;
  for(size_t eleNr=0;eleNr<eleHandle->size();eleNr++){
    const reco::GsfElectron& ele = (*eleHandle)[eleNr];
    if(requireEcalDriven_) {
      if(ele.et()/ele.energy()*ele.superCluster()->energy()>eleEtCut_ && ele.ecalDrivenSeed()) nrEles++;
    }else{
      if(ele.et()>eleEtCut_) nrEles++;
    }
  } 
    
  return nrEles>=nrElesRequired_;
}

bool EGammaFilter::passSC(edm::Event& event)const
{
  if(nrSCsRequired_<=0) return true; //automatically passes
  
  edm::Handle<reco::SuperClusterCollection> scEBHandle;
  event.getByToken(superClusEBTag_,scEBHandle);
  edm::Handle<reco::SuperClusterCollection> scEEHandle;
  event.getByToken(superClusEETag_,scEEHandle);
  edm::Handle<CaloTowerCollection> caloTowerHandle;
  event.getByToken(caloTowerTag_,caloTowerHandle);

  EgammaTowerIsolation hcalCalc(0.15,0.,0.,-1,caloTowerHandle.product()) ;

  // std::cout <<"new event "<<std::endl;
  int nrSCs=0;
  for(size_t scNr=0;scNr<scEBHandle->size();scNr++){
    const reco::SuperCluster& superClus = (*scEBHandle)[scNr];

    // std::cout <<"barrel sc et "<<superClus.energy()*sin(superClus.position().theta());

    if(superClus.energy()*sin(superClus.position().theta()) > scEtCut_){    
      float hOverE = hcalCalc.getTowerESum(&superClus)/superClus.energy();
      if(hOverE<0.15) nrSCs++;
      //  std::cout <<" H/E "<<hOverE;
    }
    //  std::cout <<std::endl;
  }

  for(size_t scNr=0;scNr<scEEHandle->size();scNr++){
    const reco::SuperCluster& superClus = (*scEEHandle)[scNr]; 
    //std::cout <<"endcap sc et "<<superClus.energy()*sin(superClus.position().theta());

  
    if(superClus.energy()*sin(superClus.position().theta()) > scEtCut_){
      float hOverE = hcalCalc.getTowerESum(&superClus)/superClus.energy();
      if(hOverE<0.15) nrSCs++;  
      //  std::cout <<" H/E "<<hOverE;
    }
    //  std::cout <<std::endl;
  }

  //  std::cout <<"nr SC "<<nrSCs<<" eb "<<scEBHandle->size()<<" ee "<<scEEHandle->size()<<" nrSCRequired "<<nrSCsRequired_<<std::endl;
  return nrSCs >= nrSCsRequired_;

  
}


DEFINE_FWK_MODULE(EGammaFilter);
