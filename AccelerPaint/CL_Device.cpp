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
 

    //Get device debug info.
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
    queue_ = cl::CommandQueue(context, devices_[0]);
    opencl_enabled = true;

    Init();
  } 
  // Something horrible happend...
  catch(cl::Error error) 
  {
    // What exploded and the error code related to it.
		printf("A Error occured with loading OpenCL.\n");
    opencl_enabled = false;
  }
}
void OpenCL_Dev::Init()
{
  Build_Kernel("Fill");
  Build_Kernel("Blend");
  Build_Kernel("Opacity");
  Build_Kernel("Blur");
  Build_Kernel("Inverter");
  Build_Kernel("Threshold");
  Build_Kernel("Sobel");
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
bool OpenCL_Dev::Fill(image img_data, rect fill_region, color fill_color)
{
  if(!opencl_enabled)
    return false;

  //Channel Buffers
  unsigned pixel_count = img_data.pos_data.width * img_data.pos_data.height;
  cl::Buffer RGB_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer Alpha_Chan  = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count);
  
  //Don't block for our events.
  //Run other code while we wait for them to enqueue we will block
  //on them later when enqueueing the kernel range.
  cl::vector< cl::Event > Events;
  cl::Event new_event;
  queue_.enqueueWriteBuffer(RGB_Chan, CL_FALSE, 0, pixel_count * 3, img_data.rgb_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Alpha_Chan, CL_FALSE, 0, pixel_count, img_data.alpha_data, NULL, &new_event);
  Events.push_back(new_event);

  //Constant info
  cl::Buffer Region_Data = cl::Buffer(context_, CL_MEM_READ_ONLY, sizeof(fill_region));
  cl::Buffer Color_Data  = cl::Buffer(context_, CL_MEM_READ_ONLY, sizeof(fill_color));
  queue_.enqueueWriteBuffer(Region_Data, CL_FALSE, 0,  sizeof(fill_region), &fill_region, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Color_Data, CL_FALSE, 0, sizeof(fill_color), &fill_color, NULL, &new_event);
  Events.push_back(new_event);

  //Number of threads to run.
  cl::NDRange global(img_data.pos_data.width, img_data.pos_data.height);

  //Get work group size
  cl::NDRange local = Work_Group(img_data);

  //Set it as an argument
  cl::Kernel* kern = kernels["Fill"];
  kern->setArg(0, RGB_Chan);
  kern->setArg(1, Alpha_Chan);
  kern->setArg(2, img_data.pos_data.width);
  kern->setArg(3, Region_Data);
  kern->setArg(4, Color_Data);

  queue_.enqueueNDRangeKernel(*kern, cl::NullRange, global, local, &Events);
  
  queue_.enqueueReadBuffer(Alpha_Chan, CL_TRUE, 0, pixel_count, img_data.alpha_data);
  queue_.enqueueReadBuffer(RGB_Chan, CL_TRUE, 0, pixel_count * 3, img_data.rgb_data);

  return true;
}
bool OpenCL_Dev::Blend(image img_base, image img_forground)
{
  if(!opencl_enabled)
    return false;

  //Channel Buffers
  unsigned pixel_count = img_base.pos_data.width * img_base.pos_data.height;
  cl::Buffer RGB_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer Alpha_Chan  = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count);
  cl::Buffer RGB_Chan2 = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer Alpha_Chan2  = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count);
  
  //Don't block for our events.
  //Run other code while we wait for them to enqueue we will block
  //on them later when enqueueing the kernel range.
  cl::vector< cl::Event > Events;
  cl::Event new_event;
  queue_.enqueueWriteBuffer(RGB_Chan, CL_FALSE, 0, pixel_count * 3, img_base.rgb_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Alpha_Chan, CL_FALSE, 0, pixel_count, img_base.alpha_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(RGB_Chan2, CL_FALSE, 0, pixel_count * 3, img_forground.rgb_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Alpha_Chan2, CL_FALSE, 0, pixel_count, img_forground.alpha_data, NULL, &new_event);
  Events.push_back(new_event);

  //Number of threads to run.
  cl::NDRange global(img_base.pos_data.width, img_forground.pos_data.height);

  //Get work group size
  cl::NDRange local = Work_Group(img_base);

  //Preform the Alpha pass for the blending layer
  cl::Kernel* kern = kernels["Opacity"];
  kern->setArg(0, Alpha_Chan2);
  kern->setArg(1, img_forground.pos_data.width);
  kern->setArg(2, img_forground.opacity);

  //enqueue the Range to run the kernal, also wait for our write buffers to 
  //be enqueued if they aren't done yet
  queue_.enqueueNDRangeKernel(*kern, cl::NullRange, global, local, &Events);

  //Set it as an argument
  kern = kernels["Blend"];
  kern->setArg(0, RGB_Chan);
  kern->setArg(1, Alpha_Chan);
  kern->setArg(2, img_base.pos_data.width);
  kern->setArg(3, RGB_Chan2);
  kern->setArg(4, Alpha_Chan2);

  //enqueue the Range to run the kernal, also wait for our write buffers to 
  //be enqueued if they aren't done yet
  queue_.enqueueNDRangeKernel(*kern, cl::NullRange, global, local, &Events);
  
  queue_.enqueueReadBuffer(Alpha_Chan, CL_TRUE, 0, pixel_count, img_base.alpha_data);
  queue_.enqueueReadBuffer(RGB_Chan, CL_TRUE, 0, pixel_count * 3, img_base.rgb_data);

  return true;
}
bool OpenCL_Dev::Invert(image img_base)
{  
  if(!opencl_enabled)
    return false;

  //Channel Buffers
  unsigned pixel_count = img_base.pos_data.width * img_base.pos_data.height;
  cl::Buffer RGB_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  
  //Don't block for our events.
  //Run other code while we wait for them to enqueue we will block
  //on them later when enqueueing the kernel range.
  cl::vector< cl::Event > Events;
  cl::Event new_event;
  queue_.enqueueWriteBuffer(RGB_Chan, CL_FALSE, 0, pixel_count * 3, img_base.rgb_data, NULL, &new_event);
  Events.push_back(new_event);

  //Number of threads to run.
  cl::NDRange global(img_base.pos_data.width, img_base.pos_data.height);

  //Get work group size
  cl::NDRange local = Work_Group(img_base);

  //Preform the Alpha pass for the blending layer
  cl::Kernel* kern = kernels["Inverter"];
  kern->setArg(0, RGB_Chan);
  kern->setArg(1, img_base.pos_data.width);

  //enqueue the Range to run the kernal, also wait for our write buffers to 
  //be enqueued if they aren't done yet
  queue_.enqueueNDRangeKernel(*kern, cl::NullRange, global, local, &Events);
  
  queue_.enqueueReadBuffer(RGB_Chan, CL_TRUE, 0, pixel_count * 3, img_base.rgb_data);

  return true;
}
bool OpenCL_Dev::Treshold(image img_base)
{
  if(!opencl_enabled)
    return false;

  //Channel Buffers
  unsigned pixel_count = img_base.pos_data.width * img_base.pos_data.height;
  cl::Buffer RGB_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  
  //Don't block for our events.
  //Run other code while we wait for them to enqueue we will block
  //on them later when enqueueing the kernel range.
  cl::vector< cl::Event > Events;
  cl::Event new_event;
  queue_.enqueueWriteBuffer(RGB_Chan, CL_FALSE, 0, pixel_count * 3, img_base.rgb_data, NULL, &new_event);
  Events.push_back(new_event);

  //Number of threads to run.
  cl::NDRange global(img_base.pos_data.width, img_base.pos_data.height);

  //Get work group size
  cl::NDRange local = Work_Group(img_base);

  //Preform the Alpha pass for the blending layer
  cl::Kernel* kern = kernels["Threshold"];
  kern->setArg(0, RGB_Chan);
  kern->setArg(1, img_base.pos_data.width);

  //enqueue the Range to run the kernal, also wait for our write buffers to 
  //be enqueued if they aren't done yet
  queue_.enqueueNDRangeKernel(*kern, cl::NullRange, global, local, &Events);
  
  queue_.enqueueReadBuffer(RGB_Chan, CL_TRUE, 0, pixel_count * 3, img_base.rgb_data);

  return true;
}
bool OpenCL_Dev::Blur(image img_base)
{
  if(!opencl_enabled)
    return false;

  //Channel Buffers
  unsigned pixel_count = img_base.pos_data.width * img_base.pos_data.height;
  cl::Buffer RGB_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer RGB_Chan2 = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer Alpha_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer Alpha_Chan2 = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  
  //Don't block for our events.
  //Run other code while we wait for them to enqueue we will block
  //on them later when enqueueing the kernel range.
  cl::vector< cl::Event > Events;
  cl::Event new_event;
  queue_.enqueueWriteBuffer(RGB_Chan, CL_FALSE, 0, pixel_count * 3, img_base.rgb_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Alpha_Chan, CL_FALSE, 0, pixel_count, img_base.alpha_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(RGB_Chan2, CL_FALSE, 0, pixel_count * 3, img_base.rgb_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Alpha_Chan2, CL_FALSE, 0, pixel_count, img_base.alpha_data, NULL, &new_event);
  Events.push_back(new_event);

  //Number of threads to run.
  cl::NDRange global(img_base.pos_data.width, img_base.pos_data.height);
  
  //Get work group size
  cl::NDRange local = Work_Group(img_base);

  //Preform the Alpha pass for the blending layer
  cl::Kernel* kern = kernels["Blur"];
  kern->setArg(0, RGB_Chan);
  kern->setArg(1, Alpha_Chan);
  kern->setArg(2, RGB_Chan2);
  kern->setArg(3, Alpha_Chan2);
  kern->setArg(4, img_base.pos_data.width);

  //enqueue the Range to run the kernal, also wait for our write buffers to 
  //be enqueued if they aren't done yet
  queue_.enqueueNDRangeKernel(*kern, cl::NullRange, global, local, &Events);
  
  queue_.enqueueReadBuffer(Alpha_Chan, CL_TRUE, 0, pixel_count, img_base.alpha_data);
  queue_.enqueueReadBuffer(RGB_Chan, CL_TRUE, 0, pixel_count * 3, img_base.rgb_data);

  return true;
}
bool OpenCL_Dev::Sobel(image img_base)
{
  if(!opencl_enabled)
    return false;

  //Channel Buffers
  unsigned pixel_count = img_base.pos_data.width * img_base.pos_data.height;
  cl::Buffer RGB_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer RGB_Chan2 = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer Alpha_Chan = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  cl::Buffer Alpha_Chan2 = cl::Buffer(context_, CL_MEM_READ_WRITE, pixel_count * 3);
  
  //Don't block for our events.
  //Run other code while we wait for them to enqueue we will block
  //on them later when enqueueing the kernel range.
  cl::vector< cl::Event > Events;
  cl::Event new_event;
  queue_.enqueueWriteBuffer(RGB_Chan, CL_FALSE, 0, pixel_count * 3, img_base.rgb_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Alpha_Chan, CL_FALSE, 0, pixel_count, img_base.alpha_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(RGB_Chan2, CL_FALSE, 0, pixel_count * 3, img_base.rgb_data, NULL, &new_event);
  Events.push_back(new_event);
  queue_.enqueueWriteBuffer(Alpha_Chan2, CL_FALSE, 0, pixel_count, img_base.alpha_data, NULL, &new_event);
  Events.push_back(new_event);

  //Number of threads to run.
  cl::NDRange global(img_base.pos_data.width, img_base.pos_data.height);
  
  //Get work group size
  cl::NDRange local = Work_Group(img_base);

  //Preform the Alpha pass for the blending layer
  cl::Kernel* kern = kernels["Sobel"];
  kern->setArg(0, RGB_Chan);
  kern->setArg(1, Alpha_Chan);
  kern->setArg(2, RGB_Chan2);
  kern->setArg(3, Alpha_Chan2);
  kern->setArg(4, img_base.pos_data.width);

  //enqueue the Range to run the kernal, also wait for our write buffers to 
  //be enqueued if they aren't done yet
  queue_.enqueueNDRangeKernel(*kern, cl::NullRange, global, local, &Events);
  
  queue_.enqueueReadBuffer(Alpha_Chan, CL_TRUE, 0, pixel_count, img_base.alpha_data);
  queue_.enqueueReadBuffer(RGB_Chan, CL_TRUE, 0, pixel_count * 3, img_base.rgb_data);

  return true;
}
cl::NDRange OpenCL_Dev::Work_Group(image& img_data)
{
  //Try to find a better work size then 1.
  //Sizes 128 and 64 are optimum
  int local_size = 32;
  while(local_size > 1)
  {
    if(img_data.pos_data.width % local_size == 0 && 
      img_data.pos_data.height % local_size == 0)
      break;

    local_size /= 2;
  }
  return cl::NDRange(local_size, local_size);
}