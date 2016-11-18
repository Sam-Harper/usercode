#ifndef SHJET
#define SHJET

//This unsurprisingly is my jet class
//Currently as I'm writing this class, I know *nothing* about
//jets at CMS so I'm basically copying whatever looks interesting
//in the CaloJet class....
//By that you should understand this is very basic

#include "TLorentzVector.h"
#include "TObject.h"

namespace pat{
  class Jet;
}

class SHJet : public TObject {

private: 
  //kinematic quanities
  float pt_;
  float eta_;
  float phi_;
  float nrgy_;
  float invNrgyCorr_;

  //id quanities
  float neutralHadNrgy_;
  float neutralEmNrgy_;
  float chargedHadNrgy_;
  float chargedEmNrgy_;
  float muonNrgy_;
  unsigned char chargedMultiplicity_;
  unsigned char neutralMultiplicity_;

  //btag
  float combinedSecVtxBTag_;
  float combinedMVABTag_;
  
  //mc info
  //in theory only need 4 bits for flavour so wasting 8 bits here
  //but easier this way
  signed char partonFlavour_; 
  signed char hadronFlavour_; 


  //constants for storting the flavour and multiplicity info
  static constexpr unsigned char kMaxMultiplicity_=std::numeric_limits<unsigned char>::max();
  static constexpr signed char kMinFlavour_=std::numeric_limits<signed char>::min();
  static constexpr signed char kMaxFlavour_=std::numeric_limits<signed char>::max();

  // //completely internal to SHJet for parton flavour storage
  // enum class PartonType{   
  //   NONE=0,ANTIUP,ANTIDOWN,ANTICHARM,ANTISTRANGE,ANTIBOTTOM,ANTITOP,
  //     UP,DOWN,CHARM,STRANGE,BOTTOM,TOP,GLUON,UNDEFINED;
  // };
  

public:
  SHJet();
  SHJet(const pat::Jet & jet); //CMSSW hook
  SHJet(const SHJet& rhs)=default;
  ~SHJet(){}
  
  SHJet& operator=(const SHJet& rhs)=default;
  bool operator==(const SHJet& rhs)const;
  bool operator!=(const SHJet& rhs)const{return !(*this==rhs);}

  TLorentzVector p4()const{TLorentzVector p4;p4.SetPtEtaPhiE(pt_,eta_,phi_,nrgy_);return p4;}
  float pt()const{return pt_;}	
  float ptUnCorr()const{return pt()*invNrgyCorr();}
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  float nrgy()const{return nrgy_;}
  float nrgyUnCorr()const{return nrgy()*invNrgyCorr();}

  float invNrgyCorr()const{return invNrgyCorr_;}

  float neutralHadNrgy()const{return neutralHadNrgy_;}
  float neutralEmNrgy()const{return neutralEmNrgy_;}
  float chargedHadNrgy()const{return chargedHadNrgy_;}
  float chargedEmNrgy()const{return chargedEmNrgy_;}
  float muonNrgy()const{return muonNrgy_;}
  
  float neutralHadNrgyFrac()const{return neutralHadNrgy()/nrgyUnCorr();}
  float neutralEmNrgyFrac()const{return neutralEmNrgy()/nrgyUnCorr();}
  float chargedHadNrgyFrac()const{return chargedHadNrgy()/nrgyUnCorr();}
  float chargedEmNrgyFrac()const{return chargedEmNrgy()/nrgyUnCorr();}
  float muonNrgyFrac()const{return muonNrgy()/nrgyUnCorr();}
  float hadNrgyFrac()const{return neutralHadNrgyFrac() + chargedHadNrgyFrac();}
  float emNrgyFrac()const{return neutralEmNrgyFrac() + chargedEmNrgyFrac();}

  unsigned char chargedMultiplicity()const{return chargedMultiplicity_;}
  unsigned char neutralMultiplicity()const{return neutralMultiplicity_;}

  signed char partonFlavour()const{return partonFlavour_;}
  signed char hadronFlavour()const{return hadronFlavour_;}
  
private:
  static constexpr unsigned char convertMultiplicity(int val){return val<0 ? kMaxMultiplicity_ : val>kMaxMultiplicity_ ? kMaxMultiplicity_ : val;}
  static constexpr unsigned char convertFlavour(int val){return val<kMinFlavour_ ? kMinFlavour_ : val>kMaxFlavour_ ? kMaxFlavour_ : val;}
  
  ClassDef(SHJet,6)

};

#endif
