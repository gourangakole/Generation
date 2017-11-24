#!/usr/bin/perl

# ----------------------------------------------------------------------------
#      MAIN PROGRAM
# ----------------------------------------------------------------------------

use Env;

#PG lettura dei parametri da cfg file
#PG --------------------------------
print "reading ".$ARGV[0]."\n" ;

open (USERCONFIG,$ARGV[0]) ;

while (<USERCONFIG>)
{
    chomp; 
    s/#.*//;                # no comments
    s/^\s+//;               # no leading white
    s/\s+$//;               # no trailing white
#    next unless length;     # anything left?
    my ($var, $value) = split(/\s*=\s*/, $_, 2);
    $User_Preferences{$var} = $value;
}

$BASEDir          = $User_Preferences{"BASEDir"} ;
$JOBS_dir         = $User_Preferences{"JOBS_dir"} ;
$EXEName          = $User_Preferences{"EXEName"} ;
$JOBCfgTemplate   = $User_Preferences{"JOBCfgTemplate"} ;
$HEPMCinput       = $User_Preferences{"HEPMCinput"} ;
$OUTPUTSAVEPath   = $User_Preferences{"OUTPUTSAVEPath"} ;
$OUTPUTFILEName   = $User_Preferences{"OUTPUTFILEName"} ;
$EVENTSNumber     = $User_Preferences{"EVENTSNumber"} ;
$EVENTSPerjob     = $User_Preferences{"EVENTSPerjob"} ;
$SCRAM_ARCH       = $User_Preferences{"SCRAM_ARCH"} ;
$X509_USER_PROXY  = $User_Preferences{"X509_USER_PROXY"} ;
$QUEUE            = $User_Preferences{"QUEUE"};




print "BASEDir = "          .$BASEDir."\n" ;
print "JOBS_dir = "         .$JOBS_dir."\n" ;
print "EXEName = "          .$EXEName."\n" ;
print "JOBCfgTemplate = "   .$JOBCfgTemplate."\n" ;
print "HEPMCinput  = "      .$HEPMCinput ."\n" ;
print "OUTPUTSAVEPath = "   .$OUTPUTSAVEPath."\n" ;
print "OUTPUTFILEName = "   .$OUTPUTFILEName."\n" ;
print "EVENTSNumber = "     .$EVENTSNumber."\n" ;
print "EVENTSPerjob = "     .$EVENTSPerjob."\n" ;
print "SCRAM_ARCH = "       .$SCRAM_ARCH."\n" ;
print "X509_USER_PROXY = "  .$X509_USER_PROXY."\n" ;
print "QUEUE  = "           .$QUEUE."\n\n" ;





$sampleJobListFile = "./lancia.sh";
open(SAMPLEJOBLISTFILE, ">", $sampleJobListFile);


