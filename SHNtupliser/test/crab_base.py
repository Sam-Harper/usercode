from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'TOSED:REQUESTNAME'
config.General.workArea = 'crab_projects'
config.General.transferOutput = True


config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'shNtupliser_autoGen_cfg.py'

config.section_("Data")
config.Data.inputDataset = 'TOSED:DATASETPATH'
config.Data.dbsUrl = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 'TOSED:UNITSPERJOB'
config.Data.publication = False
config.Data.publishDbsUrl = 'phys03' 
config.Data.publishDataName = 'TOSED:PUBLISHDATANAME'
config.Data.outlfn = 'TOSED:OUTPUTDIR'

config.section_("Site")
config.Site.storageSite = "T2_UK_SGrid_RALPP"

config.section_("User")
