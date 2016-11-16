#stolen from HLTrigger/Configuration/python/Tools/dasFileQuery.py
#also parts stolen  from edmPickEvents.py

import sys
import json
import das_client
import os
import pwd
def x509():
    "Helper function to get x509 either from env or tmp file"
    proxy = os.environ.get('X509_USER_PROXY', '')
    if  not proxy:
        proxy = '/tmp/x509up_u%s' % pwd.getpwuid( os.getuid() ).pw_uid
        if  not os.path.isfile(proxy):
            return ''
    return proxy

def dasFileQuery(dataset,runnr):
  query   = 'dataset dataset=%s' % dataset
  if runnr!=-1: query+=" run=%s" % runnr
  host    = 'https://cmsweb.cern.ch'      # default
  idx     = 0                             # default
  limit   = 0                             # unlimited
  debug   = 0                             # default
  thr     = 300                           # default
  ckey    = x509()                            # default
  cert    = x509()                            # default
  jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)

  # check if the pattern matches none, many, or one dataset
  if not jsondict['data'] or not jsondict['data'][0]['dataset']:
    sys.stderr.write('Error: the pattern "%s" does not match any dataset\n' % dataset)
    sys.exit(1)
    return []
  elif len(jsondict['data']) > 1:
    sys.stderr.write('Error: the pattern "%s" matches multiple datasets\n' % dataset)
    for d in jsondict['data']:
      sys.stderr.write('    %s\n' % d['dataset'][0]['name'])
    sys.exit(1)
    return []
  else:
    # expand the dataset name
    dataset = jsondict['data'][0]['dataset'][0]['name']
    query = 'file dataset=%s' % dataset
    if runnr!=-1: query+=" run=%s" % runnr
    jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
    # parse the results in JSON format, and extract the list of files
    files = sorted( f['file'][0]['name'] for f in jsondict['data'] )
    return files


class Event (dict):

    dataset = None
    import re
    splitRE = re.compile (r'[\s:,]+')
    def __init__ (self, line, **kwargs):
        pieces = Event.splitRE.split (line.strip())
        try:
            self['run']     = int( pieces[0] )
            self['lumi']    = int( pieces[1] )
            self['event']   = int( pieces[2] )
            self['dataset'] =  Event.dataset
        except:
            raise RuntimeError, "Can not parse '%s' as Event object" \
                  % line.strip()
        if not self['dataset']:
            print "No dataset is defined for '%s'.  Aborting." % line.strip()
            raise RuntimeError, 'Missing dataset'

    def __getattr__ (self, key):
        return self[key]

    def __str__ (self):
        return "run = %(run)i, lumi = %(lumi)i, event = %(event)i, dataset = %(dataset)s"  % self



def getFileNames (event):
  
  query = "file dataset=%(dataset)s run=%(run)i lumi=%(lumi)i | grep file.name" % event
  host    = 'https://cmsweb.cern.ch'      # default
  idx     = 0                             # default
  limit   = 0                             # unlimited
  debug   = 0                             # default
  thr     = 300                           # default
  ckey    = x509()                            # default
  cert    = x509()                            # default
  print "getting file containing run %(run)i lumi %(lumi)i for %(dataset)s" % event
  jsondict = [] #das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
  attemptNr=1
  while "data" not in jsondict and attemptNr<=3:
    if attemptNr>1:
      print "query failed (likely das error), retrying"
    jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
    attemptNr+=1


  files=[]
  if "data" not in jsondict:
    print "tried ",attempNr," times, failed to reach das, skipping  run=%(run)i lumi=%(lumi)i" % event
    return files


  for f in jsondict['data']:
    if len(f['file'])>0:
      files.append(f['file'][0]['name'])
    else:
      print "run %(run)i lumi %(lumi)i in %(dataset)s not found in das" % event 
         
  return files
      
                    
#  files = sorted( f['file'][0]['name'] for f in jsondict['data'] )
#  print files
                   #return files
  


def getLumis(filename):
 
  query = "lumi file=%s" % filename
  host    = 'https://cmsweb.cern.ch'      # default
  idx     = 0                             # default
  limit   = 0                             # unlimited
  debug   = 0                             # default
  thr     = 300                           # default
  ckey    = x509()                            # default
  cert    = x509()                            # default
#    print "getting file containing run %(run)i lumi %(lumi)i for %(dataset)s" % event
  jsondict = [] #das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
  attemptNr=1
   # print query
  while "data" not in jsondict and attemptNr<=3:
    if attemptNr>1:
      print "query failed (likely das error), retrying"
    jsondict = das_client.get_data(host, query, idx, limit, debug, thr, ckey, cert)
    attemptNr+=1

  lumis=[]
  for f in jsondict['data']:
    if len(f['lumi'])>0:
      #print f['lumi'][0]
      for lumiPair in f['lumi'][0]['number']:
        lumis.append(lumiPair)
    else:
      print "file not found ",filename
  return lumis
    
def getFileNamesCache(event,fileCache):

  
  runnr = event['run']
  lumi = event['lumi']
  if runnr in fileCache:
    runCache=fileCache[runnr]
    if lumi in runCache:
      return runCache[lumi]
  else:
    fileCache[runnr]={}
    
  files=getFileNames(event)
  for filename in files:
 #   print filename
    lumis=getLumis(filename)
 #   print lumis

    runCache=fileCache[runnr]
    for lumiPair in lumis:
      for lumi in range(lumiPair[0],lumiPair[1]+1):
        if lumi not in runCache:
          runCache[lumi]=set()
        runCache[lumi].add(filename)

#  print runCache

  return files
