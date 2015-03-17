#ifndef SHL1CLUSTER_HH
#define SHL1CLUSTER_HH

#include "TObject.h"
#include "TLorentzVector.h"

#include <iostream>

namespace l1t{
  class EGamma;
  class CaloCluster;
}
namespace l1t{
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
        EXT_DOWN            = 14,
	JET2X3              = 30,
	STAGE1              = 31
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
  bool isIsolated_;

  TLorentzVector p4_;

  int hademTS_;
  int clusterFlags_;
  int etSeedTS_;

  int quality_;

  mutable TLorentzVector p4Temp_; //! a transisent variable to fix the fact we dont have the p4 set correctly

public:
  SHL1Cluster();
  SHL1Cluster(int iEtTS,int iEmEtOverTotEt,int iIEta,int iIPhi);
  SHL1Cluster(const l1t::EGamma& egamma);
  SHL1Cluster(const l1t::EGamma& egamma,const l1t::CaloCluster& clus);
  ~SHL1Cluster(){}

  void setIsol(int em,int had){isolEmEtTS_=em;isolHadEtTS_=had;}
  void setHademTS(int val){hademTS_=val;}
  void setClusterFlags(int val){clusterFlags_=val;}
  void setEtSeedTS(int val){etSeedTS_=val;}
  void setP4(const TLorentzVector& val){p4_=val;}
  void setQuality(int val){quality_=val;}
  
  int etTS()const{return etTS_;}
  int emEtOverTotEt()const{return emEtOverTotEt_;}
  int iEta()const{return iEta_;}
  int iPhi()const{return iPhi_;}
  int isolEmEtTS()const{return isolEmEtTS_;}
  int isolHadEtTS()const{return isolHadEtTS_;}
  int hademTS()const{return hademTS_;}
  int iEtaMax()const;
  int iPhiMax()const;
  int clusterFlags()const{return clusterFlags_;}
  int etSeedTS()const{return etSeedTS_;}
  int quality()const{return quality_;}

  int trigScale()const{return trigScale_;}
  bool isIsolated()const{return isIsolated_;}
  int leadTower()const{return 0.;} // always zero, here for compat reasons

  const TLorentzVector& p4()const;

  int localIEta2ndStrip()const{return checkClusterFlag(TRIM_LEFT) ? 1 : -1;} //is the 2nd eta strip of the cluster in the -ve or +ve direction
  

  void setTrigScale(int iTrigScale){trigScale_=iTrigScale;}
  
  friend std::ostream &operator <<(std::ostream& output,const SHL1Cluster& clus);
  std::ostream& print(std::ostream& output)const;

 
  //  float etCalib()const{return p4().Et()*l1ClusCalibration(p4().Eta());}
  float etCalib()const{return p4().Et();}
  bool checkClusterFlag(int flag)const{return (clusterFlags_&(0x1<<flag))!=0;}
  bool isStage1()const{return clusterFlags_==0;} //our current stage 1 check
  //void setClusterFlag(int flag);

  float eta()const;
  float phi()const;

  ClassDef(SHL1Cluster,4)

};


#endif
