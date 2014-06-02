#ifndef SHL1CLUSTER_HH
#define SHL1CLUSTER_HH

#include "TObject.h"
#include "TLorentzVector.h"

#include <iostream>

namespace l1t{
  class EGamma;
  class CaloCluster;
}

class SHL1Cluster : public TObject { 
public:
  enum ClusterFlag{
        PASS_THRES_SEED     = 0,
        PASS_FILTER_CLUSTER = 1,
        TRIM_NW             = 2,
        TRIM_N              = 3,
        TRIM_NE             = 4,
        TRIM_E              = 5,
        TRIM_SE             = 7,
        TRIM_S              = 8,
        TRIM_SW             = 9,
        TRIM_W              = 10,
        TRIM_LEFT           = 11,
        TRIM_RIGHT          = 12,
        EXT_UP              = 13,
        EXT_DOWN            = 14
      };
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

  int hademTS_;
  int clusterFlags_;
  int etSeedTS_;

  mutable TLorentzVector p4Temp_; //! a transisent variable to fix the fact we dont have the p4 set correctly

public:
  SHL1Cluster();
  SHL1Cluster(const l1t::EGamma& egamma,const l1t::CaloCluster& clus);
  SHL1Cluster(const l1t::CaloCluster& clus);
  ~SHL1Cluster(){}

  int etTS()const{return etTS_;}
  int emEtOverTotEt()const{return emEtOverTotEt_;}
  int iEta()const{return iEta_;}
  int iPhi()const{return iPhi_;}
  int isolEmEtTS()const{return isolEmEtTS_;}
  int isolHadEtTS()const{return isolHadEtTS_;}

  int iEtaMax()const;
  int iPhiMax()const;
  
  int trigScale()const{return trigScale_;}
  bool fgVeto()const{return fgVeto_;}
  bool isIsolated()const{return isIsolated_;}

  const TLorentzVector& p4()const;

  int leadTower()const{return leadTower_;}
  int constituents()const{return constituents_;}
  int localIEta2ndStrip()const{return checkClusterFlag(TRIM_LEFT) ? 1 : -1;} //is the 2nd eta strip of the cluster in the -ve or +ve direction
  

  void setTrigScale(int iTrigScale){trigScale_=iTrigScale;}
  
  friend std::ostream &operator <<(std::ostream& output,const SHL1Cluster& clus);
  std::ostream& print(std::ostream& output)const;

  //naughty, shouldnt be tied to the class
  static float l1ClusCalibration(float eta);
  static float l1ClusCalibrationOld(float eta);
  
  //  float etCalib()const{return p4().Et()*l1ClusCalibration(p4().Eta());}
  float etCalib()const{return p4().Et();}
  bool checkClusterFlag(int flag)const{return (clusterFlags_&(0x1<<flag))!=0;}

  float eta()const;
  float phi()const;

  ClassDef(SHL1Cluster,3)

};


#endif
