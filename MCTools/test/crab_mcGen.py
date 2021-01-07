from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'TOSED:REQUESTNAME'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True


config.section_("JobType")
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'shNtupliser_autoGen_cfg.py'
#config.JobType.inputFiles=[TOSED:INPUTFILES]
#TOSED:EXTRAJOBTYPEINFO

config.section_("Data")
config.Data.outputPrimaryDataset = 'TOSED:DATASETPATH'
config.Data.inputDBS = 'global'
config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 'TOSED:UNITSPERJOB'
config.Data.totalUnits = 'TOSED:TOTALUNITS'
config.Data.publication = False
config.Data.publishDBS = 'phys03' 
config.Data.outputDatasetTag = 'TOSED:PUBLISHDATANAME'
config.Data.outLFNDirBase = 'TOSED:OUTPUTDIR'
 
config.section_("Site")
config.Site.storageSite = "T2_UK_SGrid_RALPP"

config.section_("User")