#####################################################
# PG prepare the array containing the root files list
#####################################################
{
    print("Sample: ".$JOBS_dir." \n") ;  

    system ("rm -r ".$JOBS_dir." \n") ;
    system ("mkdir ".$JOBS_dir." \n") ;
   

    $jobNumber = 0;
  
    $jobNumber = int($EVENTSNumber/$EVENTSPerjob);
    if( $EVENTSNumber%$EVENTSPerjob != 0)
    {
	$jobNumber = $jobNumber+1;
    }
    
    print "NumberOfJobs = ".$jobNumber."\n";
    
  
    ################
    # loop over jobs 
    ################
    
    for($jobIt = 1; $jobIt <= $jobNumber; ++$jobIt)
    { 
	$currDir = `pwd` ;
	chomp ($currDir) ;

        $FIRSTEVENT = 1 + ($jobIt-1)*$EVENTSPerjob;

        if( $jobIt == $jobNumber)
        {
	    $EVENTSPerjob = $EVENTSNumber - $FIRSTEVENT + 1;
        }

        $seed1 = $jobIt;
        $seed2 = 2*$jobIt;
        $seed3 = 3*$jobIt;
        $seed4 = 4*$jobIt;
    
	$jobDir = $currDir."/".$JOBS_dir."/JOB_".$jobIt ;
	system ("mkdir ".$jobDir." \n") ;
    
	$tempBjob = $jobDir."/bjob_".$jobIt.".sh" ;
	$command = "touch ".$tempBjob ;
	system ($command) ;
	$command = "chmod 777 ".$tempBjob ;
	system ($command) ;

        $tempo1 = "./tempo1" ;
	system ("cat ".$JOBCfgTemplate."   | sed -e s%OUTPUTFILEName%".$OUTPUTFILEName."_".$jobIt.
		                       "%g > ".$tempo1) ;

        $tempo2 = "./tempo2" ;
	system ("cat ".$tempo1."   | sed -e s%JOBID%".$jobIt.
		                       "%g > ".$tempo2) ;

        $tempo3 = "./tempo3" ;
	system ("cat ".$tempo2."   | sed -e s%FIRSTEVENT%".$FIRSTEVENT.
		                       "%g > ".$tempo3) ;

        $tempo4 = "./tempo4" ;
	system ("cat ".$tempo3."   | sed -e s%GRIDPACK%".$HEPMCinput.
		                       "%g > ".$tempo4) ;

        $tempo5 = "./tempo5" ;
	system ("cat ".$tempo4."   | sed -e s%EVENTSperJOB%".$EVENTSPerjob.
		                       "%g > ".$tempo5) ;

        $tempo6 = "./tempo6" ;
	system ("cat ".$tempo5."   | sed -e s%SEED1%".$seed1.
		                       "%g > ".$tempo6) ;
        
        $tempo7 = "./tempo7" ;
	system ("cat ".$tempo6."   | sed -e s%SEED2%".$seed2.
		                       "%g > ".$tempo7) ;
        
        $tempo8 = "./tempo8" ;
	system ("cat ".$tempo7."   | sed -e s%SEED3%".$seed3.
		                       "%g > ".$tempo8) ;

        $tempo9 = "./tempo9" ;
	system ("cat ".$tempo8."   | sed -e s%SEED4%".$seed4.
		                       "%g > ".$tempo9) ;
        
        $JOBCfgFile = $jobDir."/".$EXEName ;
	system ("mv ".$tempo9." ".$JOBCfgFile) ;
        system ("rm ./tempo*");
	
     
        
    ######################
    # make job files
    ######################    
    
	open (SAMPLEJOBFILE, ">", $tempBjob) or die "Can't open file ".$tempBjob;

	$command = "#!/bin/tcsh" ;
	print SAMPLEJOBFILE $command."\n";

	$command = "cd ".$BASEDir ;
	print SAMPLEJOBFILE $command."\n";

	$command = "setenv SCRAM_ARCH ".$SCRAM_ARCH ;
	print SAMPLEJOBFILE $command."\n";
    
	$command = "eval `scramv1 ru -csh`" ;
	print SAMPLEJOBFILE $command."\n";
    
	$command = "cd -" ;
	print SAMPLEJOBFILE $command."\n";

        $command = "export X509_USER_PROXY=".$X509_USER_PROXY;
	print SAMPLEJOBFILE $command."\n";

	$command = "eos mkdir ".$OUTPUTSAVEPath;
	print SAMPLEJOBFILE $command."\n";


	$command = "cmsRun ".$jobDir."/".$EXEName;
	print SAMPLEJOBFILE $command."\n";

	$command = "xrdcp ".$OUTPUTFILEName."_".$jobIt.".root root://eoscms.cern.ch/".$OUTPUTSAVEPath;
	print SAMPLEJOBFILE $command."\n";
        
        $command = "rm *.root";
	print SAMPLEJOBFILE $command."\n";

	
	############
	# submit job
	############
	
	$command = "bsub -cwd ".$jobDir." -q ".$QUEUE." ".$tempBjob."\n" ;  
	print SAMPLEJOBLISTFILE $command."\n";
    
    }

} 
