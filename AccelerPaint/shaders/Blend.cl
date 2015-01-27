//Blend.cl

//Takes two images and makes a imposes the FORground images onto
//the BACKground image. Alpha blending is preformed when making the
//composit image.

//#pragma opencl cl_khr_byte_addressable_store : enable

typedef struct channel
{
  bool Red;
  bool Green;
  bool Blue;
} channel;

__kernel void Blend(__global unsigned char* RGB_BACK, __global unsigned char* ALPHA_BACK, unsigned WIDTH_IN, 
				    __global unsigned char* RGB_FOR, __global unsigned char* ALPHA_FOR,
					__constant struct channel* channels) {

    // Get the index of the current element to be processed
    int x = get_global_id(0);
    int y = get_global_id(1);

    // Do the operation
    // Invert all 4 bytes in one go to be SUPER fast.
	unsigned char ba = ALPHA_BACK[y * WIDTH_IN + x];
	unsigned char fa = ALPHA_FOR[y * WIDTH_IN + x];

	if(channels->Red)
	{
		unsigned char bc = RGB_BACK[(y * WIDTH_IN + x) * 3];
		unsigned char fc = RGB_FOR[(y * WIDTH_IN + x) * 3];
		RGB_BACK[(y * WIDTH_IN + x) * 3] = fc * fa / 255.0f + bc * (255.0f - fa) / 255.0f;
	}
	else
	{
		unsigned char bc = RGB_BACK[(y * WIDTH_IN + x) * 3];
		RGB_BACK[(y * WIDTH_IN + x) * 3] = bc * (255.0f - fa) / 255.0f;
	}
	
	if(channels->Green)
	{
		unsigned char bc = RGB_BACK[(y * WIDTH_IN + x) * 3 + 1];
		unsigned char fc = RGB_FOR[(y * WIDTH_IN + x) * 3 + 1];
		RGB_BACK[(y * WIDTH_IN + x) * 3 + 1] = fc * fa / 255.0f + bc * (255.0f - fa) / 255.0f;
	}
	else
	{
		unsigned char bc = RGB_BACK[(y * WIDTH_IN + x) * 3 + 1];
		RGB_BACK[(y * WIDTH_IN + x) * 3 + 1] = bc * (255.0f - fa) / 255.0f;
	}

	if(channels->Blue)
	{
		unsigned char bc = RGB_BACK[(y * WIDTH_IN + x) * 3 + 2];
		unsigned char fc = RGB_FOR[(y * WIDTH_IN + x) * 3 + 2];
		RGB_BACK[(y * WIDTH_IN + x) * 3 + 2] = fc * fa / 255.0f + bc * (255.0f - fa) / 255.0f;
	}
	else
	{
		unsigned char bc = RGB_BACK[(y * WIDTH_IN + x) * 3 + 2];
		RGB_BACK[(y * WIDTH_IN + x) * 3 + 2] = bc * (255.0f - fa) / 255.0f;
	}

	ALPHA_BACK[y * WIDTH_IN + x] = fa + ba* ((255.0f - fa) / 255.0f);
}