#include "SHarper/TrigNtup/interface/TrigBitsDef.hh"
#include "SHarper/TrigNtup/interface/TrigFuncs.hh"
#include "SHarper/SHNtupliser/interface/SHEvent.hh"
#include "SHarper/SHNtupliser/interface/LogErr.hh"


std::vector<std::string> TrigBitsDef::trigPathNames_;
std::vector<std::string> TrigBitsDef::trigFilterNames_;

std::array<int,TrigBitsDef::kNrPathWords> 
TrigBitsDef::getTrigCode(const SHEvent& event)
{
  if(trigPathNames_.empty()) setTrigNames();

  std::array<int,TrigBitsDef::kNrPathWords> trigCode;
  trigCode.fill(0x0);

  for(size_t trigNr=0;trigNr<trigPathNames_.size();trigNr++){
    if(trigPathNames_[trigNr].empty())continue;
    auto wordAndBitNr = getWordAndBitNr(trigNr);
    if(wordAndBitNr.first>=kNrPathWords) outOfBoundsErr(LogErrStr);
    int trigBit = 0x1 << wordAndBitNr.second;
    
    if(event.getTrigSum().getTrig(trigPathNames_[trigNr]).accept()) trigCode[wordAndBitNr.first] |=trigBit;
  } 

  return trigCode;
}

std::array<int,TrigBitsDef::kNrFilterWords> 
TrigBitsDef::getTrigCode(const SHEvent& event,double detEta,double detPhi,double eta,double phi)
{
  if(trigFilterNames_.empty()) setTrigNames();
  

  std::array<int,TrigBitsDef::kNrFilterWords> trigCode;
  trigCode.fill(0x0);

  const SHTrigSummary& trigSum = event.getTrigSum();
  const SHBitsDef& bitsDef = trigSum.filterBitsDef();
  const float maxDR=SHTrigObj::kStdMaxDR;
  
  std::vector<const SHTrigObj*> matchedHLT = trigSum.getTrigObjs(detEta,detPhi,SHTrigObj::HLT ^ SHTrigObj::ELECTRON,maxDR); 
  std::vector<const SHTrigObj*> matchedElectron = trigSum.getTrigObjs(eta,phi,SHTrigObj::ELECTRON,maxDR);
  std::vector<const SHTrigObj*> matchedL1 = trigSum.getTrigObjs(detEta,detPhi,SHTrigObj::L1 | SHTrigObj::UNDEFINED,0.2);
  

  for(size_t trigNr=0;trigNr<trigFilterNames_.size();trigNr++){
    if(trigFilterNames_[trigNr].empty()) continue;

    auto wordAndBitNr = getWordAndBitNr(trigNr);
    if(wordAndBitNr.first>=kNrFilterWords) outOfBoundsErr(LogErrStr);

    int trigBit = 0x1 << wordAndBitNr.second;
    size_t trigObjBitNr=bitsDef.getBitNr(trigFilterNames_[trigNr]);
    bool pass=false;
  
    //    std::cout <<"trig bit "<<trigBit<<" obj bit nr "<<trigObjBitNr<<" filter "<<trigFilterNames_[trigNr]<<std::endl;

    for(auto trigObj : matchedHLT) if(trigObj->pass(trigObjBitNr)) pass=true;
    for(auto trigObj : matchedElectron) if(trigObj->pass(trigObjBitNr)) pass=true;
    for(auto trigObj : matchedL1) if(trigObj->pass(trigObjBitNr)) pass=true;
    
    if(pass) trigCode[wordAndBitNr.first] |=trigBit;

  }

  return trigCode;
}


std::array<int,TrigBitsDef::kNrFilterWords> 
TrigBitsDef::getTrigCode(const SHTrigObj& trigObj,const SHBitsDef& bitsDef)
{
  std::array<int,TrigBitsDef::kNrFilterWords> trigCode;
  trigCode.fill(0x0);
  for(size_t trigNr=0;trigNr<trigFilterNames_.size();trigNr++){
    if(trigFilterNames_[trigNr].empty()) continue;

    auto wordAndBitNr = getWordAndBitNr(trigNr);
    if(wordAndBitNr.first>=kNrFilterWords) outOfBoundsErr(LogErrStr);
    int trigBit = 0x1 << wordAndBitNr.second;
    size_t trigObjBitNr=bitsDef.getBitNr(trigFilterNames_[trigNr]);    
    if(trigObj.pass(trigObjBitNr)) trigCode[wordAndBitNr.first] |=trigBit;
  }
  return trigCode;
}

TLorentzVector TrigBitsDef::getTrigP4(const SHEvent& event,double detEta,double detPhi)
{
  if(trigFilterNames_.empty()) setTrigNames();
  
  const SHTrigSummary& trigSum = event.getTrigSum();
  TBits bits = trigSum.filterBitsDef().getBits(trigFilterNames_);
  
  TLorentzVector bestP4;
  bestP4.SetPtEtaPhiM(0.001,0,0,0);
  const float maxDR=0.1;  
  std::vector<const SHTrigObj*> matchedEgamma = trigSum.getTrigObjs(detEta,detPhi,SHTrigObj::EGAMMA,maxDR);
  
  for(auto trigObj : matchedEgamma){
    if(trigObj->pass(bits)){
      bestP4.SetPtEtaPhiM(trigObj->pt(),trigObj->eta(),trigObj->phi(),0);
      break;
    }
  }
  return bestP4;
}


void TrigBitsDef::outOfBoundsErr(const std::string& funcStr)
{
  std::cout <<funcStr<<" has index out of bounds, terminating"<<std::endl;
  exit(0);
}


