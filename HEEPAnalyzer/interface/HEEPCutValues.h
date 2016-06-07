#ifndef SHARPER_HEEPANALYZER_HEEPCUTVALUES
#define SHARPER_HEEPANALYZER_HEEPCUTVALUES

//This is a simple struct to hold the values of a particular set of cuts, eg barrel heep cuts
//Some cuts dont make sense for both barrel and endcap but its easier to just have a common struct


#include <iostream>
#include <string>

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPGsfEleExtra.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"

namespace edm{
  class ParameterSet;
}
  
namespace heep {
  struct EleCutValues  {
  public:
    int cutMask;  //the cuts which we wish to check  
    double minEt;
    double minEta;
    double maxEta;
    double maxDEtaIn;
    double dEtaInConstTerm;
    double dEtaInGradTerm;
    double maxDPhiIn;
    double maxHadem;
    double hademConstTerm;
    double maxSigmaIEtaIEta;
    double minE2x5Over5x5;
    double minE1x5Over5x5;
    double isolEmHadDepth1ConstTerm;
    double isolEmHadDepth1GradTerm;
    double isolEmHadDepth1GradStart;
    double isolEmHadDepth1RhoEA;
    double isolHadDepth2ConstTerm;
    double isolHadDepth2GradTerm;
    double isolHadDepth2GradStart;
    double isolHadDepth2RhoEA;
    double isolPtTrksConstTerm;
    double isolPtTrksGradTerm;
    double isolPtTrksGradStart;
    double isolPtTrksRhoStart; //new for HEPE V6.1
    double isolPtTrksRhoEA;
    double maxIsolPtTrksRel03; // WP80
    double isolPtTrksRelRhoEA;
    double maxIsolEmRel03;     // WP80
    double isolEmRelRhoEA;
    double maxIsolHadRel03;    // WP80
    double isolHadRelRhoEA;
    int maxNrMissHits; 
    float maxDXY; //HEEP V4.1 conversion cut
    
    EleCutValues();
    explicit EleCutValues(const edm::ParameterSet& iConfig);
    EleCutValues& operator=(const edm::ParameterSet& iConfig);
    
    float getEt(const reco::GsfElectron& ele)const{return ele.et();}
      
    bool passEt(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      return getEt(ele)>=minEt;
    }
    bool passDetEta(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float etaAbs = std::abs(ele.superCluster()->eta());
      return etaAbs>= minEta && etaAbs<=maxEta;
    }
    bool passCrack(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      return !ele.isEBEtaGap();
    }
    bool passEcalDriven(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      return ele.ecalDrivenSeed();
    }
    bool passDEtaIn(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float dEtaInAbs = std::abs(ele.deltaEtaSuperClusterTrackAtVtx());
      return dEtaInAbs <= std::max(dEtaInConstTerm+dEtaInGradTerm*getEt(ele),maxDEtaIn);
    }
    bool passDEtaInSeed(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float dEtaInAbs = std::abs(ele.deltaEtaSeedClusterTrackAtVtx());
      return dEtaInAbs <= maxDEtaIn;
    }
    bool passDPhiIn(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float dPhiInAbs = std::abs(ele.deltaPhiSuperClusterTrackAtVtx());
      return dPhiInAbs <= maxDPhiIn;
    }
    bool passHadem(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float scEnergy = ele.superCluster()->energy();
      return ele.hadronicOverEm()*scEnergy <= hademConstTerm + maxHadem*scEnergy;
    }
    bool passSigmaIEtaIEta(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      return ele.full5x5_sigmaIetaIeta()<maxSigmaIEtaIEta;
    }
    bool passIsolEmHadDepth1(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float isol = ele.dr03EcalRecHitSumEt()+ele.dr03HcalDepth1TowerSumEt();
      const float isolRhoCorr = isol - eleExtra.rho*isolEmHadDepth1RhoEA;
      const float etFromStart =  std::max(0.,getEt(ele)-isolEmHadDepth1GradStart);
      const float cutValue = isolEmHadDepth1ConstTerm + isolEmHadDepth1GradTerm*etFromStart;
      return isolRhoCorr <= cutValue;
      
    }
    bool passE2x5Over5x5(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      return ele.full5x5_e2x5Max()/ele.full5x5_e5x5() >= minE2x5Over5x5 ||
	ele.full5x5_e1x5()/ele.full5x5_e5x5()<minE1x5Over5x5;
    }
    bool passIsolHadDepth2(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float isol = ele.dr03HcalDepth2TowerSumEt();
      const float isolRhoCorr = isol - eleExtra.rho*isolHadDepth2RhoEA;
      const float etFromStart =  std::max(0.,getEt(ele)-isolHadDepth2GradStart);
      const float cutValue = isolHadDepth2ConstTerm + isolHadDepth2GradTerm*etFromStart;
      return isolRhoCorr <= cutValue;
    }
    bool passIsolPtTrks(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float isol = ele.dr03TkSumPt();
      const float isolRhoCorr = getEt(ele) > isolPtTrksRhoStart ? isol - eleExtra.rho*isolPtTrksRhoEA : isol;
      const float etFromStart =  std::max(0.,getEt(ele)-isolPtTrksGradStart);
      const float cutValue = isolPtTrksConstTerm + isolPtTrksGradTerm*etFromStart;
      return isolRhoCorr <= cutValue;
    }
    bool passIsolPtTrksRel03(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float isol = ele.dr03TkSumPt();
      const float isolRhoCorr = isol - eleExtra.rho*isolPtTrksRelRhoEA;
      const float relIso = isolRhoCorr/ele.trackMomentumAtVtx().rho();
      return relIso <= maxIsolPtTrksRel03;
    }
    bool passIsolEmRel03(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float isol = ele.dr03EcalRecHitSumEt();
      const float isolRhoCorr = isol - eleExtra.rho*isolEmRelRhoEA;
      const float relIso = isolRhoCorr/ele.trackMomentumAtVtx().rho();
      return relIso <= maxIsolEmRel03;
    }
    bool passIsolHadRel03(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float isol = ele.dr03HcalTowerSumEt();
      const float isolRhoCorr = isol - eleExtra.rho*isolHadRelRhoEA;
      const float relIso = isolRhoCorr/ele.trackMomentumAtVtx().rho();
      return relIso <= maxIsolHadRel03;
    }
    bool passNrMissHits(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      return ele.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS) <= maxNrMissHits;
    }
    bool passDXY(const reco::GsfElectron& ele,const heep::GsfEleExtra& eleExtra)const{
      const float dxyAbs = std::abs(ele.gsfTrack()->dxy(eleExtra.primaryVertex));
      return dxyAbs<=maxDXY;
    }



  };
}


 


#endif
