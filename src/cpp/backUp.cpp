#include "cpp/backUp.h"

/******************************************************************************
 *
 * This is the source file to backup data
 *
 ******************************************************************************/

/******************************************************************************
 * Example Usage:
 *
 *
 ******************************************************************************/

/******************************************************************************
 * Static Function declarations
 ******************************************************************************/
static int clearGlobalVariables();
static int setGlobalVariables(int shotNumber);
static int saveData(int shotNumber, std::string baseFileName);

/******************************************************************************
 * Global variables
 ******************************************************************************/
static gsl_vector *timeNeutronGlobal = NULL;
static gsl_vector *neutron10Global = NULL;
static gsl_vector *neutron12Global = NULL;
static gsl_vector *timeXRayGlobal = NULL;
static gsl_vector *xRay1Global = NULL;
static gsl_vector *xRay2Global = NULL;
static gsl_vector *xRay3Global = NULL;
static gsl_vector *xRay4Global = NULL;
static gsl_vector *timeIgnitronGlobal = NULL;
static gsl_vector *ignitronOut1Global = NULL;
static gsl_vector *ignitronOut2Global = NULL;
static gsl_vector *ignitronOut3Global = NULL;
static gsl_vector *ignitronOut4Global = NULL;
static gsl_vector *ignitronOut5Global = NULL;
static gsl_vector *ignitronOut6Global = NULL;
static gsl_vector *ignitronCap1Global = NULL;
static gsl_vector *ignitronCap2Global = NULL;
static gsl_vector *ignitronCap3Global = NULL;
static gsl_vector *ignitronCap4Global = NULL;
static gsl_vector *ignitronCap5Global = NULL;
static gsl_vector *ignitronCap6Global = NULL;
static gsl_vector *timeVGapGlobal = NULL;
static gsl_vector *vgapGlobal = NULL;
static gsl_vector *timeGVGlobal = NULL;
static gsl_vector *gv1Global = NULL;
static gsl_vector *gv2Global = NULL;
static gsl_vector *gv3Global = NULL;


/******************************************************************************
 * Function: softXRayRun
 * Inputs:
 * Returns: 
 * Description: Runs soft X-ray analysis
 ******************************************************************************/

int backUpRun() {

  int shotNumber,
    ii;

  std::string baseFileName = "/home/fuze/Downloads/backup/";
  
  for (ii = 200901006; ii < 200901035; ii++) {
    setGlobalVariables(shotNumber);
    saveData(shotNumber, baseFileName);
    shotNumber = shotNumber + 1;
  }

  for (ii = 200902009; ii < 200902010; ii++) {
    setGlobalVariables(shotNumber);
    saveData(shotNumber, baseFileName);
    shotNumber = shotNumber + 1;
  }
  
  clearGlobalVariables();
  
  return 0;
  
}


/******************************************************************************
 * Function: setGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will set the global variables
 ******************************************************************************/

static int setGlobalVariables(int shotNumber) {
  
  timeNeutronGlobal = readMDSplusVectorDim(shotNumber, "\\neutron_10_s", "fuze");
  neutron10Global = readMDSplusVector(shotNumber, "\\neutron_10_s", "fuze");
  neutron12Global = readMDSplusVector(shotNumber, "\\neutron_12_s", "fuze");
  timeXRayGlobal = readMDSplusVectorDim(shotNumber, "\\xray_1", "fuze");
  xRay1Global = readMDSplusVector(shotNumber, "\\xray_1", "fuze");
  xRay2Global = readMDSplusVector(shotNumber, "\\xray_2", "fuze");
  xRay3Global = readMDSplusVector(shotNumber, "\\xray_3", "fuze");
  xRay4Global = readMDSplusVector(shotNumber, "\\xray_4", "fuze");
  timeIgnitronGlobal = readMDSplusVectorDim(shotNumber, "\\i_ibm_1_out", "fuze");
  ignitronOut1Global = readMDSplusVector(shotNumber, "\\i_ibm_1_out", "fuze");
  ignitronOut2Global = readMDSplusVector(shotNumber, "\\i_ibm_2_out", "fuze");
  ignitronOut3Global = readMDSplusVector(shotNumber, "\\i_ibm_3_out", "fuze");
  ignitronOut4Global = readMDSplusVector(shotNumber, "\\i_ibm_4_out", "fuze");
  ignitronOut5Global = readMDSplusVector(shotNumber, "\\i_ibm_5_out", "fuze");
  ignitronOut6Global = readMDSplusVector(shotNumber, "\\i_ibm_6_out", "fuze");
  ignitronCap1Global = readMDSplusVector(shotNumber, "\\i_ibm_1_cap", "fuze");
  ignitronCap2Global = readMDSplusVector(shotNumber, "\\i_ibm_2_cap", "fuze");
  ignitronCap3Global = readMDSplusVector(shotNumber, "\\i_ibm_3_cap", "fuze");
  ignitronCap4Global = readMDSplusVector(shotNumber, "\\i_ibm_4_cap", "fuze");
  ignitronCap5Global = readMDSplusVector(shotNumber, "\\i_ibm_5_cap", "fuze");
  ignitronCap6Global = readMDSplusVector(shotNumber, "\\i_ibm_6_cap", "fuze");
  timeVGapGlobal = readMDSplusVectorDim(shotNumber, "\\V_GAP", "fuze");
  vgapGlobal = readMDSplusVector(shotNumber, "\\V_GAP", "fuze");
  timeGVGlobal = readMDSplusVectorDim(shotNumber, "\\i_gv_1_valve", "fuze");
  gv1Global = readMDSplusVector(shotNumber, "\\i_gv_1_valve", "fuze");
  gv2Global = readMDSplusVector(shotNumber, "\\i_gv_2_valve", "fuze");
  gv3Global = readMDSplusVector(shotNumber, "\\i_gv_3_valve", "fuze");

  return 0;

}