void TrigBitsDef::setTrigNamesRun2016WPLoose()
{
  setTrigNamesRun2016();
  trigFilterNames_.clear();
  trigFilterNames_.push_back("hltEGL1SingleEG40OR35OR30OR25OR20ORDoubleEG2210ORSingleJet200Filter");
  trigFilterNames_.push_back("hltEG33EtFilter");
  trigFilterNames_.push_back("hltDiEG33EtUnseededFilter");
  trigFilterNames_.push_back("hltL1sL1SingleEG40");
  trigFilterNames_.push_back("hltEle33CaloIdLPixelMatchFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLGsfTrkIdVLDPhiFilter");
  trigFilterNames_.push_back("hltEG33CaloIdLMWPMS2Filter");
  trigFilterNames_.push_back("hltEle33CaloIdLGsfTrkIdVLMWPMS2Filter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLPixelMatchUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLMWPMS2UnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLGsfTrkIdVLMWPMS2UnseededFilter");

  trigFilterNames_.push_back("hltEG85HEFilter");
  trigFilterNames_.push_back("hltDiEG85HEUnseededFilter");
  trigFilterNames_.push_back("hltEGL1SingleEG40ORSingleIsoEG22erOrSingleIsoEG24erORSingleIsoEG24OrSingleIsoEG26Filter");
  trigFilterNames_.push_back("hltEG27L1IsoEG22erORIsoEG24erORIsoEG24ORIsoEG26OREG40EtFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseClusterShapeFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseHEFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseEcalIsoFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseHcalIsoFilter");
  trigFilterNames_.push_back("hltEle27noerWPLoosePixelMatchFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseGsfOneOEMinusOneOPFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseGsfChi2Filter");
  trigFilterNames_.push_back("hltEle27noerWPLooseGsfMissingHitsFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseGsfDetaFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseGsfDphiFilter");
  trigFilterNames_.push_back("hltEle27noerWPLooseGsfTrackIsoFilter");
 

  
}

void TrigBitsDef::setTrigNames2012()
{
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v");
  trigPathNames_.push_back("HLT_DoublePhoton70_v");
  trigPathNames_.push_back("HLT_DoublePhoton80_v");
  trigPathNames_.push_back("HLT_Photon30_CaloIdVL_v");  
  trigPathNames_.push_back("HLT_Photon50_CaloIdVL_v"); 
  trigPathNames_.push_back("HLT_Photon75_CaloIdVL_v"); 
  trigPathNames_.push_back("HLT_Photon90_CaloIdVL_v"); 
  trigPathNames_.push_back("HLT_Photon125_v");
  trigPathNames_.push_back("HLT_Photon135_v");
  trigPathNames_.push_back("HLT_Photon150_v");
  trigPathNames_.push_back("HLT_Photon160_v");
  trigPathNames_.push_back("HLT_Ele32_CaloIdT_CaloIsoT_TrkIdT_TrkIsoT_SC17_Mass50_v");
  trigPathNames_.push_back("HLT_Ele32_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdT_v");
  trigPathNames_.push_back("HLT_Photon26_Photon18_v");
  trigPathNames_.push_back("HLT_Photon36_Photon22_v");
  trigPathNames_.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
  trigPathNames_.push_back("HLT_Jet370_NoJetID_v");
  trigPathNames_.push_back("HLT_Jet400_NoJetID_v");
  trigPathNames_.push_back("HLT_HT750_v");
  
  trigFilterNames_.push_back("hltEG70HEFilter");
  trigFilterNames_.push_back("hltDoubleEG70HEDoubleFilter");
  trigFilterNames_.push_back("hltEG80HEFilter");
  trigFilterNames_.push_back("hltDoubleEG80HEDoubleFilter");
  trigFilterNames_.push_back("hltEG30CaloIdVLHEFilter");
  trigFilterNames_.push_back("hltPhoton50CaloIdVLHEFilter");
  trigFilterNames_.push_back("hltPhoton75CaloIdVLHEFilter");
  trigFilterNames_.push_back("hltPhoton90CaloIdVLHEFilter");
  trigFilterNames_.push_back("hltPhoton135HEFilter");
  trigFilterNames_.push_back("hltPhoton150HEFilter");
  trigFilterNames_.push_back("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTSC17TrackIsoFilter"); 
  trigFilterNames_.push_back("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTSC17PMMassFilter");
  trigFilterNames_.push_back("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTTrackIsoFilter"); 
  trigFilterNames_.push_back("hltPhoton26Photon18EgammaLHEFilter");
  trigFilterNames_.push_back("hltPhoton26Photon18EgammaLHELastFilter");
  trigFilterNames_.push_back("hltEG36HE10LastFilter");
  trigFilterNames_.push_back("hltEG22HE10DoubleLastFilterUnseeded");
  trigFilterNames_.push_back("hltEle33CaloIdLPixelMatchFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLPixelMatchDoubleFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLGsfTrkIdVLDPhiDoubleFilter");
  trigFilterNames_.push_back("hltEle33CaloIdTPixelMatchFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdTPixelMatchDoubleFilter");  
  trigFilterNames_.push_back("hltEle80CaloIdVTTrkIdTDphiFilter");
  trigFilterNames_.push_back("hltEle80CaloIdVTGsfTrkIdTDPhiFilter");
  trigFilterNames_.push_back("hltL1sL1SingleEG22");
  trigFilterNames_.push_back("hltEle27WP80TrackIsoFilter");
  trigFilterNames_.push_back("l1UTC2015Iso");
  trigFilterNames_.push_back("l1UTC2015Relaxed");
  trigFilterNames_.push_back("hltSingleJet370RegionalNoJetID");
  //trigFilterNames_.push_back("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTSC17TrackIsoFilterNewNrgy"); 
  //trigFilterNames_.push_back("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTSC17PMMassFilterNewNrgy");
  //trigFilterNames_.push_back("hltEle32CaloIdTCaloIsoTTrkIdTTrkIsoTTrackIsoFilterNewNrgy"); 


  
}
void TrigBitsDef::setTrigNames()
{
  setTrigNamesOpen();
}

