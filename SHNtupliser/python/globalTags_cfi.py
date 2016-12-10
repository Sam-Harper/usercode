def getGlobalTagNameData(version):
    if version=="Run2016H-PromptReco-v1": return "80X_dataRun2_Prompt_v12"
    if version=="Run2016H-PromptReco-v2": return "80X_dataRun2_Prompt_v14"
    if version=="Run2016H-PromptReco-v3": return "80X_dataRun2_Prompt_v14"
    if version=="Run2016B-23Sep2016-v3": return "80X_dataRun2_2016SeptRepro_v4"
    if version.find("23Sep2016-v1")!=-1: return "80X_dataRun2_2016SeptRepro_v3"
    
    defaultGT= "80X_dataRun2_2016SeptRepro_v4"
    print "getGlobalTagNameData, unknown dataset version ",version," returning ",defaultGT
    
    return defaultGT
