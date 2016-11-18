#ifndef SHMCPARTICLE
#define SHMCPARTICLE

//stores a Monte Carlo Particle, just p4 and basic truth info

#include "TObject.h"
#include "TLorentzVector.h"



class SHMCParticle: public TObject {

private:
  float pt_;
  float eta_;
  float phi_;
  float mass_;
  float nrgy_; //only non zero when pt=0
  float vx_;
  float vy_;
  float vz_;

  int index_;
  int stdhep_;
  int idhep_;//pdg id 
  int jmo1_;
  int jmo2_;
  int nrMo_; 
  int jda1_; 
  int jda2_;
  int nrDa_;
  
public:
  SHMCParticle();
  SHMCParticle(int partIndx,int partStdHep,int partIdHep,
	       int partJmo1,int partJmo2,int partNrMo,
	       int partJda1,int partJda2,int partNrDa,
	       const TLorentzVector& p4,const TVector3& pos);
  SHMCParticle(const SHMCParticle& rhs);
  virtual ~SHMCParticle(){}
  friend std::ostream &operator <<(std::ostream& output,const SHMCParticle &mcPart);
  std::ostream &print(std::ostream& output)const;
  // std::ostream &print()const ;
  int index()const{return index_;}
  int stdhep()const{return stdhep_;}
  int status()const{return stdhep_;}
  int idhep()const{return idhep_;}
  int pid()const{return idhep_;}
  int jmo1()const{return jmo1_;}
  int jmo2()const{return jmo2_;}
  int jda1()const{return jda1_;}
  int jda2()const{return jda2_;}
  TLorentzVector p4()const{TLorentzVector val;
    if(pt_!=0.) val.SetPtEtaPhiM(pt_,eta_,phi_,mass_);
    else val.SetXYZT(0,0,std::sqrt(nrgy_*nrgy_-mass_*mass_),nrgy_);
    return val;
  }
  double nrgy()const{return pt_==0. ? nrgy_ :  p4().E();}
  double mass()const{return mass_;}
  TVector3 pos()const{TVector3 val(vx_,vy_,vz_);return val;}
  double detEta()const;
  
  ClassDef(SHMCParticle,3)

};

#endif
