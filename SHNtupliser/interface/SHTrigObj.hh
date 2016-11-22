#ifndef SHEVENT_SHTRIGOBJ_HH
#define SHEVENT_SHTRIGOBJ_HH

#include "SHarper/SHNtupliser/interface/TempFuncs.hh"

#include "TBits.h"
#include "TLorentzVector.h"

class SHTrigObj {
public:
 
  enum Type{                
    UNDEFINED=0x80000000,
    PHOTON=0x1,ELECTRON=0x2,EGAMMA=0x3,JET=0x4,TAU=0x8,MUON=0x10,SUM=0x20,MISC=0x40,EGUNSEEDED=0x80,//EGUNSEEDED new for V3 (version bump for our book keeping, root doesnt care about this change)
    HLT=0x7F, //note, the EGUNSEEDED is not included here as it must EGUNSEEDED + TYPE
    L1EGISO=0x100,L1EGNONISO=0x200,L1EG=0x300,L1JET=0x400,L1TAU=0x800,L1MUON=0x1000,L1SUM=0x2000,L1MISC=0x4000,
    L1=0x7F00,
    L1S1EGISO=0x10000,L1S1EGNONISO=0x20000,L1S1EG=0x30000,L1S1JET=0x40000,L1S1TAU=0x80000,L1S1MUON=0x100000,L1S1SUM=0x200000,L1S1MISC=0x400000,
    L1S1=0x7F0000,
    ALL=0xFFFFFFFF
  };
public:
  static constexpr float kStdMaxDR=0.1;
  static constexpr float kStdMaxDR2=kStdMaxDR*kStdMaxDR;

private:
  float pt_;
  float eta_;
  float phi_;
  float mass_;
  int type_; 
  
  TBits filtersPassed_;
  std::vector<std::pair<std::string,float> > vars_; //a sorted vector containing all the extra debug variables
  
  using VarSorter = TempFuncs::PairComp<std::string,float,std::less<std::string> >;

public:
  SHTrigObj():pt_(0),eta_(-999),phi_(-999),mass_(0.),type_(0){}
  SHTrigObj(float iPt,float iEta,float iPhi,float iMass,int iType,const TBits& iFiltersPassed);
  virtual ~SHTrigObj(){}
  
  //little nasty hack func
  void setType(int val){type_=val;}

  float pt()const{return pt_;}
  float eta()const{return eta_;}
  float phi()const{return phi_;}
  float mass()const{return mass_;}
  int type()const{return type_;}
  bool isHLT()const{return (type_&HLT)!=0;}
  bool isL1()const{return (type_&L1)!=0;}
  bool isL1S1()const{return (type_&L1S1)!=0;}
  bool isType(const SHTrigObj::Type val)const{return (type_&val)!=0;}
  TLorentzVector p4()const{TLorentzVector vec;vec.SetPtEtaPhiM(pt(),eta(),phi(),0.);return vec;}
  bool hasDebug()const{return !vars_.empty();}

  const TBits& filtersPassed()const{return filtersPassed_;}
  bool pass(size_t bitNr)const{return filtersPassed_.TestBitNumber(bitNr);}
  bool pass(const TBits& bits)const;//{ bits&=filtersPassed_; return bits.CountBits()!=0;} //intentionally copying TBits
  bool valid()const{return type_!=0;}
  bool isUndefined()const{return type_==static_cast<int>(UNDEFINED);}
  float var(const std::string& varName)const; 
  void clearVars(){vars_.clear();}
  void addVars(const std::vector<std::pair<std::string,float> >& vars);
  void addVars(std::vector<std::pair<std::string,float> > &&vars);
  template<typename T> void setVars(T&& inputVars){
    vars_=std::forward<T>(inputVars);
    sort_();
  }

	       
  
  std::ostream& print(std::ostream&)const;
 

  float deltaR2(float rhsEta,float rhsPhi)const;

  bool matches(float rhsEta,float rhsPhi,float maxDR2=kStdMaxDR2)const{
    return deltaR2(rhsEta,rhsPhi)<=maxDR2;
  }
  bool passL1CMSSWMatch(float rhsEta,float rhsPhi)const;
private:
  float deltaR2L1_(float rhsEta,float rhsPhi)const;
  //private for now as the vector should always be sorted
  void sort_(){std::sort(vars_.begin(),vars_.end(),VarSorter());}

  ClassDef(SHTrigObj,3)
};


#endif
