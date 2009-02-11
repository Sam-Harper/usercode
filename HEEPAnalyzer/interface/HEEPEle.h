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

 
#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"
#include "SHarper/HEEPAnalyzer/interface/HEEPTrigCodes.h"

namespace heep {
  class Ele { 
    
  public:
    //helper struct to store the isolations
    struct IsolData {
      float nrTrks;
      float ptTrks;
      float em;
      float hadDepth1;
      float hadDepth2;
    };
    
  public:
    //helper struct to store the cluster shapes
    struct ClusShapeData {
      float sigmaEtaEta;
      float sigmaIEtaIEta;
      float e2x5MaxOver5x5;
      float e1x5Over5x5;
      float e5x5;
    };
    
    
  private:
    const reco::GsfElectron* gsfEle_; //pointers to the underlying electron (we do not own this)
                                      //remember that a pat::Electron inherits from a GsfElectron...
 
    ClusShapeData clusShapeData_;
    IsolData isolData_;
    
    //this is a bit-packed word telling me which cuts the electron fail of the heep selection (ie 0x0 is passed all cuts)
    int cutCode_;
    
    //and these are the trigger bits stored
    //note that the trigger bits are defined at the begining of each job
    //and do not necessaryly map between jobs
    heep::TrigCodes::TrigBitSet trigBits_;
    
  public:
    
    Ele(const reco::GsfElectron& ele,const ClusShapeData& shapeData,const IsolData& isolData):
      gsfEle_(&ele),clusShapeData_(shapeData),isolData_(isolData),
      cutCode_(heep::CutCodes::INVALID){}
    ~Ele(){}
    
    //modifiers  
    void setCutCode(int code){cutCode_=code;}
    void setTrigBits(TrigCodes::TrigBitSet bits){trigBits_=bits;}

    const reco::GsfElectron& gsfEle()const{return *gsfEle_;}
    const reco::SuperCluster& superCluster()const{return *(gsfEle_->superCluster());}
    //kinematic and geometric methods
    float et()const{return gsfEle_->et();}
    float scEt()const{return gsfEle_->superCluster()->position().rho()/gsfEle_->superCluster()->position().r()*caloEnergy();}
    float energy()const{return gsfEle_->energy();}
    float caloEnergy()const{return gsfEle_->caloEnergy();}
    float eta()const{return gsfEle_->eta();}  
    float scEta()const{return gsfEle_->superCluster()->eta();}
    float detEta()const{return scEta();} //det stands for detector, ie with respect to 0,0,0 (same as supercluster)
    float detEtaAbs()const{return fabs(detEta());}
    float phi()const{return gsfEle_->phi();}
    float scPhi()const{return gsfEle_->superCluster()->phi();}
    float zVtx()const{return gsfEle_->TrackPositionAtVtx().z();}
    const math::XYZTLorentzVector& p4()const{return gsfEle_->p4();}
    
    //classification (couldnt they have just named it 'type')
    int classification()const{return gsfEle_->classification();}
   
    //track methods
    int charge()const{return gsfEle_->charge();}
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
    float bremFrac()const{return (pVtx()-pCalo())/pVtx();}
    float invEOverInvP()const{return 1./gsfEle_->caloEnergy() - 1./gsfEle_->trackMomentumAtVtx().R();}

    //cluster shape variables (the sc infront of them is due to the gsfElectron naming scheme,
    //standing for supercluster)
    //note in 3_X, we can directly access these from the GsfElectron and do away with the 
    //clusShapeData struct
    float scSigmaEtaEta()const;
    float scSigmaEtaEtaUnCorr()const{return clusShapeData_.sigmaEtaEta;}
    float scSigmaIEtaIEta()const{return clusShapeData_.sigmaIEtaIEta;}
    float scE1x5()const{return clusShapeData_.e1x5Over5x5*clusShapeData_.e5x5;}
    float scE2x5Max()const{return clusShapeData_.e2x5MaxOver5x5*clusShapeData_.e5x5;}
    float scE1x5Over5x5()const{return clusShapeData_.e1x5Over5x5;}
    float scE2x5MaxOver5x5()const{return clusShapeData_.e2x5MaxOver5x5;}
    float scE5x5()const{return clusShapeData_.e5x5;}
   
    //isolation
    float isolEm()const{return isolData_.em;} 
    float isolHad()const{return isolData_.hadDepth1+isolData_.hadDepth2;}
    float isolEmHadDepth1()const{return isolEm()+isolHadDepth1();}
    float isolHadDepth1()const{return isolData_.hadDepth1;}
    float isolHadDepth2()const{return isolData_.hadDepth2;}
    float isolNrTrks()const{return isolData_.nrTrks;}
    float isolPtTrks()const{return isolData_.ptTrks;}
    
    //selection cuts
    int cutCode()const{return cutCode_;}
    bool passCuts(int cutMask=~0x0)const{return (cutCode() & cutMask)==0x0;} //defaults to all cuts, note: bit wise operators (&,|,^) are of lower presendence than == and != operators hence the ( ) are very necessary 
    
    //trigger info
    heep::TrigCodes::TrigBitSet trigBits()const{return trigBits_;}

  };
}

#endif
