#ifndef SHL1CLUSTER_HH
#define SHL1CLUSTER_HH

#include "TObject.h"
#include "TLorentzVector.h"

#include <iostream>

namespace l1slhc{
  class L1CaloCluster;
}

class SHL1Cluster : public TObject {
private:
  //TS = in trigger scale, not GeV. 

  int etTS_;
  int emEtOverTotEt_;
  int iEta_;
  int iPhi_;
  int isolEmEtTS_;
  int isolHadEtTS_;
  
  int trigScale_;
  bool fgVeto_;
  bool isIsolated_;

  TLorentzVector p4_;

  int leadTower_; //which of the 2x2 is the lead tower, first bit = eta offset, second bit = phi offset
  int constituents_; //bit 1 = 0,0 bit 2= 0,1, bit 3=1,0, bit 4=1,1
public:
  SHL1Cluster();
  SHL1Cluster(const l1slhc::L1CaloCluster& clus);
  ~SHL1Cluster(){}

  int etTS()const{return etTS_;}
  int emEtOverTotEt()const{return emEtOverTotEt_;}
  int iEta()const{return iEta_;}
  int iPhi()const{return iPhi_;}
  int isolEmEtTS()const{return isolEmEtTS_;}
  int isolHadEtTS()const{return isolHadEtTS_;}
  
  int trigScale()const{return trigScale_;}
  bool fgVeto()const{return fgVeto_;}
  bool isIsolated()const{return isIsolated_;}

  const TLorentzVector& p4()const{return p4_;}

  void setTrigScale(int iTrigScale){trigScale_=iTrigScale;}
  
  friend std::ostream &operator <<(std::ostream& output,const SHL1Cluster& clus);
  std::ostream& print(std::ostream& output)const;

  ClassDef(SHL1Cluster,2)

};


#endif
