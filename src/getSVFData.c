#include "getSVFData.h"

#define FRAME_NUMBER 180


/******************************************************************************
 * Example Usage:
 *
 * int test() {
 *
 * plotKiranaImage("/home/fuze/Kirana_Videos/2017/170411/170411019.SVF", 95);
 *
 * return 0;
 *
 *}
 *
 ******************************************************************************/

/******************************************************************************
 * Function: readSVFFile
 * Inputs: kiranaSVFData *, char *
 * Returns: int
 * Description: Should populate a struct with kirana data for a given SVF file
 ******************************************************************************/

int readSVFFile(kiranaSVFData *kiranaStruct, char *fileName) {

  FILE *fp;
  size_t fb;

  short byteDepth;  // Number of bytes per pixel
  short compressionType;  // Compression type

  int magicNumber,  // magic Number to read at begining ("SVF")
    imageNum,       // number of images
    imageWidth,     // width of image in pixels
    imageHeight,    // height of image in pixels
    frameZeroSize,  // Size of frame 0
    frameZeroOffset,// Frame 0 offset
    headerSize,     // size of header in bytes
    metaDataSize,   // size of meta data block (images)
    imageInfoSize,  // size of image info structure
    imageSeqSize,   // Size of image sequence
    offsetBytes,    //  number of bytes offset for the file
    setupDataSize,  // Size of xml setup block
    versionNum;     // Version number
    
  long metaData,   // Offset of meta data block
    setupData,     // Offset of xml setup block
    imageInfo,     // Offset of image info structure
    imageSeq;      // Offset for image sequence

  /* sizes and offset for all 180 frame */
  int frameSize[FRAME_NUMBER];
  int frameOffset[FRAME_NUMBER];
  
  /* 
   * Opening the file
   */
  fp = fopen(fileName, "r");

  /*
   * Going to begining of the file
   */
  fseek(fp, 0, SEEK_SET);

  /*
   * Reading magic number ("SVF")
   */
  fb = fread((void *) &magicNumber, (size_t) 4, (size_t) 1, fp);
  
  if ( (fb != 1) || (strcmp("FVS", (char *) &magicNumber) != 0) ) {

    printf("Error reading Kirana file or != \"FVS\":\n");
    fclose(fp);
    exit(1);
    return 0;
    
  }

  /* Getting header size */
  fb = fread((void *) &headerSize, (size_t) 4 , (size_t) 1, fp);
  
  /* getting version number */
  fb = fread((void *) &versionNum, (size_t) 4 , (size_t) 1, fp);

  /* getting size of setup xml data */
  fb = fread((void *) &setupDataSize, (size_t) 4 , (size_t) 1, fp);

  /* getting offset of setup xml data */
  fb = fread((void *) &setupData, (size_t) 8 , (size_t) 1, fp);

  /*
   * Going to part where meta data is
   */
  fseek(fp, 0, SEEK_SET);
  offsetBytes = 4*4+8*1;
  fseek(fp, offsetBytes, SEEK_SET);

  /* Getting meta data block size */
  fb = fread((void *) &metaDataSize, (size_t) 4 , (size_t) 1, fp);

  /*
   * Going to part where meta data offset is is
   */
  fseek(fp, 0, SEEK_SET);
  offsetBytes = 4*5+8*1;
  fseek(fp, offsetBytes, SEEK_SET);

  /* Getting meta data block size */
  fb = fread((void *) &metaData, (size_t) 8 , (size_t) 1, fp);

  /*
   * Going to part where meta data is
   */
  fseek(fp, 0, SEEK_SET);
  offsetBytes = 4*5+8*2;
  fseek(fp, offsetBytes, SEEK_SET);

  /* Getting meta data block size */
  fb = fread((void *) &imageInfoSize, (size_t) 4 , (size_t) 1, fp);
  
  /*
   * Going to part where meta data offset is is
   */
  fseek(fp, 0, SEEK_SET);
  offsetBytes = 4*6+8*2;
  fseek(fp, offsetBytes, SEEK_SET);

  /* Getting meta data block size */
  fb = fread((void *) &imageInfo, (size_t) 8 , (size_t) 1, fp);


  /*
   * Going to part where meta data is
   */
  fseek(fp, 0, SEEK_SET);
  offsetBytes = 4*6+8*3;
  fseek(fp, offsetBytes, SEEK_SET);

  /* Getting meta data block size */
  fb = fread((void *) &imageSeqSize, (size_t) 4 , (size_t) 1, fp);
  
  /*
   * Going to part where meta data offset is is
   */
  fseek(fp, 0, SEEK_SET);
  offsetBytes = 4*7+8*3;
  fseek(fp, offsetBytes, SEEK_SET);

  /* Getting meta data block size */
  fb = fread((void *) &imageSeq, (size_t) 8 , (size_t) 1, fp);

  /*
   * Going to image info
   */
  fseek(fp, 0, SEEK_SET);
  offsetBytes = imageInfo;
  fseek(fp, offsetBytes, SEEK_SET);

  /* Getting meta data block size */
  fb = fread((void *) &imageNum, (size_t) 4 , (size_t) 1, fp);

  /* Getting meta data block size */
  fb = fread((void *) &imageWidth, (size_t) 4 , (size_t) 1, fp);

  /* Getting meta data block size */
  fb = fread((void *) &imageHeight, (size_t) 4 , (size_t) 1, fp);

  /* Getting meta data block size */
  fb = fread((void *) &byteDepth, (size_t) 2 , (size_t) 1, fp);
  fb = fread((void *) &byteDepth, (size_t) 2 , (size_t) 1, fp);

  /* Getting compression type */
  fb = fread((void *) &compressionType, (size_t) 2 , (size_t) 1, fp);
  
  /*
   * Going to image sequence
   */
  offsetBytes = imageSeq;
  fseek(fp, offsetBytes, SEEK_SET);

  int kk;
  for (kk = 0; kk < FRAME_NUMBER; kk++) {
    
    /* Getting frame size */
    fb = fread((void *) &frameZeroSize, (size_t) 4 , (size_t) 1, fp);
    frameSize[kk] = frameZeroSize;

    /* Getting frame offset */
    fb = fread((void *) &frameZeroOffset, (size_t) 4 , (size_t) 1, fp);
    frameOffset[kk] = frameZeroOffset;

  }
  
  /*
   * Going to frame 0
   */
  offsetBytes = frameOffset[0];
  fseek(fp, offsetBytes, SEEK_SET);

  /* Allocating memory for frames */
  kiranaStruct->data = malloc((size_t) sizeof(short)*FRAME_NUMBER*imageHeight*imageWidth);

  /* Setting values of data struct passed to function */
  kiranaStruct->image = gsl_matrix_alloc(imageHeight, imageWidth);
  kiranaStruct->width = imageHeight;
  kiranaStruct->height = imageWidth;
  kiranaStruct->frameNum = FRAME_NUMBER;
  kiranaStruct->frameSize = frameSize[0];

  /* Setting kirana struct image matrix */
  short bufShort;
  int ii, jj;
  for (kk = 0; kk < FRAME_NUMBER; kk++) {
    for (ii = 0; ii < imageHeight; ii++) {
      for (jj = 0; jj < imageWidth; jj++) {
	
	fb = fread((void *) &bufShort, (size_t) 2 , (size_t) 1, fp);
	kiranaStruct->data[ii + jj*imageHeight + kk*imageHeight*imageWidth]=bufShort;

      }
    }
  }

  for (ii = 0; ii < imageHeight; ii++) {

    for (jj = 0; jj < imageWidth; jj++) {

      gsl_matrix_set(kiranaStruct->image,ii,jj,kiranaStruct->data[ii+jj*imageHeight+
								  imageHeight*imageWidth*0]);

    }
  }

  fclose(fp);

  return 0;
}