void TrigBitsDef::setTrigNamesDebug()
{
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_MW_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v");
  trigFilterNames_.push_back("hltEG33EtFilter");
  trigFilterNames_.push_back("hltEG33HEFilter");
  trigFilterNames_.push_back("hltEG33CaloIdLClusterShapeFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLPixelMatchFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLPixelMatchS2Filter"); 
  trigFilterNames_.push_back("hltEle33CaloIdLNewPixelMatchFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLGsfTrkIdVLDEtaFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLGsfTrkIdVLDPhiFilter");
  trigFilterNames_.push_back("hltDoubleEG33EtDoubleFilter");
  trigFilterNames_.push_back("hltDoubleEG33HEDoubleFilter");
  trigFilterNames_.push_back("hltDoubleEG33CaloIdLClusterShapeDoubleFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLPixelMatchUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLNewPixelMatchUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLPixelMatchS2UnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLGsfTrkIdVLDEtaUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter");
  trigFilterNames_.push_back("hltEle27WP80NewPixelMatchFilter");
  trigFilterNames_.push_back("hltEle27WP80GsfTrackIsoFilter");
  trigFilterNames_.push_back("hltEle80CaloIdVTGsfTrkIdTGsfDphiFilter");
  trigFilterNames_.push_back("hltEle80CaloIdVTNewPixelMatchFilter");
  trigFilterNames_.push_back("hltEG175HEFilter");

}
  
