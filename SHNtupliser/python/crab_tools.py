import tarfile
import CRABClient.Emulator
from CRABClient import __version__
from CRABClient.Commands.SubCommand import SubCommand
from CRABClient.ClientExceptions import RESTCommunicationException, ConfigurationException, UnknownOptionException

from CRABClient.ClientUtilities import initLoggers, flushMemoryLogger, removeLoggerHandlers
import SHarper.SHNtupliser.dasFileQuery as dasFileQuery


def make_job_data(job_lumis,dataset):
    fileCache={}
    filesSet=set([])
    for run in job_lumis.keys():
        for lumipair in job_lumis[run]:
            for lumi in range(lumipair[0],lumipair[1]+1):
                for filename in dasFileQuery.getFileNamesCache({"dataset" : dataset,"run" : int(run),"lumi":int(lumi)},fileCache):
                    filesSet.add(filename)
                                                                

    print filesSet
#    print filename,lumis
                
class CrabJobInputs(SubCommand):
    """
    Gets a list of the inputs to the jobs
    """ 
    name = 'report' #we're pretending this is something other than it really is (using existing crab cmd name)
    shortnames = ['rep']
    def __init__(self, logger, crab_dir):
        
        SubCommand.__init__(self, logger,["--dir",crab_dir])
        self.crab_dir=crab_dir
    
    def __call__(self):
        serverFactory = CRABClient.Emulator.getEmulator('rest')
        server = serverFactory(self.serverurl, self.proxyfilename, self.proxyfilename, version=__version__)
        self.logger.debug('Looking up report for task %s' % self.cachedinfo['RequestName'])
        dictresult, status, reason = server.get(self.uri, data = {'workflow': self.cachedinfo['RequestName'], 'subresource': 'report'})

        self.logger.debug("Result: %s" % dictresult)

        if status != 200:
            msg = "Problem retrieving report:\ninput:%s\noutput:%s\nreason:%s" % (str(self.cachedinfo['RequestName']), str(dictresult), str(reason))
            raise RESTCommunicationException(msg)



        crab_data =tarfile.open(self.crab_dir+"/inputs/debugFiles.tgz")    
        config_file = crab_data.extractfile(crab_data.getmember("debug/crabConfig.py"))
        exec(''.join(config_file.readlines()))
        print config.Data.inputDataset
#        print dictresult[0]['lumisToProcess']

        try:
            job_lumis = dictresult['result'][0]['lumisToProcess']
            print job_lumis
            job_data = {}
            for key in job_lumis.keys():
                job_data[key] = make_job_data(job_lumis=job_lumis[key],dataset=config.Data.inputDataset)
            

        except KeyError:
            print "error"
            return {}

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='checks if the crab output is ')
    parser.add_argument('crab_dirs',nargs='+',help='list of crab directories to check')
    args = parser.parse_args()

    for crab_dir in args.crab_dirs:
        tblogger, logger, memhandler = initLoggers()
        obj = CrabJobInputs(logger,crab_dir)
        res = obj()