/******************************************************************************
 * Function: saveKiranaImage
 * Inputs: gsl_matrix *, char *
 * Returns: int
 * Description: Save binary matrix data to be read by gnuplot such as:
 * MS = zeros(length(x)+1,length(y)+1);
 * MS(1,1) = length(x);
 * MS(1,2:end) = y;
 * MS(2:end,1) = x;
 * MS(2:end,2:end) = M';
 * % Write data into the file
 * fid = fopen(file,'w');
 * fwrite(fid,MS,'float');
 * fclose(fid);
 * plot 'color_map.bin' binary matrix with image
 * Example:
 * plot 'data/lineIntegrated.dat' binary matrix with image title "Line Integrated"
 ******************************************************************************/

int saveKiranaImage(gsl_matrix *mInput, char *fileName) {

  int numRows = mInput->size1;
  int numCols = mInput->size2;

  /* Creates matrix to get written */
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;
  /* Setting y vector values */
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii);
  }

  /* Setting x vector values */
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii);
  }

  /* Setting matrix values */
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(mInput,ii-1, jj-1));

    }
  }
  
  /* Writting temp matrix to a file */
  FILE *fp2;
  fp2 = fopen(fileName, "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  return 0;

}


/******************************************************************************
 * Function: kiranaSetImage
 * Inputs: kiranaSVFData *, int
 * Returns: int
 * Description: Should select a given image for the kirana. Should be in the
 * struct at some point.
 ******************************************************************************/

int kiranaSetImage(kiranaSVFData *kiranaStruct, int frameNumber) {

  int imageWidth = kiranaStruct->width,
    imageHeight = kiranaStruct->height;

  /* Picking off the data and loading it into the image */
  int ii, jj;
  for (ii = 0; ii < imageWidth; ii++) {

    for (jj = 0; jj < imageHeight; jj++) {

      gsl_matrix_set(kiranaStruct->image,ii,jj,
		     kiranaStruct->data[ii+jj*imageWidth+imageWidth*imageHeight*frameNumber]);

    }
  }

  return 0;

}


/******************************************************************************
 * Function: plotKiranaImage
 * Inputs: char *fileName, int 
 * Returns: int
 * Description: This will plot the image from a Kirana video file
 ******************************************************************************/

int plotKiranaImage(char *fileName, int frameNumber) {

  kiranaSVFData kiranaStruct;

  /* Reading in Kirana data*/
  readSVFFile(&kiranaStruct, fileName);

  /* Setting image in kirana struct */
  kiranaSetImage(&kiranaStruct, frameNumber);

  /* Saving kirana image */
  saveKiranaImage(kiranaStruct.image, "data/kiranaImage.dat");

  /* Plotting kirana data */
  system("script/plot_kirana_image.sh");

  return 0;

}
