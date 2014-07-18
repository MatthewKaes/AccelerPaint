
__kernel void Inverter(__global unsigned char* RGB_CHAN, unsigned WIDTH_IN) {
 
    // Get the index of the current element to be processed
    int x = get_global_id(0);
    int y = get_global_id(1);

	RGB_CHAN[(y * WIDTH_IN + x) * 3] = 255 - RGB_CHAN[(y * WIDTH_IN + x) * 3];
	RGB_CHAN[(y * WIDTH_IN + x) * 3 + 1] = 255 - RGB_CHAN[(y * WIDTH_IN + x) * 3 + 1];
	RGB_CHAN[(y * WIDTH_IN + x) * 3 + 2] = 255 - RGB_CHAN[(y * WIDTH_IN + x) * 3 + 2];
}