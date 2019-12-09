import os
from os import listdir
import nibabel as nib
import gzip

def extract_gz(source_path, dest_path, block_size=65536):
  with gzip.open(source_path, 'rb') as input_file, \
  open(dest_path, 'wb') as output_file:
    while True:
      block = input_file.read(block_size)
      if not block:
        break
      else:
        output_file.write(block)
    output_file.write(block)

path = os.getcwd() + '/image/'
imgPathList = [path + f for f in os.listdir(path) if f.endswith('.img.gz')]
unzippedImgList = []
for imgPath in imgPathList:
  extract_gz(imgPath, imgPath[:-3])
  unzippedImgList.append(imgPath[:-3])
for imgPath in unzippedImgList:
  img = nib.load(imgPath)
  nib.save(img, imgPath.replace('.img', '.nii'))
  os.remove(imgPath)