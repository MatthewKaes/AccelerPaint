//Fill.cl

//Takes an image, a rectangle and a color and fills the
//rectangle on the image with the passed color. Sets RGBA.


//#pragma opencl cl_khr_byte_addressable_store : enable

typedef struct color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} color;

typedef struct rect
{
    unsigned x;
    unsigned y;
    unsigned width;
    unsigned height;
} rect;

__kernel void Fill(__global unsigned char* RGB_IN, __global unsigned char* ALPHA_IN, unsigned WIDTH_IN, 
						  __constant struct rect* draw_rect, __constant struct color* fill) {
    // Get the index of the current element to be processed
    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x < draw_rect->x || y < draw_rect->y ||
       x > draw_rect->x + draw_rect->width || y > draw_rect->y + draw_rect->height)
       return;

    // Do the operation
    // Invert all 4 bytes in one go to be SUPER fast.
    RGB_IN[(y * WIDTH_IN + x) * 3] = fill->red;
    RGB_IN[(y * WIDTH_IN + x) * 3 + 1] = fill->green;
    RGB_IN[(y * WIDTH_IN + x) * 3 + 2] = fill->blue;
    ALPHA_IN[y * WIDTH_IN + x] = fill->alpha;
}