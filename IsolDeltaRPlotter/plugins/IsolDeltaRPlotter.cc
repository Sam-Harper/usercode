#include "SHarper/IsolDeltaRPlotter/interface/IsolDeltaRPlotter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "PhysicsTools/Utilities/interface/deltaR.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h" 
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h" 
#include "DataFormats/EgammaReco/interface/SuperCluster.h" 
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
 
#include "TH1D.h"

//here we set up our tags to get the ValueMaps
IsolDeltaRPlotter::IsolDeltaRPlotter(const edm::ParameterSet& iPara)
{
  electronsTag_ = iPara.getParameter<edm::InputTag>("electronsTag");
  superClustersEBTag_ = iPara.getParameter<edm::InputTag>("superClustersEBTag");
  superClustersEETag_ = iPara.getParameter<edm::InputTag>("superClustersEETag");
  caloTowersTag_ = iPara.getParameter<edm::InputTag>("caloTowersTag");
  nrBins_ = iPara.getParameter<int>("nrBins");
  xMin_ = iPara.getParameter<double>("xMin");
  xMax_ = iPara.getParameter<double>("xMax");
  maxDeltaR_ = iPara.getParameter<double>("maxDeltaR");
  minEtaBarrel_ = iPara.getParameter<double>("minEtaBarrel");
  maxEtaBarrel_ = iPara.getParameter<double>("maxEtaBarrel");
  minEtaEndcap_ = iPara.getParameter<double>("minEtaEndcap");
  maxEtaEndcap_ = iPara.getParameter<double>("maxEtaEndcap");
}


//here we will define our histograms
void IsolDeltaRPlotter::beginJob(const edm::EventSetup& iSetup)
{
  edm::Service<TFileService> fs;
 
  eleHcalIsolDeltaREBHist_ = fs->make<TH1D>("eleHcalIsolDeltaREBHist","#DeltaR of Calo Towers to SuperCluster Position;#DeltaR;# Events",nrBins_,xMin_,xMax_);
  eleHcalIsolDeltaREEHist_ = fs->make<TH1D>("eleHcalIsolDeltaREEHist","#DeltaR of Calo Towers to SuperCluster Position;#DeltaR;# Events",nrBins_,xMin_,xMax_);
  scHcalIsolDeltaREBHist_  = fs->make<TH1D>("scHcalIsolDeltaREBHist","#DeltaR of Calo Towers to SuperCluster Position;#DeltaR;# Events",nrBins_,xMin_,xMax_);
  scHcalIsolDeltaREEHist_  = fs->make<TH1D>("scHcalIsolDeltaREEHist","#DeltaR of Calo Towers to SuperCluster Position;#DeltaR;# Events",nrBins_,xMin_,xMax_);
}

void IsolDeltaRPlotter::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{  

  edm::Handle<reco::GsfElectronCollection> elesHandle;
  iEvent.getByLabel(electronsTag_,elesHandle);
  const reco::GsfElectronCollection& eles = *elesHandle.product();
  
  edm::Handle<reco::SuperClusterCollection> superClustersEBHandle;
  iEvent.getByLabel(superClustersEBTag_,superClustersEBHandle);  
  const reco::SuperClusterCollection& superClustersEB = *superClustersEBHandle.product();
  
  edm::Handle<reco::SuperClusterCollection> superClustersEEHandle;
  iEvent.getByLabel(superClustersEETag_,superClustersEEHandle);
  const reco::SuperClusterCollection& superClustersEE = *superClustersEEHandle.product();
  
  edm::Handle<CaloTowerCollection> caloTowersHandle;
  iEvent.getByLabel(caloTowersTag_,caloTowersHandle);

  //loop over all the electrons and fill the hists
  //no quality cuts are applied or dup removal (shouldnt be that big of a deal)
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    double scEtaAbs = fabs(eles[eleNr].superCluster()->eta());
    if(scEtaAbs>=minEtaBarrel_ && scEtaAbs<maxEtaBarrel_) {
      fillHcalIsolDRHist(eleHcalIsolDeltaREBHist_,eles[eleNr].superCluster()->position(),*caloTowersHandle.product(),maxDeltaR_);
    }else if(scEtaAbs>=minEtaEndcap_ && scEtaAbs<maxEtaEndcap_) {
      fillHcalIsolDRHist(eleHcalIsolDeltaREEHist_,eles[eleNr].superCluster()->position(),*caloTowersHandle.product(),maxDeltaR_);
    }//end barrel/endcap check
  }//end loop over all electrons

  //fill the barrel superclusters
  for(size_t scNr=0;scNr<superClustersEB.size();scNr++){
    const reco::SuperCluster& superClus = superClustersEB[scNr];
    double scEtaAbs = fabs(superClus.eta()); 
    
    if(scEtaAbs>=minEtaBarrel_ && scEtaAbs<maxEtaBarrel_ ){
      fillHcalIsolDRHist(scHcalIsolDeltaREBHist_,superClus.position(),*caloTowersHandle.product(),maxDeltaR_);
    }//end barrel check
  }//end loop over barrel super clusters

  //fill the endcap superclusters
  for(size_t scNr=0;scNr<superClustersEE.size();scNr++){
    const reco::SuperCluster& superClus = superClustersEE[scNr];
    double scEtaAbs = fabs(superClus.eta()); 
    if(scEtaAbs>=minEtaEndcap_ && scEtaAbs<maxEtaEndcap_){
      fillHcalIsolDRHist(scHcalIsolDeltaREEHist_,superClus.position(),*caloTowersHandle.product(),maxDeltaR_);
    }//end endcap check
  }//end loop over endcap super clusters

}



void IsolDeltaRPlotter::endJob()
{

}
  
void IsolDeltaRPlotter::fillHcalIsolDRHist(TH1* hist,const math::XYZPoint& centre,const CaloTowerCollection& caloTowers,double maxDeltaR)
{
  
  double centreEta=centre.eta();
  double centrePhi=centre.phi();

  for(size_t towerNr=0;towerNr<caloTowers.size();towerNr++){
    const CaloTower& caloTower = caloTowers[towerNr];
    double towerEta=caloTower.eta();
    double towerPhi=caloTower.phi();
    double deltaR = reco::deltaR(centreEta,centrePhi,towerEta,towerPhi);
    if(deltaR<=maxDeltaR && caloTower.hadEnergy()>0) hist->Fill(deltaR);
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(IsolDeltaRPlotter);
