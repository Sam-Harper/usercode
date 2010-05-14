#ifndef SHELECMSSWSTRUCTS
#define SHELECMSSWSTRUCTS

//all stolen from GsfElectron as I cant forward declare them

//all stolen from GsfElectron as I cant forward declare them

#include <string>
#include <limits>

namespace cmssw{
  struct FiducialFlags
  {
    bool isEB ;        // true if particle is in ECAL Barrel
    bool isEE ;        // true if particle is in ECAL Endcaps
    bool isEBEEGap ;   // true if particle is in the crack between EB and EE
    bool isEBEtaGap ;  // true if particle is in EB, and inside the eta gaps between modules
    bool isEBPhiGap ;  // true if particle is in EB, and inside the phi gaps between modules
    bool isEEDeeGap ;  // true if particle is in EE, and inside the gaps between dees
    bool isEERingGap ; // true if particle is in EE, and inside the gaps between rings
    
    static const std::string contents_;

    FiducialFlags()
      : isEB(false), isEE(false), isEBEEGap(false),
         isEBEtaGap(false), isEBPhiGap(false),
	isEEDeeGap(false), isEERingGap(false)
    {}
    
    static const std::string contents(){return contents_;}
   
  } ;
  
  
  struct ShowerShape
  {
    float sigmaEtaEta ;        // weighted cluster rms along eta and inside 5x5 (absolute eta)
    float sigmaIetaIeta ;      // weighted cluster rms along eta and inside 5x5 (new, Xtal eta)
    float e1x5 ;               // energy inside 1x5 in etaxphi around the seed Xtal
    float e2x5Max ;            // energy inside 2x5 in etaxphi around the seed Xtal (max bwt the 2 possible sums)
    float e5x5 ;               // energy inside 5x5 in etaxphi around the seed Xtal
    float hcalDepth1OverEcal ; // hcal over ecal seed cluster energy using first hcal depth (hcal is energy of towers within dR=015)
    float hcalDepth2OverEcal ; // hcal over ecal seed cluster energy using 2nd hcal depth (hcal is energy of towers within dR=015)
    float eMax;
    float e3x3;

    static const std::string contents_;
    ShowerShape()
      : sigmaEtaEta(std::numeric_limits<float>::infinity()),
	sigmaIetaIeta(std::numeric_limits<float>::infinity()),
	e1x5(0.), e2x5Max(0.), e5x5(0.),
	hcalDepth1OverEcal(0), hcalDepth2OverEcal(0),
	eMax(0),e3x3(0)
       {}

    static const std::string contents(){return contents_;}
  } ;
  
  struct IsolationVariables
  {
    float tkSumPt ;                // track iso deposit with electron footprint removed
    float ecalRecHitSumEt ;        // ecal iso deposit with electron footprint removed
    float hcalDepth1TowerSumEt ;   // hcal depht 1 iso deposit with electron footprint removed
    float hcalDepth2TowerSumEt ;   // hcal depht 2 iso deposit with electron footprint removed

 static const std::string contents_;
    IsolationVariables()
      : tkSumPt(0.), ecalRecHitSumEt(0.), hcalDepth1TowerSumEt(0.), hcalDepth2TowerSumEt(0.)
    {}
    static const std::string contents(){return contents_;}

  } ;
}
#endif
