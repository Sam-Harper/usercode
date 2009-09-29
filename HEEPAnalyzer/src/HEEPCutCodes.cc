#include "SHarper/HEEPAnalyzer/interface/HEEPCutCodes.h"


heep::ComCodes heep::CutCodes::codes_(CutCodes::makeCodeMap_());

//slightly inefficiency but its done once per job
heep::ComCodes heep::CutCodes::makeCodeMap_()
{
  heep::ComCodes codes;
  codes.setCode("et",ET);
  codes.setCode("pt",PT);
  codes.setCode("detEta",DETETA);
  codes.setCode("crack",CRACK);

  codes.setCode("dEtaIn",DETAIN);
  codes.setCode("dPhiIn",DPHIIN);
  codes.setCode("hadem",HADEM);
  codes.setCode("sigmaIEtaIEta",SIGMAIETAIETA);
  codes.setCode("e2x5Over5x5",E2X5OVER5X5);
  codes.setCode("isolEmHadDepth1",ISOLEMHADDEPTH1);
  codes.setCode("isolHadDepth2",ISOLHADDEPTH2);
  codes.setCode("isolPtTrks",ISOLPTTRKS);
  codes.setCode("ecalDriven",ECALDRIVEN);
  codes.setCode("invalid",INVALID);
  codes.sort();
  return codes;
}

