#ifndef SHARPER_HEEPANALYZER_HEEPELE
#define SHARPER_HEEPANALYZER_HEEPELE

//class: HEEP Ele (HEEP Electron)
//
//author: Sam Harper (July 2008) (sharper@<at you know where>.ch)
//
//
//aim: to allow easy access to electron ID variables for heep analyses
//     this a stop gap to produce a simple electron class with all variables easily accessable via methods 
//     this class is supposed to quickly react to electron ID changes (ie when data comes)
//     as this class can and will change (allthough the interface should be stable), users are discouraged 
//     from writing it to file
//
//implimentation: 
//    aims to be a wrapper for GsfElectron / pat::Electron methods, 
//    it is hoped that in time these methods will be directly added to GsfElectron and so make this 
//    class obsolute
//    Sadly I think we will always need this class as it can react to changes much faster than pat::Electron 
//    or GsfElectron

//the methods of most interest (EB = barrel var, EE= endcap var, note an barrel var will be defined for
//the endcap and vice versa but they often they are not used for id in the other detector)
//p4(),et(),energy(),eta(),phi() : electron based 4-fourmomentum, use this for physics (eg making a mass spectrum)
//scEta(),scPhi() : supercluster eta/phi w.r.t to 0,0,0 (otherwise known as detector coordinates, use for fiducial cuts
//dEtaIn(),dPhiIn(),hOverE(),scE2x5MaxOver5x5() (EB),scE1x5Over5x5() (EB),scSigmaIEtaIEta() (EE): 'robust' ID variables
//isolEmHadDepth1(),isolHadDepth2() (EE),isolPtTrks(): isolation varibles
//cutCode() : list of cuts failed, ==0x0 all passed 
//passCuts(int cutMask) : simplifies the test whether it passed the specified cuts (defaults to all)
//trigBits() : list of triggers passed, !=0x0 a trigger was fired

#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

 
#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPTrigCodes.h"

#include <limits>

namespace heep {
  class Ele { 

    
    
  private:
    const reco::GsfElectron* gsfEle_; //pointers to the underlying electron (we do not own this)
                                      //remember that a pat::Electron inherits from a GsfElectron...
    const pat::Electron* patEle_; //the electron as a pat electron (note that the address is same as gsfEle_, we could have just done some nasty casting but its better this way). Note that if we do not orginally have a pat electron, it is NULL
    
    //this is a bit-packed word telling me which cuts the electron fail of the heep selection (ie 0x0 is passed all cuts)
    int cutCode_;
    
    //and these are the trigger bits stored
    //note that the trigger bits are defined at the begining of each job
    //and do not necessaryly map between jobs
    heep::TrigCodes::TrigBitSet trigBits_;

    //we redefine the p4 of the electron to always take the calorimeter energy and not to combine or use the tracker momentum
    //note we still use the track eta/phi for the directions;
    math::XYZTLorentzVector p4_;
    
  public:
    
    //this uses dynamic_casting to see if we really have a pat electron or not, patEle_ is null if its not
    Ele(const reco::GsfElectron& ele):
      gsfEle_(&ele),patEle_(dynamic_cast<const pat::Electron*>(&ele)),
      cutCode_(heep::CutCodes::INVALID),
      p4_(ele.p4()*ele.caloEnergy()/ele.energy()){}
  

    Ele(const pat::Electron& ele):
      gsfEle_(&ele),patEle_(&ele),
      cutCode_(heep::CutCodes::INVALID),
      p4_(ele.p4()*ele.caloEnergy()/ele.energy()){}
    ~Ele(){}
    
    //modifiers  
    void setCutCode(int code){cutCode_=code;}
    void setTrigBits(TrigCodes::TrigBitSet bits){trigBits_=bits;}

    const reco::GsfElectron& gsfEle()const{return *gsfEle_;}
    const pat::Electron& patEle()const; //this function will throw an exception if its not a pat electron (check with isPatEle())
    const reco::SuperCluster& superCluster()const{return *(gsfEle_->superCluster());}
    bool isPatEle()const{return patEle_!=NULL;}
    //kinematic and geometric methods
    float et()const{return p4_.Et();}
    float gsfEt()const{return gsfEle_->et();} 
    float scEt()const{return gsfEle_->superCluster()->position().rho()/gsfEle_->superCluster()->position().r()*caloEnergy();}
    float energy()const{return gsfEle_->caloEnergy();}
    float gsfEnergy()const{return gsfEle_->energy();}
    float caloEnergy()const{return gsfEle_->caloEnergy();}
    float eta()const{return gsfEle_->eta();}  
    float scEta()const{return gsfEle_->superCluster()->eta();}
    float detEta()const{return scEta();} //det stands for detector, ie with respect to 0,0,0 (same as supercluster)
    float detEtaAbs()const{return fabs(detEta());}
    float phi()const{return gsfEle_->phi();}
    float scPhi()const{return gsfEle_->superCluster()->phi();}
    float detPhi()const{return scPhi();}
    float zVtx()const{return gsfEle_->TrackPositionAtVtx().z();}
    const math::XYZTLorentzVector& p4()const{return p4_;}
    const math::XYZTLorentzVector& gsfP4()const{return gsfEle_->p4();}
  
    
    //classification (couldnt they have just named it 'type')
    int classification()const{return gsfEle_->classification();}
    bool isEcalDriven()const{return gsfEle_->isEcalDriven();}
    bool isTrackerDriven()const{return gsfEle_->isTrackerDriven();}
    bool isEB()const{return gsfEle_->isEB();}
    bool isEE()const{return gsfEle_->isEE();}
   
