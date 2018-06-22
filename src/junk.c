/******************************************************************************
 * Function: getInversionMatrix
 * Inputs: int
 * Returns: gsl_matrix*
 * Description: For an onion model, with each shell/layer being given by the column
 * number, j, and each impact parameter given by the row number, i, find the
 * matrix that represents the amount each shell contributes to the impact
 * parameter, i. M(i,j), where M = 0 for j<i because if the impact parameter
 * is greater then the shell, it can't have any contribution
 ******************************************************************************/

gsl_matrix* getInversionMatrix(int sizeM) {

  int ii, jj;
  double num;

  gsl_matrix *retMatrix = gsl_matrix_alloc(sizeM,sizeM);

  /* 
   * Here is the method I came up with to get the length of the
   * chord of impact parameter ii, through the shell, jj. The chord
   * is the chord through the center of the 1 pixel width rectangle
   * that passes through the plasma
   */
  for (jj = 0; jj < 10; jj++) {
    for (ii = 0; ii <= jj; ii++) {

	num = sqrt(gsl_pow_2(jj+1)-
		   gsl_pow_2(ii+0.5))
	  -sqrt(gsl_pow_2(jj)-
		gsl_pow_2(ii+0.5));

	gsl_matrix_set(retMatrix, ii, jj, num);
	
    }
  }

  /* 
   * Double back over cases where i = j, because the formula doesn't work
   * for that case.
   */
  for (jj = 0; jj < 10; jj++) {

	num = sqrt(gsl_pow_2(jj+1)-
		   gsl_pow_2(jj+0.5));

	gsl_matrix_set(retMatrix, jj, jj, num);
	
  }

  return retMatrix;

}


/******************************************************************************
 * Function: calculateMatrixPrint
 * Inputs: void 
 * Returns: int
 * Description: For an onion model, with each shell/layer being given by the column
 * number, j, and each impact parameter given by the row number, i, find the
 * matrix that represents the amount each shell contributes to the impact
 * parameter, i. M(i,j), where M = 0 for j<i because if the impact parameter
 * is greater then the shell, it can't have any contribution
 ******************************************************************************/

int calculateMatrixPrint (void) {

  int ii, jj;
  double num;

  gsl_matrix *mikeMethod = gsl_matrix_alloc(10,10);
  gsl_matrix *myMethod = gsl_matrix_alloc(10,10);

  /* 
   * This method (m-file) but 0-9 instead of 1-10:
   * for j = 1:1:10
   *   for i = 1:1:j
   *      A(i,j) = sqrt(((j+1))^2-(i)^2)-sqrt((j)^2-(i)^2);
   *   end
   * end
   * This is what Mike does in this code. I don't know what ii, and
   * jj correspond to, I think ii = impact parameter, j = shell
   */
  for (jj = 0; jj < 10; jj++) {
    for (ii = 0; ii <= jj; ii++) {

      num = sqrt(gsl_pow_2(jj+2)-
		 gsl_pow_2(ii+1))
	-sqrt(gsl_pow_2(jj+1)-
	      gsl_pow_2(ii+1));
      
      gsl_matrix_set(mikeMethod, ii, jj, num);

    }
  }

  /* 
   * Here is the method I came up with to get the length of the
   * chord of impact parameter ii, through the shell, jj. The chord
   * is the chord through the center of the 1 pixel width rectangle
   * that passes through the plasma
   */
  for (jj = 0; jj < 10; jj++) {
    for (ii = 0; ii <= jj; ii++) {

	num = sqrt(gsl_pow_2(jj+1)-
		   gsl_pow_2(ii+0.5))
	  -sqrt(gsl_pow_2(jj)-
		gsl_pow_2(ii+0.5));

	gsl_matrix_set(myMethod, ii, jj, num);
	
    }
  }

  /* 
   * Double back over cases where i = j, because the formula doesn't work
   * for that case.
   */
  for (jj = 0; jj < 10; jj++) {

	num = sqrt(gsl_pow_2(jj+1)-
		   gsl_pow_2(jj+0.5));

	gsl_matrix_set(myMethod, jj, jj, num);
	
  }

  printf("\n\n**********Mike's Method***************\n");

  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj < 10; jj++) {

      printf("%.10e    ", gsl_matrix_get(mikeMethod,
				       ii, jj));

    }
    printf("\n");
  }

  printf("\n\n**********My Method***************\n");

  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj < 10; jj++) {

      printf("%.10e    ", gsl_matrix_get(myMethod,
				       ii, jj));

    }
    printf("\n");
  }

  return 0;

}


