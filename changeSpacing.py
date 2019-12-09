from __future__ import print_function

import SimpleITK as sitk
import numpy as np
import sys, os

data_directory = "./dicomInput"
reader = sitk.ImageSeriesReader()
dicom_names = reader.GetGDCMSeriesFileNames(data_directory)
reader.SetFileNames(dicom_names)
image = reader.Execute()
size = image.GetSize()
spacing = image.GetSpacing()
print("Original image size(n_pixel):", size)
print("Original image spacing:", spacing)

image.SetSpacing(np.array(spacing) * 0.5)

resampled_size = image.GetSize()
resampled_spacing = image.GetSpacing()
print("Resampled image size(n_pixel):", resampled_size)
print("Resampled image spacing:", resampled_spacing)

writer = sitk.ImageFileWriter()
writer.KeepOriginalImageUIDOn()
#Bring input image_file_names and sort
inputImageList = os.listdir('./dicomInput')
inputImageList.sort()
#Write to the output_directory
for i in range(image.GetDepth()):
    resampled_image_slice = image[:,:,i]
    writer.SetFileName(os.path.join('./dicomOutput', inputImageList[i]))
    writer.Execute(resampled_image_slice)

#.img format으로 저장하기
#sitk.WriteImage( image, "3DVolume.img" )
#if ( not "SITK_NOSHOW" in os.environ ):
#    sitk.Show( image, "Dicom Series" )

#나중에 서로 다른 크기의 이미지 파일을 하나로 통일해야할 때 data loss를 감내하면서 사용해야할 메소드
#new_spacing = np.array(spacing) * 2
#new_size = (np.round(size * (np.array(spacing)/new_spacing))).astype(int).tolist()
#resampled_image = sitk.Resample(image, new_size, sitk.Transform(), sitk.sitkNearestNeighbor, image.GetOrigin(), new_spacing, image.GetDirection(), 0.0, image.GetPixelID())
#resampled_image_size = resampled_image.GetSize()
#resampled_image_spacing = resampled_image.GetSpacing()

#Store resampled_image in output directory
#for i in range(resampled_image.GetDepth()):
#    resampled_image_slice = resampled_image[:,:,i]
#    writer.SetFileName(os.path.join('./dicomOutput', str(inputImageList[i])))
#    writer.Execute(resampled_image_slice)
