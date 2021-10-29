#!/bin/bash

proxy=${1}
input="root://cms-xrd-global.cern.ch/${2}"
output=${3}
cmssw=${4}
analysis=${5}
trigger=${6}
if [ -z "${analysis}" ]; then
    analysis=analysis_mu
fi
if [ -z "${trigger}" ]; then
    trigger=HLT_HIMu15
fi


cd ${cmssw}/src
eval `scram r -sh`
export X509_USER_PROXY=${proxy}
voms-proxy-info -all
voms-proxy-info -all -file ${proxy}
cd -

$CMSSW_BASE/src/PhysicsTools/NanoAODTools/scripts/nano_postproc.py output ${input} \
    --json $CMSSW_BASE/src/PPSTools/LowPU2017H/data/combined_RPIN_CMS_LOWMU.json \
    --bi $CMSSW_BASE/src/PPSTools/LowPU2017H/scripts/keep_Data_in.txt \
    --bo $CMSSW_BASE/src/PPSTools/LowPU2017H/scripts/keep_and_drop_Data_out.txt \
    -c "${trigger}" \
    -I PPSTools.LowPU2017H.LowPU_analysis ${analysis}

mv -v output/*.root ${output}
