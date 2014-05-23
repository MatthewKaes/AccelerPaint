#include "CL_Device.h"

OpenCL_Dev::OpenCL_Dev()
{
  try { 
    // Get available platforms
    cl::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
 
    //Check to make sure that there is a platform avalible
    //that can use OpenCL.
    if(platforms.size() == 0)
    {
		  printf("No Platform Support for OpenCL 1.1 avalible.\n");
      throw NULL;
    }

    // Select the default platform and create a context using this platform and the GPU
    cl_context_properties cps[3] = { 
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)(platforms[0])(), 
        0 
    };
    cl::Context context( CL_DEVICE_TYPE_GPU, cps);
 
    // Get a list of devices on this platform
    devices_ = context.getInfo<CL_CONTEXT_DEVICES>();
 

    printf("OpenCL 1.1 started successfully.\n\n");
    std::string s = platforms[0].getInfo<CL_PLATFORM_NAME>();
    printf("Platform: %s\n", s.c_str());
    s = platforms[0].getInfo<CL_PLATFORM_VERSION>();
    printf("Version: %s\n", s.c_str());
    s = devices_[0].getInfo<CL_DEVICE_NAME>();
    printf("Device: %s\n", s.c_str());
    s = devices_[0].getInfo<CL_DRIVER_VERSION>();
    printf("Driver: %s\n\n", s.c_str());
    platforms_ = platforms;
    context_ = context;
    opencl_enabled = true;
  } 
  // Something horrible happend...
  catch(cl::Error error) 
  {
    // What exploded and the error code related to it.
		printf("A Error occured with loading OpenCL.\n");
    opencl_enabled = false;
  }
}