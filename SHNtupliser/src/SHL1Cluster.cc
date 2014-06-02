#include "SHarper/SHNtupliser/interface/SHL1Cluster.hh"

#include "SHarper/SHNtupliser/interface/DetIdTools.hh"

#include "TGraph.h"

ClassImp(SHL1Cluster)

SHL1Cluster::SHL1Cluster():
  etTS_(-999),
  emEtOverTotEt_(0),
  iEta_(0),iPhi_(0),
  isolEmEtTS_(0),isolHadEtTS_(0),
  trigScale_(0),
  fgVeto_(false),
  isIsolated_(false),
  p4_(),
  leadTower_(-1),
  constituents_(0),
  hademTS_(0),
  clusterFlags_(0),
  etSeedTS_(0)
{

}

int SHL1Cluster::iEtaMax()const
{
  if((leadTower_&0x1)==0x0) return iEta_;
  else{
    DetIdTools::CaloNavigator nav(DetIdTools::makeL1CaloDetId(iEta_,iPhi_),true);
    return DetIdTools::iEtaCalo(nav.getIdAtPos(1,0));
  } 
}
int SHL1Cluster::iPhiMax()const
{
  if((leadTower_&0x2)==0x0) return iPhi_;
  else{
    DetIdTools::CaloNavigator nav(DetIdTools::makeL1CaloDetId(iEta_,iPhi_),true);
    return DetIdTools::iPhiCalo(nav.getIdAtPos(0,1));
  } 
}


std::ostream& SHL1Cluster::print(std::ostream& output)const
{
  output <<" etTS "<<etTS()<<" iEta "<<iEta()<<" iPhi "<<iPhi()<<" lead tower "<<leadTower_<<" emEtOverTotEt "<<emEtOverTotEt()<<" isolEmEtTS "<<isolEmEtTS()<<" isolHadEtTS "<<isolHadEtTS()<<" fg "<<fgVeto()<<" isIsolated "<<isIsolated()<<" et "<<p4().Et()<<" nrgy "<<p4().E()<<" eta "<<p4().Eta()<<" phi "<<p4().Phi();
  return output;


}

std::ostream& operator <<(std::ostream& output,const SHL1Cluster &clus)
{
  return clus.print(output);
}

float SHL1Cluster::l1ClusCalibration(float eta)
{
  float absEta = fabs(eta);
  if(absEta>2.6) return 1;
  
  static TGraph* calibGraph=0;
  if(calibGraph==0){
    double etaPoints[]={0.125, 0.375, 0.625, 0.875, 1.125, 1.3645, 1.6145, 1.875, 2.125, 2.375};
    double calibFactors[]={0.0952467, 0.101389, 0.10598, 0.12605, 0.162749, 0.193123, 0.249227, 0.2800289, 0.271548, 0.27855};
    calibGraph=  new TGraph(10,etaPoints,calibFactors);
  }
  float factor = calibGraph->Eval(absEta);

  return factor==1 ? 1 : 1./(1.-factor);
 

}


float SHL1Cluster::l1ClusCalibrationOld(float eta)
{
  float absEta = fabs(eta);
  if(absEta>2.6) return 1;
  
  static std::vector<float> barrelCoeffs;
  static std::vector<float> endcapCoeffs;
  static std::vector<float> binCorrs;
  if(barrelCoeffs.empty()){
    barrelCoeffs.push_back(1.112);
    barrelCoeffs.push_back(-0.02623);
    barrelCoeffs.push_back(0.08898);
    endcapCoeffs.push_back(1.506);
    endcapCoeffs.push_back(0.2938);
    endcapCoeffs.push_back(-0.1501);
    binCorrs.push_back(0.980478);
    binCorrs.push_back(0.977497);
    binCorrs.push_back(0.991688);
    binCorrs.push_back(0.974088);
    binCorrs.push_back(0.957387);
    binCorrs.push_back(0.958428);
    binCorrs.push_back(0.946692);
    binCorrs.push_back(1.010764);
    binCorrs.push_back(1.000194);
    binCorrs.push_back(0.876818);
    binCorrs.push_back(0.859533);
    binCorrs.push_back(0.766770);
    binCorrs.push_back(0.852342);
  }

    //  AnaFuncs::makeVecFromInputString(barrelCoeffs,"1.112:-0.02623:0.08898");
    //AnaFuncs::makeVecFromInputString(endcapCoeffs,"1.506:0.2938:-0.1501");
    //AnaFuncs::makeVecFromInputString(binCorrs,"0.980478:0.977497:0.991688:0.974088:0.957387:0.958428:0.946692:1.010764:1.000194:0.876818:0.859533:0.766770:0.852342");
 
  
  std::vector<float>& coeffs = absEta<1.6 ? barrelCoeffs : endcapCoeffs; //L1 code uses 1.6 which is odd but probably harmless, atleast more so than changing things
    
  // int binIndex = absEta*5.0;
  // std::cout <<"binIndex "<<binIndex<<" binCorrs size "<<binCorrs.size()<<std::endl;
  float binCorr = binCorrs.at(int(absEta*5.0));
  
  if(absEta>=1.6) absEta -= 1.6;
  
  float factor = coeffs[0]+coeffs[1]*absEta+coeffs[2]*absEta*absEta;
  return factor*binCorr;
  


}

float SHL1Cluster::eta()const
{
  const float towerEtas[29] = {0,.087,0.174,0.261,0.348,0.435,0.522,0.609,0.696,0.783,0.879,0.957,1.044,1.131,1.218,1.305,1.392,1.479,1.566,1.653,1.740,1.830,1.930,2.0432,2.1732,2.322,2.5,2.650,3.000};
  
  float eta = (towerEtas[abs(iEta())]+towerEtas[abs(iEta())-1])/2;
  float sign = iEta()>0 ? 1 : -1;
  return eta*sign;
}

float SHL1Cluster::phi()const
{
  return (iPhi()-0.5)*5/180*3.14;

}

const TLorentzVector& SHL1Cluster::p4()const
{
  p4Temp_.SetPtEtaPhiM(etTS()*trigScale(),eta(),phi(),0);
  return p4Temp_;
}
