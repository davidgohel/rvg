/*
 * RVG device - Raster handling
 */
#ifndef RVG_RASTER_INCLUDED
#define RVG_RASTER_INCLUDED

void raster_to_file(unsigned int *raster, int w, int h, double width, double height, bool interpolate, char *file_name);

#endif // RVG_RASTER_INCLUDED
