#include "SHarper/TrigNtup/interface/EleStructs.hh"

#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/SHElectron.hh"
#include "SHarper/SHNtupliser/interface/DetIdTools.hh"


#include <string>


void EleStructs::EleSimpleStruct::fill(const SHElectron& ele)
{
  nrgy = ele.nrgy();
  preShowerNrgy = ele.preShowerNrgy();
  et = ele.et();
  clusEt = ele.clusEt();
  trkPt = ele.trkPt();
  eta = ele.eta();
  phi = ele.phi();
  detEta = ele.detEta();
  detPhi = ele.detPhi();
  nrClus = ele.superClus()!=NULL ? ele.superClus()->nrClus() : -999.;
  nrCryst = ele.seedClus()!=NULL ? ele.seedClus()->nrCrys() : -999.;
  q = ele.charge();
  type = ele.type();
  region=ele.region();
  epIn = ele.epIn();
  dEtaIn = ele.dEtaIn();
  dEtaInSeed = ele.dEtaInSeed();
  dPhiIn = ele.dPhiIn();
  hadem = ele.hadem();
  sigmaIEtaIEta = ele.sigmaIEtaIEta();
  e1x5Over5x5 = ele.e1x5Over5x5();
  e2x5Over5x5 = ele.e2x5Over5x5();
  e5x5 = ele.e5x5();
  isolEm = ele.isolEm();
  isolHadDepth1 = ele.isolHadDepth1();
  isolHadDepth2 = ele.isolHadDepth2();
  isolPtTrks = ele.isolPtTrks();
  const SHBasicCluster* seedClus = ele.seedClus();
  if(seedClus){
    eMaxPhi = DetIdTools::isEcalBarrel(seedClus->seedId()) ? DetIdTools::iPhiBarrel(seedClus->seedId()) : DetIdTools::iYEndcap(seedClus->seedId());
    eMaxEta = DetIdTools::isEcalBarrel(seedClus->seedId()) ? DetIdTools::iEtaBarrel(seedClus->seedId()) : DetIdTools::iXEndcap(seedClus->seedId());
  }else{
    eMaxPhi=0;
    eMaxEta=0;
  } 
  dxy = ele.dxy();
  nrMissHits = ele.nrMissingHits(); 
  passPFPreSel=ele.passPFlowPreSel();
  dzTrkVtx = ele.dzTrkVtx();
  z0 = ele.posTrackVtx().Z();
}

void EleStructs::EvtInfoStruct::fill(const SHEvent* event)
{
  runnr = event->runnr();
  lumiSec = event->lumiSec();
  eventnr=event->eventnr();
  bx = event->bx();
  datasetCode = event->datasetCode();
}

void EleStructs::GenPUStruct::fill(const SHEvent& event)
{
  nrPUInt = event.nrPUInteractions();
  nrPUIntPos = event.nrPUInteractionsPos();
  nrPUIntNeg = event.nrPUInteractionsNeg(); 
  nrTruePUInt = event.nrTruePUInteractions();
}

bool EleStructs::EvtInfoStruct::operator<(const EleStructs::EvtInfoStruct& rhs)const
{
  if(runnr<rhs.runnr) return true;
  else if(runnr>rhs.runnr) return false;
  else { //same run nr
    if(lumiSec<rhs.lumiSec) return true;
    else if(lumiSec>rhs.lumiSec) return false;
    else{
      if(eventnr<rhs.eventnr) return true;
      else return false;
    }
  }
}
void EleStructs::HLTEgammaStruct::fill(const SHTrigObj & cand,const std::string& tag)
{
  static const std::string pmProd("hltEgammaPixelMatchVars");

  et=cand.p4().Et();
  nrgy=cand.p4().E();
  eta=cand.eta();
  phi=cand.phi();
  hadem=cand.var("hltEgammaHoverE"+tag);
  sigmaIEtaIEta=cand.var("hltEgammaClusterShape"+tag+"sigmaIEtaIEta5x5");
  dEtaIn=cand.var("hltEgammaGsfTrackVars"+tag+"DetaSeed");
  dPhiIn=cand.var("hltEgammaGsfTrackVars"+tag+"Dphi");
  nrMissHits=cand.var("hltEgammaGsfTrackVars"+tag+"MissingHits");
  pmDPhi1=cand.var(pmProd+tag+"dPhi1");
  pmDPhi2=cand.var(pmProd+tag+"dPhi2");
  pmDPhi3=cand.var(pmProd+tag+"dPhi3");
  pmDPhi4=cand.var(pmProd+tag+"dPhi4");
  pmDPhiInfo1=cand.var(pmProd+tag+"dPhi1Info");
  pmDPhiInfo2=cand.var(pmProd+tag+"dPhi2Info");
  pmDPhiInfo3=cand.var(pmProd+tag+"dPhi3Info");
  pmDPhiInfo4=cand.var(pmProd+tag+"dPhi4Info");
  pmDRZ1=cand.var(pmProd+tag+"dRZ1");
  pmDRZ2=cand.var(pmProd+tag+"dRZ2");
  pmDRZ3=cand.var(pmProd+tag+"dRZ3");
  pmDRZ4=cand.var(pmProd+tag+"dRZ4");
  pmDRZInfo1=cand.var(pmProd+tag+"dRZ1Info");
  pmDRZInfo2=cand.var(pmProd+tag+"dRZ2Info");
  pmDRZInfo3=cand.var(pmProd+tag+"dRZ3Info");
  pmDRZInfo4=cand.var(pmProd+tag+"dRZ4Info");

  nrClus = cand.var(pmProd+tag+"nrClus");
  seedClusEFrac = cand.var(pmProd+tag+"seedClusEFrac");


  // auto convertBackToInt=[](int part1,int part2){
  //   return part1 | (part2<<16);
  // };
  // hit1DetId = convertBackToInt(cand.var(pmProd+tag+"seedHit1DetId1"),cand.var(pmProd+tag+"seedHit1DetId2"));
  // hit2DetId = convertBackToInt(cand.var(pmProd+tag+"seedHit2DetId1"),cand.var(pmProd+tag+"seedHit2DetId2"));
  
}
