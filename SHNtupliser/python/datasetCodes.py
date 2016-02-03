def getDatasetCode(filename):
    
    if filename.find("DYToEE")!=-1: return 101
    if filename.find("ZprimeToEE_M-5000")!=-1: return 1201;
    if filename.find("WJetsToLNu")!=-1: return 211;
    if filename.find("DYToLL_M_1")!=-1: return 101;
    if filename.find("QCD_Pt_15to30")!=-1: return 601;
    if filename.find("QCD_Pt_30to50")!=-1: return 602;
    if filename.find("QCD_Pt_50to80")!=-1: return 603;
    if filename.find("QCD_Pt_80to120")!=-1: return 604;
    if filename.find("QCD_Pt_120to170")!=-1: return 605;
    if filename.find("QCD_Pt_170to300")!=-1: return 606;
    if filename.find("QCD_Pt_300to470")!=-1: return 607;
    if filename.find("QCD_Pt_470to600")!=-1: return 608;
    if filename.find("QCD_Pt_600to800")!=-1: return 609;
    if filename.find("QCD_Pt_800to1000")!=-1: return 610;
    if filename.find("QCD_Pt_1400to1800")!=-1: return 611;
    if filename.find("QCD_Pt_1800to2400")!=-1: return 612;
    if filename.find("QCD_Pt_2400to3200")!=-1: return 613;
    if filename.find("QCD_Pt_3200toInf")!=-1: return 614;

    return 0
    
    
