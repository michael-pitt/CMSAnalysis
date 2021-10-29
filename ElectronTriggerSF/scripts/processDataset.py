import os
import sys
import optparse

def main():

    #configuration
    usage = 'usage: %prog [options]'
    parser = optparse.OptionParser(usage)
    parser.add_option('-i', '--in',     dest='input',  help='input dataset',    default='/SingleMuon/Run2017H-UL2017_MiniAODv2_NanoAODv9-v1/NANOAOD', type='string')
    parser.add_option('-o', '--out',    dest='output', help='output directory', default='/eos/user/p/psilva/data/sdanalysis/SingleMuon/Chunks',       type='string')
    parser.add_option('-a', '--analysis', dest='analysis', help='analysis',    default='analysis_mu', type='string')
    parser.add_option('-t', '--trigger',  dest='trigger',  help='trigger',     default='HLT_HIMu15', type='string')
    parser.add_option('--submit',         dest='submit', help='submit jobs',   action='store_true')
    (opt, args) = parser.parse_args()

    cmssw=os.environ['CMSSW_BASE']

    #warn user to start a proxy
    proxy='{}/src/PPSTools/LowPU2017H/data/voms_proxy.txt'.format(cmssw)
    print('Warning: assuming you have a valid proxy under {}'.format(proxy))
    print('If not the case, before submitting to condor, please start one with')
    print('voms-proxy-init --voms cms')
    print('cp `voms-proxy-info -p` {}'.format(proxy))
        
    #get list of files
    file_list=os.popen('dasgoclient --query=\"file dataset={} status=*\"'.format(opt.input)).read().split()

    #prepare output
    os.system('mkdir -p {}'.format(opt.output))

    #print a condor file
    condor_script='{}_{}.sub'.format( opt.input.replace('/','_')[1:],opt.analysis )
    scripts_dir='{}/src/PPSTools/LowPU2017H/scripts'.format(cmssw)
    with open (condor_script,'w') as condor:
        condor.write('Proxy_path = {}\n'.format(proxy))
        condor.write('executable = {}/lowpu_worker.sh\n'.format(scripts_dir))
        condor.write('arguments = $(Proxy_path) $(rfile) {} {} {} {}\n'.format(opt.output,cmssw,opt.analysis,opt.trigger))
        condor.write('output     = jobs$(ClusterId).out\n')
        condor.write('error      = jobs$(ClusterId).err\n')
        condor.write('log        = jobs$(ClusterId).log\n')
        condor.write('+JobFlavour = "testmatch"\n')
        condor.write('queue rfile from (\n')
        for f in file_list:
            condor.write('\t{}\n'.format(f.strip()))
        condor.write(')\n')

    #submit to condor
    if opt.submit:
        os.system('condor_submit {}'.format(condor_script))



if __name__ == "__main__":
    sys.exit(main())
