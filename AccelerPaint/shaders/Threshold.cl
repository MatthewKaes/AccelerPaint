
__kernel void Threshold(__global unsigned char* RGB_CHAN, unsigned WIDTH_IN) {
 
    // Get the index of the current element to be processed
    int x = get_global_id(0);
    int y = get_global_id(1);

	for(int i = 0; i < 3; i++)
	{
		if(RGB_CHAN[(y * WIDTH_IN + x) * 3 + i] >= 125)
		{
			RGB_CHAN[(y * WIDTH_IN + x) * 3 + i] = 255;
		}
		else
		{
			RGB_CHAN[(y * WIDTH_IN + x) * 3 + i] = 0;
		}
	}
}