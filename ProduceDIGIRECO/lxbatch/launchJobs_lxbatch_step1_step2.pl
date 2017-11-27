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

$BASEDir          = $User_Preferences{"BASEDir"};
$JOBS_dir         = $User_Preferences{"JOBS_dir"};
$LISTOFSamples    = $User_Preferences{"LISTOFSamples"} ;
$EXEName1         = $User_Preferences{"EXEName1"} ;
$EXEName2         = $User_Preferences{"EXEName2"} ;
$JOBCfgTemplate1  = $User_Preferences{"JOBCfgTemplate1"} ;
$JOBCfgTemplate2  = $User_Preferences{"JOBCfgTemplate2"} ;
$OUTPUTSAVEPath   = $User_Preferences{"OUTPUTSAVEPath"} ;
$OUTPUTFILEName   = $User_Preferences{"OUTPUTFILEName"} ;
$SCRAM_ARCH       = $User_Preferences{"SCRAM_ARCH"} ;
$X509_USER_PROXY  = $User_Preferences{"X509_USER_PROXY"} ;
$JOBModulo        = $User_Preferences{"JOBModulo"} ;
$QUEUE            = $User_Preferences{"QUEUE"};


print "BASEDir = "          .$BASEDir."\n" ;
print "JOBS_dir = "         .$JOBS_dir."\n" ;
print "LISTOFSamples = "    .$LISTOFSamples."\n" ;
print "EXEName1 = "         .$EXEName1."\n" ;
print "EXEName2 = "         .$EXEName2."\n" ;
print "JOBCfgTemplate1 = "  .$JOBCfgTemplate1."\n" ;
print "JOBCfgTemplate2 = "  .$JOBCfgTemplate2."\n" ;
print "OUTPUTSAVEPath = "   .$OUTPUTSAVEPath."\n" ;
print "OUTPUTFILEName = "   .$OUTPUTFILEName."\n" ;
print "JOBModulo = "        .$JOBModulo."\n" ;
print "SCRAM_ARCH = "       .$SCRAM_ARCH."\n" ;
print "X509_USER_PROXY = "  .$X509_USER_PROXY."\n" ;
print "QUEUE  = "           .$QUEUE."\n" ;



$sampleJobListFile = "./lancia.sh";
open(SAMPLEJOBLISTFILE, ">", $sampleJobListFile);


#####################################################
# PG prepare the array containing the root files list
#####################################################


system("cd ".$BASEDir."\n");
    
chomp($_);
    
print("Sample: ".$JOBS_dir."\n") ;  

system ("rm -r ".$JOBS_dir."\n") ;
system ("mkdir ".$JOBS_dir."\n") ;
    
 
$LISTOFFiles = "./list_.txt" ;
system ("eos ls ".$LISTOFSamples." | grep root > ".$LISTOFFiles."\n") ;
  
  
$totNumber = 0;
$jobNumber = 0;
  
open (LISTOFFiles,$LISTOFFiles) ;
 while (<LISTOFFiles>)
{
	++$totNumber;
}

$jobNumber = int($totNumber/$JOBModulo);
if( $totNumber%$JOBModulo != 0)
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
    
	$jobDir = $currDir."/".$JOBS_dir."/JOB_".$jobIt ;
	system ("mkdir ".$jobDir." \n") ;
    
	$tempBjob = $jobDir."/bjob_".$jobIt.".sh" ;
	$command = "touch ".$tempBjob ;
	system ($command) ;
	$command = "chmod 777 ".$tempBjob ;
	system ($command) ;
    


        $totalSEED = $jobIt*$SEED;
    
	$tempo1 = "./tempo1" ;
	system ("cat ".$JOBCfgTemplate1."   | sed -e s%OUTPUTFILENAME%".$OUTPUTFILEName."_".$jobIt.
		                       "%g > ".$tempo1) ;
    
	$it = 0;
	$JOBLISTOFFiles;

	open (LISTOFFiles2,$LISTOFFiles) ;
	while (<LISTOFFiles2>)
	{
	    chomp; 
	    s/#.*//;                # no comments
	    s/^\s+//;               # no leading white
	    s/\s+$//;               # no trailing white
	    $file = $_ ;
	    
	    if( ($it >= ($jobIt - 1)*$JOBModulo) && ($it < ($jobIt)*$JOBModulo) )
	    { 
                #print $JOBLISTOFFiles."APICE".$file."APICE,";
		#$JOBLISTOFFiles = "APICE".$INPUTSAVEPath."/".$sample."/".$file."APICE,";
		$JOBLISTOFFiles = $JOBLISTOFFiles."APICE".$LISTOFSamples.$file."APICE,";
	    }
	    ++$it;
	}
	
        
	$tempo2 = "./tempo2" ;    
	system ("cat ".$tempo1." | sed -e s%LISTOFFILES%".$JOBLISTOFFiles."%g > ".$tempo2) ;
	$JOBLISTOFFiles = "" ;

	$tempo3 = "./tempo3" ;
	system ("cat ".$tempo2." | sed -e s%APICE%\\'%g > ".$tempo3) ;

        $tempo4 = "./tempo4" ;
	system ("cat ".$tempo3."   | sed -e s%OUTPUTFILEName%".$OUTPUTFILEName."_".$jobIt.".root".
		                       "%g > ".$tempo4) ;
        $tempo5 = "./tempo5" ;
	system ("cat ".$tempo4."   | sed -e s%SEED%".$jobIt.
		                       "%g > ".$tempo5) ;
    
        $tempo6 = "./tempo6" ;
	system ("cat ".$JOBCfgTemplate2."   | sed -e s%OUTPUTFile%".$OUTPUTFILEName."_".$jobIt.
		                       "%g > ".$tempo6) ;
    
	$JOBCfgFile1 = $jobDir."/".$EXEName1 ;
	system ("mv ".$tempo5." ".$JOBCfgFile1) ;
        $JOBCfgFile2 = $jobDir."/".$EXEName2 ;
	system ("mv ".$tempo6." ".$JOBCfgFile2) ;
	system ("rm ./tempo*") ;
    
    
    
    
    
    
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

	$command = "cmsRun ".$jobDir."/".$EXEName1;
	print SAMPLEJOBFILE $command."\n";

        $command = "cmsRun ".$jobDir."/".$EXEName2;
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

    system ("rm ".$LISTOFFiles) ;
  
