#include "SHarper/TrigNtup/interface/EGRegTreeStruct.hh"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"


void EGRegTreeStruct::createBranches(TTree* tree)
{
  tree->Branch("nrVert",&nrVert,"nrVert/I");
  tree->Branch("rho",&rho,"rho/F");
  tree->Branch("nrPUInt",&nrPUInt,"nrPUInt/F");
  tree->Branch("nrPUIntTrue",&nrPUIntTrue,"nrPUIntTrue/F");
  tree->Branch("evt",&evt,evt.contents().c_str());
  tree->Branch("sc",&sc,sc.contents().c_str());
  tree->Branch("ssFull",&ssFull,ssFull.contents().c_str());
  tree->Branch("ssFrac",&ssFrac,ssFrac.contents().c_str());
  tree->Branch("ele",&ele,ele.contents().c_str());
  tree->Branch("pho",&pho,pho.contents().c_str());
  tree->Branch("eleSSFull",&eleSSFull,eleSSFull.contents().c_str());
  tree->Branch("phoSSFull",&phoSSFull,phoSSFull.contents().c_str());  
  tree->Branch("mc",&mc,mc.contents().c_str());
  tree->Branch("clus1",&clus1,clus1.contents().c_str());
  tree->Branch("clus2",&clus2,clus2.contents().c_str());
  tree->Branch("clus3",&clus3,clus3.contents().c_str());
}

