#include "cpp/analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/

using namespace MDSplusAccess;

static bool checkFileExists(std::string fileName);
static std::string getFileFromShotNumber(long shotNumber);
static int plotSpectroscopy(int shotNumber, std::string tempDataFile,
			    std::string tempScriptFile);
static int plotIP(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		  int tLow, int tHigh);
static int plotVGap(int shotNumber, std::string tempDataFile,
		    std::string tempScriptFile,
		    int tLow, int tHigh);
static int plotCompCurrent(int shotNumber, std::string tempDataFile,
			   std::string tempScriptFile,
			   int tLow, int tHigh);
static int plotNeutron(int shotNumber, gsl_vector *time, gsl_vector *det1, int detNum1,
		       gsl_vector *det2, int detNum2,  gsl_vector *det3, int detNum3,
		       std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh);
static int plotNeutron2(int shotNumber, gsl_vector *time, gsl_vector *det1,
			int detNum1,
		       gsl_vector *det2, int detNum2, std::string tempDataFile,
			std::string tempScriptFile, int tLow, int tHigh);
static int plotAzimuthalArray(int shotNumber, std::string nodeName,
			      std::string tempDataFile, std::string tempScriptFile, 
			      int tLow, int tHigh);
static int plotM1Mode(int shotNumber, std::string tempDataFile,
		      std::string tempScriptFile,
		      int tLow, int tHigh);
static int kiranaImageAnalysis();
static int plotPinchCurrentData(int shotNumber, std::string tempDataFile,
				std::string tempScriptFile, int tLow, int tHigh);
static int plotM0Mode(int shotNumber, std::string tempDataFile,
		      std::string tempScriptFile,
		      int tLow, int tHigh);
static int plotPinchCurrentAverageData(int shotNumber, std::string tempDataFile,
				       std::string tempScriptFile, int tLow,
				       int tHigh);
static int saveIBMData();
static int plotPinchCurrentScaling(std::string tempDataFile,
				   std::string tempScriptFile);
static int plotPinchM1Scaling(std::string tempDataFile, std::string tempScriptFile);
static int plotPinchVgapScaling(std::string tempDataFile, std::string tempScriptFile);
static int plotDualBanksAnalysis();
static int plotDualBanksAnalysis2();
static int saveData(int shotNumber);
static int setGlobalVariables(int shotNumber);
static int clearGlobalVariables();
static gsl_vector *timeIPGlobal;
static gsl_vector *ip1Global;
static gsl_vector *ip2Global;
static gsl_vector *ip3Global;
static gsl_vector *timeVGapGlobal;
static gsl_vector *vgap1Global;
static gsl_vector *vgap2Global;
static gsl_vector *vgap3Global;
static gsl_vector *timeProbeGlobal;
static gsl_vector *m0p5Global;
static gsl_vector *m0p15Global;
static gsl_vector *m0p35Global;
static gsl_vector *m0p45Global;
static gsl_vector *m1p5Global;
static gsl_vector *m1p15Global;
static gsl_vector *m1p35Global;
static gsl_vector *m1p45Global;
static gsl_vector *timeNeutronGlobal;
static gsl_vector *neutron1Global;
static gsl_vector *neutron2Global;
static gsl_vector *neutron3Global;
static gsl_vector *neutron4Global;
static gsl_vector *neutron5Global;
static gsl_vector *neutron6Global;
static gsl_vector *neutron7Global;
static gsl_vector *neutron8Global;
static gsl_vector *neutron9Global;
static gsl_vector *neutron10Global;
static gsl_vector *neutron12Global;
static gsl_vector *timeIBMOutGlobal;
static gsl_vector *ibmOut1Global;
static gsl_vector *ibmOut2Global;
static gsl_vector *ibmOut3Global;
static gsl_vector *ibmOut4Global;
static gsl_vector *ibmOut5Global;
static gsl_vector *ibmOut6Global;
static gsl_vector *timeTBMOutGlobal;
static gsl_vector *tbmOut1Global;
static gsl_vector *tbmOut2Global;
static gsl_vector *tbmOut3Global;
static gsl_vector *tbmOut4Global;
static gsl_vector *tbmOut5Global;
static gsl_vector *tbmOut6Global;
static gsl_vector *tbmOut7Global;
static gsl_vector *tbmOut8Global;
static gsl_vector *tbmOut9Global;
static gsl_vector *tbmOut10Global;
static gsl_vector *tbmOut11Global;
static gsl_vector *tbmOut12Global;

#define TLOW 0
#define THIGH 50
#define USE_IGNITRON_IP true
#define USE_THYRISTOR_IP false
#define USE_NEUTRON true
#define USE_THYRISTOR_BANK false
#define USE_IGNITRON_BANK true

