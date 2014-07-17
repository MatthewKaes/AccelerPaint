//Blend.cl

//Takes two images and makes a imposes the FORground images onto
//the BACKground image. Alpha blending is preformed when making the
//composit image.

//#pragma opencl cl_khr_byte_addressable_store : enable

__kernel void Opacity(__global unsigned char* ALPHA_A, unsigned WIDTH_IN, float PASS_A) {
    // Get the index of the current element to be processed
    int x = get_global_id(0); 
    int y = get_global_id(1); 

    // Do the operation
    // Invert all 4 bytes in one go to be SUPER fast.
	ALPHA_A[y * WIDTH_IN + x] = ALPHA_A[y * WIDTH_IN + x] * PASS_A;
}