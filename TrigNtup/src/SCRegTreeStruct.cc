#include "SHarper/TrigNtup/interface/SCRegTreeStruct.hh"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "FWCore/Utilities/interface/isFinite.h"

void SCRegTreeStruct::createBranches(TTree* tree)
{
  tree->Branch("nrVert",&nrVert,"nrVert/I");
  tree->Branch("evt",&evt,evt.contents().c_str());
  tree->Branch("sc",&sc,sc.contents().c_str());
  tree->Branch("mc",&mc,mc.contents().c_str());
  tree->Branch("clus1",&clus1,clus1.contents().c_str());
  tree->Branch("clus2",&clus2,clus2.contents().c_str());
  tree->Branch("clus3",&clus3,clus3.contents().c_str());
}

void SCRegTreeStruct::setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("nrVert",&nrVert);
  tree->SetBranchAddress("evt",&evt);
  tree->SetBranchAddress("sc",&sc);
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

void GenInfo::fill(const reco::GenParticle& genPart,float iDR)
{
  energy = genPart.energy();
  pt = genPart.pt();
  eta = genPart.eta();
  phi = genPart.phi();
  pdgId = genPart.pdgId();
  status = genPart.status();
  dR = iDR;
}

void SCRegTreeStruct::fill(const edm::Event& event,int iNrVert,const reco::SuperCluster& iSC,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo,const reco::GenParticle* iMC,const pat::Electron* ele)
{
  nrVert = iNrVert;
  evt.fill(event);
  sc.fill(iSC,ecalHitsEB,ecalHitsEE,topo,ele!=nullptr);
  if(iMC) mc.fill(*iMC,std::sqrt(reco::deltaR2(iSC.eta(),iSC.phi(),iMC->eta(),iMC->phi())));
  else mc.clear();
  clus1.fill(0,0,0);
  clus2.fill(0,0,0);
  clus3.fill(0,0,0);
    
  auto fillClus = [&iSC](ClustStruct& clus,size_t index){
    if(index<static_cast<size_t>(iSC.clusters().size())){
      auto iClus = iSC.clusters()[index];
      clus.fill( iClus->energy(),//-iSC.seed()->energy(),
		 iClus->eta()-iSC.seed()->eta(),
		 reco::deltaPhi(iClus->phi(),iSC.seed()->phi()));
    }else{
      clus.fill( 0, 0, 0 );
    }
  };  
  fillClus(clus1,1);  
  fillClus(clus2,2);  
  fillClus(clus3,3);  
  
}

void SuperClustStruct::fill(const reco::SuperCluster& sc,const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo,bool iEleMatch)
{
  auto& seedClus = *sc.seed();
  isEB = seedClus.seed().subdetId()==EcalBarrel;
  const EcalRecHitCollection& ecalHits = isEB ? ecalHitsEB : ecalHitsEE;

  rawEnergy = sc.rawEnergy();
  etaWidth = sc.etaWidth();
  phiWidth = sc.phiWidth();
  seedClusEnergy = seedClus.energy();
  corrEnergy74X = sc.energy();
  scEta = sc.eta();
  scPhi = sc.phi();
  seedEta = seedClus.eta();
  seedPhi = seedClus.phi();
  e3x3 = EcalClusterTools::e3x3(seedClus,&ecalHits,&topo);
  eMax = EcalClusterTools::eMax(seedClus,&ecalHits);
  e2nd = EcalClusterTools::e2nd(seedClus,&ecalHits);
  float eLeft = EcalClusterTools::eLeft(seedClus,&ecalHits,&topo);
  float eRight = EcalClusterTools::eRight(seedClus,&ecalHits,&topo);
  float eLeftRightSum  = eLeft + eRight;
  float eLeftRightDiff  = eLeft - eRight;
  eLeftRightDiffSumRatio  = eLeftRightSum !=0. ? eLeftRightDiff/eLeftRightSum : 0.;
  float eTop = EcalClusterTools::eTop(seedClus,&ecalHits,&topo);
  float eBottom = EcalClusterTools::eBottom(seedClus,&ecalHits,&topo);
  float eTopBottomSum  = eTop + eBottom;
  float eTopBottomDiff  = eTop - eBottom;
  eTopBottomDiffSumRatio  = eTopBottomSum !=0. ? eTopBottomDiff/eTopBottomSum : 0.;
  auto localCovs =  EcalClusterTools::localCovariances(seedClus,&ecalHits,&topo);
  sigmaIEtaIEta =std::sqrt(localCovs[0]);
  sigmaIEtaIPhi = std::numeric_limits<float>::max();
  sigmaIPhiIPhi = std::numeric_limits<float>::max();
  if (!edm::isNotFinite(localCovs[2])) sigmaIPhiIPhi = std::sqrt(localCovs[2]) ;

  const bool applySPPBug = true;
  const float seeBySpp = applySPPBug ? sigmaIEtaIEta*std::numeric_limits<float>::max() : sigmaIEtaIEta*sigmaIPhiIPhi;
  
  if(  seeBySpp > 0 ) {
    sigmaIEtaIPhi = localCovs[1] / seeBySpp;
  } else if ( localCovs[1] > 0 ) {
    sigmaIEtaIPhi = 1.f;
  } else {
    sigmaIEtaIPhi = -1.f;
  }

  numberOfClusters = sc.clusters().size();
  numberOfSubClusters = std::max(0,static_cast<int>(sc.clusters().size())-1);

  if(isEB){
    EBDetId ebDetId(seedClus.seed());
    iEtaOrX = ebDetId.ieta();
    iPhiOrY = ebDetId.iphi();
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
  if(sc.clusters().size()>1){
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
  eleMatch = iEleMatch;
}
