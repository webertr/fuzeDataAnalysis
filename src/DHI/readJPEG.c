#include "DHI/readJPEG.h"

/*
 * Someone else wrote this for handling errors, I don't want it
 * but if we remove it, it breaks the code.
 * This is a definition, not a declaration of a variable.
 */
struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 * #define METHODDEF(type)		static type
 */
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/******************************************************************************
 * Function: readJPEGImage
 * Inputs: char *
 * Returns: gsl_matrix*
 * Description: Converts a jpeg file to a gsl_matrix and returns it
 ******************************************************************************/

gsl_matrix *readJPEGImage(char * filename) {
  
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */

  struct my_error_mgr jerr;
  
  /* More stuff */
  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return (gsl_matrix*) NULL;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return (gsl_matrix*) NULL;
  }

  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  double rgb_red = 0.3;
  double rgb_green = 0.59;
  double rgb_blue = 0.11;
  int jj = 0,
    ii = 0;
  gsl_matrix *image = gsl_matrix_alloc(cinfo.output_height, cinfo.output_width);
  unsigned char *pChar;
  double rpix, gpix, bpix;

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    //put_scanline_someplace(buffer[0], row_stride);
    
    pChar = buffer[0];

    for (jj = 0 ; jj < cinfo.output_width; jj++) {

      rpix=*(pChar)*rgb_red;   // Red
      pChar++;
      gpix=*(pChar)*rgb_green; // Green
      pChar++;
      bpix=*(pChar)*rgb_blue; // Blue
      pChar++;
      gsl_matrix_set(image, ii, jj, rpix+gpix+bpix);

    }

    pChar--;

    ii = ii+1;

  }


  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
  return image;

}


/******************************************************************************
 * Function: saveJPEGImage
 * Inputs: gsl_matrix *, char *
 * Returns: int
 * Description: Save a gsl matrix to a jpeg file.
 ******************************************************************************/

int saveJPEGImage(gsl_matrix *mInput, char *fileName) {

  /* Quality should be between 0 and 100 */
  int quality = 100;

  /* jpeg struct */
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  /* pointer to raw image */
  unsigned char *raw_image = NULL;

  /* dimensions of the image we want to write */
  int width=mInput->size1, 
    height=mInput->size2,
    bytes_per_pixel=3;

  /* JCS_GRAYSCALE = 1 for J_COLOR_SPACE enum. JCS_RGB = 2 */
  int color_space=JCS_RGB;

  /* alloc raw image data */
  raw_image = (unsigned char *)malloc((sizeof (unsigned char))*width*height*bytes_per_pixel);

  /* convert tensor to raw bytes */
  int ii, jj;
  for (ii=0; ii<width; ii++) {
    for (jj=0; jj<height; jj++) {

      raw_image[(ii*width+jj)*bytes_per_pixel] = (unsigned char) 200;

    }
  }

  /* this is a pointer to one row of image data */
  JSAMPROW row_pointer[1];
  FILE *outfile = NULL;

  outfile = fopen( fileName, "wb" );

  cinfo.err = jpeg_std_error( &jerr );
  jpeg_create_compress(&cinfo);

  jpeg_stdio_dest(&cinfo, outfile);
  
  /* Setting the parameters of the output file here */
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = bytes_per_pixel;
  cinfo.in_color_space = color_space;

  /* default compression parameters, we shouldn't be worried about these */
  jpeg_set_defaults( &cinfo );
  jpeg_set_quality(&cinfo, quality, (boolean)0);

  /* Now do the compression .. */
  jpeg_start_compress( &cinfo, TRUE );

  /* like reading a file, this time write one row at a time */
  while( cinfo.next_scanline < cinfo.image_height ) {
    
    row_pointer[0] = 
      &raw_image[ cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];

    jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    
  }
  
  /* similar to read file, clean up after we're done compressing */
  jpeg_finish_compress( &cinfo );
  jpeg_destroy_compress( &cinfo );

  fclose( outfile );

  /* some cleanup */
  free(raw_image);

  /* success code is 1! */
  return 1;

}
