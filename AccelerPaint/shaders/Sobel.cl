
__kernel void Sobel(__global unsigned char* RGB_CHAN, __global unsigned char* ALPHA_CHAN, 
				   __global unsigned char* RGB_CHANC, __global unsigned char* ALPHA_CHANC,  unsigned WIDTH_IN) {
 
    // Get the index of the current element to be processed
    int x = get_global_id(0);
    int y = get_global_id(1);

    for(unsigned i = 0; i < 3; i++)
    {
		int valuex = 0;
		valuex += RGB_CHANC[((y - 1) * WIDTH_IN + (x - 1)) * 3 + i] * -1;
		valuex += RGB_CHANC[((y) * WIDTH_IN + (x - 1)) * 3 + i] * -2;
		valuex += RGB_CHANC[((y + 1) * WIDTH_IN + (x - 1)) * 3 + i] * -1;
		valuex += RGB_CHANC[((y - 1) * WIDTH_IN + (x + 1)) * 3 + i] * 1;
		valuex += RGB_CHANC[((y) * WIDTH_IN + (x + 1)) * 3 + i] * 2;
		valuex += RGB_CHANC[((y + 1) * WIDTH_IN + (x + 1)) * 3 + i] * 1;

		valuex = abs(valuex);
		if(valuex > 255)
		{
			valuex = 255;
		}

		int valuey = 0;
		valuey += RGB_CHANC[((y - 1) * WIDTH_IN + (x - 1)) * 3 + i] * -1;
		valuey += RGB_CHANC[((y - 1) * WIDTH_IN + (x)) * 3 + i] * -2;
		valuey += RGB_CHANC[((y - 1) * WIDTH_IN + (x + 1)) * 3 + i] * -1;
		valuey += RGB_CHANC[((y + 1) * WIDTH_IN + (x - 1)) * 3 + i] * 1;
		valuey += RGB_CHANC[((y + 1) * WIDTH_IN + (x)) * 3 + i] * 2;
		valuey += RGB_CHANC[((y + 1) * WIDTH_IN + (x + 1)) * 3 + i] * 1;

		valuey = abs(valuey);
		if(valuey > 255)
		{
			valuey = 255;
		}

		RGB_CHAN[(y * WIDTH_IN + x) * 3 + i] = valuex / 2 + valuey / 2;
    }

	ALPHA_CHAN[(y * WIDTH_IN + x)] = 255;
}