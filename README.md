# Generation

1) Install:

    * scram project CMSSW_9_2_7
    * cd CMSSW_9_2_7/src/
    * cmsenv
    * git clone  git@github.com:bmarzocc/Generation.git
    * cd Generation
    * scram b -j 5

2) Run GEN-SIM production on lxbatch:

    * launch step:
   
      * cd ProduceGENSIM/lxbatch/
      * perl launchJobs_lxbatch_GEN-SIM.pl params_lxbatch_GEN-SIM.CFG
      * sh lancia.sh
      
    * Parameters settings in "params_lxbatch_GEN-SIM.CFG":
   
      * BASEDir: absolute path of the lxbatch directory
      * JOBdir: directory to store job directories
      * JOBCfgTemplate: name of the python to launch
      * HEPMCinput: name and path of the gridpack to be run
      * OUTPUTSAVEPath: path of the output files (EOS directories as default)
      * OUTPUTFILEName: name of the output file.root
      * EVENTSNumber: total number of events to generate
      * EVENTSPerjob: number of events per job
      * EXEName: name of the job executable
      * SCRAM_ARCH: scram_arch
      * X509_USER_PROXY: path of the proxy (before launching better do: export X509_USER_PROXY=/afs/cern.ch/work//XXX/x509up_XXX)
      * QUEUE: lxbatch queue
      
    * NOTE: Any new GEN-SIM step cfg can be adapted in order to work with this package. Change/add the following lines:
    
      ...
      ```Pythonscript
      process.maxEvents = cms.untracked.PSet(
         input = cms.untracked.int32(EVENTSperJOB)
      )
      ```
      ...
      
      
3)

4) Run GEN analysis:

    * cd Generation/GenStudies
    * GenParticlesAnalysis python/GenParticlesAnalysis_cfg.py
