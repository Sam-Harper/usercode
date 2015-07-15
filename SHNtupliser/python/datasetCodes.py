def getDatasetCode(filename):
    if filename.find("QCD_Pt-30to50_")!=-1: return 603
    if filename.find("QCD_Pt-50to80_")!=-1: return 604
    if filename.find("QCD_Pt-80to120_")!=-1: return 605
    if filename.find("QCD_Pt-120to170_")!=-1: return 606
    if filename.find("QCD_Pt-170to300_")!=-1: return 607
    if filename.find("QCD_Pt-300to470_")!=-1: return 608
    if filename.find("QCD_Pt-470to600_")!=-1: return 609
    if filename.find("QCD_Pt-600to800_")!=-1: return 610
    if filename.find("QCD_Pt-800to1000_")!=-1: return 611
    if filename.find("ZprimeToEE_M-5000")!=-1: return 1201


    return 0
    
    
