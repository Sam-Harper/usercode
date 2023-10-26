from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'TOSED:REQUESTNAME'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True


config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'shNtupliser_autoGen_cfg.py'
config.JobType.maxJobRuntimeMin = 180
#config.JobType.maxMemoryMB = 3000
#config.JobType.numCores = 4
#config.JobType.inputFiles=['ged_regression_20161208.db',]
#TOSED:EXTRAJOBTYPEINFO

config.section_("Data")
config.Data.inputDataset = 'TOSED:DATASETPATH'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 'TOSED:UNITSPERJOB'
config.Data.totalUnits = 'TOSED:TOTALUNITS'
config.Data.publication = False
config.Data.publishDBS = 'phys03' 
config.Data.outputDatasetTag = 'TOSED:PUBLISHDATANAME'
config.Data.outLFNDirBase = 'TOSED:OUTPUTDIR'
config.Data.allowNonValidInputDataset = True
config.section_("Site")
config.Site.storageSite = "T2_UK_SGrid_RALPP"

config.section_("User")