/******************************************************************************
 * Function: plotPostShotAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

int plotPostShotAnalysis() {

  int shotNumber,
    currShotNumber = getCurrentPulseNumber();

  printf("\nEnter Pulse Number> ");
  scanf("%d", &shotNumber);
 
 if (shotNumber <= 0) {
    shotNumber = currShotNumber+shotNumber;
  }

  getchar();

  //plotDualBanksAnalysis();
  //plotDualBanksAnalysis2();
  //plotPinchCurrentData(shotNumber, "data/data.txt", "data/data.sh", 20, 50);
  //saveIBMData();
  //plotPinchVgapScaling("data/data.txt", "data/data.sh");
  //plotPinchCurrentScaling("data/data.txt", "data/data.sh");
  //plotPinchM1Scaling("data/data.txt", "data/data.sh");
  //exit(0);
  
  //setGlobalVariables(shotNumber);
    
  int pid1 = fork();
  int pid2 = fork();
  int pid3 = fork();

  if ( (pid1 == 0) && (pid2==0) && (pid3==0) ) {
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 == 0) && (pid3 > 0 ) ) {
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 == 0 )) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 == 0) ) {
    exit(0);
  }
  else if ( (pid1 == 0) && (pid2 > 0) && (pid3 > 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 == 0) ) {

    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 == 0) && (pid3 > 0) ) {
    exit(0);
  }
  else if ( (pid1 > 0) && (pid2 > 0) && (pid3 > 0) ) {
    exit(0);
  }

  if (0) {
    
    plotSpectroscopy(shotNumber, "data/spec1.txt", "data/spec1.sh");
    plotNeutron(shotNumber, timeNeutronGlobal, neutron1Global, 1, neutron4Global, 4,
		neutron5Global, 5, "data/neutron1.txt", "data/neutron1.sh",
		TLOW, THIGH);
    plotNeutron2(shotNumber, timeNeutronGlobal, neutron10Global, 10, neutron12Global,
		 12, "data/neutron2.txt", "data/neutron2.sh", TLOW, THIGH);
    plotIP(shotNumber, "data/ip1.txt", "data/ip1.sh", TLOW, THIGH);
    plotVGap(shotNumber, "data/vgap2.txt", "data/vgap2.sh", TLOW, THIGH);
    plotCompCurrent(shotNumber, "data/comp3.txt", "data/comp3.sh", TLOW, THIGH);
    plotAzimuthalArray(shotNumber, "\\b_p15_000", "data/azimuth4.txt",
		       "data/azimuth4.sh", TLOW, THIGH);
    kiranaImageAnalysis();
    plotPinchCurrentData(shotNumber, "data/data.txt", "data/data.sh", 0, 100);
    plotM0Mode(shotNumber, "data/m0.txt", "data/m0.sh", 20, 50);
    plotPinchCurrentAverageData(shotNumber, "data/data.txt", "data/data.sh", 20, 50);
    saveIBMData();
    plotPinchCurrentScaling("data/data.txt", "data/data.sh");
    plotPinchM1Scaling("data/data.txt", "data/data.sh");
    plotPinchVgapScaling("data/data.txt", "data/data.sh");
    plotM1Mode(shotNumber, "data/m14.txt", "data/m15.sh", TLOW, THIGH);
    plotDualBanksAnalysis();
    plotDualBanksAnalysis2();
    saveData(shotNumber);
    setGlobalVariables(shotNumber);
    
  }

  clearGlobalVariables();
  
  return 0;

}


/******************************************************************************
 * Function: setGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int setGlobalVariables(int shotNumber) {

  timeIPGlobal = readMDSplusVectorDim(shotNumber, "\\I_P", "fuze");
  ip1Global = readMDSplusVector(shotNumber, "\\I_P", "fuze");
  ip2Global = readMDSplusVector(shotNumber-1, "\\I_P", "fuze");
  ip3Global = readMDSplusVector(shotNumber-2, "\\I_P", "fuze");

  timeVGapGlobal = readMDSplusVectorDim(shotNumber, "\\V_GAP", "fuze");
  vgap1Global = readMDSplusVector(shotNumber, "\\V_GAP", "fuze");
  vgap2Global = readMDSplusVector(shotNumber-1, "\\V_GAP", "fuze");
  vgap3Global = readMDSplusVector(shotNumber-2, "\\V_GAP", "fuze");

  timeProbeGlobal = readMDSplusVectorDim(shotNumber, "\\b_p5_000", "fuze");
  gsl_vector_scale(timeProbeGlobal, 1E6);
  m1p15Global = getM1Mode(shotNumber, "\\b_p15_000");
  m1p35Global = getM1Mode(shotNumber, "\\b_p35_000");
  m1p45Global = getM1Mode(shotNumber, "\\b_p45_000");

  if (USE_NEUTRON) {
    
    timeNeutronGlobal = readMDSplusVectorDim(shotNumber, "\\neutron_10_s", "fuze");
    //neutron1Global = readMDSplusVector(shotNumber, "\\neutron_1_s", "fuze");
    //neutron2Global = readMDSplusVector(shotNumber, "\\neutron_2_s", "fuze");
    //neutron3Global = readMDSplusVector(shotNumber, "\\neutron_3_s", "fuze");
    //neutron4Global = readMDSplusVector(shotNumber, "\\neutron_4_s", "fuze");
    //neutron5Global = readMDSplusVector(shotNumber, "\\neutron_5_s", "fuze");
    //neutron6Global = readMDSplusVector(shotNumber, "\\neutron_6_s", "fuze");
    //neutron7Global = readMDSplusVector(shotNumber, "\\neutron_7_s", "fuze");
    //neutron8Global = readMDSplusVector(shotNumber, "\\neutron_8_s", "fuze");
    //neutron9Global = readMDSplusVector(shotNumber, "\\neutron_9_s", "fuze");
    neutron10Global = readMDSplusVector(shotNumber, "\\neutron_10_s", "fuze");
    neutron12Global = readMDSplusVector(shotNumber, "\\neutron_12_s", "fuze");

  }

  m0p5Global = getM0Mode(shotNumber, "\\b_p5_000");
  gsl_vector_scale(m0p5Global, 5E2);
  m0p15Global = getM0Mode(shotNumber, "\\b_p15_000");
  gsl_vector_scale(m0p15Global, 5E2);  
  m0p35Global = getM0Mode(shotNumber, "\\b_p35_000");
  gsl_vector_scale(m0p35Global, 5E2);  
  m0p45Global = getM0Mode(shotNumber, "\\b_p45_000");
  gsl_vector_scale(m0p45Global, 5E2);  

  if (USE_IGNITRON_BANK) {
    
    if (USE_IGNITRON_IP) {

      timeIBMOutGlobal = readMDSplusVectorDim(shotNumber, "\\I_IBM_1_OUT", "fuze");
    
      ibmOut1Global = readMDSplusVector(shotNumber-2, "\\I_IBM_1_OUT", "fuze");
      ibmOut2Global = readMDSplusVector(shotNumber-2, "\\I_IBM_2_OUT", "fuze");
      ibmOut3Global = readMDSplusVector(shotNumber-2, "\\I_IBM_3_OUT", "fuze");
      ibmOut4Global = readMDSplusVector(shotNumber-2, "\\I_IBM_4_OUT", "fuze");
      ibmOut5Global = readMDSplusVector(shotNumber-2, "\\I_IBM_5_OUT", "fuze");
      ibmOut6Global = readMDSplusVector(shotNumber-2, "\\I_IBM_6_OUT", "fuze");
    
      gsl_vector_free(timeIPGlobal);
      timeIPGlobal = readMDSplusVectorDim(shotNumber, "\\I_IBM_1_OUT", "fuze");
    
      gsl_vector_free(ip3Global);
      ip3Global = readMDSplusVector(shotNumber-2, "\\I_IBM_1_OUT", "fuze");
      gsl_vector_add(ip3Global, ibmOut2Global);
      gsl_vector_add(ip3Global, ibmOut3Global);
      gsl_vector_add(ip3Global, ibmOut4Global);
      gsl_vector_add(ip3Global, ibmOut5Global);
      gsl_vector_add(ip3Global, ibmOut6Global); 

      gsl_vector_free(ibmOut1Global);
      gsl_vector_free(ibmOut2Global);
      gsl_vector_free(ibmOut3Global);
      gsl_vector_free(ibmOut4Global);
      gsl_vector_free(ibmOut5Global);
      gsl_vector_free(ibmOut6Global);

      gsl_vector_free(ip2Global);
      ip2Global = readMDSplusVector(shotNumber-1, "\\I_IBM_1_OUT", "fuze");  
      ibmOut1Global = readMDSplusVector(shotNumber-1, "\\I_IBM_1_OUT", "fuze");
      ibmOut2Global = readMDSplusVector(shotNumber-1, "\\I_IBM_2_OUT", "fuze");
      ibmOut3Global = readMDSplusVector(shotNumber-1, "\\I_IBM_3_OUT", "fuze");
      ibmOut4Global = readMDSplusVector(shotNumber-1, "\\I_IBM_4_OUT", "fuze");
      ibmOut5Global = readMDSplusVector(shotNumber-1, "\\I_IBM_5_OUT", "fuze");
      ibmOut6Global = readMDSplusVector(shotNumber-1, "\\I_IBM_6_OUT", "fuze");
  
      gsl_vector_add(ip2Global, ibmOut2Global);
      gsl_vector_add(ip2Global, ibmOut3Global);
      gsl_vector_add(ip2Global, ibmOut4Global);
      gsl_vector_add(ip2Global, ibmOut5Global);
      gsl_vector_add(ip2Global, ibmOut6Global); 

      gsl_vector_free(ibmOut1Global);
      gsl_vector_free(ibmOut2Global);
      gsl_vector_free(ibmOut3Global);
      gsl_vector_free(ibmOut4Global);
      gsl_vector_free(ibmOut5Global);
      gsl_vector_free(ibmOut6Global);
    
      gsl_vector_free(ip1Global);
      ip1Global = readMDSplusVector(shotNumber, "\\I_IBM_1_OUT", "fuze");  
      ibmOut1Global = readMDSplusVector(shotNumber, "\\I_IBM_1_OUT", "fuze");
      ibmOut2Global = readMDSplusVector(shotNumber, "\\I_IBM_2_OUT", "fuze");
      ibmOut3Global = readMDSplusVector(shotNumber, "\\I_IBM_3_OUT", "fuze");
      ibmOut4Global = readMDSplusVector(shotNumber, "\\I_IBM_4_OUT", "fuze");
      ibmOut5Global = readMDSplusVector(shotNumber, "\\I_IBM_5_OUT", "fuze");
      ibmOut6Global = readMDSplusVector(shotNumber, "\\I_IBM_6_OUT", "fuze");
  
      gsl_vector_add(ip1Global, ibmOut2Global);
      gsl_vector_add(ip1Global, ibmOut3Global);
      gsl_vector_add(ip1Global, ibmOut4Global);
      gsl_vector_add(ip1Global, ibmOut5Global);
      gsl_vector_add(ip1Global, ibmOut6Global);

    } else {

      timeIBMOutGlobal = readMDSplusVectorDim(shotNumber, "\\I_IBM_1_OUT", "fuze");
      ibmOut1Global = readMDSplusVector(shotNumber, "\\I_IBM_1_OUT", "fuze");
      ibmOut2Global = readMDSplusVector(shotNumber, "\\I_IBM_2_OUT", "fuze");
      ibmOut3Global = readMDSplusVector(shotNumber, "\\I_IBM_3_OUT", "fuze");
      ibmOut4Global = readMDSplusVector(shotNumber, "\\I_IBM_4_OUT", "fuze");
      ibmOut5Global = readMDSplusVector(shotNumber, "\\I_IBM_5_OUT", "fuze");
      ibmOut6Global = readMDSplusVector(shotNumber, "\\I_IBM_6_OUT", "fuze");

    }

  }

  if (USE_THYRISTOR_BANK) {
    
    if (USE_THYRISTOR_IP) {

      timeTBMOutGlobal = readMDSplusVectorDim(shotNumber, "\\I_TBM_1_OUT", "fuze");
    
      tbmOut1Global = readMDSplusVector(shotNumber-2, "\\I_TBM_1_OUT", "fuze");
      tbmOut2Global = readMDSplusVector(shotNumber-2, "\\I_TBM_2_OUT", "fuze");
      tbmOut3Global = readMDSplusVector(shotNumber-2, "\\I_TBM_3_OUT", "fuze");
      tbmOut4Global = readMDSplusVector(shotNumber-2, "\\I_TBM_4_OUT", "fuze");
      tbmOut5Global = readMDSplusVector(shotNumber-2, "\\I_TBM_5_OUT", "fuze");
      tbmOut6Global = readMDSplusVector(shotNumber-2, "\\I_TBM_6_OUT", "fuze");
      tbmOut7Global = readMDSplusVector(shotNumber-2, "\\I_TBM_7_OUT", "fuze");
      tbmOut8Global = readMDSplusVector(shotNumber-2, "\\I_TBM_8_OUT", "fuze");
      tbmOut9Global = readMDSplusVector(shotNumber-2, "\\I_TBM_9_OUT", "fuze");
      tbmOut10Global = readMDSplusVector(shotNumber-2, "\\I_TBM_10_OUT", "fuze");
      tbmOut11Global = readMDSplusVector(shotNumber-2, "\\I_TBM_11_OUT", "fuze");
      tbmOut12Global = readMDSplusVector(shotNumber-2, "\\I_TBM_12_OUT", "fuze");
    
      gsl_vector_free(timeIPGlobal);
      timeIPGlobal = readMDSplusVectorDim(shotNumber, "\\I_TBM_1_OUT", "fuze");
    
      gsl_vector_free(ip3Global);
      ip3Global = readMDSplusVector(shotNumber-2, "\\I_TBM_1_OUT", "fuze");
      gsl_vector_add(ip3Global, tbmOut2Global);
      gsl_vector_add(ip3Global, tbmOut3Global);
      gsl_vector_add(ip3Global, tbmOut4Global);
      gsl_vector_add(ip3Global, tbmOut5Global);
      gsl_vector_add(ip3Global, tbmOut6Global);
      gsl_vector_add(ip3Global, tbmOut7Global);
      gsl_vector_add(ip3Global, tbmOut8Global);
      gsl_vector_add(ip3Global, tbmOut9Global);
      gsl_vector_add(ip3Global, tbmOut10Global);
      gsl_vector_add(ip3Global, tbmOut11Global);
      gsl_vector_add(ip3Global, tbmOut12Global);

      gsl_vector_free(tbmOut1Global);
      gsl_vector_free(tbmOut2Global);
      gsl_vector_free(tbmOut3Global);
      gsl_vector_free(tbmOut4Global);
      gsl_vector_free(tbmOut5Global);
      gsl_vector_free(tbmOut6Global);
      gsl_vector_free(tbmOut7Global);
      gsl_vector_free(tbmOut8Global);
      gsl_vector_free(tbmOut9Global);
      gsl_vector_free(tbmOut10Global);
      gsl_vector_free(tbmOut11Global);
      gsl_vector_free(tbmOut12Global);

      gsl_vector_free(ip2Global);
      ip2Global = readMDSplusVector(shotNumber-1, "\\I_TBM_1_OUT", "fuze");  
      tbmOut1Global = readMDSplusVector(shotNumber-1, "\\I_TBM_1_OUT", "fuze");
      tbmOut2Global = readMDSplusVector(shotNumber-1, "\\I_TBM_2_OUT", "fuze");
      tbmOut3Global = readMDSplusVector(shotNumber-1, "\\I_TBM_3_OUT", "fuze");
      tbmOut4Global = readMDSplusVector(shotNumber-1, "\\I_TBM_4_OUT", "fuze");
      tbmOut5Global = readMDSplusVector(shotNumber-1, "\\I_TBM_5_OUT", "fuze");
      tbmOut6Global = readMDSplusVector(shotNumber-1, "\\I_TBM_6_OUT", "fuze");
      tbmOut7Global = readMDSplusVector(shotNumber-1, "\\I_TBM_7_OUT", "fuze");
      tbmOut8Global = readMDSplusVector(shotNumber-1, "\\I_TBM_8_OUT", "fuze");
      tbmOut9Global = readMDSplusVector(shotNumber-1, "\\I_TBM_9_OUT", "fuze");
      tbmOut10Global = readMDSplusVector(shotNumber-1, "\\I_TBM_10_OUT", "fuze");
      tbmOut11Global = readMDSplusVector(shotNumber-1, "\\I_TBM_11_OUT", "fuze");
      tbmOut12Global = readMDSplusVector(shotNumber-1, "\\I_TBM_12_OUT", "fuze");
  
      gsl_vector_add(ip2Global, tbmOut2Global);
      gsl_vector_add(ip2Global, tbmOut3Global);
      gsl_vector_add(ip2Global, tbmOut4Global);
      gsl_vector_add(ip2Global, tbmOut5Global);
      gsl_vector_add(ip2Global, tbmOut6Global); 
      gsl_vector_add(ip2Global, tbmOut7Global);
      gsl_vector_add(ip2Global, tbmOut8Global);
      gsl_vector_add(ip2Global, tbmOut9Global);
      gsl_vector_add(ip2Global, tbmOut10Global);
      gsl_vector_add(ip2Global, tbmOut11Global);
      gsl_vector_add(ip2Global, tbmOut12Global);
      
      gsl_vector_free(tbmOut1Global);
      gsl_vector_free(tbmOut2Global);
      gsl_vector_free(tbmOut3Global);
      gsl_vector_free(tbmOut4Global);
      gsl_vector_free(tbmOut5Global);
      gsl_vector_free(tbmOut6Global);
      gsl_vector_free(tbmOut7Global);
      gsl_vector_free(tbmOut8Global);
      gsl_vector_free(tbmOut9Global);
      gsl_vector_free(tbmOut10Global);
      gsl_vector_free(tbmOut11Global);
      gsl_vector_free(tbmOut12Global);
      
      gsl_vector_free(ip1Global);
      ip1Global = readMDSplusVector(shotNumber, "\\I_TBM_1_OUT", "fuze");  
      tbmOut1Global = readMDSplusVector(shotNumber, "\\I_TBM_1_OUT", "fuze");
      tbmOut2Global = readMDSplusVector(shotNumber, "\\I_TBM_2_OUT", "fuze");
      tbmOut3Global = readMDSplusVector(shotNumber, "\\I_TBM_3_OUT", "fuze");
      tbmOut4Global = readMDSplusVector(shotNumber, "\\I_TBM_4_OUT", "fuze");
      tbmOut5Global = readMDSplusVector(shotNumber, "\\I_TBM_5_OUT", "fuze");
      tbmOut6Global = readMDSplusVector(shotNumber, "\\I_TBM_6_OUT", "fuze");
      tbmOut7Global = readMDSplusVector(shotNumber, "\\I_TBM_7_OUT", "fuze");
      tbmOut8Global = readMDSplusVector(shotNumber, "\\I_TBM_8_OUT", "fuze");
      tbmOut9Global = readMDSplusVector(shotNumber, "\\I_TBM_9_OUT", "fuze");
      tbmOut10Global = readMDSplusVector(shotNumber, "\\I_TBM_10_OUT", "fuze");
      tbmOut11Global = readMDSplusVector(shotNumber, "\\I_TBM_11_OUT", "fuze");
      tbmOut12Global = readMDSplusVector(shotNumber, "\\I_TBM_12_OUT", "fuze");
      
      gsl_vector_add(ip1Global, tbmOut2Global);
      gsl_vector_add(ip1Global, tbmOut3Global);
      gsl_vector_add(ip1Global, tbmOut4Global);
      gsl_vector_add(ip1Global, tbmOut5Global);
      gsl_vector_add(ip1Global, tbmOut6Global);
      gsl_vector_add(ip1Global, tbmOut7Global);
      gsl_vector_add(ip1Global, tbmOut8Global);
      gsl_vector_add(ip1Global, tbmOut9Global);
      gsl_vector_add(ip1Global, tbmOut10Global);
      gsl_vector_add(ip1Global, tbmOut11Global);
      gsl_vector_add(ip1Global, tbmOut12Global);
      
    } else {

      timeTBMOutGlobal = readMDSplusVectorDim(shotNumber, "\\I_TBM_01_OUT", "fuze");
      tbmOut1Global = readMDSplusVector(shotNumber, "\\I_TBM_01_OUT", "fuze");
      tbmOut2Global = readMDSplusVector(shotNumber, "\\I_TBM_02_OUT", "fuze");
      tbmOut3Global = readMDSplusVector(shotNumber, "\\I_TBM_03_OUT", "fuze");
      tbmOut4Global = readMDSplusVector(shotNumber, "\\I_TBM_04_OUT", "fuze");
      tbmOut5Global = readMDSplusVector(shotNumber, "\\I_TBM_05_OUT", "fuze");
      tbmOut6Global = readMDSplusVector(shotNumber, "\\I_TBM_06_OUT", "fuze");
      tbmOut7Global = readMDSplusVector(shotNumber, "\\I_TBM_07_OUT", "fuze");
      tbmOut8Global = readMDSplusVector(shotNumber, "\\I_TBM_08_OUT", "fuze");
      tbmOut9Global = readMDSplusVector(shotNumber, "\\I_TBM_09_OUT", "fuze");
      tbmOut10Global = readMDSplusVector(shotNumber, "\\I_TBM_10_OUT", "fuze");
      tbmOut11Global = readMDSplusVector(shotNumber, "\\I_TBM_11_OUT", "fuze");
      tbmOut12Global = readMDSplusVector(shotNumber, "\\I_TBM_12_OUT", "fuze");
      
    }
    
  }
  
  return 0;

}


/******************************************************************************
 * Function: clearGlobalVariables
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int clearGlobalVariables() {

  gsl_vector_free(timeIPGlobal);
  gsl_vector_free(ip1Global);
  gsl_vector_free(ip2Global);
  gsl_vector_free(ip3Global);
  gsl_vector_free(timeVGapGlobal);
  gsl_vector_free(vgap1Global);
  gsl_vector_free(vgap2Global);
  gsl_vector_free(vgap3Global);
  gsl_vector_free(timeProbeGlobal);
  gsl_vector_free(m0p5Global);
  gsl_vector_free(m0p15Global);
  gsl_vector_free(m0p35Global);
  gsl_vector_free(m0p45Global);
  gsl_vector_free(m1p5Global);
  gsl_vector_free(m1p15Global);
  gsl_vector_free(m1p35Global);
  gsl_vector_free(m1p45Global);
  gsl_vector_free(timeNeutronGlobal);
  gsl_vector_free(neutron1Global);
  gsl_vector_free(neutron2Global);
  gsl_vector_free(neutron3Global);
  gsl_vector_free(neutron4Global);
  gsl_vector_free(neutron5Global);
  gsl_vector_free(neutron6Global);
  gsl_vector_free(neutron7Global);
  gsl_vector_free(neutron8Global);
  gsl_vector_free(neutron9Global);
  gsl_vector_free(neutron10Global);
  gsl_vector_free(neutron12Global);
  gsl_vector_free(timeIBMOutGlobal);
  gsl_vector_free(ibmOut1Global);
  gsl_vector_free(ibmOut2Global);
  gsl_vector_free(ibmOut3Global);
  gsl_vector_free(ibmOut4Global);
  gsl_vector_free(ibmOut5Global);
  gsl_vector_free(ibmOut6Global);
  gsl_vector_free(timeTBMOutGlobal);
  gsl_vector_free(tbmOut1Global);
  gsl_vector_free(tbmOut2Global);
  gsl_vector_free(tbmOut3Global);
  gsl_vector_free(tbmOut4Global);
  gsl_vector_free(tbmOut5Global);
  gsl_vector_free(tbmOut6Global);
  gsl_vector_free(tbmOut7Global);
  gsl_vector_free(tbmOut8Global);
  gsl_vector_free(tbmOut9Global);
  gsl_vector_free(tbmOut10Global);
  gsl_vector_free(tbmOut11Global);
  gsl_vector_free(tbmOut12Global);

  return 0;
  
}


/******************************************************************************
 * Function: plotSpectroscopy
 * Inputs: int
 * Returns: int
 * Description: This will analyze images from a LightField file/Spectrometer
 ******************************************************************************/

