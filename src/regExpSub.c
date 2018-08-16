#include "regExpSub.h"

/******************************************************************************
 *
 * This is the source file to modify files for MCNP
 *
 ******************************************************************************/

static char *regExpSub(char *testString, char *regExp, char *replaceString);
static int regExpSubHelper(regex_t regex, char *testString, int *offset, int *end);

/******************************************************************************
 * Function: regExpSubRun
 * Inputs: void
 * Returns: int
 * Description: Replace a regular exprssion with a new string
 ******************************************************************************/

int regExpSubRun() {

  char *testString = "Let's see if HE can findHE the matches HE HE",
    *regExp = "[Hh][Ee]",
    *replaceString = "Corn Dogs";

  char *test = regExpSub(testString, regExp, replaceString);

  printf("%s\n", test);
  
  return 0;
  
}
  
/******************************************************************************
 * Function: regExpSub
 * Inputs: void
 * Returns: int
 * Description: Replace a regular exprssion with a new string
 ******************************************************************************/

static char *regExpSub(char *testString, char *regExp, char *replaceString) {

  regex_t regex;
  int reti,
    ii,jj,kk,mm,
    lengthOld = strlen(testString),
    lengthNew = lengthOld,
    lengthReplace = strlen(replaceString),
    offset,
    end,
    endInd = 0,
    numMatches;

  int offsetArray[lengthOld],
    sizeArray[lengthOld];

  /* Compile regular expression */
  reti = regcomp(&regex, regExp, 0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    return "";
  }

  ii = 0;
  while (regExpSubHelper(regex, &testString[endInd], &offset, &end) == 1) {
    offsetArray[ii] = endInd+offset;
    sizeArray[ii] = end - offset;
    endInd = endInd + end;
    ii++;
  }
  numMatches = ii;
  offsetArray[ii] = offset;

  for (ii = 0; ii < numMatches; ii++) {
    lengthNew = lengthNew + lengthReplace - sizeArray[ii];
  }

  char *newString = (char *)malloc((lengthNew+1)*sizeof(char));
  newString[lengthNew+1] = '\0';
  
  kk = 0;
  jj = 0;
  ii = 0;
  while ( offsetArray[ii] != -1) {
    if ( offsetArray[ii] == kk ) {
      for (mm = 0; mm < lengthReplace; mm++) {
	newString[jj+mm] = replaceString[mm];
      }
      kk = kk + sizeArray[ii];
      jj = jj + lengthReplace;
      ii++;
    } else {
      newString[jj] = testString[kk];
      jj++;
      kk++;
    }
  }
  
  regfree(&regex);

  return newString;

}


/******************************************************************************
 * Function: regExpSubHelper
 * Inputs: 
 * Returns: int
 * Description: A regular expression helper function
 ******************************************************************************/

static int regExpSubHelper(regex_t regex, char *testString, int *offset, int *end) {

  char msgbuf[100];
  int reti;
  regmatch_t regMatches[1];
  
  /* Execute regular expression */
  reti = regexec(&regex, testString, 1, regMatches, 0);
  if (!reti) {
    *offset = regMatches[0].rm_so;
    *end = regMatches[0].rm_eo;
    return 1;
  }
  else if (reti == REG_NOMATCH) {
    *offset = -1;
    *end = -1;
    return 0;
  }
  else {
    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    *offset = -1;
    *end = -1;
    return 0;
  }

}


/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

int testRegExpSub() {

  char *testString = "Let's see if HE can findHE the matches HE HE",
    *regExp = "[Hh][Ee]",
    *replaceString = "1234567891",
    *result = "Corn";

  char *test = regExpSub(testString, regExp, replaceString);
  
  printf("%s\n", test);
  printf("Should be:\n%s\n", result);
  
  /* char *testString = "Let's see if HE can findHE the matches HE HE", */
  /*   *regExp = "[Hh][Ee]", */
  /*   *replaceString = "Corn Dogs"; */

  /* regex_t regex; */
  /* char msgbuf[100]; */
  /* regmatch_t regMatches[1]; */
  /* int reti, */
  /*   ii,jj,kk,mm, */
  /*   lengthOld = strlen(testString), */
  /*   lengthNew = lengthOld, */
  /*   lengthReplace = strlen(replaceString), */
  /*   offset, */
  /*   end, */
  /*   endInd = 0, */
  /*   numMatches; */

  /* int offsetArray[lengthOld], */
  /*   endArray[lengthOld], */
  /*   sizeArray[lengthOld]; */

  /* /\* Compile regular expression *\/ */
  /* reti = regcomp(&regex, regExp, 0); */
  /* if (reti) { */
  /*   fprintf(stderr, "Could not compile regex\n"); */
  /*   exit(1); */
  /* } */

  /* ii = 0; */
  /* while (regExpSubHelper(regex, &testString[endInd], &offset, &end) == 1) { */
  /*   offsetArray[ii] = endInd+offset; */
  /*   endArray[ii] = endInd + end; */
  /*   sizeArray[ii] = end - offset; */
  /*   endInd = endInd + end; */
  /*   ii++; */
  /* } */
  /* numMatches = ii; */
  /* offsetArray[ii] = offset; */
  /* endArray[ii] = end; */

  /* for (ii = 0; ii < numMatches; ii++ ){ */
  /*   printf("Offset: %d\n", offsetArray[ii]); */
  /*   printf("End: %d\n", endArray[ii]); */
  /*   printf("Size: %d\n", sizeArray[ii]); */
  /* } */

  /* for (ii = 0; ii < numMatches; ii++) { */
  /*   lengthNew = lengthNew + lengthReplace - sizeArray[ii]; */
  /* } */
  /* printf("New Length: %d\n", lengthNew); */
  /* printf("Old Length: %d\n", lengthOld); */
  /* printf("Number Matches: %d\n", numMatches); */
  /* printf("Should be: %d\n", lengthReplace - sizeArray[0]); */

  /* char *newString = (char *)malloc(lengthNew*sizeof(char)); */
  /* memset(newString,'-',lengthNew); */

  /* kk = 0; */
  /* jj = 0; */
  /* ii = 0; */
  /* while ( offsetArray[ii] != -1) { */
  /*   if ( offsetArray[ii] == kk ) { */
  /*     for (mm = 0; mm < lengthReplace; mm++) { */
  /* 	newString[jj+mm] = replaceString[mm]; */
  /*     } */
  /*     kk = kk + sizeArray[ii]; */
  /*     jj = jj + lengthReplace; */
  /*     ii++; */
  /*     newString[jj] = testString[kk]; */
  /*   } else { */
  /*     newString[jj] = testString[kk]; */
  /*     jj++; */
  /*     kk++; */
  /*   } */
  /* } */

  /* printf("%s\n", newString); */

  /* regfree(&regex); */
  /* free(newString); */

  return 0;

}
