#ifndef SHARPER_HEEPANALYZER_HEEPELETYPECODES
#define SHARPER_HEEPANALYZER_HEEPELETYPECODES


//defines bitwise commincation codes for the electron type
//this was intended to allow different cuts for crack, golden, narrow etc
//but is only really used today to seperate barrel and endcap for
//heep::EleSelector and thats mainly historical


#include "SHarper/HEEPAnalyzer/interface/HEEPComCodes.h"

#include <cstring>
#include <string>
#include <iostream>

namespace heep {
  
  class EleTypeCodes { 
  public:
    
    enum EleTypeCode{
      //common cuts
      BARREL     = 0x0001,
      ENDCAP     = 0x0002,
      GOLDEN     = 0x0004,
      NARROW     = 0x0008,
      BIGBREM    = 0x0010,
      SHOWERING  = 0x0020,
      CRACK      = 0x0040
    };
    
  private:
    static ComCodes codes_;
    
  private:
    
    EleTypeCodes(){} //no instanitation possible
    ~EleTypeCodes(){}
    
  public:
    
    static int makeTypeCode(int eleType);
    static int getCode(const std::string& descript){return codes_.getCode(descript.c_str());}
    static void getCodeName(int code,std::string& id){return codes_.getCodeName(code,id);}
    static std::string getCodeName(int code){return codes_.getCodeName(code);}

  private:
    static ComCodes setCodes_();
    
  };
}
#endif
