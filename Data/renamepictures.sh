#!/bin/bash
a=0
for i in *.jpg; do
  new=$(printf "test%d.jpg" "$a")
  mv -i -- "$i" "$new"
  let a=a+1
done

for f in *.jpg
do
# extract the size of the image
  SIZE=$(identify $f | awk '{print $3}')
  exiftool -all= $f

  case $SIZE in
  719x1280)
    mogrify -rotate 90 $f
  ;;

esac
done
