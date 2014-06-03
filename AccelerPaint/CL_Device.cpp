#include "CL_Device.h"
#include <fstream>

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
void OpenCL_Dev::Build_Kernel(const char* name)
{
  if(!opencl_enabled)
    return;

  cl::Program program;
  try { 
    //Get the shader file.
    std::string file_name("Shaders\\");
    file_name.append(name);
    file_name.append(".cl");      
      
    //Read in shader into a buffer
    std::ifstream sourceFile(file_name.c_str());
    std::string sourceCode(
        std::istreambuf_iterator<char>(sourceFile),
        (std::istreambuf_iterator<char>()));
    cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
 
    // Make program of the source code in the context
    cl::Program program = cl::Program(context_, source);
 
    // We can now build the Kernel on the fly.
    program.build(devices_);
 
    // Make kernel and give it a name so we can use it.
    cl::Kernel* kernel = new cl::Kernel(program, name);
 
    // Loads and stores loaded kernel
    kernels[name] = kernel;
  } 
  // Something horrible happend...
  catch(cl::Error error) 
  {
    // What exploded and the error code related to it.
    std::string error_msg("Failed to build ");
    error_msg.append(name);
    error_msg.append(".cl using OpenCL 1.1.\n");
    error_msg.append(error.what());
    printf("%s\n", error_msg.c_str());
  }
}