/******************************************************************************
 * Function: getInversionMatrixMike
 * Inputs: int
 * Returns: gsl_matrix*
 * Description: This reproduces the matrix as cacluated by Mike in this m-file:
 * dr = 0.25;
 * A = zeros(10,10);
 * for j = 1:1:10
 *   for i = 1:1:j
 *      A(i,j) = sqrt(((j+1))^2-(i)^2)-sqrt((j)^2-(i)^2);
 *   end
 * end
 * A=A*2*dr;
 * A
 ******************************************************************************/

gsl_matrix* getInversionMatrixMike(int sizeM) {

  int ii, jj;
  double num;

  gsl_matrix *mikeMethod = gsl_matrix_alloc(sizeM,sizeM);

  for (jj = 0; jj < 10; jj++) {
    for (ii = 0; ii <= jj; ii++) {

      num = sqrt(gsl_pow_2(jj+2)-
		 gsl_pow_2(ii+1))
	-sqrt(gsl_pow_2(jj+1)-
	      gsl_pow_2(ii+1));
      
      gsl_matrix_set(mikeMethod, ii, jj, num*2*0.25);

    }
  }

  return mikeMethod;

}






/******************************************************************************
 * Function: findDensityOffset
 * Inputs: gsl_vector*, gsl_vector*
 * Returns: int
 * Description: This function should determine what is the appropriate offset
 * for the smaller profile to make it equal to the larger profile. Equal is
 * defined by the L2 norm of the vector, which is the just the vector length.
 ******************************************************************************/

int findDensityOffset(gsl_vector* smallCrossSection, gsl_vector* largeCrossSection) {

  /*
   * Make a temporary vector, on each iteration, set to equal to:
   * large(i) - small(i) - offset
   * Then, take the L2 norm, and divide by the length
   * Then, see if that value is the current minimum, if so,
   * then update the offset value as the optimal
   */

  double offsetDelta = 5E21, // This is the offset change for each iteration
    offsetValue,             // The actual offset value
    val1,                    // Value to set the vector to
    minCheckSum = DBL_MAX,   // Minimum Check sum value. Set to max double
    minOffsetValue,          // Minimum offset value
    checkSum;                // The value to see how the offset worked
  
  int offsetIteration = 15,           // The number of offsets to try
    length = smallCrossSection->size,
    ii, jj;

  /*
   * Iterating through the different values for the
   * offset of the smaller profile
   */
  for (ii = -offsetIteration; ii < offsetIteration; ii++) {

    offsetValue = ii*offsetDelta;
    checkSum = 0;

    /*
     * This takes a running check sum total of the
     * square of the difference between the large
     * vector and the small vector the offset
     */
    for (jj = 0; jj < length; jj++) {

      val1 = gsl_vector_get(largeCrossSection, jj) -
	(gsl_vector_get(smallCrossSection, jj) +
	 offsetValue);
      checkSum = checkSum + gsl_pow_2(val1);
	
    }    

    /* 
     * If the checkSum is at a minimum, then update the
     * minimum offset value
     */
    if (checkSum < minCheckSum) {
      
      minOffsetValue  = offsetValue;
      minCheckSum = checkSum;

    }

  }

  /*
   * Setting the offset for the smaller vector
   */
  for (jj = 0; jj < length; jj++) {

    gsl_vector_set(smallCrossSection,
		   jj,
		   gsl_vector_get(smallCrossSection, jj) +
		   minOffsetValue);

  }

  return 0;

}


/*
 * Now that we have two equal left and right vectors, leftTruncatedProfile, and
 * rightTruncatedProfile, which should map ultimately back to the
 * matrices that will hold the final values, we want to find the appropriate
 * density offset for the smaller of the two profiles to get them closest to
 * equal. We will call this function to do just that. It should return the
 * offset profiles, simply stored in the leftTruncatedProfile and
 * rightTruncatedProfile. Then, we can come up with a metric to see how
 * well the center worked.
 * At this point, I don't think we need to deal with this, because the 
 * inversion matrix does not span the whole length so there should be no offset
 * problem. I'm commenting all this out.
 */
