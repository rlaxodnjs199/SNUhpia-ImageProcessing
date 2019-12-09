#!/bin/bash
# ##############################################################################
# Airtrapping_v1.sh Subj HU0 HU1 mg0 Img1
# Airtrapping_v1.sh SN100001 -950 -856 IN EX
# Airtrapping_v1.sh LAM005 -960 -960 IN0 IN1
# ##############################################################################
# ---------------------
Path1=/data1/jiwchoi/IR
# ---------------------
Subj=$1 
cutoffTLC=$2
cutoffFRC=$3
level0=$4
level1=$5
# AirtrappingCommand=${codes_scripts}/Code/Airtrapping/Build/Airtrapping.exe
AirtrappingCommand=${Path1}/Code/Airtrapping/build/Airtrapping.exe

ImageTLCPrefix=${Subj}_${level0}
ImageFRCPrefix=${Subj}_${level1}

defResultDir=./   # ./SSTVD/
imageAndLobeDir=./   # ./analyzed/
registrationResultDir=./   # ./SSTVD/

ImageDimension=3

#TLC lobe
LobesTLCMaskPrefix=${ImageTLCPrefix}_vida-lobes
LobesTLCMask=${imageAndLobeDir}${LobesTLCMaskPrefix}.hdr

#FRC lobe
LobesFRCMaskPrefix=${ImageFRCPrefix}_vida-lobes
LobesFRCMask=${imageAndLobeDir}${LobesFRCMaskPrefix}.hdr

RegistrationPrefixFRC_TLC=${ImageFRCPrefix}-TO-${ImageTLCPrefix}-SSTVD
WarpedImageFRC_TLC=${registrationResultDir}${RegistrationPrefixFRC_TLC}.hdr

# Regional Ventilation
PixelType=SHORT
DefaultPixelValue=-1000
ImageTLC=${imageAndLobeDir}${ImageTLCPrefix}.hdr
ImageFRC=${imageAndLobeDir}${ImageFRCPrefix}.hdr

RVPrefixFRC_TLC=${registrationResultDir}${RegistrationPrefixFRC_TLC}
RVSuffixFRC_TLC=img
echo "Emphesyma and Airtrapping analysis for TLC and FRC, respectively"
$AirtrappingCommand -imgDim $ImageDimension -incutoffTLC $cutoffTLC -incutoffFRC $cutoffFRC -fixed $ImageTLC -warped $WarpedImageFRC_TLC -iFRC $ImageFRC -TLCMask $LobesTLCMask -FRCMask $LobesFRCMask -outPrefix $RVPrefixFRC_TLC -outSuffix $RVSuffixFRC_TLC 

