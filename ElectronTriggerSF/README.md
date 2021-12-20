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

json luminosity files are stored [here](https://github.com/michael-pitt/CMSAnalysis/tree/main/ElectronTriggerSF/data/json)

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
voms-proxy-init --voms cms --valid 172:00 --out FarmLocalNtuple/myproxy509
```

Then call:

```
python scripts/submit_condor.py -o /eos/user/m/mpitt/PPS_ttbar/ElTrigger -i data/list_samples.txt -s -b \"DeepCSV\" -w 1
```
## Results

Several scripts can be found under `ElectronTriggerSF/scripts` that are used to extract SF and making control plots.

   * Plot1DSF.cc: Ploting 1D SF as a function of a single variable
   * Plot1DSFsys.cc: Ploting 1D SF as a function of a single variable for different event selection
   * Plot1DSFRun.cc: Ploting 1D SF as a function of a single variable for different run conditions
   * To product final plot of SF and write corresponding file run `root -l -b -q 'Plot2DSF_final.cc("")'`
   * PrintCorrelation.cc - used to plot correlation factor
   * PrintFractions.cc - used to plot lumi-based fractions and SFs