if (rightSize > leftSize) {

  findDensityOffset(&leftTruncatedProfile.vector, 
		    &rightTruncatedProfile.vector);
  
 }
 else if (rightSize < leftSize) {
   
   findDensityOffset(&rightTruncatedProfile.vector, 
		     &leftTruncatedProfile.vector);

 }



/*
 * Someone else wrote this for handling errors, I don't want it
 * but if we remove it, it breaks the code
 */
struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
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
 * Function: read_JPEG_file
 * Inputs: char *
 * Returns: gsl_matrix*
 * Description: Converts a jpeg file to a gsl_matrix and returns it
 ******************************************************************************/

gsl_matrix *read_JPEG_file (char * filename)
{
  
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
    ii = cinfo.output_height - 1;
  gsl_matrix *image = gsl_matrix_alloc(cinfo.output_width, cinfo.output_height);
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
      gsl_matrix_set(image, jj, ii, rpix+gpix+bpix);

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



/* 
 * The old way I inverted matrices was like this (using functions still around in
 * abel.c):
 */

int temp (void) {

  /* Get the matrix that will invert the line integrated density */
  gsl_matrix *invertLeft = getInvertedMatrixLeft(numRows, param->deltaY);
  gsl_matrix *invertRight = getInvertedMatrixRight(numRows, param->deltaY);


  /* 
   * Taking a sub matrix of the matrix which will invert the line integrated density
   * &leftInversionMatrix.matrix to give you a matrix pointer. Left matrix is
   * left-rigth row flipped so you need an offset which is the top left element
   * First two parameters specify upper left component of matrix, then next
   * are the sizes
   */
  leftInversionMatrix  = gsl_matrix_submatrix(invertLeft, 0, numRows-leftSize, 
					      leftSize, leftSize);
  rightInversionMatrix = gsl_matrix_submatrix(invertRight, 0, 0, 
					      rightSize, rightSize);

  /* 
   * Does matrix multiplication. Output is:
   * &leftTruncatedProfile.vector and
   * &rightTruncatedProfile.vector
   * which should connect back to den_num_l/r
   * The left inversion matrix should flip the final product left-right so it
   * doesn't have to be done prior to this.
   */
  gsl_blas_dgemv(CblasNoTrans, 1.0, &leftInversionMatrix.matrix, 
		 &leftTruncated.vector, 0.0, &leftTruncatedProfile.vector);

  gsl_blas_dgemv(CblasNoTrans, 1.0, &rightInversionMatrix.matrix, 
		 &rightTruncated.vector, 0.0, &rightTruncatedProfile.vector); 

}



static int saveHologramsBackup(void) {

  /* 
   * Line integrated density with position information
   * plot 'data/lineIntegrated.dat' binary matrix with image title "Line Integrated"
   */
  //saveHologramImageBinary(twinImageReduce, xPhase, yPhase, saveFile);

  /*
   * Line integrated with just pixel number, no position
   * plot 'data/test.dat' binary matrix with image title "Line Integrated"
   */
  //saveHologramImageBinaryOne(twinImageReduce, saveFile);

  /*
   * Saving line integrated with pixel number and center line encoded on it
   * plot 'data/test.dat' binary matrix with image title "Line Integrated"
   */
  lineIntegratedCenterLine(twinImageUnwrap, saveFile, fileCentroidLocation);

}


/******************************************************************************
 * Function: plotIPApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot IP for a talk that I'm giving
 * on April 28th, 2018
 ******************************************************************************/

static int plotIPApril2018Talk(int shotNumber) {

  int sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);
  
  /* Declaring variables */
  gsl_vector *iP = gsl_vector_alloc(sigSize),
    *cap1 = gsl_vector_alloc(sigSize),
    *timeDetaq = gsl_vector_alloc(sigSize),
    *timeCAMAC = gsl_vector_alloc(sigSize);

  int status;
  
  char *gnuPlotFile = "script/temp.sh",
    *ipFile = "data/ip.txt",
    *cap1File = "data/cap1.txt";
  
  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\I_P", iP, timeDetaq);
  initializeMagneticDataAndTime(shotNumber, "\\I_TBM_01_CAP", cap1, timeCAMAC);


  /* Saving data */
  save2VectorData(timeDetaq, iP, ipFile);
  save2VectorData(timeCAMAC, cap1, cap1File);



  /* Creating gnuplot file */

  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set xrange[-15E-6:600E-6]\n");
  fprintf(fp, "plot '%s' using 1:($2/1E3) title 'IP',\\\n", ipFile);
  fprintf(fp, "     '%s' using ($1-16E-6):(12*$2/1E3) title 'Cap 1'\n", cap1File);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_vector_free(iP);
  gsl_vector_free(cap1);
  gsl_vector_free(timeDetaq);
  gsl_vector_free(timeCAMAC);

  return 0;

}


