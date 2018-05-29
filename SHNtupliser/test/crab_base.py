from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'TOSED:REQUESTNAME'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.instance = 'preprod'

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'shNtupliser_autoGen_cfg.py'
config.JobType.maxMemoryMB = 3000
config.JobType.numCores = 4
#config.JobType.inputFiles=['EEAlign_2018.db','alignments_PIX_MP.db','alignments_PIXSTRIP_MP.db','TrackerAlignment_PCL_byRun_v1_express_315648.db']
#TOSED:EXTRAJOBTYPEINFO

config.section_("Data")
config.Data.inputDataset = 'TOSED:DATASETPATH'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 'TOSED:UNITSPERJOB'
config.Data.totalUnits = 'TOSED:TOTALUNITS'
#config.Data.publication = True
config.Data.publishDBS = 'phys03' 
config.Data.outputDatasetTag = 'TOSED:PUBLISHDATANAME'
config.Data.outLFNDirBase = 'TOSED:OUTPUTDIR'
config.Data.allowNonValidInputDataset = True
config.section_("Site")
config.Site.storageSite = "CHANGETOYOURT2"
#config.Site.whitelist = ["T2_UK_SGrid_RALPP"]

config.section_("User")
