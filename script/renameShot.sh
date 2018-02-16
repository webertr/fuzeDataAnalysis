startPulse=1
startFile=1
numImages=10

for i in `seq 1 $numImages`; 
do

    newFile=$(printf "1802150%02d.JPG" "$startPulse")
    oldFile=$(printf "DSC_00%02d.JPG" "$startFile")
    mv -i -- "$oldFile" "$newFile"
    echo "Moving $oldFile to $newFile"
    let startPulse=startPulse+1
    let startFile=startFile+1

done