static int plotSpectroscopy(int shotNumber, std::string tempDataFile,
			    std::string tempScriptFile) {

  std::ostringstream oss;
  const int numFibers = 20;
  const int numEdges = 21;
  std::string shotNumberFileName = getFileFromShotNumber(shotNumber);
  gsl_vector *fiberCenters = 0;
  gsl_vector *fiberEdges = 0 ;
  const double centers[numFibers] = {51, 100, 149, 195, 243, 295, 341, 390, 441, 490,
				     539, 590, 640, 683, 737, 780, 837, 880, 935, 975};

  /* Checking to see if there is a file available */
  if ( !checkFileExists(shotNumberFileName)) {
    std::cout << "No light field file available for shot number "
	      << shotNumber << "\n";
    return false;
  }
        
  /* Getting light field .spe file data from current shot */
  LightField lfObject = LightField(shotNumberFileName);
    
  /* Checking to see if the fiber centers were found. if not, abort */
  if (lfObject.chordsOK) {
    std::cout << "Fiber centers found for shot number "
	      << shotNumber << "\n";
  } else {

      std::cout << "Fiber centers not found \n";
      
      lfObject.setNumEdges(numEdges);
      lfObject.setNumFibers(numFibers);
      fiberCenters = gsl_vector_alloc(numFibers);
      fiberEdges = gsl_vector_alloc(numEdges);
      
      for (int ii = 0; ii < numFibers; ii++) {
	gsl_vector_set(fiberCenters, ii, centers[ii]);
      }

      gsl_vector_set(fiberEdges, 0, 0);
      gsl_vector_set(fiberEdges, numFibers, lfObject.ydim - 1);

      int halfDist, fibE;
      
      for (int ii = 1; ii < numFibers; ii++) {
	halfDist = (int) (gsl_vector_get(fiberCenters, ii) -
			  gsl_vector_get(fiberCenters, ii-1)) / 2;
	fibE = (int) gsl_vector_get(fiberCenters, ii) - halfDist;
	gsl_vector_set(fiberEdges, ii, fibE);
      }
      
      lfObject.setFiberCenters(fiberCenters);
      lfObject.setFiberEdges(fiberEdges);
      lfObject.setNumFibers(fiberCenters->size);
      lfObject.setNumEdges(fiberEdges->size);
      lfObject.populateChords();

  }

  std::string tempLabel;
  oss << "with points title 'Data'";
  tempLabel = oss.str();
  oss.str("");

  // chord1ParamDouble[4] =     {ampPara, centerParam, sigmaPara, offsetParam}
  // 200929048, 227.1 nm CV
  double chordParamDouble[4] = {65000, 227.125, 0.05, 47500};

  // 200929045, 227.1 nm CV
  //double chordParamDouble[4] = {30000, 227.125, 0.05, 50000}

  // 200929044, 227.1 nm CV
  //double chordParamDouble[4] = {245000, 227.1, 0.05, 35000};
  
  // chord1ParamInt[3] = {offset, length, chordNum, plot}
  // 200929048, 227.1 nm CV
  int chordParamInt[4] = {320, 100, 0, 1};

  // 200929045, 227.1 nm CV
  //int chordParamInt[3] = {340, 50, 1};

  // 200929044, 227.1 nm CV
  //int chordParamInt[3] = {340, 40, 1};
    
  // Chord # to plot
  gsl_vector *vectorTemp = lfObject.chord1;
  
  std::string keyWords = "set title 'Spectrum'\n"
    "set ylabel 'Signal'\n"
    "set xlabel 'Pixels'\n"
    "set key left top\n"
    "set yrange[:]\n";

  plot1DVectorData(vectorTemp, tempLabel, "", tempDataFile, tempScriptFile);

  keyWords = "set title 'Spectrum'\n"
    "set ylabel 'Signal'\n"
    "set xlabel 'Wavelength (nm))'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  plotVectorData(lfObject.waveLength, vectorTemp, tempLabel,
		 keyWords, tempDataFile, tempScriptFile);

  gsl_vector *tempVec;
  gsl_vector *temperatureVector = gsl_vector_alloc(20);
  gsl_vector_set_zero(temperatureVector);
  gsl_vector *temperatureErrorVector = gsl_vector_alloc(20);
  gsl_vector_set_zero(temperatureErrorVector);
  gsl_vector *temperatureX = gsl_vector_alloc(20);
  gsl_vector_set_zero(temperatureX);
  
  for (int ii = 0; ii < 20; ii++) {
    
    // Parameters update here to fit values after each iteration.
    chordParamInt[2] = ii+1;
    tempVec =  lfObject.getTemperature(chordParamInt[2],
				       chordParamInt[0],
				       chordParamInt[1],
				       &chordParamDouble[2],
				       &chordParamDouble[0],
				       &chordParamDouble[1],
				       &chordParamDouble[3],
				       chordParamInt[3],
				       tempDataFile, tempScriptFile);
    gsl_vector_set(temperatureVector, ii, gsl_vector_get(tempVec, 0));
    gsl_vector_set(temperatureErrorVector, ii, gsl_vector_get(tempVec, 1));
    gsl_vector_set(temperatureX, ii, ii);
    gsl_vector_free(tempVec);
    
  }

  oss << "title 'T'";
  tempLabel = oss.str();
  oss.str("");

  keyWords = "set title 'Temperature for 200929048 pulse'\n"
    //"set terminal png\n"
    //"set output '/home/fuze/Downloads/temp.png'\n"
    "set ylabel 'Temperature (keV)'\n"
    "set xlabel 'Chord'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  plotVectorDataWithError(temperatureX, temperatureVector,
			  temperatureErrorVector, tempLabel, keyWords,
			  tempDataFile, tempScriptFile);

  gsl_vector_free(fiberEdges);
  gsl_vector_free(fiberCenters);
  gsl_vector_free(temperatureVector);
  gsl_vector_free(temperatureErrorVector);
  gsl_vector_free(temperatureX);
  
  return 0;

  // std::cout << "Fit ion temperature: "
  // 	    << gsl_vector_get(tempVec, 0) << " +/- "
  // 	    << gsl_vector_get(tempVec, 1)
  // 	    << " keV\n";

  // gsl_vector *temperatureVector = gsl_vector_alloc(20);
  // gsl_vector_set_zero(temperatureVector);
  // gsl_vector *temperatureErrorVector = gsl_vector_alloc(20);
  // gsl_vector_set_zero(temperatureErrorVector);
  // for (int ii = 10; ii < 14; ii=ii+2) {
  //   ampParam = 18500;
  //   centerParam = 227.1;
  //   sigmaParam = 0.2;
  //   offsetParam = 315000;
  //   offSet = 320;
  //   length = 80;
  //   chordNum = ii;
  //   tempVec =  lfObject.getTemperature(ii, offSet, length, sigmaParam,
  // 				       ampParam, centerParam, offsetParam);
  //   gsl_vector_set(temperatureVector, ii, gsl_vector_get(tempVec, 0));
  //   gsl_vector_set(temperatureErrorVector, ii, gsl_vector_get(tempVec, 1));
  // }

  // std::string tempLabel;
  // oss << "with points title 'Data'";
  // tempLabel = oss.str();
  // oss.str("");
  
  // plot1DVectorData(temperatureVector, tempLabel, "", tempDataFile, tempScriptFile);
  
  // gsl_vector_view wL = gsl_vector_subvector(lfObject.waveLength, offSet, length);
  // gsl_vector_view chord = gsl_vector_subvector(lfObject.chord8, offSet, length);

  // std::string dataLabel;
  // oss << "with points title 'Data'";
  // dataLabel = oss.str();
  // oss.str("");

  // std::string fitLabel;
  // oss << "with points title 'Fit'";
  // fitLabel = oss.str();
  // oss.str("");

  // std::string keyWords = "set title 'Temperature'\n"
  //   "set ylabel 'Signal'\n"
  //   "set xlabel 'Wavelength (nm))'\n"
  //   "set key left top\n"
  //   "set yrange[:]\n";
  
  // plot2VectorData(&wL.vector, &chord.vector, dataLabel,
  // 		  lfObject.chord8Fit, fitLabel, keyWords,
  // 		  tempDataFile, tempScriptFile);

  //return 0;

}


