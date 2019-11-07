#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Gnuplot comment is:
 *  plot 'convert.txt' using 1:2 title "Corn"
 */

int convert(char *fileIn, char *fileOut);

int main() {

  convert("ip.txt", "ip_convert.txt");
  convert("ic1.txt", "ic1_convert.txt");
  convert("iout1.txt", "iout1_convert.txt");
  convert("icb1.txt", "icb1_convert.txt");
  //convert("vgap.txt", "vgap_convert.txt");
  //convert("id1.txt", "id1_convert.txt");
  //convert("it1.txt", "it1_convert.txt");
  //convert("vgapR.txt", "vgapR_convert.txt");
  //convert("vgapL.txt", "vgapL_convert.txt");

  return 0;

}

int convert(char *fileIn, char *fileOut) {


  FILE * fp,
    *fp1;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  int numberPoints = 0;
  
  fp = fopen(fileIn, "r");
  fp1 = fopen(fileOut, "w");
  
  if (fp == NULL) {
    printf("Failure to open: %s", fileIn);
    exit(EXIT_FAILURE);
  }

  while ((read = getline(&line, &len, fp)) != -1) {

    if (read >= 13) {
      line[11] = '\0';
      if (0==strcmp(line, "No. Points:")) {
	numberPoints = atoi(&line[12]);
	break;
      }

    }
    
  }
  while ((read = getline(&line, &len, fp)) != -1) {

    if (0 == strcmp(line, "Values:\n")) {
      break;
    }
    
  }

  int ii = 0,
    jj=0;
  char temp[100];
  while ((read = getline(&line, &len, fp)) != -1) {

    sprintf(temp, "%d", ii);
    
    jj =  strlen(temp);
    
    if (read > 24) {
      line[read-1] = '\t';
      fprintf(fp1, "%s", &line[2+jj]);
    }

    read = getline(&line, &len, fp);
    if (read > 20) {
      fprintf(fp1,"%s", &line[1]);
    }
    
    read = getline(&line, &len, fp);
    if (read == -1) {
      break;
    }
    ii++;
      
  }

  fclose(fp);
  fclose(fp1);
  
  return 0;

}
