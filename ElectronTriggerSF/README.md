# Electron trigger SF tools

Contains scripts to extract trigger SF tools from the data using CMSSW

## CMSSW setup
```
cmsrel CMSSW_10_6_27
cd CMSSW_10_6_27/src
cmsenv

#setup nanoAOD-tools
git clone https://github.com/cms-nanoAOD/nanoAOD-tools.git PhysicsTools/NanoAODTools
scram b -j

#This package
git clone git@github.com:michael-pitt/CMSAnalysis.git
scram b -j
```

## Analysis

Analysis module is in [Trigger_analysis.py](https://github.com/michael-pitt/CMSAnalysis/blob/main/ElectronTriggerSF/python/Trigger_analysis.py), which select events, and store relevant branches used to derive electron trigger SF

inputs:
- year: can be 2016 pre/post PFV, 2017 or 2018
- bTagAlgo: DeepJet or DeepCSV
- btagID: b-tagging working point 0 for loose, 1 for medium and 2 for tight

json luminosity files:
[combined_RPIN_CMS_LOWMU.json](https://github.com/michael-pitt/PPSTools/blob/main/LowPU2017H/data/combined_RPIN_CMS_LOWMU.json)

### Running on a single file:

example of running on a file from `SingleMuon` stream
```
python $CMSSW_BASE/src/CMSAnalysis/ElectronTriggerSF/processors/run_analysis.py \
output root://cms-xrd-global.cern.ch//store/data/Run2017B/SingleMuon/NANOAOD/UL2017_MiniAODv2_NanoAODv9-v1/120000/09FD9FD6-A164-9A45-80BB-F3D1FBF9C462.root \
--bi scripts/keep_in.txt --bo scripts/keep_and_drop_out.txt -b "DeepCSV" -w 1

```

example of running on a file from MC sample
```
python $CMSSW_BASE/src/CMSAnalysis/ElectronTriggerSF/processors/run_analysis.py \
output root://cms-xrd-global.cern.ch//store/mc/RunIISummer20UL17NanoAODv9/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/2510000/10C33CC0-846D-134B-99EA-F7E4200E9A48.root \
--bi scripts/keep_in.txt --bo scripts/keep_and_drop_out.txt -b "DeepCSV" -w 1
```

### Submitting to condor

To submit condor jobs for an entire data set.
Start a local proxy under the data directory:

```
voms-proxy-init --voms cms --valid 172:00 --out data/voms_proxy.txt
```

Then call:

```
python scripts/processDataset.py  -i /SingleMuon/Run2017H-UL2017_MiniAODv1_NanoAODv2-v1/NANOAOD -o /eos/user/p/psilva/data/sdanalysis/SingleMuon/Chunks
```
