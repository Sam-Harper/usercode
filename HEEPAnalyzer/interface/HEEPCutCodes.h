#ifndef SHARPER_HEEPANALYZER_HEEPCUTCODES
#define SHARPER_HEEPANALYZER_HEEPCUTCODES

//class: heep::CutCodes
//Description: defines which bits correspond to which cuts
//Implimentation:
//   defined as both a enum (which means it can be accessed fast)
//   and a std::string to bit map (slightly slower access but more convient)
//   It is a static class so only one defination of HEEPCutCodes allowed at one time

#include "SHarper/HEEPAnalyzer/interface/HEEPComCodes.h"

#include <cstring>
#include <map>
#include <string>
#include <iostream>

namespace heep {
  class CutCodes { //class to handle the cutcodes used in electron cutting
    
  public:
    
    
    enum CutCode{
      //common cuts
      ET              =0x0001,
      PT              =0x0002,
      DETETA          =0x0004,
      CRACK           =0x0008,
      
      DETAIN          =0x0010,
      DPHIIN          =0x0020,
      HADEM           =0x0040,
      SIGMAIETAIETA   =0x0080,
      E2X5OVER5X5     =0x0100,
      ISOLEMHADDEPTH1 =0x0200,
      ISOLHADDEPTH2   =0x0400,
      ISOLPTTRKS      =0x0800,
      ISOLNRTRKS      =0x1000,
      
      //flag that if its set, shows the code is invalid
      INVALID       =0x80000000
      
    };
    
  private:
    static heep::ComCodes codes_;
    
  private:
    CutCodes(){} //not going to allow instainitiation
    ~CutCodes(){}
    
  public:
    static int getCode(const std::string& descript){return codes_.getCode(descript.c_str());}
    static void getCodeName(int code,std::string& id){return codes_.getCodeName(code,id);}
    static std::string getCodeName(int code){return codes_.getCodeName(code);}

  private:
    static heep::ComCodes makeCodeMap_();
  
  };
}
#endif
