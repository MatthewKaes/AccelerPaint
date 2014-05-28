#include "Accel_Image.h"

Accel_Image::Accel_Image()
{
	iHeight = 0;
	iWidth = 0;
	iData = NULL;
  iRender = false;
	iDepth = DEFAULT_DEPTH;
  iColorDepth = UCHAR_MAX;
}
Accel_Image::Accel_Image(int width, int height, BYTE* data, int depth, unsigned max)
{
	iHeight = height;
	iWidth = width;
  iRender = false;
	iDepth = depth;
  iColorDepth = UCHAR_MAX;
	iData = new PIXEL[iHeight * iWidth * iDepth];
}
Accel_Image::~Accel_Image()
{
  if(iData)
    delete[] iData;
}

PIXEL Accel_Image::GetData(int x, int y, int d)
{
  return iData[(iWidth * iDepth * y) + (iDepth * x) + d];
}
void Accel_Image::SetData(int x, int y, int d, BYTE data)
{
  iData[(iWidth * iDepth * y) + (iDepth * x) + d] = data;
}

unsigned Accel_Image::GetHeight(){
  return iHeight;
}
unsigned Accel_Image::GetWidth(){
  return iWidth;
}
unsigned Accel_Image::GetDepth(){
  return iDepth;
}
void Accel_Image::ScaleTo(int width, int height, ScalerTypes scaleType)
{

}
void Accel_Image::LoadFrom(const wxString& path, const wxString& name)
{
  //Use wxWidgets as a loader
  wxImage temp_image;
  if (sImage.LoadFile(path) == TRUE)
  {
    iColorDepth = UCHAR_MAX;
    BuildFrom(temp_image.GetData(), temp_image.GetWidth(), temp_image.GetHeight(), DEFAULT_DEPTH);
  }
}
void Accel_Image::BuildFrom(BYTE *data, int width, int height, int depth)
{
  if(iData)
    delete[] iData;

  iHeight = height;
  iWidth = width;
  iDepth = depth;

  iData = new PIXEL[height * width * depth];
  BYTE* filler = iData;
  BYTE* getter = data;

  for(int pixels = iHeight * iWidth * iDepth; pixels > 0; pixels--)
  {
    filler = getter;
    filler++;
    getter++;
  }
  iRender = true;
}
void Accel_Image::MakeFrom(PIXEL data, int width, int height, int depth)
{
  if(iData)
    delete[] iData;

  iHeight = height;
  iWidth = width;
  iDepth = depth;

  iData = new PIXEL[height * width * depth];
  BYTE* filler = iData;
  BYTE* getter = data;

  for(int pixels = iHeight * iWidth * iDepth; pixels > 0; pixels--)
  {
    filler = getter;
    filler++;
    getter++;
  }
  iRender = true;
}
void Accel_Image::CopyFrom(Accel_Image* image)
{
  iColorDepth = image->iColorDepth;
  BuildFrom(image->iData, image->GetWidth(), image->GetHeight(), image->GetDepth());

}