/******************************************************************************
 * Function: clearGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will clear the global variables.
 ******************************************************************************/

static int clearGlobalVariables() {

  gsl_vector_free(timeNeutronGlobal);
  gsl_vector_free(neutron10Global);
  gsl_vector_free(neutron12Global);
  gsl_vector_free(timeXRayGlobal);
  gsl_vector_free(xRay2Global);
  gsl_vector_free(xRay4Global);
  gsl_vector_free(timeIgnitronGlobal);
  gsl_vector_free(ignitronOut1Global);
  gsl_vector_free(ignitronOut2Global);
  gsl_vector_free(ignitronOut3Global);
  gsl_vector_free(ignitronOut4Global);
  gsl_vector_free(ignitronOut5Global);
  gsl_vector_free(ignitronOut6Global);
  gsl_vector_free(ignitronCap1Global);
  gsl_vector_free(ignitronCap2Global);
  gsl_vector_free(ignitronCap3Global);
  gsl_vector_free(ignitronCap4Global);
  gsl_vector_free(ignitronCap5Global);
  gsl_vector_free(ignitronCap6Global);
  gsl_vector_free(timeVGapGlobal);
  gsl_vector_free(vgapGlobal);
  gsl_vector_free(timeGVGlobal);
  gsl_vector_free(gv1Global);
  gsl_vector_free(gv2Global);
  gsl_vector_free(gv3Global);

  return 0;
  
}


/******************************************************************************
 * Function: saveData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int saveData(int shotNumber, std::string baseFileName) {

  /* Make folder with shot number */
  std::string fileName;

  baseFileName += std::to_string(shotNumber);

  mkdir(baseFileName.c_str(),0777);
  baseFileName += "/";
  
  if (0) {
    
    fileName = baseFileName + "neutron10";
    save2VectorData(timeNeutronGlobal, neutron10Global, fileName);
    
    fileName = baseFileName + "neutron12";
    save2VectorData(timeNeutronGlobal, neutron12Global, fileName);

    fileName = baseFileName + "xray1";
    save2VectorData(timeXRayGlobal, xRay1Global, fileName);
    fileName = baseFileName + "xray2";
    save2VectorData(timeXRayGlobal, xRay2Global, fileName);
    fileName = baseFileName + "xray3";
    save2VectorData(timeXRayGlobal, xRay3Global, fileName);
    fileName = baseFileName + "xray4";
    save2VectorData(timeXRayGlobal, xRay4Global, fileName);

    fileName = baseFileName + "ignitronOut1";
    save2VectorData(timeIgnitronGlobal, ignitronOut1Global, fileName);
    fileName = baseFileName + "ignitronOut2";
    save2VectorData(timeIgnitronGlobal, ignitronOut2Global, fileName);
    fileName = baseFileName + "ignitronOut3";
    save2VectorData(timeIgnitronGlobal, ignitronOut3Global, fileName);
    fileName = baseFileName + "ignitronOut4";
    save2VectorData(timeIgnitronGlobal, ignitronOut4Global, fileName);
    fileName = baseFileName + "ignitronOut5";
    save2VectorData(timeIgnitronGlobal, ignitronOut5Global, fileName);
    fileName = baseFileName + "ignitronOut6";
    save2VectorData(timeIgnitronGlobal, ignitronOut6Global, fileName);

    fileName = baseFileName + "ignitronCap1";
    save2VectorData(timeIgnitronGlobal, ignitronCap1Global, fileName);
    fileName = baseFileName + "ignitronCap2";
    save2VectorData(timeIgnitronGlobal, ignitronCap2Global, fileName);
    fileName = baseFileName + "ignitronCap3";
    save2VectorData(timeIgnitronGlobal, ignitronCap3Global, fileName);
    fileName = baseFileName + "ignitronCap4";
    save2VectorData(timeIgnitronGlobal, ignitronCap4Global, fileName);
    fileName = baseFileName + "ignitronCap5";
    save2VectorData(timeIgnitronGlobal, ignitronCap5Global, fileName);
    fileName = baseFileName + "ignitronCap6";
    save2VectorData(timeIgnitronGlobal, ignitronCap6Global, fileName);

    fileName = baseFileName + "vgap";
    save2VectorData(timeVGapGlobal, vgapGlobal, fileName);

    fileName = baseFileName + "gv1";
    save2VectorData(timeGVGlobal, gv1Global, fileName);
    fileName = baseFileName + "gv2";
    save2VectorData(timeGVGlobal, gv2Global, fileName);
    fileName = baseFileName + "gv3";
    save2VectorData(timeGVGlobal, gv3Global, fileName);

  }
  
  return 0;

}
