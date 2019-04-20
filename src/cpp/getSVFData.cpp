#include "cpp/getSVFData.h"

/******************************************************************************
 * Function: Kirana
 * Inputs: 
 * Returns: 
 * Description: Default constructor
 ******************************************************************************/

Kirana::Kirana(std::string fileNameParam) {

  FILE *fp;
  size_t fb;

  fileName = fileNameParam;

  /* 
   * Opening the file
   */
  fp = fopen(fileName.c_str(), "r");

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
    frameSizeArray[kk] = frameZeroSize;

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
  data = (short *) malloc((size_t) sizeof(short)*FRAME_NUMBER*imageHeight*imageWidth);

  /* Setting values of data struct passed to function */
  width = imageHeight;
  height = imageWidth;
  frameNum = FRAME_NUMBER;
  frameSize = frameSizeArray[0];

  /* Setting kirana struct image matrix */
  short bufShort;
  int ii, jj;
  for (kk = 0; kk < FRAME_NUMBER; kk++) {
    for (ii = 0; ii < imageHeight; ii++) {
      for (jj = 0; jj < imageWidth; jj++) {
	
	fb = fread((void *) &bufShort, (size_t) 2 , (size_t) 1, fp);
	data[ii + jj*imageHeight + kk*imageHeight*imageWidth]=bufShort;

      }
    }
  }

  fclose(fp);  
  
}

/******************************************************************************
 * Function: ~Kirana
 * Inputs: 
 * Returns: 
 * Description: Default destructor. Only delete the malloc memory.
 ******************************************************************************/

Kirana::~Kirana() {

  free(data);

}


/******************************************************************************
 * Function: getImage
 * Inputs: int
 * Returns: gsl_matrix
 * Description: Get the specified image number and return it. Starts at 0
 ******************************************************************************/

gsl_matrix *Kirana::getImage(int imageNumber) {

  gsl_matrix *retImage = gsl_matrix_alloc(imageHeight, imageWidth);

  int ii, jj;
  for (ii = 0; ii < imageHeight; ii++) {
    for (jj = 0; jj < imageWidth; jj++) {

      gsl_matrix_set(retImage,ii,jj,data[ii+jj*imageHeight+
					 imageHeight*imageWidth*imageNumber]);

    }
  }

  return retImage;

}


/******************************************************************************
 * Function: saveImage
 * Inputs: int, std::string
 * Returns: void
 * Description: Save the specified image.
 * Should be able to open it with gnuplot as such:
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

void Kirana::saveImage(int imageNumber, std::string fileName) {

  gsl_matrix *saveImage = gsl_matrix_alloc(imageHeight, imageWidth);

  int ii, jj;
  for (ii = 0; ii < imageHeight; ii++) {
    for (jj = 0; jj < imageWidth; jj++) {

      gsl_matrix_set(saveImage,ii,jj,data[ii+jj*imageHeight+
					 imageHeight*imageWidth*imageNumber]);

    }
  }

  /* Creates matrix to get written */
  gsl_matrix_float* temp = gsl_matrix_float_alloc(imageHeight+1, imageWidth+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) imageWidth);

  /* Setting y vector values */
  for (ii = 1; ii < imageHeight+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii);
  }

  /* Setting x vector values */
  for (ii = 1; ii < imageWidth+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii);
  }

  /* Setting matrix values */
  for (ii = 1; ii < imageHeight+1; ii++) {
    for (jj = 1; jj < imageWidth + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(saveImage,ii-1, jj-1));

    }
  }
  
  /* Writting temp matrix to a file */
  FILE *fp2;
  fp2 = fopen(fileName.c_str(), "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);


  gsl_matrix_float_free(temp);  
  gsl_matrix_free(saveImage);

  return;

}