void TrigBitsDef::setTrigNamesPHYS14()
{
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v");
  trigPathNames_.push_back("HLT_DoublePhoton85_v");
  trigPathNames_.push_back("HLT_Photon175_v");
  trigPathNames_.push_back("HLT_ECALHT800_v");
  trigPathNames_.push_back("HLT_CaloJet500_NoJetID_v");
  trigPathNames_.push_back("HLT_Ele32_eta2p1_WP75_Gsf_v");
  trigPathNames_.push_back("HLT_Ele25_WP60_SC4_Mass55");
 
  trigFilterNames_.push_back("hltL1sL1SingleEG40ORL1SingleEG35ORL1DoubleEG2210ORL1DoubleEG1510");
  trigFilterNames_.push_back("hltEle33CaloIdLPixelMatchFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLGsfTrkIdVLDPhiFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLNewPixelMatchFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLPixelMatchUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLNewPixelMatchUnseededFilter");
  trigFilterNames_.push_back("hltL1sL1SingleEG40");
  trigFilterNames_.push_back("hltEG85HEFilter");
  trigFilterNames_.push_back("hltDiEG85HEUnseededFilter");
  trigFilterNames_.push_back("hltEG175HEFilter");
  trigFilterNames_.push_back("hltSingleCaloJet500");
  trigFilterNames_.push_back("hltL1sL1SingleIsoEG30er");
  trigFilterNames_.push_back("hltEle32WP75GsfTrackIsoFilter");
  trigFilterNames_.push_back("hltL1sL1SingleEG20");
  trigFilterNames_.push_back("hltEle25WP60SC4TrackIsoFilter");
  trigFilterNames_.push_back("hltEle25WP60SC4EtUnseededFilter");
  trigFilterNames_.push_back("hltEG135HEFilter");
  trigFilterNames_.push_back("hltEG300erEtFilter");
  trigFilterNames_.push_back("hltEG165HE10Filter");
  trigFilterNames_.push_back("hltEle105CaloIdVTGsfTrkIdTGsfDphiFilter");
}
void TrigBitsDef::setTrigNamesRun2015()
{
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v");
  trigPathNames_.push_back("HLT_DoublePhoton85_v");
  trigPathNames_.push_back("HLT_Photon175_v");
  trigPathNames_.push_back("HLT_ECALHT800_v");
  trigPathNames_.push_back("HLT_CaloJet500_NoJetID_v");
  trigPathNames_.push_back("HLT_Ele23_WPLoose_Gsf_v");
  trigPathNames_.push_back("HLT_Ele27_eta2p1_WPLoose_Gsf_v");
  trigPathNames_.push_back("HLT_Ele32_eta2p1_WPLoose_Gsf_v");
  trigPathNames_.push_back("HLT_Ele32_eta2p1_WPTight_Gsf_v");
  trigPathNames_.push_back("HLT_Ele25_WP60_SC4_Mass55");
  trigPathNames_.push_back("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  trigPathNames_.push_back("HLT_Photon22_v");
  trigPathNames_.push_back("HLT_Photon30_v");
  trigPathNames_.push_back("HLT_Photon36_v");
  trigPathNames_.push_back("HLT_Photon50_v"); 
  trigPathNames_.push_back("HLT_Photon75_v"); 
  trigPathNames_.push_back("HLT_Photon90_v"); 
  trigPathNames_.push_back("HLT_Photon120_v"); 
  trigPathNames_.push_back("HLT_Photon175_v"); 
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  trigPathNames_.push_back("HLT_DoublePhoton60_v");

  trigFilterNames_.push_back("hltL1sL1SingleEG40OR35OR30OR25OR20ORL1DoubleEG2210ORL1SingleJet200");
  trigFilterNames_.push_back("hltEle33CaloIdLPixelMatchFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLGsfTrkIdVLDPhiFilter");
  trigFilterNames_.push_back("hltEle33CaloIdLNewPixelMatchFilter");
  trigFilterNames_.push_back("hltDiEG33EtUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLPixelMatchUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter");
  trigFilterNames_.push_back("hltDiEle33CaloIdLNewPixelMatchUnseededFilter");
  trigFilterNames_.push_back("hltL1sL1SingleEG40");
  trigFilterNames_.push_back("hltEG85HEFilter");
  trigFilterNames_.push_back("hltDiEG85HEUnseededFilter");
  trigFilterNames_.push_back("hltSingleCaloJet500");
  trigFilterNames_.push_back("hltL1sL1SingleEG20ORL1SingleEG15");
  trigFilterNames_.push_back("hltEle23WPLooseGsfTrackIsoFilter");
  trigFilterNames_.push_back("hltL1sL1SingleIsoEG22erOrSingleEG25");
  trigFilterNames_.push_back("hltEle27WPLooseGsfTrackIsoFilter");
  trigFilterNames_.push_back("hltL1sL1SingleIsoEG30erOrSingleEG40");
  trigFilterNames_.push_back("hltEle32WPLooseGsfTrackIsoFilter");
  trigFilterNames_.push_back("hltEle32WPTightGsfTrackIsoFilter");
  trigFilterNames_.push_back("hltEG30HEFilter");
  trigFilterNames_.push_back("hltEG36HEFilter");
  trigFilterNames_.push_back("hltEG50HEFilter");
  trigFilterNames_.push_back("hltEG75HEFilter");
  trigFilterNames_.push_back("hltEG90HEFilter");
  trigFilterNames_.push_back("hltEG120HEFilter");
  trigFilterNames_.push_back("hltEG175HEFilter");
  trigFilterNames_.push_back("hltEG300erEtFilter");
  trigFilterNames_.push_back("hltEle105CaloIdVTGsfTrkIdTGsfDphiFilter");
  trigFilterNames_.push_back("hltEle17Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg2Filter");
  trigFilterNames_.push_back("hltEle17Ele12CaloIdLTrackIdLIsoVLDZFilter");
  trigFilterNames_.push_back("hltEle17Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg1Filter");
  trigFilterNames_.push_back("hltL1sL1DoubleEG1510");
}

void TrigBitsDef::setTrigNamesRun2016()
{

  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_MW_v");
  trigPathNames_.push_back("HLT_DoublePhoton60_v");
  trigPathNames_.push_back("HLT_DoublePhoton85_v");
  trigPathNames_.push_back("HLT_Photon300_NoHE_v");
  trigPathNames_.push_back("HLT_ECALHT800_v");
  trigPathNames_.push_back("HLT_CaloJet500_NoJetID_v");
  trigPathNames_.push_back("HLT_Ele23_WPLoose_Gsf_v");
  trigPathNames_.push_back("HLT_Ele27_WPLoose_Gsf_v");
  trigPathNames_.push_back("HLT_Ele27_WPTight_Gsf_v");
  trigPathNames_.push_back("HLT_Ele27_eta2p1_WPLoose_Gsf_v");
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  trigPathNames_.push_back("HLT_Photon22_v");
  trigPathNames_.push_back("HLT_Photon30_v");
  trigPathNames_.push_back("HLT_Photon36_v");
  trigPathNames_.push_back("HLT_Photon50_v"); 
  trigPathNames_.push_back("HLT_Photon75_v"); 
  trigPathNames_.push_back("HLT_Photon90_v"); 
  trigPathNames_.push_back("HLT_Photon120_v"); 
  trigPathNames_.push_back("HLT_Photon175_v"); 
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  trigPathNames_.push_back("HLT_PFJet450_v");
  trigPathNames_.push_back("HLT_PFJet500_v");
  trigPathNames_.push_back("HLT_PFHT900_v");

  //trigFilterNames_.push_back("hltL1sSingleEG40IorSingleEG30IorSingleEG26IorDoubleEG2210IorDoubleEG2417IorSingleJet200");

  trigFilterNames_.assign(32*4,"");
  trigFilterNames_[0] = "hltEGL1SingleEG40OR35OR30OR25OR20ORDoubleEG2210ORSingleJet200Filter";
  trigFilterNames_[1]="hltEG33EtFilter";
  trigFilterNames_[2]="hltDiEG33EtUnseededFilter";
  trigFilterNames_[3]="hltL1sL1SingleEG40";
  trigFilterNames_[4]="hltEle33CaloIdLPixelMatchFilter";
  trigFilterNames_[5]="hltEle33CaloIdLGsfTrkIdVLDPhiFilter";
  trigFilterNames_[6]="hltEG33CaloIdLMWPMS2Filter";
  trigFilterNames_[7]="hltEle33CaloIdLGsfTrkIdVLMWPMS2Filter";
  trigFilterNames_[8]="hltDiEle33CaloIdLPixelMatchUnseededFilter";
  trigFilterNames_[9]="hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter";
  trigFilterNames_[10]="hltDiEle33CaloIdLMWPMS2UnseededFilter";                        
  trigFilterNames_[11]="hltDiEle33CaloIdLGsfTrkIdVLMWPMS2UnseededFilter";

  trigFilterNames_[12]="hltEG27L1IsoEG22erORIsoEG24erORIsoEG24ORIsoEG26OREG40EtFilter";
  trigFilterNames_[13]="hltEle27noerWPLooseClusterShapeFilter";
  trigFilterNames_[14]="hltEle27noerWPLooseHEFilter";
  trigFilterNames_[15]="hltEle27noerWPLooseEcalIsoFilter";
  trigFilterNames_[16]="hltEle27noerWPLooseHcalIsoFilter";
  trigFilterNames_[17]="hltEle27noerWPLoosePixelMatchFilter";
  trigFilterNames_[18]="hltEle27noerWPLooseGsfOneOEMinusOneOPFilter";
  trigFilterNames_[19]="hltEle27noerWPLooseGsfChi2Filter";
  trigFilterNames_[20]="hltEle27noerWPLooseGsfMissingHitsFilter";
  trigFilterNames_[21]="hltEle27noerWPLooseGsfDetaFilter";
  trigFilterNames_[22]="hltEle27noerWPLooseGsfDphiFilter";
  trigFilterNames_[23]="hltEle27noerWPLooseGsfTrackIsoFilter";
  trigFilterNames_[24]="hltSingleCaloJet500";

  trigFilterNames_[32]="hltEG30HEFilter";
  trigFilterNames_[33]="hltEG36HEFilter";
  trigFilterNames_[34]="hltEG50HEFilter";
  trigFilterNames_[35]="hltEG75HEFilter";
  trigFilterNames_[36]="hltEG90HEFilter";
  trigFilterNames_[37]="hltEG120HEFilter";
  trigFilterNames_[38]="hltEG175HEFilter";
  trigFilterNames_[39]="hltEG300erEtFilter";
  trigFilterNames_[40]="hltEG85HEFilter";
  trigFilterNames_[41]="hltDiEG85HEUnseededFilter";

  trigFilterNames_[12+2*32]="hltEGL1SingleEGOrFilter";
  trigFilterNames_[13+2*32]="hltEle27WPTightClusterShapeFilter";
  trigFilterNames_[14+2*32]="hltEle27WPTightHEFilter";
  trigFilterNames_[15+2*32]="hltEle27WPTightEcalIsoFilter";
  trigFilterNames_[16+2*32]="hltEle27WPTightHcalIsoFilter";
  trigFilterNames_[17+2*32]="hltEle27WPTightPixelMatchFilter";
  trigFilterNames_[18+2*32]="hltEle27WPTightGsfOneOEMinusOneOPFilter";
  trigFilterNames_[19+2*32]="hltEle27WPTightGsfChi2Filter";
  trigFilterNames_[20+2*32]="hltEle27WPTightGsfMissingHitsFilter";
  trigFilterNames_[21+2*32]="hltEle27WPTightGsfDetaFilter";
  trigFilterNames_[22+2*32]="hltEle27WPTightGsfDphiFilter";
  trigFilterNames_[23+2*32]="hltEle27WPTightGsfTrackIsoFilter";

  trigFilterNames_[0+3*32]="hltEG33HEFilter";
  trigFilterNames_[1+3*32]="hltEG33CaloIdLClusterShapeFilter";
  trigFilterNames_[2+3*32]="hltEle33CaloIdLPixelMatchMixedLayerPairsFilter";
  trigFilterNames_[3+3*32]="hltEle33CaloIdLPixelMatchTECFilter";
  trigFilterNames_[4+3*32]="hltSinglePFJet450";
  trigFilterNames_[5+3*32]="hltSinglePFJet500";
  trigFilterNames_[5+3*32]="hltPFHT900Jet30";
  
  
  
}

void TrigBitsDef::setTrigNamesRun2017()
{

  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoublePhoton33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_MW_v");
  trigPathNames_.push_back("HLT_DoubleEle5_CaloIdL_MW_v");
  trigPathNames_.push_back("HLT_DoublePhoton70_v");
  trigPathNames_.push_back("HLT_DoublePhoton85_v");
  trigPathNames_.push_back("HLT_Photon300_NoHE_v");
  trigPathNames_.push_back("HLT_ECALHT800_v");
  trigPathNames_.push_back("HLT_CaloJet500_NoJetID_v");
  trigPathNames_.push_back("HLT_CaloJet550_NoJetID_v");
  trigPathNames_.push_back("HLT_Ele27_WPTight_Gsf_v");
  trigPathNames_.push_back("HLT_Ele27_WPTight_Gsf_NoPM_v");
  trigPathNames_.push_back("HLT_Ele32_WPTight_Gsf_v"); 
  trigPathNames_.push_back("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v"); 
  trigPathNames_.push_back("HLT_Photon22_v");
  trigPathNames_.push_back("HLT_Photon33_v");
  trigPathNames_.push_back("HLT_Photon50_v"); 
  trigPathNames_.push_back("HLT_Photon75_v"); 
  trigPathNames_.push_back("HLT_Photon90_v"); 
  trigPathNames_.push_back("HLT_Photon120_v"); 
  trigPathNames_.push_back("HLT_Photon150_v"); 
  trigPathNames_.push_back("HLT_Photon175_v");  
  trigPathNames_.push_back("HLT_Photon200_v");  
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  trigPathNames_.push_back("HLT_DiEle27_WPTightCaloOnly_L1DoubleEG_v");
  
  
 

  //trigFilterNames_.push_back("hltL1sSingleEG40IorSingleEG30IorSingleEG26IorDoubleEG2210IorDoubleEG2417IorSingleJet200");

  trigFilterNames_.assign(32*4,"");
  trigFilterNames_[0]="hltEGL1SingleAndDoubleEGNonIsoOrWithEG26WithJetAndTauFilter";
  trigFilterNames_[1]="hltEG33EtFilter";
  trigFilterNames_[2]="hltEG33HEFilter";
  trigFilterNames_[3]="hltEG33CaloIdLClusterShapeFilter";
  trigFilterNames_[4]="hltEle33CaloIdLPixelMatchFilter";
  trigFilterNames_[5]="hltEle33CaloIdLGsfTrkIdVLDPhiFilter";
  trigFilterNames_[6]="hltEle33CaloIdLMWPMS2Filter";
  trigFilterNames_[7]="hltEle33CaloIdLGsfTrkIdVLMWPMS2Filter";
  trigFilterNames_[8]="hltDiEG33EtUnseededFilter";
  trigFilterNames_[9]="hltDiEG33CaloIdLClusterShapeUnseededFilter";
  trigFilterNames_[10]="hltDiEle33CaloIdLPixelMatchUnseededFilter";
  trigFilterNames_[11]="hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter";
  trigFilterNames_[12]="hltDiEle33CaloIdLMWPMS2UnseededFilter"; 
  trigFilterNames_[13]="hltDiEle33CaloIdLGsfTrkIdVLMWPMS2UnseededFilter";

  trigFilterNames_[14]="hltL1sL1SingleEG40";
  trigFilterNames_[15]="hltEGL1SingleEG5WithJetAndTauFilter";
  trigFilterNames_[16]="hltEG5EtFilter";
  trigFilterNames_[17]="hltEG5HEFilter";
  trigFilterNames_[18]="hltEG5CaloIdLClusterShapeFilter";
  trigFilterNames_[19]="hltEle5CaloIdLPixelMatchFilter";
  trigFilterNames_[20]="hltEle5CaloIdLGsfTrkIdVLDEtaFilter";

  trigFilterNames_[21]="hltEG30EIso15HE30EcalIsoLastFilter";
  trigFilterNames_[22]="hltEG18EIso15HE30EcalIsoUnseededFilter";
  trigFilterNames_[23]="hltEG33CaloIdLMWPMS2Filter";
  trigFilterNames_[24]="hltEG115EtFilter";
  trigFilterNames_[25]="hltEG115CaloIdVTClusterShapeFilter";
  trigFilterNames_[26]="hltEG115CaloIdVTHEFilter";
  trigFilterNames_[27]="hltEle115CaloIdVTPixelMatchFilter";
  trigFilterNames_[28]="hltEle115CaloIdVTGsfTrkIdTGsfDphiFilter";
  
  trigFilterNames_[0+1*32]="hltEG25HEFilter";
  trigFilterNames_[1+1*32]="hltEG33HEFilter";
  trigFilterNames_[2+1*32]="hltEG50HEFilter";
  trigFilterNames_[3+1*32]="hltEG75HEFilter";
  trigFilterNames_[4+1*32]="hltEG90HEFilter";
  trigFilterNames_[5+1*32]="hltEG120HEFilter";
  trigFilterNames_[6+1*32]="hltEG150HEFilter";
  trigFilterNames_[7+1*32]="hltEG175HEFilter"; 
  trigFilterNames_[8+1*32]="hltEG200HEFilter"; 
  trigFilterNames_[9+1*32]="hltEG250erEtFilter";
  trigFilterNames_[10+1*32]="hltEG300erEtFilter";
  trigFilterNames_[11+1*32]="hltEG85HEFilter";
  trigFilterNames_[12+1*32]="hltDiEG85HEUnseededFilter";

  trigFilterNames_[0+2*32]="hltEGL1SingleEGOrFilter";
  trigFilterNames_[1+2*32]="hltEle27WPTightClusterShapeFilter";
  trigFilterNames_[2+2*32]="hltEle27WPTightHEFilter";
  trigFilterNames_[3+2*32]="hltEle27WPTightEcalIsoFilter";
  trigFilterNames_[4+2*32]="hltEle27WPTightHcalIsoFilter";
  trigFilterNames_[5+2*32]="hltEle27WPTightPixelMatchFilter";
  trigFilterNames_[6+2*32]="hltEle27WPTightPMS2Filter";
  trigFilterNames_[7+2*32]="hltEle27WPTightGsfOneOEMinusOneOPFilter";
  trigFilterNames_[8+2*32]="hltEle27WPTightGsfMissingHitsFilter";
  trigFilterNames_[9+2*32]="hltEle27WPTightGsfDetaFilter";
  trigFilterNames_[10+2*32]="hltEle27WPTightGsfDphiFilter";
  trigFilterNames_[11+2*32]="hltEle27WPTightGsfTrackIsoFilter";

  trigFilterNames_[12+2*32]="hltEle27L1SingleAndDoubleEGFilter";
  trigFilterNames_[13+2*32]="hltEle27L1DoubleEGWPTightClusterShapeFilter";
  trigFilterNames_[14+2*32]="hltEle27L1DoubleEGWPTightHEFilter";
  trigFilterNames_[15+2*32]="hltEle27L1DoubleEGWPTightEcalIsoFilter";
  trigFilterNames_[16+2*32]="hltEle27L1DoubleEGWPTightHcalIsoFilter";

  trigFilterNames_[17+2*32]="hltEle32WPTightHcalIsoFilter";
  trigFilterNames_[18+2*32]="hltEle32WPTightPixelMatchFilter";
  trigFilterNames_[19+2*32]="hltEle32WPTightGsfDphiFilter";
  trigFilterNames_[20+2*32]="hltEle32WPTightGsfTrackIsoFilter";
  trigFilterNames_[21+2*32]="hltEle35noerWPTightGsfTrackIsoFilter";

  trigFilterNames_[0+3*32]="hltEGL1SingleAndDoubleEGOrPairFilter";
  trigFilterNames_[1+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEtLeg1Filter";
  trigFilterNames_[2+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEtLeg2Filter";
  trigFilterNames_[3+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLClusterShapeLeg1Filter";
  trigFilterNames_[4+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLClusterShapeLeg2Filter";
  trigFilterNames_[5+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHELeg1Filter";
  trigFilterNames_[6+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHELeg2Filter";
  trigFilterNames_[7+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEcalIsoLeg1Filter";
  trigFilterNames_[8+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEcalIsoLeg2Filter";
  trigFilterNames_[9+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHcalIsoLeg1Filter";
  trigFilterNames_[10+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHcalIsoLeg2Filter";
  trigFilterNames_[11+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLPixelMatchLeg1Filter";
  trigFilterNames_[12+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLPixelMatchLeg2Filter";
  trigFilterNames_[13+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLOneOEMinusOneOPLeg1Filter";
  trigFilterNames_[14+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLOneOEMinusOneOPLeg2Filter";
  trigFilterNames_[15+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDetaLeg1Filter";
  trigFilterNames_[16+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDetaLeg2Filter";
  trigFilterNames_[17+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDphiLeg1Filter";
  trigFilterNames_[18+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDphiLeg2Filter";
  trigFilterNames_[19+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg1Filter";
  trigFilterNames_[20+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDZFilter";
  
}

void TrigBitsDef::setTrigNamesDebug2018()
{

  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoublePhoton33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW_v");
  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_MW_v");
  trigPathNames_.push_back("HLT_DoubleEle5_CaloIdL_MW_v");
  trigPathNames_.push_back("HLT_DoublePhoton70_v");
  trigPathNames_.push_back("HLT_DoublePhoton85_v");
  trigPathNames_.push_back("HLT_Photon300_NoHE_v");
  trigPathNames_.push_back("HLT_ECALHT800_v");
  trigPathNames_.push_back("HLT_CaloJet500_NoJetID_v");
  trigPathNames_.push_back("HLT_CaloJet550_NoJetID_v");
  trigPathNames_.push_back("HLT_Ele27_WPTight_Gsf_v");
  trigPathNames_.push_back("HLT_Ele27_WPTight_Gsf_NoPM_v");
  trigPathNames_.push_back("HLT_Ele32_WPTight_Gsf_v"); 
  trigPathNames_.push_back("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v"); 
  trigPathNames_.push_back("HLT_Photon22_v");
  trigPathNames_.push_back("HLT_Photon33_v");
  trigPathNames_.push_back("HLT_Photon50_v"); 
  trigPathNames_.push_back("HLT_Photon75_v"); 
  trigPathNames_.push_back("HLT_Photon90_v"); 
  trigPathNames_.push_back("HLT_Photon120_v"); 
  trigPathNames_.push_back("HLT_Photon150_v"); 
  trigPathNames_.push_back("HLT_Photon175_v");  
  trigPathNames_.push_back("HLT_Photon200_v");  
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v");
  trigPathNames_.push_back("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v");
  trigPathNames_.push_back("HLT_DiEle27_WPTightCaloOnly_L1DoubleEG_v");
  

  trigFilterNames_.assign(32*4,"");
  trigFilterNames_[0]="hltEGL1SingleAndDoubleEGNonIsoOrWithEG26WithJetAndTauFilter";
  trigFilterNames_[1]="hltEG33EtFilter";
  trigFilterNames_[2]="hltEG33HEFilter";
  trigFilterNames_[3]="hltEG33CaloIdLClusterShapeFilter";
  trigFilterNames_[4]="hltEle33CaloIdLPixelMatchFilter";
  trigFilterNames_[5]="hltEle33CaloIdLGsfTrkIdVLDPhiFilter";
  trigFilterNames_[6]="hltEle33CaloIdLMWPMS2Filter";
  trigFilterNames_[7]="hltEle33CaloIdLGsfTrkIdVLMWPMS2Filter";
  trigFilterNames_[8]="hltDiEG33EtUnseededFilter";
  trigFilterNames_[9]="hltDiEG33CaloIdLClusterShapeUnseededFilter";
  trigFilterNames_[10]="hltDiEle33CaloIdLPixelMatchUnseededFilter";
  trigFilterNames_[11]="hltDiEle33CaloIdLGsfTrkIdVLDPhiUnseededFilter";
  trigFilterNames_[12]="hltDiEle33CaloIdLMWPMS2UnseededFilter"; 
  trigFilterNames_[13]="hltDiEle33CaloIdLGsfTrkIdVLMWPMS2UnseededFilter";

  trigFilterNames_[14]="hltL1sL1SingleEG40";
  trigFilterNames_[15]="hltEGL1SingleEG5WithJetAndTauFilter";
  trigFilterNames_[16]="hltEG5EtFilter";
  trigFilterNames_[17]="hltEG5HEFilter";
  trigFilterNames_[18]="hltEG5CaloIdLClusterShapeFilter";
  trigFilterNames_[19]="hltEle5CaloIdLPixelMatchFilter";
  trigFilterNames_[20]="hltEle5CaloIdLGsfTrkIdVLDEtaFilter";

  trigFilterNames_[21]="hltEG30EIso15HE30EcalIsoLastFilter";
  trigFilterNames_[22]="hltEG18EIso15HE30EcalIsoUnseededFilter";
  trigFilterNames_[23]="hltEG33CaloIdLMWPMS2Filter";
  trigFilterNames_[24]="hltEG115EtFilter";
  trigFilterNames_[25]="hltEG115CaloIdVTClusterShapeFilter";
  trigFilterNames_[26]="hltEG115CaloIdVTHEFilter";
  trigFilterNames_[27]="hltEle115CaloIdVTPixelMatchFilter";
  trigFilterNames_[28]="hltEle115CaloIdVTGsfTrkIdTGsfDphiFilter";
  
  trigFilterNames_[0+1*32]="hltEG25HEFilter";
  trigFilterNames_[1+1*32]="hltEG33HEFilter";
  trigFilterNames_[2+1*32]="hltEG50HEFilter";
  trigFilterNames_[3+1*32]="hltEG75HEFilter";
  trigFilterNames_[4+1*32]="hltEG90HEFilter";
  trigFilterNames_[5+1*32]="hltEG120HEFilter";
  trigFilterNames_[6+1*32]="hltEG150HEFilter";
  trigFilterNames_[7+1*32]="hltEG175HEFilter"; 
  trigFilterNames_[8+1*32]="hltEG200HEFilter"; 
  trigFilterNames_[9+1*32]="hltEG250erEtFilter";
  trigFilterNames_[10+1*32]="hltEG300erEtFilter";
  trigFilterNames_[11+1*32]="hltEG85HEFilter";
  trigFilterNames_[12+1*32]="hltDiEG85HEUnseededFilter";

  trigFilterNames_[0+2*32]="hltEGL1SingleEGOrFilter";
  trigFilterNames_[1+2*32]="hltEle32WPTightClusterShapeFilter";
  trigFilterNames_[2+2*32]="hltEle32WPTightHEFilter";
  trigFilterNames_[3+2*32]="hltEle32WPTightEcalIsoFilter";
  trigFilterNames_[4+2*32]="hltEle32WPTightHcalIsoFilter";
  trigFilterNames_[5+2*32]="hltEle32WPTightPixelMatchFilter";
  trigFilterNames_[6+2*32]="hltEle32WPTightPMS2Filter";
  trigFilterNames_[7+2*32]="hltEle32WPTightGsfOneOEMinusOneOPFilter";
  trigFilterNames_[8+2*32]="hltEle32WPTightGsfMissingHitsFilter";
  trigFilterNames_[9+2*32]="hltEle32WPTightGsfDetaFilter";
  trigFilterNames_[10+2*32]="hltEle32WPTightGsfDphiFilter";
  trigFilterNames_[11+2*32]="hltEle32WPTightGsfTrackIsoFilter";

  trigFilterNames_[12+2*32]="hltEle27WPTightClusterShapeFilter";
  trigFilterNames_[13+2*32]="hltEle27WPTightHEFilter";
  trigFilterNames_[14+2*32]="hltEle27WPTightEcalIsoFilter";
  trigFilterNames_[15+2*32]="hltEle27WPTightHcalIsoFilter";
  trigFilterNames_[16+2*32]="hltEle27WPTightPixelMatchFilter";
  trigFilterNames_[17+2*32]="hltEle27WPTightPMS2Filter";
  trigFilterNames_[18+2*32]="hltEle27WPTightGsfOneOEMinusOneOPFilter";
  trigFilterNames_[19+2*32]="hltEle27WPTightGsfMissingHitsFilter";
  trigFilterNames_[20+2*32]="hltEle27WPTightGsfDetaFilter";
  trigFilterNames_[21+2*32]="hltEle27WPTightGsfDphiFilter";
  trigFilterNames_[22+2*32]="hltEle27WPTightGsfTrackIsoFilter";


  trigFilterNames_[0+3*32]="hltEGL1SingleAndDoubleEGOrPairFilter";
  trigFilterNames_[1+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEtLeg1Filter";
  trigFilterNames_[2+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEtLeg2Filter";
  trigFilterNames_[3+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLClusterShapeLeg1Filter";
  trigFilterNames_[4+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLClusterShapeLeg2Filter";
  trigFilterNames_[5+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHELeg1Filter";
  trigFilterNames_[6+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHELeg2Filter";
  trigFilterNames_[7+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEcalIsoLeg1Filter";
  trigFilterNames_[8+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLEcalIsoLeg2Filter";
  trigFilterNames_[9+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHcalIsoLeg1Filter";
  trigFilterNames_[10+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLHcalIsoLeg2Filter";
  trigFilterNames_[11+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLPixelMatchLeg1Filter";
  trigFilterNames_[12+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLPixelMatchLeg2Filter";
  trigFilterNames_[13+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLOneOEMinusOneOPLeg1Filter";
  trigFilterNames_[14+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLOneOEMinusOneOPLeg2Filter";
  trigFilterNames_[15+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDetaLeg1Filter";
  trigFilterNames_[16+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDetaLeg2Filter";
  trigFilterNames_[17+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDphiLeg1Filter";
  trigFilterNames_[18+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDphiLeg2Filter";
  trigFilterNames_[19+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLTrackIsoLeg1Filter";
  trigFilterNames_[20+3*32]="hltEle23Ele12CaloIdLTrackIdLIsoVLDZFilter";
  
}

void TrigBitsDef::setTrigNamesOpen()
{
  trigPathNames_.clear();
  trigPathNames_.assign(32,"");
  trigFilterNames_.clear();
  trigFilterNames_.assign(32*4,"");
    
  trigPathNames_[0] = "HLT_Ele5_Open_v";
  trigPathNames_[1] = "HLT_Ele5_Open_Unseeded_v";

  trigFilterNames_[0] = "hltEGL1SingleEGOrFilter";
  trigFilterNames_[1] = "hltEG5L1SingleEGOrEtFilter";
  trigFilterNames_[2] = "hltEle5WPTightClusterShapeFilter";
  trigFilterNames_[3] = "hltEle5WPTightHEFilter";
  trigFilterNames_[4] = "hltEle5WPTightEcalIsoFilter";
  trigFilterNames_[5] = "hltEle5WPTightHcalIsoFilter";
  trigFilterNames_[6] = "hltEle5WPTightPixelMatchFilter";
  trigFilterNames_[7] = "hltEle5WPTightPMS2Filter";
  trigFilterNames_[8] = "hltEle5WPTightGsfOneOEMinusOneOPFilter";
  trigFilterNames_[9] = "hltEle5WPTightGsfMissingHitsFilter";
  trigFilterNames_[10] = "hltEle5WPTightGsfDetaFilter";
  trigFilterNames_[11] = "hltEle5WPTightGsfDphiFilter";
  trigFilterNames_[12] = "hltEle5WPTightGsfTrackIsoFilter";
  

  trigFilterNames_[1 + 1*32] = "hltEG5EtUnseededFilter";
  trigFilterNames_[2 + 1*32] = "hltEle5WPTightClusterShapeUnseededFilter";
  trigFilterNames_[3 + 1*32] = "hltEle5WPTightHEUnseededFilter";
  trigFilterNames_[4 + 1*32] = "hltEle5WPTightEcalIsoUnseededFilter";
  trigFilterNames_[5 + 1*32] = "hltEle5WPTightHcalIsoUnseededFilter";
  trigFilterNames_[6 + 1*32] = "hltEle5WPTightPixelMatchUnseededFilter";
  trigFilterNames_[7 + 1*32] = "hltEle5WPTightPMS2UnseededFilter";
  trigFilterNames_[8 + 1*32] = "hltEle5WPTightGsfOneOEMinusOneOPUnseededFilter";
  trigFilterNames_[9 + 1*32] = "hltEle5WPTightGsfMissingHitsUnseededFilter";
  trigFilterNames_[10 + 1*32] = "hltEle5WPTightGsfDetaUnseededFilter";
  trigFilterNames_[11 + 1*32] = "hltEle5WPTightGsfDphiUnseededFilter";
  trigFilterNames_[12 + 1*32] = "hltEle5WPTightGsfTrackIsoUnseededFilter";


}

void TrigBitsDef::write(TDirectory* dir)
{
  dir->WriteObject(&trigPathNames_,"trigPathNames");
  dir->WriteObject(&trigFilterNames_,"trigFilterNames");
}

 