/******************************************************************************
 * Function: plotAccelApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for acceleration data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

static int plotAccelApril2018Talk() {

  int shotNumber = 180223035,
    status,
    sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);

  /* Declaring variables */
  gsl_vector *n45 = gsl_vector_alloc(sigSize),
    *n35 = gsl_vector_alloc(sigSize),
    *n25 = gsl_vector_alloc(sigSize),
    *n15 = gsl_vector_alloc(sigSize),
    *timeDetaq = gsl_vector_alloc(sigSize);

  
  char *gnuPlotFile = "script/temp.sh",
    *accelFile = "data/accel.txt";
  
  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\b_n45_000_sm", n45, timeDetaq);
  initializeMagneticData(shotNumber, "\\b_n35_000_sm", n35);
  initializeMagneticData(shotNumber, "\\b_n25_000_sm", n25);
  initializeMagneticData(shotNumber, "\\b_n15_000_sm", n15);


  /* Saving data */
  save5VectorData(timeDetaq, n45, n35, n25, n15, accelFile);


  /* Creating gnuplot file */

  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/accel180223035'\n");
  fprintf(fp, "set xrange[0:15]\n");
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Acceleration Region for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+14E-6)*1E6):($2) with line lw 3 lc rgb 'black' \
title 'z = -45 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($3) with line lw 3 lc rgb 'red' \
title 'z = -35 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($4) with line lw 3 lc rgb 'blue' \
title 'z = -25 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($5) with line lw 3 lc rgb 'yellow' \
title 'z = -15 cm'\n", accelFile);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_vector_free(n45);
  gsl_vector_free(n35);
  gsl_vector_free(n25);
  gsl_vector_free(n15);
  gsl_vector_free(timeDetaq);

  return 0;

}


/******************************************************************************
 * Function: plotModeApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for mode data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

static int plotModeApril2018Talk() {

  int shotNumber = 180222040,
    status;

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt";

  
  
  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, "\\b_p15_000_sm");
  getAzimuthalArrayModes(azimuthArray);

  

  /* Saving data */
  saveMatrixData(azimuthArray, modeFile);

  
  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set terminal pngcairo\n");
  fprintf(fp, "set output 'data/modeData.png'\n");
  fprintf(fp, "set xrange[15:45]\n");
  fprintf(fp, "set yrange[0:1]\n");
  fprintf(fp, "set y2range[0:]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set arrow from 15,0.2 to 45,0.2 nohead dt 4 lw 3 lc rgb 'green'\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Normalized modes at z=15 cm for pulse #%d' font '0,14'\n", shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Normalized Modes' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set y2tics nomirror tc lt 2\n");
  fprintf(fp, "set y2label 'Pinch Current (kA)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+15.2E-6)*1E6):($3) with line dt 2 lw 3 lc rgb 'red' \
title 'm=1',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($4) with line dt 3 lw 3 lc rgb 'blue' \
title 'm=2',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($2/0.002) with line lw 3 lc rgb 'black' \
title 'Pinch Current' axes x1y2\n", modeFile);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_matrix_free(azimuthArray);

  
  return 0;


}



