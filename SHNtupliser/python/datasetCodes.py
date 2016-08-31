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
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_50_120")!=-1: return 111 
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_120_200")!=-1: return 112 
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_200_400")!=-1: return 113
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_400_800")!=-1: return 114
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_800_1400")!=-1: return 115
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_1400_2300")!=-1: return 116
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_2300_3500")!=-1: return 117
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_3500_4500")!=-1: return 118
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_4500_6000")!=-1: return 119
    if filename.find("ZToEE_NNPDF30_13TeV-powheg_M_6000_Inf")!=-1: return 120 
    

    return 0
    
    
