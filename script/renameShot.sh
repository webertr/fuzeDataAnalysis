a=11

for i in *.JPG; do
    new=$(printf "1802150%2d.JPG" "$a") #04 pad to length of 4
    echo $new
    mv -i -- "$i" "$new"
    let a=a+1
done
