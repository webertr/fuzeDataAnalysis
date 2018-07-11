fileBase=180710
startPulse=180710001
startFile=1
numImages=36

for i in `seq 1 $numImages`; 
do

    newFile=$(printf "/home/fuze/DHI_Images/%s/%d.JPG" "$fileBase" "$startPulse")
    oldFile=$(printf "/home/fuze/DHI_Images/%s/DSC_00%02d.JPG" "$fileBase" "$startFile")
    mv -i -- "$oldFile" "$newFile"
    echo "Moving $oldFile to $newFile"
    let startFile=startFile+1
    newFile=$(printf "/home/fuze/DHI_Images/%s/%d_Baseline.JPG" "$fileBase" "$startPulse")
    oldFile=$(printf "/home/fuze/DHI_Images/%s/DSC_00%02d.JPG" "$fileBase" "$startFile")
    mv -i -- "$oldFile" "$newFile"
    echo "Moving $oldFile to $newFile"
    let startFile=startFile+1 
    let startPulse=startPulse+1

done