/******************************************************************************
 * Function: kiranaImageAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will analyze images from the kirana
 ******************************************************************************/

static int kiranaImageAnalysis() {

  Kirana kirana("/home/fuze/webertrNAS/kiranaVideo/170915068.SVF");
  
  kirana.setCropRow(315, 410);
  //kirana.saveImage(0, "data/kiranaImage.dat");
  
  gsl_matrix *image = kirana.getImage(1);

  std::string keywords = "set palette gray\nset title 'corn'"; 
  plotImageData(image, 1, 1, keywords, "data/temp.dat", "data/temp.sh");

  return 0;

}


/******************************************************************************
 * Function: plotIP
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotIP(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		  int tLow, int tHigh) {

  std::ostringstream oss;
  std::string ip1Label;
  std::string ip2Label;
  std::string ip3Label;
  std::string rangeLabel;

  gsl_vector_scale(timeIPGlobal, 1E6);

  gsl_vector_scale(ip1Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'IP for " << shotNumber << "'";
  ip1Label = oss.str();
  oss.str("");

  gsl_vector_scale(ip2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'IP for " << shotNumber-1 << "'";
  ip2Label = oss.str();
  oss.str("");

  gsl_vector_scale(ip3Global, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'IP for " << shotNumber-2 << "'";
  ip3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'I_{P}'\n"
    "set ylabel 'Current (kA)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";

  keyWords.append(rangeLabel);

  plot3VectorData(timeIPGlobal, ip1Global, ip1Label, ip2Global, ip2Label,
		  ip3Global, ip3Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotVgap
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotVGap(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
		    int tLow, int tHigh) {

  std::ostringstream oss;
  std::string vgap1Label;
  std::string vgap2Label;
  std::string vgap3Label;
  std::string rangeLabel;

  gsl_vector_scale(timeVGapGlobal, 1E6);

  gsl_vector_scale(vgap1Global, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'V_{GAP} for " << shotNumber << "'";
  vgap1Label = oss.str();
  oss.str("");

  gsl_vector_scale(vgap2Global, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'V_{GAP} for " << shotNumber-1 << "'";
  vgap2Label = oss.str();
  oss.str("");

  gsl_vector_scale(vgap3Global, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'V_{GAP} for " << shotNumber-2 << "'";
  vgap3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'V_{GAP}'\n"
    "set ylabel 'Voltage (kV)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left top\n"
    "set yrange[:]\n";
  
  keyWords.append(rangeLabel);

  plot3VectorData(timeVGapGlobal, vgap1Global, vgap1Label, vgap2Global, vgap2Label,
		  vgap3Global, vgap3Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotCompCurrent
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotCompCurrent(int shotNumber, std::string tempDataFile, std::string tempScriptFile,
			   int tLow, int tHigh) {

  std::ostringstream oss;
  std::string p5Label;
  std::string p15Label;
  std::string p35Label;
  std::string p45Label;
  std::string keyWords;
  std::string rangeLabel;

  oss << "with line lw 3 lc rgb 'black' title 'I_{P-5}'";
  p5Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title 'I_{P-15}'";
  p15Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'green' title 'I_{P-35}'";
  p35Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'blue' title 'I_{P-45}'";
  p45Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm=0 at different z values for " << shotNumber << "'\n"
      << "set ylabel 'Current (kA)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[:]\n"
      << "set key left top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(timeProbeGlobal, m0p5Global, p5Label, m0p15Global, p15Label, m0p35Global, p35Label,
		  m0p45Global, p45Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotM1Mode
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotM1Mode(int shotNumber, std::string tempDataFile,
		      std::string tempScriptFile,
		      int tLow, int tHigh) {

  std::ostringstream oss;
  std::string p5Label;
  std::string p15Label;
  std::string p35Label;
  std::string p45Label;
  std::string keyWords;
  std::string rangeLabel;

  m1p5Global = getM1Mode(shotNumber, "\\b_p5_000");
  oss << "with line lw 3 lc rgb 'black' title 'm=1 at P5'";
  p5Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title 'm=1 at P15'";
  p15Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'green' title 'm=1 at P35'";
  p35Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'blue' title 'm=1 at P45'";
  p45Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm=1 at different z values for " << shotNumber << "'\n"
      << "set ylabel 'Normalized mode'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:1]\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(timeProbeGlobal, m1p5Global, p5Label, m1p15Global,
		  p15Label, m1p35Global, p35Label,
		  m1p45Global, p45Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotNeutron
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotNeutron(int shotNumber, gsl_vector *time, gsl_vector *det1, int detNum1,
		       gsl_vector *det2, int detNum2,  gsl_vector *det3, int detNum3,
		       std::string tempDataFile, std::string tempScriptFile,
		       int tLow, int tHigh) {

  std::ostringstream oss;
  std::string neutron1Label;
  std::string neutron2Label;
  std::string neutron3Label;
  std::string rangeLabel;
  
  gsl_vector_scale(timeNeutronGlobal, 1E6);

  oss << "with line lw 3 lc rgb 'black' title 'ND" << detNum1 << "for " << shotNumber << "'";
  neutron1Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title 'ND" << detNum2 << "for " << shotNumber << "'";
  neutron2Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'green' title 'ND" << detNum3 << "for " << shotNumber << "'";
  neutron3Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Neutron Detectors'\n"
    "set ylabel 'Signal (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left bottom\n"
    "set yrange[:]\n";

  keyWords.append(rangeLabel);
  
  plot3VectorData(time, det1, neutron1Label, det2, neutron2Label,
		  det3, neutron3Label, keyWords, tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotNeutron2
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotNeutron2(int shotNumber, gsl_vector *time, gsl_vector *det1,
			int detNum1, gsl_vector *det2, int detNum2,
			std::string tempDataFile, std::string tempScriptFile,
			int tLow, int tHigh) {

  std::ostringstream oss;
  std::string neutron1Label;
  std::string neutron2Label;
  std::string rangeLabel;
  
  gsl_vector_scale(timeNeutronGlobal, 1E6);

  oss << "with line lw 3 lc rgb 'black' title 'ND" << detNum1 << "for "
      << shotNumber << "'";
  neutron1Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title 'ND" << detNum2 << "for "
      << shotNumber << "'";
  neutron2Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Neutron Detectors'\n"
    "set ylabel 'Signal (V)'\n"
    "set xlabel 'Time ({/Symbol m}sec)'\n"
    "set key left bottom\n"
    "set yrange[:]\n";

  keyWords.append(rangeLabel);
  
  plot2VectorData(time, det1, neutron1Label, det2, neutron2Label, keyWords,
		  tempDataFile, tempScriptFile);

  return 0;

}


/******************************************************************************
 * Function: plotAzimuthalArray
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotAzimuthalArray(int shotNumber, std::string nodeName,
			      std::string tempDataFile, std::string tempScriptFile,
			      int tLow, int tHigh) {

  std::ostringstream oss;
  std::string p_000Label,
    p_045Label,
    p_090Label,
    p_135Label,
    p_180Label,
    p_225Label,
    p_270Label,
    p_315Label;

  std::string rangeLabel;

  gsl_matrix *azimuthalArray = get8AzimuthalArray(shotNumber, nodeName);  

  int vecSize = azimuthalArray->size1;

  gsl_vector* time = gsl_vector_alloc(vecSize),
    *p_000 = gsl_vector_alloc(vecSize),
    *p_045 = gsl_vector_alloc(vecSize),
    *p_090 = gsl_vector_alloc(vecSize),
    *p_135 = gsl_vector_alloc(vecSize),
    *p_180 = gsl_vector_alloc(vecSize),
    *p_225 = gsl_vector_alloc(vecSize),
    *p_270 = gsl_vector_alloc(vecSize),
    *p_315 = gsl_vector_alloc(vecSize);

  gsl_matrix_get_col(time, azimuthalArray, 0);
  gsl_vector_scale(time, 1E6);
  gsl_matrix_get_col(p_000, azimuthalArray, 1);
  gsl_matrix_get_col(p_045, azimuthalArray, 2);
  gsl_matrix_get_col(p_090, azimuthalArray, 3);
  gsl_matrix_get_col(p_135, azimuthalArray, 4);
  gsl_matrix_get_col(p_180, azimuthalArray, 5);
  gsl_matrix_get_col(p_225, azimuthalArray, 6);
  gsl_matrix_get_col(p_270, azimuthalArray, 7);
  gsl_matrix_get_col(p_315, azimuthalArray, 8);

  oss << "with line lw 3 lc rgb 'black' title '0 Deg.'";
  p_000Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'red' title '45 Deg.'";
  p_045Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'green' title '90 Deg.'";
  p_090Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'orange' title '135 Deg.'";
  p_135Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'yellow' title '180 Deg.'";
  p_180Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'purple' title '225 Deg.'";
  p_225Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'brown' title '270 Deg.'";
  p_270Label = oss.str();
  oss.str("");

  oss << "with line lw 3 lc rgb 'pink' title '315 Deg.'";
  p_315Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  std::string keyWords = "set title 'Azimuthal Array'\n"
    "set xlabel 'Time ({/Symbol m}sec)\n"
    "set key left top\n"; 

  keyWords.append(rangeLabel);

  plot8VectorData(time, p_000, p_000Label, p_045, p_045Label, p_090, p_090Label, 
		  p_135, p_135Label, p_180, p_180Label, p_225, p_225Label,
		  p_270, p_270Label,
		  p_315, p_315Label, keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p_000);
  gsl_vector_free(p_045);
  gsl_vector_free(p_090);
  gsl_vector_free(p_135);
  gsl_vector_free(p_180);
  gsl_vector_free(p_225);
  gsl_vector_free(p_270);
  gsl_vector_free(p_315);

  gsl_matrix_free(azimuthalArray);

  return 0;

}


/******************************************************************************
 * Function: plotPinchCurrentData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotPinchCurrentData(int shotNumber, std::string tempDataFile,
				std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p15M1;
  gsl_vector *p15M0;
  std::string p15M0Label;
  std::string p15M1Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\b_p15_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p15M0 = getM0Mode(shotNumber, "\\b_p15_000");
  gsl_vector_scale(p15M0, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'm=0'";
  p15M0Label = oss.str();
  oss.str("");
  
  p15M1 = getM1Mode(shotNumber, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'red' title 'm=1' axes x1y2";
  p15M1Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm= 0 and 1 at z=15 cm for shotnumber: " << shotNumber << 
    " Thyristor Bank only" << "'\n"
    //<< "set terminal png\n"
    //<< "set output '/home/fuze/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set y2label 'Normalized m=1'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set y2range[0:1]\n"
      << "set y2tics nomirror tc lt 2\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot2VectorData(time, p15M0, p15M0Label, p15M1, p15M1Label, keyWords,
		  tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p15M1);
  gsl_vector_free(p15M0);

  return 0;

}


/******************************************************************************
 * Function: plotM0Mode
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotM0Mode(int shotNumber, std::string tempDataFile,
		      std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p5;
  gsl_vector *p15;
  gsl_vector *p35;
  gsl_vector *p45;
  std::string p5Label;
  std::string p15Label;
  std::string p35Label;
  std::string p45Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\b_p5_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p5 = getM0Mode(shotNumber, "\\b_p5_000");
  gsl_vector_scale(p5, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'm=0 at P5'";
  p5Label = oss.str();
  oss.str("");

  p15 = getM0Mode(shotNumber, "\\b_p15_000");
  gsl_vector_scale(p15, 5E2);
  oss << "with line lw 3 lc rgb 'red' title 'm=0 at P15'";
  p15Label = oss.str();
  oss.str("");

  p35 = getM0Mode(shotNumber, "\\b_p35_000");
  gsl_vector_scale(p35, 5E2);
  oss << "with line lw 3 lc rgb 'green' title 'm=0 at P35'";
  p35Label = oss.str();
  oss.str("");

  p45 = getM0Mode(shotNumber, "\\b_p45_000");
  gsl_vector_scale(p45, 5E2);
  oss << "with line lw 3 lc rgb 'blue' title 'm=0 at P45'";
  p45Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm=0 at different z values for " << shotNumber << "'\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set terminal png\n"
      << "set output '/home/webertr/Downloads/temp.png'\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(time, p5, p5Label, p15, p15Label, p35, p35Label, p45, p45Label,
		  keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p5);
  gsl_vector_free(p15);
  gsl_vector_free(p35);
  gsl_vector_free(p45);

  return 0;

}


/******************************************************************************
 * Function: plotPinchCurrentAverageData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotPinchCurrentAverageData(int shotNumber, std::string tempDataFile,
				       std::string tempScriptFile, int tLow, int tHigh) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p5M0;
  gsl_vector *p15M0;
  gsl_vector *p25M0;
  gsl_vector *p35M0;
  gsl_vector *p45M0;
  gsl_vector *pM0;
  gsl_vector *p5M1;
  gsl_vector *p15M1;
  gsl_vector *p25M1;
  gsl_vector *p35M1;
  gsl_vector *p45M1;
  gsl_vector *pM1;
  
  std::string pM0Label;
  std::string pM1Label;
  std::string keyWords;
  std::string rangeLabel;

  double avg = 0;

  int ii, sizeVec;
  
  time = readMDSplusVectorDim(shotNumber, "\\b_p15_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p5M0 = getM0Mode(shotNumber, "\\b_p5_000");
  p15M0 = getM0Mode(shotNumber, "\\b_p15_000");
  p25M0 = getM0Mode(shotNumber, "\\b_p25_000");
  p35M0 = getM0Mode(shotNumber, "\\b_p35_000");
  p45M0 = getM0Mode(shotNumber, "\\b_p45_000");

  p5M1 = getM1Mode(shotNumber, "\\b_p5_000");
  p15M1 = getM1Mode(shotNumber, "\\b_p15_000");
  p25M1 = getM1Mode(shotNumber, "\\b_p25_000");
  p35M1 = getM1Mode(shotNumber, "\\b_p35_000");
  p45M1 = getM1Mode(shotNumber, "\\b_p45_000");

  sizeVec = p5M0->size;
  pM0 = gsl_vector_alloc(sizeVec);
  pM1 = gsl_vector_alloc(sizeVec);
  
  /* Averaging all axial positions together */
  for (ii = 0; ii < sizeVec; ii++) {
    
    avg = gsl_vector_get(p5M0, ii) + gsl_vector_get(p15M0, ii) +
      gsl_vector_get(p25M0, ii)	
      + gsl_vector_get(p35M0, ii) + gsl_vector_get(p45M0, ii);
    gsl_vector_set(pM0, ii, avg/5.0);

    avg = gsl_vector_get(p5M1, ii) + gsl_vector_get(p15M1, ii) +
      gsl_vector_get(p25M1, ii)
      + gsl_vector_get(p35M1, ii) + gsl_vector_get(p45M1, ii);
    gsl_vector_set(pM1, ii, avg/5.0);
    
  }
  
  gsl_vector_scale(pM0, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'm=0'";
  pM0Label = oss.str();
  oss.str("");
  
  oss << "with line lw 3 lc rgb 'red' title 'm=1' axes x1y2";
  pM1Label = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm= 0 and 1 averaged over z=5-45cm for #" << shotNumber << "'\n"
      << "set terminal png\n"
      << "set output '/home/webertr/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set y2label 'Normalized m=1'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set y2range[0:1]\n"
      << "set y2tics nomirror tc lt 2\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot2VectorData(time, pM0, pM0Label, pM1, pM1Label, keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p5M0);
  gsl_vector_free(p15M0);
  gsl_vector_free(p25M0);
  gsl_vector_free(p35M0);
  gsl_vector_free(pM0);
  gsl_vector_free(p5M1);
  gsl_vector_free(p15M1);
  gsl_vector_free(p25M1);
  gsl_vector_free(p35M1);
  gsl_vector_free(pM1);

  return 0;

}


