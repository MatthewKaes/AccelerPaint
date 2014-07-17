#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS

#pragma warning(push)
#pragma warning(disable : 4290)
#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.hpp"
#endif
#pragma warning(pop)

#include <unordered_map>


//Structs used to talk to the GPU
typedef struct color
{
  unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	unsigned char Alpha;
} color;

typedef struct rect
{
  unsigned x;
	unsigned y;
	unsigned width;
	unsigned height;
} rect;

typedef struct image
{
  rect pos_data;
  unsigned char* rgb_data;
  unsigned char* alpha_data;
  float opacity;
} image;

class OpenCL_Dev {

public:
  OpenCL_Dev();
  void Init();
  void Build_Kernel(const char* name);
  bool Fill(image img_data, rect fill_region, color fill_color);
  bool Blend(image img_base, image img_forground);
  bool Blur(image img_base);

private:
  cl::vector<cl::Platform> platforms_;
  cl::Context context_;
  cl::vector<cl::Device> devices_;
  cl::CommandQueue queue_;
  std::unordered_map<const char*, cl::Kernel*> kernels;
  bool opencl_enabled;
};