/******************************************************************************
 * Function: plotDHIApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for DHI Data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

static int plotDHIApril2018Talk() {

  int status;

  char *gnuPlotFile = "script/temp.sh";
  
  /* 
   * struct containing all the holography parameters.
   */
  //holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;

  /* Obtained line integrated data and do an abel inversion */
  //hologramMain(&param);

  
  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");

  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set palette rgb 33,13,10\n");
  fprintf(fp, "set terminal pngcairo size 19cm,25cm\n");
  fprintf(fp, "set output 'data/dhiData.png'\n");
  fprintf(fp, "set xrange[14.1:14.9]\n");
  fprintf(fp, "set yrange[-0.86:0.86]\n");
  fprintf(fp, "set label front 'Line integrated n_{e} (cm^{-2})' at graph 1.40,0.30 ");
  fprintf(fp, "rotate by 90 font 'Times Bold, 20'\n");
  fprintf(fp, "set size ratio -1\n");
  fprintf(fp, "set xtics 14.1, 0.2, 14.9\n");  
  fprintf(fp, "set title 'Pulse 180215012' font 'Times Bold, 20'\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set xlabel 'Axial Position (cm)' font 'Times Bold,20' offset 0,0\n");
  fprintf(fp, "set ylabel 'Impact Parameter (cm)' font 'Times Bold,20' offset 0,0\n\n");
  fprintf(fp, "show title\n");
  fprintf(fp, "plot 'data/lineIntegratedPosition.dat' binary matrix with image title ''\n");
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  return 0;


}


/******************************************************************************
 * Function: plotSpecCIIIImageApril2018Talk
 * Inputs: 
 * Returns: int
 * Description: This will use gnu plot to plot a ciii image for spectroscoy data
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

static int plotSpecCIIIImageApril2018Talk() {

  int status;

  char *gnuPlotFile = "script/temp.sh";
  
  saveLFImageWithWavelength("/home/webertr/Spectroscopy/Data/180222/180222  036.spe",
			    "data/lightField.dat");
  
  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");

  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set palette rgb 33,13,10\n");
  //fprintf(fp, "set terminal pngcairo size 19cm,25cm\n");
  //fprintf(fp, "set output 'data/ciiiImage.png'\n");
  fprintf(fp, "set size ratio -1\n");
  fprintf(fp, "set xrange[229.65:229.8]\n");
  fprintf(fp, "set xtics 229.65, 0.05, 229.8\n");  
  fprintf(fp, "set yrange[0:1024]\n");
  fprintf(fp, "set cbrange [0:]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set xlabel 'Wavelength (nm)' font 'Times Bold,20' offset 0,0\n");
  fprintf(fp, "set ylabel 'Pixel' font 'Times Bold,20' offset 0,0\n\n");
  fprintf(fp, "set title 'C^{+2} emission for Pulse 180222036' font 'Times Bold, 20'\n");
  fprintf(fp, "show title\n");
  fprintf(fp, "plot 'data/lightField.dat' binary matrix with image title ''\n");
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  return 0;


}


/******************************************************************************
 * Function: plotCIIILineApril2018Talk
 * Inputs: 
 * Returns: int
 * Description: This will use gnu plot to plot a ciii image for spectroscoy data
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

static int plotCIIILineApril2018Talk() {

  int status;

  char *gnuPlotFile = "script/temp.sh";

  lightFieldParameters param = LIGHT_FIELD_PARAMETERS_DEFAULT;

  strcpy(param.speFile, "/home/webertr/Spectroscopy/Data/180222/180222  036.spe");
  
  gsl_matrix *image = 0;
  gsl_vector *waveLength = 0;
  
  getLightFieldData(&image, &waveLength, &param);

  gsl_vector *sumRow = gsl_vector_alloc(image->size1);

  int ii, jj;
  double val;
  for (ii = 0; ii < image->size1; ii++) {
    val = 0;
    for (jj = 792; jj <  811; jj++) {
      val = gsl_matrix_get(image, ii, jj)+val;
    }
    gsl_vector_set(sumRow, ii, val);
  }

  /* int center[20]={43,98,141,193,246,296,342,393,445,491,534,585,635,687,735,779,836,882,936,983}; */
  /* for (ii = 0; ii < sumRow->size; ii++) { */
  /*   //printf("Value (%d): %g\n", ii, gsl_vector_get(sumRow, ii)); */
  /*   // Centers: [43,98,141,193,246,296,342,393,445,491,534,585,635,687,735,779,836,882,936,983] */
  /*   // 586 looks like a good one */
  /*   // 837 is the other one I'm trying */
  /* } */
  
  int start = 788,
    stop = 810;

  int centerRow = 837;
  
  gsl_vector *ciiiLine = gsl_vector_alloc(stop - start);
  gsl_vector *ciiiWL = gsl_vector_alloc(stop -start);

  for (jj = 0; jj < ciiiLine->size; jj++) {
    gsl_vector_set(ciiiLine, jj, gsl_matrix_get(image, centerRow, jj+start));
    gsl_vector_set(ciiiWL, jj, gsl_vector_get(waveLength, jj+start));
  }

  double lineMax = gsl_vector_max(ciiiLine);
  for (jj = 0; jj < ciiiLine->size; jj++) {
    gsl_vector_set(ciiiLine, jj, gsl_vector_get(ciiiLine, jj)/lineMax);
  }

  
  double ampParam = 0.95,
    centerParam = 229.705,
    sigmaParam = 0.0184,
    offsetParam = 0.05;

  gsl_vector *gaussFit = gsl_vector_alloc(ciiiWL->size);
  
  fitGaussian(ciiiWL, ciiiLine, gaussFit, &ampParam, &centerParam, &sigmaParam, &offsetParam, 0);

  printf("Fit amplitude: %g\n", ampParam);
  printf("Fit center WL (nm): %g nm\n", centerParam);
  printf("Fit sigma WL (nm): %g nm\n", sigmaParam);
  printf("Fit offset: %g\n", offsetParam);
  printf("Fit ion Temperature: %g keV\n", carbonIonTemperature(centerParam, sigmaParam));
  
  FILE *fp1;
  fp1 = fopen("data/ciiiLine.txt", "w");
  for (jj = 0; jj < ciiiLine->size; jj++) {
    fprintf(fp1, "%g\t%g\t%g\n", gsl_vector_get(ciiiWL,jj),
	    gsl_vector_get(ciiiLine, jj),gsl_vector_get(gaussFit, jj));
  }
  fclose(fp1);
  
  
  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");

  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set palette rgb 33,13,10\n");
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/ciiiLine.png'\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set xlabel 'Wavelength (nm)' font 'Times Bold,20' offset 0,0\n");
  fprintf(fp, "set ylabel 'Photon Counts (arb.)' font 'Times Bold,20' offset 0,0\n\n");
  fprintf(fp, "set title 'C^{+2} emission for Pulse 180222036' font 'Times Bold, 20'\n");
  fprintf(fp, "set size ratio 0.75\n");
  fprintf(fp, "set yrange[0:1.1]\n");
  fprintf(fp, "set xrange[229.65:229.77]\n");
  fprintf(fp, "set xtics 229.65, 0.025, 229.8\n");
  fprintf(fp, "set yrange[0:1.1]\n");
  fprintf(fp, "show title\n");
  fprintf(fp, "plot 'data/ciiiLine.txt' using 1:2 with points ps 2 pt 7 lc rgb 'black' ");
  fprintf(fp, "title '1s^{2}2p^{2} {/Symbol \256} 1s^{2}2s2p',\\\n");
  fprintf(fp, "     'data/ciiiLine.txt' using 1:3 with lines dt 2 lw 3 title 'Gaussian fit'\n");
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  return 0;


}