static int saveIBMData() {

  //std::string fileNameOut = "/home/fuze/Downloads/Ouput_191028003.csv";
  //std::string fileNameGap = "/home/fuze/Downloads/Gap_191028003.csv";
  std::string fileNameOutCB = "/home/fuze/Downloads/Out_200826008.csv";
  std::string fileNameCBCB = "/home/fuze/Downloads/CB_200826008.csv";
  std::string fileNameOutNoCB = "/home/fuze/Downloads/Out_200130027.csv";
  std::string fileNameCBNoCB = "/home/fuze/Downloads/CB_200130027.csv";

  int shotNumberCB = 200826008;
  int shotNumberNoCB = 200130027;
  int sizeRowOut;
  int sizeRowCB;
  int ii;

  gsl_vector *timeOut;
  gsl_vector *timeCB;
  gsl_vector *outRaw1;
  gsl_vector *outRaw2;
  gsl_vector *outRaw3;
  gsl_vector *outRaw4;
  gsl_vector *outRaw5;
  gsl_vector *outRaw6;
  gsl_vector *cbRaw1;
  gsl_vector *cbRaw2;
  gsl_vector *cbRaw3;
  gsl_vector *cbRaw4;
  gsl_vector *cbRaw5;
  gsl_vector *cbRaw6;

  remove(fileNameOutCB.c_str());
  remove(fileNameCBCB.c_str());
  remove(fileNameOutNoCB.c_str());
  remove(fileNameCBNoCB.c_str());
  FILE *fpDataOutCB = fopen(fileNameOutCB.c_str(), "w");
  FILE *fpDataCBCB = fopen(fileNameCBCB.c_str(), "w");
  FILE *fpDataOutNoCB = fopen(fileNameOutNoCB.c_str(), "w");
  FILE *fpDataCBNoCB = fopen(fileNameCBNoCB.c_str(), "w");

  if ( (fpDataOutCB == NULL) || (fpDataCBCB == NULL) ||
       (fpDataOutNoCB == NULL) || (fpDataCBNoCB == NULL) ) {
    printf("Error opening files!\n");
    exit(1);
  }

  timeOut = readMDSplusVectorDim(shotNumberCB, "\\I_IBM_1_OUT", "fuze");
  gsl_vector_scale(timeOut, 1E6);
  outRaw1 = readMDSplusVector(shotNumberCB, "\\I_IBM_1_OUT", "fuze");
  outRaw2 = readMDSplusVector(shotNumberCB, "\\I_IBM_2_OUT", "fuze");
  outRaw3 = readMDSplusVector(shotNumberCB, "\\I_IBM_3_OUT", "fuze");
  outRaw4 = readMDSplusVector(shotNumberCB, "\\I_IBM_4_OUT", "fuze");
  outRaw5 = readMDSplusVector(shotNumberCB, "\\I_IBM_5_OUT", "fuze");
  outRaw6 = readMDSplusVector(shotNumberCB, "\\I_IBM_6_OUT", "fuze");

  timeCB = readMDSplusVectorDim(shotNumberCB, "\\I_IBM_1_CB", "fuze");
  gsl_vector_scale(timeCB, 1E6);
  cbRaw1 = readMDSplusVector(shotNumberCB, "\\I_IBM_1_CB", "fuze");
  cbRaw2 = readMDSplusVector(shotNumberCB, "\\I_IBM_2_CB", "fuze");
  cbRaw3 = readMDSplusVector(shotNumberCB, "\\I_IBM_3_CB", "fuze");
  cbRaw4 = readMDSplusVector(shotNumberCB, "\\I_IBM_4_CB", "fuze");
  cbRaw5 = readMDSplusVector(shotNumberCB, "\\I_IBM_5_CB", "fuze");
  cbRaw6 = readMDSplusVector(shotNumberCB, "\\I_IBM_6_CB", "fuze");
  
  sizeRowOut = (int) timeOut->size;
  sizeRowCB = (int) timeCB->size;

  for (ii = 0; ii < sizeRowOut; ii++) {

    fprintf(fpDataOutCB, "%g\t%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(timeOut, ii), 
	    gsl_vector_get(outRaw1, ii), gsl_vector_get(outRaw2, ii),
	    gsl_vector_get(outRaw3, ii), 
	    gsl_vector_get(outRaw4, ii), gsl_vector_get(outRaw5, ii),
	    gsl_vector_get(outRaw6, ii));

  }

  for (ii = 0; ii < sizeRowCB; ii++) {

    fprintf(fpDataCBCB, "%g\t%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(timeCB, ii), 
	    gsl_vector_get(cbRaw1, ii), gsl_vector_get(cbRaw2, ii),
	    gsl_vector_get(cbRaw3, ii), 
	    gsl_vector_get(cbRaw4, ii), gsl_vector_get(cbRaw5, ii),
	    gsl_vector_get(cbRaw6, ii));

  }

  timeOut = readMDSplusVectorDim(shotNumberNoCB, "\\I_IBM_1_OUT", "fuze");
  //gsl_vector_scale(timeOut, 1E6);
  outRaw1 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_1_OUT", "fuze");
  outRaw2 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_2_OUT", "fuze");
  outRaw3 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_3_OUT", "fuze");
  outRaw4 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_4_OUT", "fuze");
  outRaw5 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_5_OUT", "fuze");
  outRaw6 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_6_OUT", "fuze");

  timeCB = readMDSplusVectorDim(shotNumberNoCB, "\\I_IBM_1_CB", "fuze");
  //gsl_vector_scale(timeCB, 1E6);
  cbRaw1 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_1_CB", "fuze");
  cbRaw2 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_2_CB", "fuze");
  cbRaw3 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_3_CB", "fuze");
  cbRaw4 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_4_CB", "fuze");
  cbRaw5 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_5_CB", "fuze");
  cbRaw6 = readMDSplusVector(shotNumberNoCB, "\\I_IBM_6_CB", "fuze");
  
  sizeRowOut = (int) timeOut->size;
  sizeRowCB = (int) timeCB->size;

  for (ii = 0; ii < sizeRowOut; ii++) {

    fprintf(fpDataOutNoCB, "%g\t%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(timeOut, ii), 
	    gsl_vector_get(outRaw1, ii), gsl_vector_get(outRaw2, ii),
	    gsl_vector_get(outRaw3, ii), 
	    gsl_vector_get(outRaw4, ii), gsl_vector_get(outRaw5, ii),
	    gsl_vector_get(outRaw6, ii));

  }

  for (ii = 0; ii < sizeRowCB; ii++) {

    fprintf(fpDataCBNoCB, "%g\t%g\t%g\t%g\t%g\t%g\t%g\n", gsl_vector_get(timeCB, ii), 
	    gsl_vector_get(cbRaw1, ii), gsl_vector_get(cbRaw2, ii),
	    gsl_vector_get(cbRaw3, ii), 
	    gsl_vector_get(cbRaw4, ii), gsl_vector_get(cbRaw5, ii),
	    gsl_vector_get(cbRaw6, ii));

  }
  
  gsl_vector_free(timeOut);
  gsl_vector_free(timeCB);
  gsl_vector_free(outRaw1);
  gsl_vector_free(outRaw2);
  gsl_vector_free(outRaw3);
  gsl_vector_free(outRaw4);
  gsl_vector_free(outRaw5);
  gsl_vector_free(outRaw6);
  gsl_vector_free(cbRaw1);
  gsl_vector_free(cbRaw2);
  gsl_vector_free(cbRaw3);
  gsl_vector_free(cbRaw4);
  gsl_vector_free(cbRaw5);
  gsl_vector_free(cbRaw6);

  return 0;

}

 
/******************************************************************************
 * Function: plotPinchCurrentScaling
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotPinchCurrentScaling(std::string tempDataFile, std::string tempScriptFile) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p15M04;
  gsl_vector *p15M05;
  gsl_vector *p15M06;
  gsl_vector *p15M07;

  int shotNumber4 = 191106003,
    shotNumber5 = 191106004,
    shotNumber6 = 191106005,
    shotNumber7 = 191106006;
    
  std::string p15M04Label;
  std::string p15M05Label;
  std::string p15M06Label;
  std::string p15M07Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber4, "\\b_p15_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p15M04 = getM0Mode(shotNumber4, "\\b_p15_000");
  gsl_vector_scale(p15M04, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'TBM=4 kV'";
  p15M04Label = oss.str();
  oss.str("");

  p15M05 = getM0Mode(shotNumber5, "\\b_p15_000");
  gsl_vector_scale(p15M05, 5E2);
  oss << "with line lw 3 lc rgb 'red' title 'TBM=5 kV'";
  p15M05Label = oss.str();
  oss.str("");

  p15M06 = getM0Mode(shotNumber6, "\\b_p15_000");
  gsl_vector_scale(p15M06, 5E2);
  oss << "with line lw 3 lc rgb 'blue' title 'TBM=6 kV'";
  p15M06Label = oss.str();
  oss.str("");

  p15M07 = getM0Mode(shotNumber7, "\\b_p15_000");
  gsl_vector_scale(p15M07, 5E2);
  oss << "with line lw 3 lc rgb 'green' title 'TBM=7 kV'";
  p15M07Label = oss.str();
  oss.str("");
  
  oss << "set title 'Pinch current at z=15 cm for 150 psi\n"
      << "set terminal png\n"
      << "set output '/home/fuze/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set xrange[15:50]\n"
      << "set yrange[0:]\n"
      << "set key left top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(time, p15M04, p15M04Label, p15M05, p15M05Label, p15M06, p15M06Label,
		  p15M07, p15M07Label,
		  keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p15M04);
  gsl_vector_free(p15M05);
  gsl_vector_free(p15M06);
  gsl_vector_free(p15M07);

  return 0;

}


/******************************************************************************
 * Function: plotPinchM1Scaling
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotPinchM1Scaling(std::string tempDataFile, std::string tempScriptFile) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p15M14;
  gsl_vector *p15M15;
  gsl_vector *p15M16;
  gsl_vector *p15M17;
  gsl_vector *p15M18;

  int shotNumber4 = 191018004,
    shotNumber5 = 191018005,
    shotNumber6 = 191018006,
    shotNumber7 = 191018007,
    shotNumber8 = 191018008;
    
  std::string p15M14Label;
  std::string p15M15Label;
  std::string p15M16Label;
  std::string p15M17Label;
  std::string p15M18Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber4, "\\b_p15_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p15M14 = getM1Mode(shotNumber4, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'black' title 'TBM=4 kV'";
  p15M14Label = oss.str();
  oss.str("");

  p15M15 = getM1Mode(shotNumber5, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'red' title 'TBM=5 kV'";
  p15M15Label = oss.str();
  oss.str("");

  p15M16 = getM1Mode(shotNumber6, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'blue' title 'TBM=6 kV'";
  p15M16Label = oss.str();
  oss.str("");

  p15M17 = getM1Mode(shotNumber7, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'green' title 'TBM=7 kV'";
  p15M17Label = oss.str();
  oss.str("");

  p15M18 = getM1Mode(shotNumber8, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'yellow' title 'TBM=8 kV'";
  p15M18Label = oss.str();
  oss.str("");
  
  oss << "set title 'Normalized m=1 mode at z=15 cm \n"
      << "set terminal png\n"
      << "set output '/home/webertr/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'Norm m=1'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set xrange[20:40]\n"
      << "set yrange[0:1]\n"
      << "set key right top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot5VectorData(time, p15M14, p15M14Label, p15M15, p15M15Label, p15M16, p15M16Label,
		  p15M17, p15M17Label, p15M18, p15M18Label,
		  keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p15M14);
  gsl_vector_free(p15M15);
  gsl_vector_free(p15M16);
  gsl_vector_free(p15M17);
  gsl_vector_free(p15M18);

  return 0;

}


/******************************************************************************
 * Function: plotPinchVgapScaling
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotPinchVgapScaling(std::string tempDataFile, std::string tempScriptFile) {

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *VGap4;
  gsl_vector *VGap5;
  gsl_vector *VGap6;
  gsl_vector *VGap7;
  gsl_vector *VGap8;

  int shotNumber4 = 191018004,
    shotNumber5 = 191018005,
    shotNumber6 = 191018006,
    shotNumber7 = 191018007,
    shotNumber8 = 191018008;
    
  std::string VGap4Label;
  std::string VGap5Label;
  std::string VGap6Label;
  std::string VGap7Label;
  std::string VGap8Label;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber4, "\\V_GAP", "fuze");
  gsl_vector_scale(time, 1E6);

  VGap4 = readMDSplusVector(shotNumber4, "\\V_GAP", "fuze");
  gsl_vector_scale(VGap4, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'TBM=4 kV'";
  VGap4Label = oss.str();
  oss.str("");

  VGap5 = readMDSplusVector(shotNumber5, "\\V_GAP", "fuze");
  gsl_vector_scale(VGap5, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'TBM=5 kV'";
  VGap5Label = oss.str();
  oss.str("");

  VGap6 = readMDSplusVector(shotNumber6, "\\V_GAP", "fuze");
  gsl_vector_scale(VGap6, 1E-3);
  oss << "with line lw 3 lc rgb 'blue' title 'TBM=6 kV'";
  VGap6Label = oss.str();
  oss.str("");

  VGap7 = readMDSplusVector(shotNumber7, "\\V_GAP", "fuze");
  gsl_vector_scale(VGap7, 1E-3);
  oss << "with line lw 3 lc rgb 'green' title 'TBM=7 kV'";
  VGap7Label = oss.str();
  oss.str("");

  VGap8 = readMDSplusVector(shotNumber8, "\\V_GAP", "fuze");
  gsl_vector_scale(VGap8, 1E-3);
  oss << "with line lw 3 lc rgb 'yellow' title 'TBM=8 kV'";
  VGap8Label = oss.str();
  oss.str("");
  
  oss << "set title 'V_{GAP} scaling with TBM setting\n"
      << "set terminal png\n"
      << "set output '/home/webertr/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'Voltage (kV)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set xrange[20:40]\n"
      << "set yrange[:0]\n"
      << "set key right bottom\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot5VectorData(time, VGap4, VGap4Label, VGap5, VGap5Label, VGap6, VGap6Label,
		  VGap7, VGap7Label, VGap8, VGap8Label,
		  keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(VGap4);
  gsl_vector_free(VGap5);
  gsl_vector_free(VGap6);
  gsl_vector_free(VGap7);
  gsl_vector_free(VGap8);

  return 0;

}


/******************************************************************************
 * Function: plotDualBanksAnalysis
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotDualBanksAnalysis() {

  /*
   * 191122008 - 191122011 are 6 kV with both banks.
   * 191122004 is 6 kV with just the thyristor bank
   */

  std::string tempDataFile = "data/data1.txt",
    tempScriptFile = "data/script1.sh";

  int shotNumberT = 191122004;
  int shotNumberTI = 191122011;

  int tLow = 20,
    tHigh = 50;

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *p15M1T;
  gsl_vector *p15M1TI;
  gsl_vector *p15M0T;
  gsl_vector *p15M0TI;
  std::string p15M0LabelT;
  std::string p15M0LabelTI;
  std::string p15M1LabelT;
  std::string p15M1LabelTI;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumberT, "\\b_p15_000", "fuze");
  gsl_vector_scale(time, 1E6);

  p15M0T = getM0Mode(shotNumberT, "\\b_p15_000");
  gsl_vector_scale(p15M0T, 5E2);
  oss << "with line lw 3 lc rgb 'black' title 'm=0 Thy'";
  p15M0LabelT = oss.str();
  oss.str("");

  p15M0TI = getM0Mode(shotNumberTI, "\\b_p15_000");
  gsl_vector_scale(p15M0TI, 5E2);
  oss << "with line lw 3 lc rgb 'red' title 'm=0 Thy+Ign'";
  p15M0LabelTI = oss.str();
  oss.str("");
  
  p15M1T = getM1Mode(shotNumberT, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'green' title 'm=1 Thy' axes x1y2";
  p15M1LabelT = oss.str();
  oss.str("");

  p15M1TI = getM1Mode(shotNumberTI, "\\b_p15_000");
  oss << "with line lw 3 lc rgb 'blue' title 'm=1 Thy+Ign' axes x1y2";
  p15M1LabelTI = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'm= 0 and 1 at z=15 cm for shotnumbers: " << shotNumberT
      << ", " << shotNumberTI << "'\n"
      << "set terminal png\n"
      << "set output '/home/fuze/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'm=0 (kA)'\n"
      << "set y2label 'Normalized m=1'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set y2range[0:1]\n"
      << "set y2tics nomirror tc lt 2\n"
      << "set key center top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot4VectorData(time, p15M0T, p15M0LabelT, p15M0TI, p15M0LabelTI, p15M1T, p15M1LabelT, 
		  p15M1TI, p15M1LabelTI, keyWords, tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(p15M1T);
  gsl_vector_free(p15M1TI);
  gsl_vector_free(p15M0T);
  gsl_vector_free(p15M0TI);

  return 0;
  
}


