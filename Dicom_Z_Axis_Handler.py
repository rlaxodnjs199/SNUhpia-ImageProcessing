# ##############################################################################
# DCMtoVidaCT_v1a_20190605.py
# ##############################################################################
# To generate analyze "zunu_vida-ct.hdr" & "zunu_vida-ct.img" from dicom images
# ##############################################################################
# Using SimpleITK
# ##############################################################################
# 6/5/2019, Jiwoong Choi
#  - cleaned up.
#  - prepare Vida case folders, e.g. 1234/, in one folder.
# 9/27/2016, Jiwoong Choi
#  - generate_zunu_vida-ct_UT_2_CBNCT_20160927.py < generate_zunu_vida-ct_UT_2.py
# -4/3/2016, Babak Haghighi
#  - generate_zunu_vida-ct_UT_2.py
# ##############################################################################
import os
import SimpleITK as sitk
# ----------------------------------------------------------------------------
# The following three lines are examples of paths

pathProj = "../todo"
#pathProj = "/data5/jiwchoi/IR/IR_PM25/ImageData/VIDA_PM25_Samples_EX"
#pathProj = "E:\\jiwchoi\\ImageData\\20180827-0903\\Prone_CBNU\\VIDA_Results"

# ----------------------------------------------------------------------------
os.chdir(pathProj)
path = os.getcwd()
print ("")
print ("---------------------------------------------------")
print (" PROGRAM BEGINS (Generate zunu_vida-ct.img & .hdr)")
print ("---------------------------------------------------")
print (" Reading project directory:", path)
n = 0
print ("===================================================")

nProj = 1
for k in range(nProj):

    ImageDir = []
    nImage = 0
    for name in sorted(os.listdir(path)):
        nImage = nImage + 1
        ImageDir.append(name)  

    print (" Number of images:", nImage, ImageDir)
    print (" ------------------------------------------------------------------------")

    i = 0
    for i in range(nImage):
        pathImage = os.path.join(pathProj,ImageDir[i]) # for Proj/Img
        pathDicom = pathImage + "/dicom"
        print ("  " + "/" + ImageDir[i] + "/")
        reader = sitk.ImageSeriesReader()
        filenamesDICOM = reader.GetGDCMSeriesFileNames(pathDicom)
        reader.SetFileNames(filenamesDICOM)
        imgOriginal = reader.Execute()
        print ("    The origin after creating DICOM:", imgOriginal.GetOrigin())
        # Flip the image. 
        # The files from Apollo have differnt z direction. 
        # Thus, we need to flip the image to make it consistent with Apollo.
        flipAxes = [ False, False, True ]
        flipped = sitk.Flip(imgOriginal,flipAxes,flipAboutOrigin=True)
        print ("    The origin after flipping DICOM:", flipped.GetOrigin())
        # Move the origin to (0,0,0)
        # After converting dicom to .hdr with itkv4, the origin of images changes. 
        # Thus we need to reset it to (0,0,0) to make it consistent with Apollo files.
        origin = [0.0,0.0,0.0]
        flipped.SetOrigin(origin)
        print ("    The origin after flipping and changing origin to [0.,0.,0.]:", flipped.GetOrigin())
        sitk.WriteImage(flipped,pathImage + "/" + "zunu_vida-ct.hdr")
	
        print ("    " + "/" + ImageDir[i] + "/" + "zunu_vida-ct.img & .hdr", "----> written") 
#       print ("    " + "\\" + ImageDir[i] + "\\" + "zunu_vida-ct.img & .hdr", "----> written") 
        n += 1
        print ("   ", i+1, "/", nImage, " images processed all in Vida/")
        print (" ------------------------------------------------------------------------")
        print ("===================================================")

#   print "==================================================="

print ("zunu_vida-ct.img/.hdr are created for {0} images".format(n)) 
print ("-------------------------------------------------")
print (" PROGRAM ENDS (Generate zunu_vida-ct.img & .hdr)")
print ("-------------------------------------------------")