    //track methods
    int charge()const{return gsfEle_->charge();} //this the estimated charge of the electron, it is not always the charge of the track 
    int trkCharge()const{return gsfEle_->gsfTrack()->charge();}
    float pVtx()const{return gsfEle_->trackMomentumAtVtx().R();}
    float pCalo()const{return gsfEle_->trackMomentumAtCalo().R();}
    float ptVtx()const{return gsfEle_->trackMomentumAtVtx().rho();}
    float ptCalo()const{return gsfEle_->trackMomentumAtCalo().rho();}
     
    //abreviations of overly long GsfElectron methods, I'm sorry but if you cant figure out what hOverE() means, you shouldnt be using this class
    float hOverE()const{return gsfEle_->hadronicOverEm();}
    float dEtaIn()const{return gsfEle_->deltaEtaSuperClusterTrackAtVtx();}
    float dPhiIn()const{return gsfEle_->deltaPhiSuperClusterTrackAtVtx();}
    float dPhiOut()const{return gsfEle_->deltaPhiSeedClusterTrackAtCalo();}
    float epIn()const{return gsfEle_->eSuperClusterOverP();}
    float epOut()const{return gsfEle_->eSeedClusterOverPout();}
    float fbrem()const{return gsfEle_->fbrem();}
    float bremFrac()const{return fbrem();}
    float invEOverInvP()const{return gsfEle_->trackMomentumAtVtx().R()!=0. ? 1./gsfEle_->caloEnergy() - 1./gsfEle_->trackMomentumAtVtx().R() : std::numeric_limits<float>::max();}

    //shower shape variables
    float sigmaEtaEta()const;
    float sigmaEtaEtaUnCorr()const{return gsfEle_->sigmaEtaEta();}
    float sigmaIEtaIEta()const{return gsfEle_->sigmaIetaIeta();}
    float e1x5()const{return gsfEle_->e1x5();}
    float e2x5Max()const{return gsfEle_->e2x5Max();}
    float e5x5()const{return gsfEle_->e5x5();}
    float e1x5Over5x5()const{return e5x5()!=0 ? e1x5()/e5x5() : 0.;}
    float e2x5MaxOver5x5()const{return e5x5()!=0 ? e2x5Max()/e5x5() : 0.;}
  
    
    //for backwards compatiblity, GsfElectron had these naming schemes but dropped them, will go away in later releases
    //in theory they are identical to the non sc versions but I dont just map them straight to my functions
    //as I want to be sensitive to bugs in the gsf electron implimention
    float scSigmaEtaEta()const;
    float scSigmaEtaEtaUnCorr()const{return gsfEle_->scSigmaEtaEta();}
    float scSigmaIEtaIEta()const{return gsfEle_->scSigmaIEtaIEta();}
    float scE1x5()const{return gsfEle_->scE1x5();}
    float scE2x5Max()const{return gsfEle_->scE2x5Max();}
    float scE5x5()const{return scE5x5();}
    float scE1x5Over5x5()const{return scE5x5()!=0 ? scE1x5()/scE5x5() : 0.;}
    float scE2x5MaxOver5x5()const{return scE5x5()!=0 ? scE2x5Max()/scE5x5() : 0.;}
    
    //isolation, we use cone of 0.3
    float isolEm()const{return gsfEle_->dr03EcalRecHitSumEt();} 
    float isolHad()const{return gsfEle_->dr03HcalTowerSumEt();}
    float isolHadDepth1()const{return gsfEle_->dr03HcalDepth1TowerSumEt();}
    float isolHadDepth2()const{return gsfEle_->dr03HcalDepth2TowerSumEt();}
    float isolPtTrks()const{return gsfEle_->dr03TkSumPt();}  
    float isolEmHadDepth1()const{return isolEm()+isolHadDepth1();}
    
    //selection cuts
    int cutCode()const{return cutCode_;}
    bool passCuts(int cutMask=~0x0)const{return (cutCode() & cutMask)==0x0;} //defaults to all cuts, note: bit wise operators (&,|,^) are of lower presendence than == and != operators hence the ( ) are very necessary 
    bool passCuts(std::string& cutsToPass)const{return passCuts(heep::CutCodes::getCode(cutsToPass));}
    std::string listCutsFailed()const{return heep::CutCodes::getCodeName(cutCode_);}
    //trigger info
    heep::TrigCodes::TrigBitSet trigBits()const{return trigBits_;}

  };
}


#endif
