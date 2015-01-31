
__kernel void Greyscale(__global unsigned char* RGB_CHAN, unsigned WIDTH_IN) {
 
    // Get the index of the current element to be processed
    int x = get_global_id(0);
    int y = get_global_id(1);
	
	unsigned char val = RGB_CHAN[(y * WIDTH_IN + x) * 3] * 0.21f + RGB_CHAN[(y * WIDTH_IN + x) * 3 + 1] * 0.72f + RGB_CHAN[(y * WIDTH_IN + x) * 3 + 2] * 0.07f;
	RGB_CHAN[(y * WIDTH_IN + x) * 3] = val;
	RGB_CHAN[(y * WIDTH_IN + x) * 3 + 1] = val;
	RGB_CHAN[(y * WIDTH_IN + x) * 3 + 2] = val;
}