/******************************************************************************
 * Function: saveData
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int saveData(int shotNumber) {

  gsl_vector *timeIP;
  gsl_vector *ip1;
  gsl_vector *ip2;
  gsl_vector *ip3;
  gsl_vector *timeVGap;
  gsl_vector *vgap1;
  gsl_vector *vgap2;
  gsl_vector *vgap3;
  
  timeIP = readMDSplusVectorDim(shotNumber, "\\I_P", "fuze");
  saveVectorData(timeIP, "data/timeIP.txt");
  
  ip1 = readMDSplusVector(shotNumber, "\\I_P", "fuze");
  saveVectorData(ip1, "data/ip1.txt");

  ip2 = readMDSplusVector(shotNumber-1, "\\I_P", "fuze");
  saveVectorData(ip2, "data/ip2.txt");

  ip3 = readMDSplusVector(shotNumber-2, "\\I_P", "fuze");
  saveVectorData(ip3, "data/ip3.txt");

  timeVGap = readMDSplusVectorDim(shotNumber, "\\V_GAP", "fuze");
  saveVectorData(timeVGap, "data/timeVGap.txt");
  
  vgap1 = readMDSplusVector(shotNumber, "\\V_GAP", "fuze");
  saveVectorData(vgap1, "data/vgap1.txt");

  vgap2 = readMDSplusVector(shotNumber-1, "\\V_GAP", "fuze");
  saveVectorData(vgap2, "data/vgap2.txt");

  vgap3 = readMDSplusVector(shotNumber-2, "\\V_GAP", "fuze");
  saveVectorData(vgap3, "data/vgap3.txt");
  
  gsl_vector_free(timeIP);
  gsl_vector_free(ip1);
  gsl_vector_free(ip2);
  gsl_vector_free(ip3);
  gsl_vector_free(timeVGap);
  gsl_vector_free(vgap1);
  gsl_vector_free(vgap2);
  gsl_vector_free(vgap3);

  return 0;

}

/******************************************************************************
 * Function: plotDualBanksAnalysis2
 * Inputs: int
 * Returns: int
 * Description: This will prompt the user for a pulse number, and output 
 * the post shot analysis
 ******************************************************************************/

