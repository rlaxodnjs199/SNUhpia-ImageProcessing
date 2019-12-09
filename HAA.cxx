// Written by: Sanghun Choi
// This code is to extract Airtrapping and Emphysema from two images
// Updated : 09/07/2016
// Date	  : 09/02/2014
/////////////////////////////////////////////////////////////////////////////// */
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDisplacementFieldJacobianDeterminantFilter.h"
#include "metaCommand.h"
#include <iostream>
#include <math.h>
#include <vector>

#define HUAIR -1000
#define HUTISSUE 55

float PercentageOfTissue(const float imageValue);
template <class TImageType, class RefImageType>

float PercentageOfTissue(const float imageValue) {
  float value;
  value = imageValue < HUAIR? HUAIR : imageValue;
  value=imageValue > HUTISSUE? HUTISSUE : imageValue;
  return (value - HUAIR) / (HUTISSUE - HUAIR);
}

int main(int argc, char** argv) {
  MetaCommand command;
  command.DisableDeprecatedWarnings();
  command.SetParseFailureOnUnrecognizedOption(true);
/*----------------------------------------------------------------------------------*/
//Dimension
  command.SetOption("ImageDimension","imgDim",true,"Image Dimension (2|3)");
  command.AddOptionField("ImageDimension","int",MetaCommand::INT,true);

//Min Value to capture
  command.SetOption("cutoffTLC","incutoffTLC",false,"Cutoff Values for Emphysema");
  command.AddOptionField("cutoffTLC","int",MetaCommand::INT,false,"-700");

//Max Value to capture
  command.SetOption("cutoffFRC","incutoffFRC",false,"Cutoff Values for Airtrapping");
  command.AddOptionField("cutoffFRC","int",MetaCommand::INT,false,"-856");

//fixed(TLC) image input
  command.SetOption("InputfixedImageFileName","fixed",false,"InputfixedImageFileName");
  command.AddOptionField("InputfixedImageFileName","filename",MetaCommand::STRING,false,"");

//warped(moving) image input
  command.SetOption("InputwarpedImageFileName","warped",false,"InputwarpedImageFileName");
  command.AddOptionField("InputwarpedImageFileName","filename",MetaCommand::STRING,false,"");

//FRC image input
  command.SetOption("InputFRCImageFileName","iFRC",true,"InputFRCImageFileName");
  command.AddOptionField("InputFRCImageFileName","filename",MetaCommand::STRING,true);

//Input TLC mask FileName.
  command.SetOption("InputTLCLungMaskImageFileName","TLCMask",false,
		  "InputTLCLungMaskImageFileName");
  command.AddOptionField("InputTLCLungMaskImageFileName","filename",
		  MetaCommand::STRING,false,"");

//Input FRC mask FileName.
  command.SetOption("InputFRCLungMaskImageFileName","FRCMask",true,
		  "InputFRCLungMaskImageFileName");
  command.AddOptionField("InputFRCLungMaskImageFileName","filename",
		  MetaCommand::STRING,true);

//Output Result Prefix
  command.SetOption("OutputResultPrefix","outPrefix",true,"OutputResultPrefix");
  command.AddOptionField("OutputResultPrefix","filename",MetaCommand::STRING,true);

  command.SetOption("OutputResultSuffix","outSuffix",true,"OutputResultSuffix");
  command.AddOptionField("OutputResultSuffix","filename",MetaCommand::STRING,true);

  if (!command.Parse(argc,argv)) return 1;

  int minTLC = command.GetValueAsInt("cutoffTLC", "int");
  int maxTLC = command.GetValueAsInt("cutoffFRC", "int");

  std::string inputFRCImageFileName = 
	  command.GetValueAsString("InputFRCImageFileName", "filename");
  std::string inputFixedImageFileName = 
	  command.GetValueAsString("InputfixedImageFileName", "filename");
  std::string inputWarpedImageFileName = 
	  command.GetValueAsString("InputwarpedImageFileName", "filename");
  std::string inputTLCLungMaskImageFileName = 
	  command.GetValueAsString("InputTLCLungMaskImageFileName", "filename");
  std::string inputFRCLungMaskImageFileName = 
	  command.GetValueAsString("InputFRCLungMaskImageFileName", "filename");

  std::string outPutPrefix = command.GetValueAsString("OutputResultPrefix", "filename");
  std::string outPutSuffix = command.GetValueAsString("OutputResultSuffix", "filename");

// csh for writing lobar ventilation
  std::string outputResultFileName = outPutPrefix+"_lobar_RangeCapture.txt";

// short
  typedef short PixelType;
  typedef float OutputPixelType; // 1D code is OK only for float

  const unsigned int ImageDimension=3;
  typedef itk::Image<PixelType, ImageDimension>  ImageType;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;

  typedef ImageReaderType::Pointer ImageReaderPointer;

  typedef ImageType::Pointer       ImagePointer;
  typedef ImageType::RegionType    RegionType;

//define input images (for lobar masks)
  typedef itk::Image<unsigned char, ImageDimension>  ImageMaskType;
  typedef itk::ImageFileReader<ImageMaskType>        MaskReaderType;	

//define output images
  typedef itk::Image<OutputPixelType, ImageDimension> OutputImageType;
  typedef OutputImageType::Pointer  OutputImagePointer;
  typedef itk::ImageRegionIterator<OutputImageType> OutIteratorType;

// for writing
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typedef WriterType::Pointer                     ImageWriterPointer;

// for Iterator
  typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;

// define Pointer
  ImageReaderPointer fixedImageReader  = ImageReaderType::New();

//Reading lung segmentation image
  MaskReaderType::Pointer TLCmaskReader = MaskReaderType::New();

// For reading: Read input file
  fixedImageReader->SetFileName(inputFixedImageFileName.c_str());
  fixedImageReader->Update();

// maskReader is vida-lobes.img -----------------------------------------------
  TLCmaskReader->SetFileName(inputTLCLungMaskImageFileName.c_str());
  TLCmaskReader->Update();
//-----------------------------------------------------------------------------

// Reader -> imgfile
  ImagePointer fixedImage = fixedImageReader->GetOutput();
  ImageMaskType::Pointer TLCLungMask = TLCmaskReader->GetOutput();

// RegionTypes are defined 
  RegionType TLCLungMaskRegion = TLCLungMask->GetBufferedRegion();	
  RegionType fixedImageRegion = fixedImage->GetBufferedRegion();	

  ImageType::SpacingType spacing = fixedImage->GetSpacing();

  float volumeOfVoxel = 1.0;
  for(unsigned int i=0; i<ImageDimension; i++) volumeOfVoxel *= spacing[i];

// fixedImageRegion is pointer and fixedLungMask->GetDirection()
  std::cerr<<"TLCImage: "<< fixedImageRegion << fixedImage->GetDirection()<<std::endl;
  std::cerr<<"TLCMaskImage:  "<< TLCLungMaskRegion << TLCLungMask->GetDirection()<<std::endl;

  std::cerr<<"TLC voxel size= "<<volumeOfVoxel<<std::endl;

// Define Iterator Regions	
  IteratorType fixedImageI( fixedImage, fixedImageRegion );

  bool isSameForMaskImageRegion=( TLCLungMaskRegion==fixedImageRegion );
  if(!isSameForMaskImageRegion)return 1;

  fixedImageI.GoToBegin();
	
  long count(0);
//int voxelsAirT[5],voxelsLobe[5];

  std::vector<int> voxelsTarget;
  std::vector<int> voxelsLobe;

  voxelsTarget.assign(5,0);
  voxelsLobe.assign(5,0);

  std::fill(voxelsTarget.begin(),voxelsTarget.end(),0);
  std::fill(voxelsLobe.begin(),voxelsLobe.end(),0);

// Iteration part

  while (!fixedImageI.IsAtEnd()) {
    bool insideLung(false);

    ImageType::IndexType index = fixedImageI.GetIndex();

    if (TLCLungMask->GetPixel(index) != 0) insideLung=true;

    if (!insideLung) {
      ++fixedImageI;
      continue;					
    }

    float TLCValue = fixedImage->GetPixel(index);	
    unsigned int lobeValue = TLCLungMask->GetPixel(index);

    switch (lobeValue) {
      case 8:
        voxelsLobe[0] += 1;
        if (minTLC < TLCValue && TLCValue < maxTLC) {
          voxelsTarget[0] += 1;
        }
        break;

      case 16:
        voxelsLobe[1] += 1;
        if (minTLC < TLCValue && TLCValue < maxTLC) {
          voxelsTarget[1] += 1;
        }
        break;

      case 32:
        voxelsLobe[2] += 1;
        if (minTLC < TLCValue && TLCValue < maxTLC) {
          voxelsTarget[2] += 1;
        }
        break;

      case 64: 
        voxelsLobe[3] += 1;         
        if (minTLC < TLCValue && TLCValue < maxTLC){
          voxelsTarget[3] += 1;
        }
        break;

      case 128: 
        voxelsLobe[4] += 1;         
        if (minTLC < TLCValue && TLCValue < maxTLC) {
            voxelsTarget[4] += 1;
        }
        break;
    }
    ++fixedImageI;
    ++count;
  }
// Finished point of while

  std::cerr<<"Lobe   HUrange  NVoxels"<<std::endl;
  for(unsigned int i=0;i<5;i++){
  std::cout<<"Lobe"<<i<<" "<<" "<< voxelsTarget[i]
	  <<" "<<voxelsLobe[i]<<std::endl;
  }

  int numberofvoxels_Target(0);
  int numberofvoxels_total(0);
  double pctgTargetAll(0.0);

  std::vector<double> pctgTarget;
  pctgTarget.assign(5,0.0);

  for (unsigned int i=0;i<5;i++) {
    numberofvoxels_Target += voxelsTarget[i];
    numberofvoxels_total += voxelsLobe[i];
  }

  for (unsigned int i=0;i<5;i++) {
    pctgTarget[i] = static_cast<double>(voxelsTarget[i]) / static_cast<double>(voxelsLobe[i]);
    pctgTargetAll = static_cast<double>(numberofvoxels_Target) / static_cast<double>(numberofvoxels_total);
  }
        
  std::ofstream result(outputResultFileName.c_str());

  result<<"Lobes  "<<"Target_ratio  "<<"voxels_Target  "<< "VoxelsAll"<<std::endl;
  for(unsigned int i=0;i<5;i++){
     result<<"Lobe"<<i<<" "<<pctgTarget[i]<<" "<<voxelsTarget[i]<<" "<<voxelsLobe[i]<<std::endl;
  }   

  result<<"Total "<<pctgTargetAll<<" "<<numberofvoxels_Target<<" "<<numberofvoxels_total<<std::endl;
  result.close();
}