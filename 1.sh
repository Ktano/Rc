#!/bin/bash

olddir="/home/user/protected-pdf-files/"

newdir="/home/user/writable-pdf-files/"

cd $olddir

for file in *.pdf; do
  echo "Deactivating DRM for $file"
  qpdf --decrypt "$file" $newdir/"$file"
done

exit 0
