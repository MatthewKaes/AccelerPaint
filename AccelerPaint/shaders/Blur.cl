
__kernel void Blur(__global unsigned char* RGB_CHAN, __global unsigned char* ALPHA_CHAN, 
				   __global unsigned char* RGB_CHANC, __global unsigned char* ALPHA_CHANC,  unsigned WIDTH_IN) {
 
    // Get the index of the current element to be processed
    int x = get_global_id(0);
    int y = get_global_id(1);

    for(unsigned i = 0; i < 3; i++)
    {
		int value = 0;
		for (int offsetx = -1; offsetx <= 1; ++offsetx)
		{
			for (int offsety = -1; offsety <= 1; ++offsety)
			{
				value += RGB_CHANC[((y + offsety) * WIDTH_IN + (x + offsetx)) * 3 + i] / 9;
			}
		}
		RGB_CHAN[(y * WIDTH_IN + x) * 3 + i] = value;
    }

	int value = 0;
	for (int offsetx = -1; offsetx <= 1; ++offsetx)
	{
		for (int offsety = -1; offsety <= 1; ++offsety)
		{
			value += ALPHA_CHANC[((y + offsety) * WIDTH_IN + (x + offsetx))] / 9;
		}
	}
	ALPHA_CHAN[(y * WIDTH_IN + x)] = value;
}