/******************************************************************************
 * Function: plotModeCompApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for mode data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

static int plotModeCompApril2018Talk() {

  int shotNumber = 180222040,
    status,
    sigSize = getSignalLengthMDSplus("\\b_p15_000_sm", shotNumber);

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt",
    *mdsFile = "data/mdsplus.txt";


  
  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, "\\b_p15_000_sm");
  getAzimuthalArrayModes(azimuthArray);
  gsl_vector *time = gsl_vector_alloc(sigSize);
  gsl_vector *data = gsl_vector_alloc(sigSize);
  initializeMagneticDataAndTime(shotNumber, "\\m_3_p15_norm_sm", data, time);

  /* Saving data */
  saveMatrixData(azimuthArray, modeFile);
  save2VectorData(time, data, mdsFile);

  
  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  //fprintf(fp, "set terminal pngcairo\n");
  //fprintf(fp, "set output 'data/modeData.png'\n");
  fprintf(fp, "set xrange[15:45]\n");
  fprintf(fp, "set yrange[0:1]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Normalized modes at z=15 cm for pulse #%d' font '0,14'\n", shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Normalized Modes' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+15.2E-6)*1E6):($5) with line dt 2 lw 3 lc rgb 'red' \
title 'm=1',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($2) with line dt 3 lw 3 lc rgb 'blue' \
title 'm=1 mds'\n", mdsFile);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_matrix_free(azimuthArray);

  
  return 0;


}