void EGRegTreeStruct::setBranchAddresses(TTree* tree)
{
  tree->SetBranchAddress("nrVert",&nrVert);
  tree->SetBranchAddress("rho",&rho);
  tree->SetBranchAddress("nrPUInt",&nrPUInt);
  tree->SetBranchAddress("nrPUIntTrue",&nrPUIntTrue);
  tree->SetBranchAddress("evt",&evt);
  tree->SetBranchAddress("sc",&sc);
  tree->SetBranchAddress("ssFull",&ssFull);
  tree->SetBranchAddress("ssFrac",&ssFrac);
  tree->SetBranchAddress("ele",&ele);
  tree->SetBranchAddress("pho",&pho);
  tree->SetBranchAddress("eleSSFull",&eleSSFull);
  tree->SetBranchAddress("phoSSFull",&phoSSFull);
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

void EGRegTreeStruct::fill(const edm::Event& event,int iNrVert,float iRho,float iNrPUInt,float iNrPUIntTrue,
			   const EcalRecHitCollection& ecalHitsEB,const EcalRecHitCollection& ecalHitsEE,const CaloTopology& topo,const EcalChannelStatus& ecalChanStatus,const reco::SuperCluster* iSC,const reco::GenParticle* iMC,const reco::GsfElectron* iEle,const reco::Photon* iPho,const reco::SuperCluster* scAlt)
{
  clear();

  nrVert = iNrVert;
  rho = iRho;
  nrPUInt = iNrPUInt;
  nrPUIntTrue = iNrPUIntTrue;
  evt.fill(event);
  if(iSC){
    sc.fill(*iSC,ecalChanStatus,scAlt);
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
  if(iEle){
    ele.fill(*iEle);
    eleSSFull.fill(iEle->full5x5_showerShape(),*iEle);
  }
  if(iPho){ 
    pho.fill(*iPho);
    phoSSFull.fill(iPho->full5x5_showerShapeVariables());
  }
    

}

template<typename DetIdType>
int getDeadCrysCode(const DetIdType& id,const EcalChannelStatus& ecalChanStatus)
{
  //constexpr int deadChanMask = EcalChannelStatusCode::kDeadFE | EcalChannelStatusCode::kDeadVFE | EcalChannelStatusCode::kNoDataNoTP | EcalChannelStatusCode::kNonRespondingIsolated;
  constexpr int deadChanMask = EcalChannelStatusCode::kNoDataNoTP;
  
  int crysCode =0 ;

  //meh I could derive this from iEtaOrXNr/iPhiOrYNr indices but sometimes simpliest is best
  int bitNr=0;
  for(int iEtaOrXNr=-1;iEtaOrXNr<=1;iEtaOrXNr++){
    for(int iPhiOrYNr=-1;iPhiOrYNr<=1;iPhiOrYNr++){
      if(iEtaOrXNr==0 && iPhiOrYNr==0) continue;
      DetIdType currId = id.offsetBy(iEtaOrXNr,iPhiOrYNr);
      if(currId.rawId()!=0){
	int statusCode = ecalChanStatus[currId.rawId()].getEncodedStatusCode();
	int bit = 0x1 << bitNr;
	if((statusCode&deadChanMask)!=0) crysCode |=bit;
      }
      bitNr++;
    }
  }
  return crysCode;
}

void SuperClustStruct::fill(const reco::SuperCluster& sc,const EcalChannelStatus& ecalChanStatus,const reco::SuperCluster* altSC)
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
  auto divideWithZeroCheck= [](float numer,float denom)->float{
    return denom!=0 ? numer/denom : 0.;
  };
  scSinTheta = divideWithZeroCheck(sc.position().rho(),sc.position().r());
  seedEta = seedClus.eta();
  seedPhi = seedClus.phi();
  seedSinTheta = divideWithZeroCheck(seedClus.position().rho(),seedClus.position().r());
  dPhiSeedSC = reco::deltaPhi(seedClus.phi(),sc.position().Phi()); //needs this way due to rounding errors
  dEtaSeedSC = seedClus.eta() - sc.position().Eta(); //needs this way due to rounding errors
  numberOfClusters = sc.clusters().size();
  numberOfSubClusters = std::max(0,static_cast<int>(sc.clusters().size())-1);

  if(isEB){
    EBDetId ebDetId(seedClus.seed());
    iEtaOrX = ebDetId.ieta();
    iPhiOrY = ebDetId.iphi();
    const int iEtaCorr = ebDetId.ieta() - (ebDetId.ieta() > 0 ? +1 : -1);
    const int iEtaCorr26 = ebDetId.ieta() - (ebDetId.ieta() > 0 ? +26 : -26);
    iEtaMod5 = iEtaCorr%5;
    //warning bug here!
    iEtaMod20 = std::abs(ebDetId.ieta())<=25 ? iEtaCorr%20 : iEtaCorr26%20;
    iPhiMod2 = (ebDetId.iphi()-1)%2;
    iPhiMod20 = (ebDetId.iphi()-2)%20;
    auto gapCode=[](int iEtaAbs){
      if(iEtaAbs==25 || iEtaAbs==45 || iEtaAbs==65 || iEtaAbs==85) return -1;//before gap
      else if(iEtaAbs==1 || iEtaAbs==26 || iEtaAbs==46 || iEtaAbs==66) return 1;//after gap
      else return 0; //no gap
    };
    etaGapCode = gapCode(ebDetId.ietaAbs());
    phiGapCode = ebDetId.iphi()%20 == 0 ? -1 : ebDetId.iphi()%20 ==1 ? 1 : 0;
    nearbyChanStatus = getDeadCrysCode(ebDetId,ecalChanStatus);
    
  }else{
    EEDetId eeDetId(seedClus.seed());
    iEtaOrX = eeDetId.ix();
    iPhiOrY = eeDetId.iy();
    auto gapCode=[](int iAbs){
      if(iAbs==45 || iAbs==50 || iAbs==55) return -1;//before gap
      else if(iAbs==46 || iAbs==51 || iAbs==56) return 1;//before gap
      else return 0; //no gap
    };
    etaGapCode = gapCode(eeDetId.ix());
    phiGapCode = gapCode(eeDetId.iy());
    nearbyChanStatus = getDeadCrysCode(eeDetId,ecalChanStatus);
  }
  clusterMaxDR     = 999.;
  clusterMaxDRDPhi = 999.;
  clusterMaxDRDEta = 999.;
  clusterMaxDRRawEnergy = 0.;

  float maxDR2 = 0;
  for(auto& clus : sc.clusters()){
    if(clus == sc.seed()) continue;
    float dR2 = reco::deltaR2(seedEta,seedPhi,clus->eta(),clus->phi());
    if(dR2 > maxDR2 ){
      maxDR2 = dR2;
      clusterMaxDR = std::sqrt(dR2);
      clusterMaxDRDPhi = reco::deltaPhi(clus->phi(),seedPhi);
      clusterMaxDRDEta = clus->eta()-seedEta;
      clusterMaxDRRawEnergy = clus->energy();
    }
  }
  if(altSC){
    corrEnergyAlt = altSC->energy();
    rawEnergyAlt = altSC->rawEnergy();
    nrClusAlt = altSC->clusters().size();
  }
}


void EleStruct::fill(const reco::GsfElectron& ele)
{
  et = ele.et();
  energy = ele.energy();
  energyErr = ele.p4Error(reco::GsfElectron::P4_COMBINATION);
  ecalEnergy = ele.ecalEnergy();
  ecalEnergyErr = ele.ecalEnergyError();
  eta = ele.eta();
  phi = ele.phi();
  trkEtaMode = ele.gsfTrack()->etaMode();
  trkPhiMode = ele.gsfTrack()->phiMode();
  trkPMode = ele.gsfTrack()->pMode();
  trkPModeErr = std::abs(ele.gsfTrack()->qoverpModeError())*ele.gsfTrack()->pMode()*ele.gsfTrack()->pMode();
  trkPInn = ele.gsfTrack()->p();
  trkPtInn = ele.gsfTrack()->pt();
  trkPVtx = std::sqrt(ele.trackMomentumAtVtx().Mag2());
  trkPOut = std::sqrt(ele.trackMomentumOut().Mag2());
  trkChi2 = ele.gsfTrack()->chi2();
  trkNDof = ele.gsfTrack()->ndof();
  fbrem = ele.fbrem();
  corrMean = 1.;
  corrSigma = 0.;
  hademTow = ele.hcalOverEcalBc();
  hademCone = ele.hcalOverEcal();
  ecalDrivenSeed = ele.ecalDrivenSeed();
  nrSatCrys = ele.nSaturatedXtals();
  scRawEnergy = ele.superCluster()->rawEnergy();
  scRawESEnergy = ele.superCluster()->preshowerEnergy();
}

void PhoStruct::fill(const reco::Photon& pho)
{
  et = pho.et();
  energy = pho.energy();
  energyErr = pho.getCorrectedEnergyError(reco::Photon::regression2);
  eta = pho.eta();
  phi = pho.phi();
  corrMean = 1.;
  corrSigma = 0.;
  hademTow = pho.hadTowOverEm();
  hademCone = pho.hadronicOverEm();
  nrSatCrys = pho.nSaturatedXtals();
  scRawEnergy = pho.superCluster()->rawEnergy();
  scRawESEnergy = pho.superCluster()->preshowerEnergy();
}

void ShowerShapeStruct::fill(const reco::GsfElectron::ShowerShape& eleSS,const reco::GsfElectron& ele)
{
  e3x3 = eleSS.r9*ele.superCluster()->rawEnergy();
  e5x5 = eleSS.e5x5;
  eMax = eleSS.eMax;
  e2nd = eleSS.e2nd;
  eLeft = eleSS.eLeft;
  eRight = eleSS.eRight;
  const float eLeftRightSum  = eLeft + eRight;
  const float eLeftRightDiff  = eLeft - eRight;
  eLeftRightDiffSumRatio  = eLeftRightSum !=0.f ? eLeftRightDiff/eLeftRightSum : 0.f;
  eTop = eleSS.eTop;
  eBottom = eleSS.eBottom;
  const float eTopBottomSum  = eTop + eBottom;
  const float eTopBottomDiff  = eTop - eBottom;
  eTopBottomDiffSumRatio  = eTopBottomSum !=0.f ? eTopBottomDiff/eTopBottomSum : 0.f;

  e2x5Bottom = eleSS.e2x5Bottom;
  e2x5Top = eleSS.e2x5Top;
  e2x5Left = eleSS.e2x5Left;
  e2x5Right = eleSS.e2x5Right;
  sigmaIEtaIEta = eleSS.sigmaIetaIeta;
  sigmaIEtaIPhi = 0;
  sigmaIPhiIPhi = eleSS.sigmaIphiIphi;
}

void ShowerShapeStruct::fill(const reco::Photon::ShowerShape& phoSS)
{
  e3x3 = phoSS.e3x3;
  e5x5 = phoSS.e5x5;
  eMax = phoSS.maxEnergyXtal;
  e2nd = phoSS.e2nd;
  eLeft = phoSS.eLeft;
  eRight = phoSS.eRight;
  const float eLeftRightSum  = eLeft + eRight;
  const float eLeftRightDiff  = eLeft - eRight;
  eLeftRightDiffSumRatio  = eLeftRightSum !=0.f ? eLeftRightDiff/eLeftRightSum : 0.f;
  eTop = phoSS.eTop;
  eBottom = phoSS.eBottom;
  const float eTopBottomSum  = eTop + eBottom;
  const float eTopBottomDiff  = eTop - eBottom;
  eTopBottomDiffSumRatio  = eTopBottomSum !=0.f ? eTopBottomDiff/eTopBottomSum : 0.f;

  e2x5Bottom = phoSS.e2x5Bottom;
  e2x5Top = phoSS.e2x5Top;
  e2x5Left = phoSS.e2x5Left;
  e2x5Right = phoSS.e2x5Right;
  sigmaIEtaIEta = phoSS.sigmaIetaIeta;
  sigmaIEtaIPhi = 0;
  sigmaIPhiIPhi = phoSS.sigmaIphiIphi;
}
