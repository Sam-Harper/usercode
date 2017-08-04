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

void TrigBitsDef::setTrigNames()
{
  setTrigNamesRun2016();
}


void TrigBitsDef::setTrigNamesRun2016()
{

  trigPathNames_.push_back("HLT_DoubleEle33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DoublePhoton33_CaloIdL_v");
  trigPathNames_.push_back("HLT_DiEle27_WPTightCaloOnly_L1DoubleEG_v");
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

  trigFilterNames_[23]="hltEG115EtFilter";
  trigFilterNames_[24]="hltEG115CaloIdVTClusterShapeFilter";
  trigFilterNames_[25]="hltEG115CaloIdVTClusterShapeFilter";
  trigFilterNames_[26]="hltEle115CaloIdVTPixelMatchFilter";
  trigFilterNames_[27]="hltEle115CaloIdVTGsfTrkIdTGsfDphiFilter";
  
  trigFilterNames_[0+1*32]="hltEG30HEFilter";
  trigFilterNames_[1+1*32]="hltEG36HEFilter";
  trigFilterNames_[2+1*32]="hltEG50HEFilter";
  trigFilterNames_[3+1*32]="hltEG75HEFilter";
  trigFilterNames_[4+1*32]="hltEG90HEFilter";
  trigFilterNames_[5+1*32]="hltEG120HEFilter";
  trigFilterNames_[6+1*32]="hltEG175HEFilter";
  trigFilterNames_[7+1*32]="hltEG300erEtFilter";
  trigFilterNames_[8+1*32]="hltEG85HEFilter";
  trigFilterNames_[9+1*32]="hltDiEG85HEUnseededFilter";

  trigFilterNames_[10+1*32]="hltEG5EtUnseededFilter";
  trigFilterNames_[11+1*32]="hltEG5HEUnseededFilter";
  trigFilterNames_[12+1*32]="hltEG5CaloIdLClusterShapeUnseededFilter";
  trigFilterNames_[13+1*32]="hltEle5CaloIdLPixelMatchUnseededFilter";
  trigFilterNames_[14+1*32]="hltEle5CaloIdLMWPMS2UnseededFilter";



  trigFilterNames_[0+2*32]="hltEGL1SingleEGOrFilter";
  trigFilterNames_[1+2*32]="hltEle27WPTightClusterShapeFilter";
  trigFilterNames_[2+2*32]="hltEle27WPTightHEFilter";
  trigFilterNames_[3+2*32]="hltEle27WPTightEcalIsoFilter";
  trigFilterNames_[4+2*32]="hltEle27WPTightHcalIsoFilter";
  trigFilterNames_[5+2*32]="hltEle27WPTightPixelMatchFilter";
  trigFilterNames_[6+2*32]="hltEle27WPTightGsfOneOEMinusOneOPFilter";
  trigFilterNames_[7+2*32]="hltEle27WPTightGsfChi2Filter";
  trigFilterNames_[8+2*32]="hltEle27WPTightGsfMissingHitsFilter";
  trigFilterNames_[9+2*32]="hltEle27WPTightGsfDetaFilter";
  trigFilterNames_[10+2*32]="hltEle27WPTightGsfDphiFilter";
  trigFilterNames_[11+2*32]="hltEle27WPTightGsfTrackIsoFilter"; 

  trigFilterNames_[12+2*32]="hltEle27L1SingleAndDoubleEGFilter";
  trigFilterNames_[13+2*32]="hltEle27L1DoubleEGWPTightClusterShapeFilter";
  trigFilterNames_[14+2*32]="hltEle27L1DoubleEGWPTightHEFilter";
  trigFilterNames_[15+2*32]="hltEle27L1DoubleEGWPTightEcalIsoFilter";
  trigFilterNames_[16+2*32]="hltEle27L1DoubleEGWPTightHcalIsoFilter";

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



void TrigBitsDef::write(TDirectory* dir)
{
  dir->WriteObject(&trigPathNames_,"trigPathNames");
  dir->WriteObject(&trigFilterNames_,"trigFilterNames");
}

 