static int plotDualBanksAnalysis2() {

  /*
   * 191122008 - 191122011 are 6 kV with both banks.
   * 191122004 is 6 kV with just the thyristor bank
   */

  std::string tempDataFile = "data/data1.txt",
    tempScriptFile = "data/script1.sh";

  int shotNumber = 191122010;

  int tLow = 0,
    tHigh = 45;

  std::ostringstream oss;
  gsl_vector *time;
  gsl_vector *bankCurrT;
  gsl_vector *bankCurrI;
  std::string bankCurrLabelT;
  std::string bankCurrLabelI;
  std::string keyWords;
  std::string rangeLabel;

  time = readMDSplusVectorDim(shotNumber, "\\I_TBM_01_OUT", "fuze");
  gsl_vector_scale(time, 1E6);

  bankCurrT = readMDSplusVector(shotNumber, "\\I_TBM_01_OUT", "fuze");
  gsl_vector_scale(bankCurrT, 1E-3);
  oss << "with line lw 3 lc rgb 'black' title 'TBM Cap Curr'";
  bankCurrLabelT = oss.str();
  oss.str("");

  bankCurrI = readMDSplusVector(shotNumber, "\\I_IBM_1_OUT", "fuze");
  gsl_vector_scale(bankCurrI, 1E-3);
  oss << "with line lw 3 lc rgb 'red' title 'IBM Cap Curr'";
  bankCurrLabelI = oss.str();
  oss.str("");

  oss << "set xrange[" << tLow << ":" << tHigh << "]\n";
  rangeLabel = oss.str();
  oss.str("");

  oss << "set title 'Current from cap 1 of two banks for shotnumber: " << shotNumber << "'\n"
      << "set terminal png\n"
      << "set output '/home/fuze/Downloads/temp.png'\n"
      << "set grid\n"
      << "set ylabel 'Current (kA)'\n"
      << "set xlabel 'Time ({/Symbol m}sec)'\n"
      << "set yrange[0:]\n"
      << "set key center top\n";

  keyWords = oss.str();
  oss.str("");

  keyWords.append(rangeLabel);
  
  plot2VectorData(time, bankCurrT, bankCurrLabelT, bankCurrI, bankCurrLabelI, keyWords, 
		  tempDataFile, tempScriptFile);

  gsl_vector_free(time);
  gsl_vector_free(bankCurrT);
  gsl_vector_free(bankCurrI);

  return 0;
  
}


