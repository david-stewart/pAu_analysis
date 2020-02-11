#!/bin/csh

# used to submit sequential jobs on the grid

# first make sure program is updated and exists
 make bin/pAuHTdijetUE || exit

set ExecPath = `pwd`
set execute = './bin/pAuHTdijetUE'
set numevents = -1
set base = /tier2/home/groups/rhi/STAR/Data/P16id/production_pAu200_2015/HT/pAu_2015_200_HT
set outDir = UE
set outFile = HTdijetUE

# Create the folder name for output
#set outFile = stock
# Make the directories since they may not exist...                                                                                                                             
if ( ! -d out/${outFile} ) then
mkdir -p out/${outFile}
endif

if ( ! -d log/${outFile} ) then
mkdir -p log/${outFile}
endif

# Now Submit jobs for each data file                                                                                                                                           
foreach input ( ${base}* )

# Create the output file base name                                                                                                                                             
set OutBase = `basename $input | sed 's/.root//g'`
set uscore = "_"
set OutBase = "$OutBase$uscore$outFile"
    
# Make the output names and path                                                                                                                                               
set outLocation = out/${outDir}/
set outName = ${OutBase}.root

# Input files                                                                                                                                                                  
set Files = ${input}

# Logfiles. Thanks cshell for this "elegant" syntax to split err and out                                                                                                       
set LogFile     = log/${outDir}/${OutBase}.log
set ErrFile     = log/${outDir}/${OutBase}.err

echo "Logging output to " $LogFile
echo "Logging errors to " $ErrFile
    
set arg = "$Files $outLocation$outName $numevents"

echo "now submitting this script: "
echo qsub -V -l mem=16GB -o $LogFile -e $ErrFile -N $1 -- ${ExecPath}/submit/qwrap.sh ${ExecPath} $execute $arg
    

qsub -V -q erhiq -l mem=16GB -o $LogFile -e $ErrFile -N pAu_analysis -- ${ExecPath}/submit/qwrap.sh ${ExecPath} $execute $arg

end