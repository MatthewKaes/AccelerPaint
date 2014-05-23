#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS

#ifdef __APPLE__
    #include "OpenCL/opencl.h"
#else
    #include "CL/cl.hpp"
#endif

class OpenCL_Dev {

public:
  OpenCL_Dev();

private:
  cl::vector<cl::Platform> platforms_;
  cl::Context context_;
  cl::vector<cl::Device> devices_;
  bool opencl_enabled;
};