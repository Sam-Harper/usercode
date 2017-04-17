#ifndef TRIGBITSDEF
#define TRIGBITSDEF

#include "TDirectory.h"
#include "TLorentzVector.h"

#include <vector>
#include <array>
#include <string>

class SHEvent;
class SHTrigObj;
class SHBitsDef;

class TrigBitsDef {
private:
  static std::vector<std::string> trigPathNames_;
  static std::vector<std::string> trigFilterNames_;
  
public:
  static constexpr size_t kNrFilterWords=4;
  static constexpr size_t kNrPathWords=1;
  static constexpr size_t kWordSize=32; //just to clear we're dealing with 32-bit words

private: //static class
  TrigBitsDef(){}
  ~TrigBitsDef(){}
public:
  static std::array<int,kNrPathWords> getTrigCode(const SHEvent& event);
  static std::array<int,kNrFilterWords> getTrigCode(const SHEvent& event,double detEta,double detPhi,double eta=-999,double phi=-999);
  static std::array<int,kNrFilterWords> getTrigCode(const SHTrigObj& trigObj,const SHBitsDef& bitsDef);
  static TLorentzVector getTrigP4(const SHEvent& event,double detEta,double detPhi);
  static const std::vector<std::string>& trigPathNames(){if(trigPathNames_.empty()) setTrigNames();return trigPathNames_;}
  static const std::vector<std::string>& trigFilterNames(){if(trigFilterNames_.empty()) setTrigNames();return trigFilterNames_;}
  
  static std::pair<size_t,size_t> getWordAndBitNr(size_t index){return {index/kWordSize,index%kWordSize};}
  static void outOfBoundsErr(const std::string& funcStr);
  static void setTrigNamesRun2016(); 
  static void setTrigNames();
  static void write(TDirectory* dir);
};

#endif
