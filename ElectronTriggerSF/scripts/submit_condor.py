import os
import sys
import optparse
import shutil
import random
import subprocess


def GetDataSets(infile):
    """ Get list of datasets and corresponding output folder from a txt file"""
    ds={}
    f = open(infile, "r")
    for line in f:
      if line[0]=="#": continue
      line=line.split()
      if not len(line)==2: continue
      print line
      ds[line[0]]=line[1]
    
    f.close()
    return ds
      

def getDatasetComponents(dataset):

    """ query components of the dataset """

    #get files in the dataset
    print 'Querying',dataset,
    p = subprocess.Popen(['dasgoclient --query=\"file dataset=%s\"'%dataset],
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE,
                         shell=True)
    out, err = p.communicate()

    dataset_files=out.split()
    nfiles=len(dataset_files)
    print '- Found %d files'%nfiles

    return dataset_files


    
def buildCondorFile(opt,FarmDirectory):

    """ builds the condor file to submit the MC production """

    cmssw=os.environ['CMSSW_BASE']
    rand='{:03d}'.format(random.randint(0,123456))
	
    datasets=GetDataSets(opt.listfiles)
    #get list of datasets:
    
    if not len(datasets): 
        print('ERROR: no valid datasets found in %s, check the file\n'%(opt.listfiles))
        return None

    #condor submission file
    condorFile='%s/condor_generator_%s.sub'%(FarmDirectory,rand)
    print '\nWrites: ',condorFile
    with open (condorFile,'w') as condor:
        condor.write('executable = {0}/worker_{1}.sh\n'.format(FarmDirectory,rand))
        condor.write('output     = {0}/output{1}.out\n'.format(FarmDirectory,rand))
        condor.write('error      = {0}/output{1}.err\n'.format(FarmDirectory,rand))
        condor.write('log        = {0}/output{1}.log\n'.format(FarmDirectory,rand))
        condor.write('+JobFlavour = "testmatch"\n')
        OpSysAndVer = str(os.system('cat /etc/redhat-release'))
        if 'SLC' in OpSysAndVer:
            OpSysAndVer = "SLCern6"
        else:
            OpSysAndVer = "CentOS7"
        condor.write('requirements = (OpSysAndVer =?= "{0}")\n\n'.format(OpSysAndVer))
        condor.write('should_transfer_files = YES\n')
        condor.write('transfer_input_files = %s\n\n'%os.environ['X509_USER_PROXY'])
        for ds in datasets:       
            fList = getDatasetComponents(ds)
            outDir = opt.output+"/"+datasets[ds]
            if not os.path.isdir(outDir):
              print('mkdir '+outDir)
              os.mkdir(outDir)
            for i in range(len(fList)):
                filename=fList[i].split("/")[-1].split(".")[0]
                outfile='%s/%s_Skim.root'%(outDir,filename)
                #print ('arguments = %s %s\n'%(fList[i],outDir))
                if os.path.isfile(outfile): 
                    #print('file %s exists'%outfile); 
                    continue
                condor.write('arguments = %s %s\n'%(fList[i],outDir))
                condor.write('queue 1\n')

    workerFile='%s/worker_%s.sh'%(FarmDirectory,rand)
    with open(workerFile,'w') as worker:
        worker.write('#!/bin/bash\n')
        worker.write('startMsg="Job started on "`date`\n')
        worker.write('echo $startMsg\n')
        worker.write('export HOME=%s\n'%os.environ['HOME']) #otherwise, 'dasgoclient' won't work on condor
        worker.write('export X509_USER_PROXY=%s\n'%os.environ['X509_USER_PROXY'])
        worker.write('filename=`echo  $1 | rev | cut -d/ -f1 | cut -d. -f2- | rev`\n')
        worker.write('WORKDIR=`pwd`/${filename}; mkdir -pv $WORKDIR\n')
        worker.write('echo "Working directory is ${WORKDIR}"\n')
        worker.write('cd %s\n'%cmssw)
        worker.write('eval `scram r -sh`\n')
        worker.write('cd ${WORKDIR}\n')
        worker.write('echo "run nano_postproc.py"\n')
        worker.write('python $CMSSW_BASE/src/CMSAnalysis/ElectronTriggerSF/processors/run_analysis.py $2 root://cms-xrd-global.cern.ch/$1 --bi scripts/keep_in.txt --bo scripts/keep_and_drop_out.txt -b %s -w %s\n'%(opt.btagAlgo,opt.btagID))
        worker.write('\necho clean output\ncd ../\nrm -rf ${WORKDIR}\n')
        worker.write('echo ls; ls -l $PWD\n')
        worker.write('echo $startMsg\n')
        worker.write('echo job finished on `date`\n')
    os.system('chmod u+x %s'%(workerFile))

    return condorFile

def main():

    if not os.environ.get('CMSSW_BASE'):
      print('ERROR: CMSSW not set')
      sys.exit(0)
    
    cmssw=os.environ['CMSSW_BASE']

    #configuration
    usage = 'usage: %prog [options]'
    parser = optparse.OptionParser(usage)
    parser.add_option('-o', '--out',     dest='output',   help='output directory',           default='/eos/user/m/mpitt/PPS_ttbar/ElTrigger', type='string')
    parser.add_option('-i', '--list',    dest='listfiles',help='file with list of datasets', default='data/list_samples.txt', type='string')
    parser.add_option('-s', '--submit',  dest='submit',  help='submit jobs',       action='store_false')
    parser.add_option("-b", "--btagAlgo", dest="btagAlgo",
                      type="string", help="Btag algorithm: [DeepJet, DeepCSV]")
    parser.add_option("-w", "--wp", dest="btagID", type="int", default=0,
                      help="btagging working point (0:loose, 1:medium, 2:tight)")

    (opt, args) = parser.parse_args()
     
    #prepare directory with scripts
    FarmDirectory=os.environ['PWD']+'/FarmLocalNtuple'
    if not os.path.exists(FarmDirectory):  os.system('mkdir -vp '+FarmDirectory)
    print('\nINFO: IMPORTANT MESSAGE: RUN THE FOLLOWING SEQUENCE:')
    print('voms-proxy-init --voms cms --valid 72:00 --out %s/myproxy509\n'%FarmDirectory)
    os.environ['X509_USER_PROXY']='%s/myproxy509'%FarmDirectory

    #build condor submission script and launch jobs
    condor_script=buildCondorFile(opt,FarmDirectory)

    #submit to condor
    if opt.submit:
        os.system('condor_submit {}'.format(condor_script))
    else:
        print('condor_submit {}\n'.format(condor_script))
		

if __name__ == "__main__":
    sys.exit(main())