/******************************************************************************
 * Function: getFileFromShotNumber
 * Inputs: 
 * Returns: boolean
 * Description: Checks to see if a passed file exists. If it does, return true
 ******************************************************************************/

static std::string getFileFromShotNumber(long shotNumber) {

  /* 
   * 190521009 should be
   * "/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/190521/190521009.spe"
   */

  std::string returnString;
  std::stringstream stringStream;

  //std::string baseFileName = "/mnt/nas/Fuze Data/Spectroscopy (NAS)/Data/";
  std::string baseFileName =   "/home/fuze/SpectroscopyData/";

  int baseShotNumber = shotNumber/1000;
  stringStream << baseShotNumber;
  std::string baseShotNumberString = stringStream.str();
  stringStream.str("");

  stringStream << shotNumber;
  std::string shotNumberString = stringStream.str();
  stringStream.str("");

  returnString.append(baseFileName);
  returnString.append(baseShotNumberString);
  returnString.append("/");
  returnString.append(shotNumberString);
  returnString.append(".spe");
  
  return returnString;

}


/******************************************************************************
 * Function: checkFileExists
 * Inputs: 
 * Returns: boolean
 * Description: Checks to see if a passed file exists. If it does, return true
 ******************************************************************************/

static bool checkFileExists(std::string fileName) {

  std::cout << "Checking to see if " << fileName << " exists\n";

  std::ifstream ifile(fileName.c_str(), std::ifstream::in);

  if (ifile.is_open()) {
    std::cout << fileName << " exists\n";
    return true;
  } else {
    std::cout << fileName << " does not exists\n";
    return false;
  }

}
