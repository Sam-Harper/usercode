#include "SHarper/TrigNtup/interface/EGRegTreeStruct.hh"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"


void EGRegTreeStruct::createBranches(TTree* tree)
{
  tree->Branch("nrVert",&nrVert,"nrVert/I");
  tree->Branch("rho",&rho,"rho/F");
  tree->Branch("evt",&evt,evt.contents().c_str());
  tree->Branch("sc",&sc,sc.contents().c_str());
  tree->Branch("ssFull",&ssFull,ssFull.contents().c_str());
  tree->Branch("ssFrac",&ssFrac,ssFrac.contents().c_str());
  tree->Branch("ele",&ele,ele.contents().c_str());
  tree->Branch("mc",&mc,mc.contents().c_str());
  tree->Branch("clus1",&clus1,clus1.contents().c_str());
  tree->Branch("clus2",&clus2,clus2.contents().c_str());
  tree->Branch("clus3",&clus3,clus3.contents().c_str());
}

void EGRegTreeStruct::setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("nrVert",&nrVert);
  tree->SetBranchAddress("rho",&rho);
  tree->SetBranchAddress("evt",&evt);
  tree->SetBranchAddress("sc",&sc);
  tree->SetBranchAddress("scFull",&ssFull);
  tree->SetBranchAddress("scFrac",&ssFrac);
  tree->SetBranchAddress("ele",&ele);
  tree->SetBranchAddress("mc",&mc);
  tree->SetBranchAddress("clus1",&clus1);
  tree->SetBranchAddress("clus2",&clus2);
  tree->SetBranchAddress("clus3",&clus3);
}

void EvtStruct::fill(const edm::Event& event)
{
  runnr = event.id().run();
  lumiSec = event.luminosityBlock();
  eventnr = event.id().event();
}

void GenInfoStruct::fill(const reco::GenParticle& genPart,float iDR)
{
  energy = genPart.energy();
  pt = genPart.pt();
  eta = genPart.eta();
  phi = genPart.phi();
  pdgId = genPart.pdgId();
  status = genPart.status();
  dR = iDR;
}

void EGRegTreeStruct::fill(const edm::Event& event,int iNrVert,float iRho,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo,const reco::SuperCluster* iSC,const reco::GenParticle* iMC,const reco::GsfElectron* iEle)
{
  clear();

  nrVert = iNrVert;
  rho = iRho,
  evt.fill(event);
  if(iSC){
    sc.fill(*iSC);
    ssFull.fill<true>(*iSC->seed(),ecalHitsEB,ecalHitsEE,topo);
    ssFrac.fill<false>(*iSC->seed(),ecalHitsEB,ecalHitsEE,topo);
    auto fillClus = [&iSC](ClustStruct& clus,size_t index){
      if(index<static_cast<size_t>(iSC->clusters().size())){
	auto iClus = iSC->clusters()[index];
      clus.fill( iClus->energy(),
		 iClus->eta()-iSC->seed()->eta(),
		 reco::deltaPhi(iClus->phi(),iSC->seed()->phi()));
      }else{
	clus.fill( 0, 0, 0 );
      }
    };  
    fillClus(clus1,1);  
    fillClus(clus2,2);  
    fillClus(clus3,3);  
  }
  if(iMC) mc.fill(*iMC, iSC ? std::sqrt(reco::deltaR2(iSC->eta(),iSC->phi(),iMC->eta(),iMC->phi())) : 999);
  if(iEle) ele.fill(*iEle);

}

void SuperClustStruct::fill(const reco::SuperCluster& sc)
{
  auto& seedClus = *sc.seed(); 
  isEB = seedClus.seed().subdetId()==EcalBarrel;
  
  rawEnergy = sc.rawEnergy();
  rawESEnergy = sc.preshowerEnergy();
  etaWidth = sc.etaWidth();
  phiWidth = sc.phiWidth();
  seedClusEnergy = seedClus.energy();
  corrEnergy = sc.energy();
  scEta = sc.eta();
  scPhi = sc.phi();
  seedEta = seedClus.eta();
  seedPhi = seedClus.phi();
  dPhiSeedSC = reco::deltaPhi(seedPhi,scPhi);
  dEtaSeedSC = seedEta-scEta;
  numberOfClusters = sc.clusters().size();
  numberOfSubClusters = std::max(0,static_cast<int>(sc.clusters().size())-1);

  if(isEB){
    EBDetId ebDetId(seedClus.seed());
    iEtaOrX = ebDetId.ieta();
    iPhiOrY = ebDetId.iphi();
    const int iEtaCorr = ebDetId.ieta() - (ebDetId.ieta() > 0 ? +1 : -1);
    const int iEtaCorr26 = ebDetId.ieta() - (ebDetId.ieta() > 0 ? +26 : -26);
    iEtaMod5 = iEtaCorr%5;
    iEtaMod20 = std::abs(ebDetId.ieta()<=25) ? iEtaCorr%20 : iEtaCorr26%20;
    iPhiMod2 = ebDetId.iphi()%2;
    iPhiMod20 = ebDetId.iphi()%20;
  }else{
    EEDetId eeDetId(seedClus.seed());
    iEtaOrX = eeDetId.ix();
    iPhiOrY = eeDetId.iy();
  }
  clusterMaxDR     = 999.;
  clusterMaxDRDPhi = 999.;
  clusterMaxDRDEta = 999.;
  clusterMaxDRRawEnergy = 0.;
  float maxDR2 = 0;
  for(auto& clus : sc.clusters()){
    float dR2 = reco::deltaR2(seedEta,seedPhi,clus->eta(),clus->phi());
    if(dR2 > maxDR2 ){
      maxDR2 = dR2;
      clusterMaxDR = std::sqrt(dR2);
      clusterMaxDRDPhi = reco::deltaPhi(clus->phi(),seedPhi);
      clusterMaxDRDEta = clus->eta()-seedEta;
      clusterMaxDRRawEnergy = clus->energy();
    }
  }
}


void EleStruct::fill(const reco::GsfElectron& ele)
{
  et = ele.et();
  energy = ele.energy();
  ecalEnergy = ele.ecalEnergy();
  eta = ele.eta();
  phi = ele.phi();
  trkEtaMode = ele.gsfTrack()->etaMode();
  trkPhiMode = ele.gsfTrack()->phiMode();
  trkPMode = ele.gsfTrack()->pMode();
  trkPModeErr = std::abs(ele.gsfTrack()->qoverpModeError())*ele.gsfTrack()->pMode()*ele.gsfTrack()->pMode();
  fbrem = ele.fbrem();
  corrMean = 1.;
  corrSigma = 0.;
  hademTow = ele.hcalOverEcalBc();
  hademCone = ele.hcalOverEcal();
}
