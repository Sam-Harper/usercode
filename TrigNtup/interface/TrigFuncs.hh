#ifndef TRIGFUNCS
#define TRIGFUNCS

#include "SHarper/SHNtupliser/interface/SHEvent.hh"

#include <string>

class SHTrigObjContainer;
class SHTrigEcalCand;
class SHTrigSummary;
class TrigFuncs {

private:
  TrigFuncs(){}
  ~TrigFuncs(){}

public:
  class PreScaleWeights{
  private:
    using EleTrigMatch = std::tuple<const SHElectron*,const SHTrigObj*,double>;
  public:
    struct PathAndFilters {
      std::string pathName;
      std::string filterName;
      float minEt;
      float maxEt;
      bool l1EG26;
      PathAndFilters(){}
      PathAndFilters(const std::string& iPath,
		     const std::string& iFilter,
		     float iMinEt,float iMaxEt,
		     bool iL1EG26):pathName(iPath),filterName(iFilter),
				  minEt(iMinEt),maxEt(iMaxEt),l1EG26(iL1EG26){}
    };
  private:
    class EventInfo {
      int runnr_,eventnr_,lumiSec_;
    public:
      EventInfo():runnr_(0),eventnr_(0),lumiSec_(0){}
      EventInfo& operator=(const SHEvent& event){
	runnr_=event.runnr();
	eventnr_=event.eventnr();
	lumiSec_=event.lumiSec();
	return *this;
      }
      bool operator==(const SHEvent& event)const{
	return runnr_==event.runnr() && eventnr_==event.eventnr() && lumiSec_==event.lumiSec();
      }
      bool operator!=(const SHEvent& event)const{return !( (*this)==event);}
    };
  private:
    std::vector<PathAndFilters> pathNames_;
    std::vector<std::string> filterNames_;
    std::vector<EleTrigMatch> eleTrigMatches_;
    EventInfo currEvent_;

    float maxDR2_;
  public:
    PreScaleWeights();
    
    int getWeight(const SHElectron* ele,const SHEvent* event,TLorentzVector& matchedTrigP4);
    void setTrigMatches(const SHEvent* event);
   
  };

private:
  const static std::vector<std::string> l1FilterNames_;
  const static std::vector<std::pair<int,std::vector<int> > > l1EG26Prescales_;
public:
  static void getElesPassingFilter(const SHEvent& event,const std::string& filterName,std::vector<const SHElectron*>& eles);
  static bool filterEtIsSCEt(const std::string& filterName);
  static bool isL1Filter(const std::string& filterName);
  static bool isEGUnseeded(const SHTrigObj& trigObj,const SHTrigSummary& trigSum);
  static bool isHIEG(const SHTrigObj& trigObj,const SHTrigSummary& trigSum);
  static std::pair<const SHTrigObj*,const SHTrigObj*> getEGTrigObj(float detEta,float detPhi,const SHTrigSummary& trigSum);
  static int eg26PS(int runnr,size_t psColumn);
  static const SHTrigEcalCand* getTrigEcalCand(const SHTrigObjContainer& event,float detEta,float detPhi,float maxDR);
  static void dropAllButRequiredTrigs(SHEvent* event);
  static std::string rmTrigVersion(std::string trigname);
  //// static TLorentzVector getL1P4(const TLorentzVector& objP4,const SHEvent* event);
  //static TLorentzVector getSinglePhoP4(const TLorentzVector& objP4,const SHEvent* event);
  //static TLorentzVector getDoublePhoSeededP4(const TLorentzVector& objP4,const SHEvent* event);
  //static TLorentzVector getDoublePhoUnSeededP4(const TLorentzVector& objP4,const SHEvent* event);
  static void convertOldTrigFormat(SHTrigSummary& trigSum,const SHEvent& event);
  static std::pair<const SHTrigObj*,const SHTrigObj*> 
  getL1Match(const std::vector<std::string>& filterNames,
	     const SHTrigSummary& trigSum,float detEta,float detPhi);
  static std::pair<const SHTrigObj*,const SHTrigObj*> 
  getL1Match(const SHTrigSummary& trigSum,float detEta,float detPhi);
  static const SHTrigObj*
  getL1SingleMatch(const std::vector<std::string>& filterNames,
		   const SHTrigSummary& trigSum,float detEta,float detPhi);
  
  static std::vector<std::pair<int,std::vector<int> > > createEG26PSLookup();

};


#endif
