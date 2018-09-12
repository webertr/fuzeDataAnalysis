#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

int main(int argc, char *argv[])
{
	// Input Dimensions
	
	double dRadialDist1 = 100;			// distance between detector 1 and central axis
	double dRadialDist2 = 100;			// distance between detector 2 and central axis
	double dRadialDist3 = 100;			// distance between detector 2 and central axis
	double dAxialDist1 = 0;				// distance between detector 1 and nose cone
	double dAxialDist2 = 25;			// distance between detector 2 and nose cone
	double dAxialDist3 = 50;			// distance between detector 2 and nose cone
	double dHeight1 = 139;				// distance between detector 1 and floor
	double dHeight2 = 139;				// distance between detector 2 and floor
	double dHeight3 = 139;				// distance between detector 2 and floor
	
	double length = 150;				// length of cylinder
	double plasmaL = 50;				// length of plasma
	double plasmaR = 0.25;				// radius of plasma
	double wThick = 0.07;				// thickness of tungsten coating
	double electR = 9.60;				// inner radius of outer electrode
	double electThick = 0.7;			// thickness of outer electrode
	double vacVesR = 14.76;				// inner radius of vacuum vessel
	double vacVesGapL = 15.60;			// vacuum vessel length
	double vacVesEndThick = 2.72;		// vacuum vessel end cap thickness
	double vacVesThick = 0.48;			// vacuum vessel thickness
	double innerElectThick = 0.7;		// inner electrode thickness (NEEDS CHECKING)
	double coneR = 5.05;				// inner electrode/nose cone radius
	double electDivideL = 50;			// length of electrode before material change (NEEDS CHECKING)
	double coneL = 7.62;				// nose cone length
	double coneAngle = 29.74;			// angle of nose cone
	
	double endWallTh = 6.99;			// graphite end wall thickness
	double endWallInnerR = 8.89; 		// graphite end wall hole radius
	double endWallOuterR = 13.08; 		// graphite end wall hole radius
	double endWallMidR = 11.46;			// graphite end wall middle radius
	double spokeAngleShift = M_PI/8;	// angle of spokes
	double spokeAngle = M_PI/4;			// angle between spokes
	double spokeTh = 1.8;				// thickness of end wall spokes(NEEDS CHECKING)
	
	double electSlotL = 40.64;			// outer electrode slot length
	double electSlotW = 4.45;			// outer electrode slot width
	double ssPlugThick = 2.24;			// stainless steel plug thickness
	double cuPlugThick = 2.54;			// copper plug thickness
	double windowGapL = 50.11;			// window gap length
	double windowGapH = 5.41;			// window gap height
	double windowL = 54.64;				// window gap length
	double windowH = 9.27;				// window height
	double windowTh = 1.27;				// window thickness
	double windowCaseL = 57.15;			// window case length
	double windowCaseH = 11.43;			// window case width
	double windowCaseShift = 3;			// window shift in x direction (NEEDS CHECKING)
	double windowOut = 1.27;			// distance from window to egde of vacuum vessel
	double windowCaseTh = 3.18;			// thickness of window case (NEEDS CHECKING)
	
	double endWindowCaseR = 5.87;		// radius of window case
	double endWindowCaseTh = 1.73;		// thickness of window case
	double endWindowR = 3.81;			// radius of window
	double endWindowTh = 0.64;			// thickness of window
	double endWindowGapR = 3.42;		// radius of gap in window case
	double endWindowGapTh = 1.04;		// thickness of gap in window case
	
	double coldPlateTh = 2.21;			// cold plate thickness
	double hotPlateTh = 1.27;			// hot plate thickness
	double plateDistance = 20.32;		// distance between hot and cold plate
	double innerPlateR = 20;			// radius of region between hot and cold plate
	double plateR = 30.48;				// hot and cold plate radii
	double innerExtra = 8;				// length of inner electrode past plates (NEEDS CHECKING)
	
	double insulatorL = 25.4;			// insulator length
	double insulatorTh = 2.54; 			// insulator thickness
	double brassSplitRingL = 4.45;      // brass split ring length
	double brassSplitRingTh = 17.53;    // brass split ring thickness
	
	double human1X = 880;			// x-coordinate of human1
	double human1Y = 254;			// y-coordinate of human1
	double human2X = 880.1;			// x-coordinate of human2
	double human2Y = 150;			// y-coordinate of human2
	double human3X = 1050;			// x-coordinate of human3
	double human3Y = 100;			// y-coordinate of human3
	double human4X = -50;			// x-coordinate of human4
	double human4Y = 254;			// y-coordinate of human4
	double human5X = 490;			// x-coordinate of human5
	double human5Y = 254;			// y-coordinate of human5
	double humanHeight = 175;		// height of human
	double humanWidth = 60;			// width of human
	double humanThick = 30;			// thickness of human
	double humanVert = 0;			// vertical position of human (zero means centered)
	double humanHor = 0;			// horizontal position of human (zero means centered)
	
	double X = 515;					// approximate coordinate of reactor
	double Y = 254;					// coordinate of reactor (check)
	double Z = 139;					// vertical coordinate of reactor 
	
	double ceilingH = 442;			// ceiling height
	double ceilingH2 = 254;			// ceiling height in right part of room
	double dividerH = 312;			// height of plywood/kevlar/drywall divider 
	double ceilingThick = 45;		// NOT THE ACTUAL THICKNESS
	double floorThick = 30;			// NOT THE ACTUAL THICKNESS
	double wallThick = 30;			// NOT THE ACTUAL THICKNESS
	double plyThick = 1.91;			// plywood thickness
	double kevlarThick = 0.2;		// kevlar thickness
	double frameThick = 8.89;		// wall frame thickness
	double drywallThick = 1.27;		// drywall thickness
	double wallWindowTh = 1.27;		// window thickness (check this)
	double doorThick = 3.5;			// door thickness
	
	double vacVesExtL = 89.1;		// length of extension
	double attachThick = 2.72;		// thickness of lip attaching extension to rest of device
	double halfAttachWidth = 2.72;	// half width of lip attaching extension to rest of device
	double windowR = 3.75;			// radius of little windows on extension
	double windowCaseR = 5.75;		// radius of metal casing around windows on extension
	double windowSpace = 17;		// spacing between windows
	double bigWindowSpace = 15;		// distance from center of last small window to center of big window
	double bigWindowR = 5.2;		// radius of big window on extension
	double bigWindowCaseR = 7.2;	// radius of metal casing around big window on extension
	double extWindowCaseTh = 2.54;	// distance window case extends radially
	double extWindowTh = 1.27;		// thickness of extension windows
	double extWindowShift = 5.2;	// distance from center of lip attaching extension to first window 
	double fudge = 0.1;             // to shift surfaces slightly
	
	double scintL = 5;				// length of scintillator in detector
	double pmtL = 20;				// length of PMT in detector
	double detectorR = 2.5;			// radius of scintillator and PMT in detector
	double detectorCaseR = 3.18;	// radius of aluminium around scintillator and PMT
	double detectorCaseL = 25.4;	// length of aluminium around scintillator and PMT
	double detectorCaseTh = 0.15;	// thickness of aluminium around scintillator and PMT
	
	double shieldR = 3.18;			// radius of hevimet shield
	double shieldCurveR = 7.32;		// radius of spherical curve on hevimet shield
	double shieldTh = 1.12;			// total thickness of hevimet shield
	
	double detectorX = -50;			// x coordinate of detector
	double detectorY = 0;			// y coordinate of detector
	double detectorZ = 0;			// z coordinate of detector
	
	// Calculated Dimensions
	
	double spokeThAngle = spokeTh/endWallMidR;
	
	double layer1 = plyThick;
	double layer2 = plyThick+kevlarThick;
	double layer3 = 2*plyThick+kevlarThick;
	double layer4 = 2*plyThick+kevlarThick+frameThick;
	double layer5 = 2*plyThick+kevlarThick+frameThick+drywallThick;
	
	double thick1 = drywallThick;
	double thick2 = drywallThick+frameThick;
	double thick3 = drywallThick+frameThick+plyThick;
	double thick4 = drywallThick+frameThick+plyThick+kevlarThick;
	double thick5 = drywallThick+frameThick+plyThick+kevlarThick+plyThick;

	double windowShift = windowCaseShift-windowCaseL/2+windowL/2;
	double windowGapShift = windowCaseShift-windowCaseL/2+windowGapL/2;
	
	double extWindowsX = X+plasmaL+vacVesGapL+2*halfAttachWidth+extWindowShift;

	double wInner	 	= electR-wThick;
	double electOuter 	= electR+electThick;
	double vacVesOuter 	= vacVesR+vacVesThick;
	double radAngle		= coneAngle*M_PI/180;
	double coneWThick	= wThick/tan(radAngle);
	double coneShift	= 1/tan(radAngle);
	double slopeSquared = pow(tan(radAngle),2);
	
	double hotPlateEdge = X-length+plasmaL-coldPlateTh-plateDistance-hotPlateTh;
	double innerElectL  = length-plasmaL-coneL+coldPlateTh+plateDistance+hotPlateTh+innerExtra;
	
	double cosAngle1 = sqrt(pow(dRadialDist1,2)-pow(Z-dHeight1,2))/dRadialDist1;
	double sinAngle1 = (Z-dHeight1)/dRadialDist1;
	double cosAngle2 = sqrt(pow(dRadialDist2,2)-pow(Z-dHeight2,2))/dRadialDist2;
	double sinAngle2 = (Z-dHeight2)/dRadialDist2;
	double cosAngle3 = sqrt(pow(dRadialDist3,2)-pow(Z-dHeight3,2))/dRadialDist3;
	double sinAngle3 = (Z-dHeight3)/dRadialDist3;
	
	double detectorX1 = dAxialDist1+X-detectorX;
	double detectorY1 = -cosAngle1*dRadialDist1+Y-detectorY;
	double detectorZ1 = -sinAngle1*dRadialDist1+Z-detectorZ;
	
	double detectorX2 = dAxialDist2+X-detectorX;
	double detectorY2 = -cosAngle2*dRadialDist2+Y-detectorY;
	double detectorZ2 = -sinAngle2*dRadialDist2+Z-detectorZ;
	
	double detectorX3 = dAxialDist3+X-detectorX;
	double detectorY3 = -cosAngle3*dRadialDist3+Y-detectorY;
	double detectorZ3 = -sinAngle3*dRadialDist3+Z-detectorZ;
	
	// Other Variables
	
	double numDensity = 1.0e24;					            // plasma number density (m^-3)
	double crossSect = 3.130e-27;				            // d-d cross section at 2eV (m^3)
	double plasmaV = M_PI*pow(plasmaR,2)*plasmaL/1e06;      // volume of plasma (m^3)
	double pulseLength = 2.5e-5;				            // Duration of one pulse (s)
	double secPerHr = 3600;						            // number of seconds in one hour
	
	double neutronRateOld = 0.5*pow(numDensity,2)*crossSect*plasmaV*pulseLength;
	double neutronRate = 1.0e8;
	double unitConversion = neutronRate/secPerHr;
	
	double barnsPercm2 = 1e24;
	double dpaConversion = neutronRate/barnsPercm2;
	

	// MCNP input

	std::ofstream fuzeOut("inp",std::ofstream::trunc);

	fuzeOut << "FuZE Machine 2.1\n"
	<< "c **********************************************************************\n"
	<< "c Detector A:\n"
	<< "c radial distance = " << dRadialDist1 << " cm, axial distance = " << dAxialDist1 << " cm, height = " << dHeight1 << " cm\n"
	<< "c\n"
	<< "c Detector B:\n"
	<< "c radial distance = " << dRadialDist2 << " cm, axial distance = " << dAxialDist2 << " cm, height = " << dHeight2 << " cm\n"
	<< "c\n"
	<< "c Detector C:\n"
	<< "c radial distance = " << dRadialDist3 << " cm, axial distance = " << dAxialDist3 << " cm, height = " << dHeight3 << " cm\n"
	<< "c **********************************************************************\n"
	<< "c\n"
			
			///*
			<< "c beginning of cell cards\n"
			<< "101    4      -3.3E-06    -101                          IMP:N=1 IMP:P=1   $ plasma\n"
			<< "102    0                  (101 -102 111):&\n"
			<< "                          (-121 122 123):&\n"
			<< "                          (101 -102 -112 119 142):&\n"
			<< "                          (101 -111 113):&\n"
			<< "                          (101 -111 112 115)            IMP:N=1 IMP:P=1   $ inside of outer electrode\n"
			<< "103    2      -19.25      102 -103 132 133              IMP:N=1 IMP:P=1   $ tungsten spray\n"
			<< "104    1      -8.96       103 -104 132 133              IMP:N=1 IMP:P=1   $ inner electrode\n"
			<< "105    0                  (104 -105 301):&\n"
			<< "                          (-302 311 312 313 314):&\n"
			<< "                          (105 -125 126 127):&\n"
			<< "                          (105 -129 130 131):&\n"
			<< "                          (102 -104 -132):&\n"
			<< "                          (-520 -501):(-520 -503):&\n"
			<< "                          (-520 -505):(-520 -507):(-520 -509):&\n"
			<< "                          (102 -104 -133)               IMP:N=1 IMP:P=1   $ space outside outer electrode\n"
			<< "106    3      -7.99       (105 -106 125 129 501 &\n"
			<< "                          503 505 507 509):(106 -108):&\n"
			<< "                          (106 -107 209)                IMP:N=1 IMP:P=1   $ vacuum chamber\n"
			<< "110    18     -1.78       -110 112 -114 -116            IMP:N=1 IMP:P=1   $ nose cone\n"
			<< "111    18     -1.78       (110 -111 112 -115):&\n"
			<< "                          (-110 -113 114 -115):&\n"
			<< "                          (-111 -115 116)               IMP:N=1 IMP:P=1   $ nose cone tungesten coating\n"
			
			<< "112    18     -1.78       (-301 302):(-311):&\n"
			<< "                          (-312):(-313):(-314)          IMP:N=1 IMP:P=1   $ graphite end cap\n"
			
			<< "113    2      -19.25      -119 120 142                  IMP:N=1 IMP:P=1   $ inner electrode tungesten coating\n"
			<< "114    1      -8.96       -120 121 109                  IMP:N=1 IMP:P=1   $ inner electrode copper\n"
			<< "115    3      -7.99       -120 121 -109                 IMP:N=1 IMP:P=1   $ inner electrode steel\n"
			<< "116    3      -7.99       -122                          IMP:N=1 IMP:P=1   $ inner electrode steel plug\n"
			<< "117    1      -8.96       -123                          IMP:N=1 IMP:P=1   $ inner electrode copper plug\n"
						
			<< "501    10     -2.32       106 -501 520                  IMP:N=1 IMP:P=1   $ ext window 1\n"
			<< "502    3      -7.99       106 -502 501                  IMP:N=1 IMP:P=1   $ ext window case 1 \n"
			<< "503    10     -2.32       106 -503 520                  IMP:N=1 IMP:P=1   $ ext window 2\n"
			<< "504    3      -7.99       106 -504 503                  IMP:N=1 IMP:P=1   $ ext window case 2\n"
			<< "505    10     -2.32       106 -505 520                  IMP:N=1 IMP:P=1   $ ext window 3\n"
			<< "506    3      -7.99       106 -506 505                  IMP:N=1 IMP:P=1   $ ext window 3 case\n"
			<< "507    10     -2.32       106 -507 520                  IMP:N=1 IMP:P=1   $ ext window 4\n"
			<< "508    3      -7.99       106 -508 507                  IMP:N=1 IMP:P=1   $ ext window 4 case\n"
			<< "509    10     -2.32       106 -509 520                  IMP:N=1 IMP:P=1   $ ext window 5\n"
			<< "510    3      -7.99       106 -510 509                  IMP:N=1 IMP:P=1   $ ext window 5 case\n"
			
			<< "121    10     -2.32       (-126):(-127)                 IMP:N=1 IMP:P=1   $ windows 1 and 2\n"
			<< "122    11     -2.70       106 -124 125 126 127          IMP:N=1 IMP:P=1   $ window case\n"
			<< "123    10     -2.32       (-130):(-131)                 IMP:N=1 IMP:P=1   $ windows 3 and 4\n"
			<< "124    11     -2.70       106 -128 129 130 131          IMP:N=1 IMP:P=1   $ window case\n"
			
			<< "125    3      -7.99       -207 208 209                  IMP:N=1 IMP:P=1   $ end window case\n"
			<< "126    10     -2.32       -208                          IMP:N=1 IMP:P=1   $ end window\n"
			<< "127    0                  208 -209                      IMP:N=1 IMP:P=1   $ end window gap\n"
			
			<< "131    3      -7.99       -141 142                      IMP:N=1 IMP:P=1   $ cold plate\n"
			<< "132    12     -3.97       -142 120                      IMP:N=1 IMP:P=1   $ insulator between plates\n"
			<< "133    3      -7.99       -143 120                      IMP:N=1 IMP:P=1   $ hot plate\n"
			<< "134    13     -8.07       -144 120                      IMP:N=1 IMP:P=1   $ brass split ring\n"
			
			<< "211    0                  -211                          IMP:N=1 IMP:P=1   $ scintillator\n"
			<< "212    0                  -212                          IMP:N=1 IMP:P=1   $ pmt\n"
			<< "213    0                  -214 213                      IMP:N=1 IMP:P=1   $ detector casing\n"
			<< "214    0                  -221 -222                     IMP:N=1 IMP:P=1   $ hevimet shield\n"
			
			<< "221    LIKE 211 BUT       TRCL 1\n"
			<< "222    LIKE 212 BUT       TRCL 2\n"
			<< "223    LIKE 213 BUT       TRCL 3\n"
			<< "224    LIKE 214 BUT       TRCL 4\n"
			
			<< "231    LIKE 211 BUT       TRCL 11\n"
			<< "232    LIKE 212 BUT       TRCL 12\n"
			<< "233    LIKE 213 BUT       TRCL 13\n"
			<< "234    LIKE 214 BUT       TRCL 14\n"
			
			<< "241    LIKE 211 BUT       TRCL 21\n"
			<< "242    LIKE 212 BUT       TRCL 22\n"
			<< "243    LIKE 213 BUT       TRCL 23\n"
			<< "244    LIKE 214 BUT       TRCL 24\n"
			
			<< "401    3      -7.99       -401                          IMP:N=1 IMP:P=1   $ Right Optical table\n"
			<< "402    3      -7.99       -402                          IMP:N=1 IMP:P=1   $ Left Optical Table\n"
			<< "403    3      -7.99       -404 403                      IMP:N=1 IMP:P=1   $ Surge Tank\n"
			<< "404    3      -7.99       -405                          IMP:N=1 IMP:P=1   $ Conflat 1\n"
			<< "405    3      -7.99       -406                          IMP:N=1 IMP:P=1   $ Conflat 2\n"
			<< "406    0                  -403                          IMP:N=1 IMP:P=1   $ Inside ST\n"
			
			<< "1      0                  -1                            IMP:N=1 IMP:P=1   $ human1\n"
			<< "2      0                  -2                            IMP:N=1 IMP:P=1   $ human2\n"
			<< "3      0                  -3                            IMP:N=1 IMP:P=1   $ human3\n"
			<< "4      0                  -4                            IMP:N=1 IMP:P=1   $ human4\n"
			<< "5      0                  -5                            IMP:N=1 IMP:P=1   $ human5\n"
			
			<< "33     8      -2.32       -33 -92                       IMP:N=1 IMP:P=1   $ wall 1a\n"
			<< "43     0                  -43 -92                       IMP:N=1 IMP:P=1   $ wall 1b\n"
			<< "53     6      -0.58       -53 -92                       IMP:N=1 IMP:P=1   $ wall 1c\n"
			<< "63     7      -1.44       -63 -92                       IMP:N=1 IMP:P=1   $ wall 1d\n"	
			<< "73     6      -0.58       -73 -92                       IMP:N=1 IMP:P=1   $ wall 1e\n"
			<< "34     8      -2.32       -34 92 -93                    IMP:N=1 IMP:P=1   $ wall 2a\n"
			<< "44     0                  -44 92 -93                    IMP:N=1 IMP:P=1   $ wall 2b\n"
			<< "54     6      -0.58       -54 92 -93                    IMP:N=1 IMP:P=1   $ wall 2c\n"
			<< "64     7      -1.44       -64 92 -93                    IMP:N=1 IMP:P=1   $ wall 2d\n"	
			<< "74     6      -0.58       -74 92 -93                    IMP:N=1 IMP:P=1   $ wall 2e\n"
			<< "35     8      -2.32       -35 93                        IMP:N=1 IMP:P=1   $ wall 3a\n"
			<< "45     0                  -45 93                        IMP:N=1 IMP:P=1   $ wall 3b\n"
			<< "55     6      -0.58       -55 93                        IMP:N=1 IMP:P=1   $ wall 3c\n"
			<< "65     7      -1.44       -65 93                        IMP:N=1 IMP:P=1   $ wall 3d\n"	
			<< "75     6      -0.58       -75 93                        IMP:N=1 IMP:P=1   $ wall 3e\n"
			
			<< "24     5      -2.25       -24 82                        IMP:N=1 IMP:P=1   $ wall 4\n"
			<< "25     5      -2.25       -25 82                        IMP:N=1 IMP:P=1   $ wall 5\n"
			<< "26     5      -2.25       -26 82                        IMP:N=1 IMP:P=1   $ wall 6\n"
			<< "27     5      -2.25       -27                           IMP:N=1 IMP:P=1   $ wall 7\n"
			<< "28     5      -2.25       -28                           IMP:N=1 IMP:P=1   $ wall 8\n"
			<< "29     5      -2.25       -29                           IMP:N=1 IMP:P=1   $ wall 9\n"
			
			<< "30     6      -0.58       -30 -90 85 94                 IMP:N=1 IMP:P=1   $ wall 10a\n"
			<< "40     7      -1.44       -40 -90 85 94                 IMP:N=1 IMP:P=1   $ wall 10b\n"
			<< "50     6      -0.58       -50 -90 85 94                 IMP:N=1 IMP:P=1   $ wall 10c\n"
			<< "60     0                  -60 -90 85 94                 IMP:N=1 IMP:P=1   $ wall 10d\n"	
			<< "70     8      -2.32       -70 -90 85 94                 IMP:N=1 IMP:P=1   $ wall 10e\n"
			<< "31     6      -0.58       -31 90 -91 87                 IMP:N=1 IMP:P=1   $ wall 11a\n"
			<< "41     7      -1.44       -41 90 -91 87                 IMP:N=1 IMP:P=1   $ wall 11b\n"
			<< "51     6      -0.58       -51 90 -91 87                 IMP:N=1 IMP:P=1   $ wall 11c\n"
			<< "61     0                  -61 90 -91 87                 IMP:N=1 IMP:P=1   $ wall 11d\n"	
			<< "71     8      -2.32       -71 90 -91 87                 IMP:N=1 IMP:P=1   $ wall 11e\n"
			<< "32     6      -0.58       -32 91                        IMP:N=1 IMP:P=1   $ wall 12a\n"
			<< "42     7      -1.44       -42 91                        IMP:N=1 IMP:P=1   $ wall 12b\n"
			<< "52     6      -0.58       -52 91                        IMP:N=1 IMP:P=1   $ wall 12c\n"
			<< "62     0                  -62 91                        IMP:N=1 IMP:P=1   $ wall 12d\n"	
			<< "72     8      -2.32       -72 91                        IMP:N=1 IMP:P=1   $ wall 12e\n"
			<< "80     5      -2.25       -80                           IMP:N=1 IMP:P=1   $ ceiling\n"
			<< "81     5      -2.25       -81                           IMP:N=1 IMP:P=1   $ ceiling2\n"
			<< "83     5      -2.25       -83                           IMP:N=1 IMP:P=1   $ ceiling2 wall\n"
			<< "84     5      -2.25       -84                           IMP:N=1 IMP:P=1   $ floor\n"
			<< "86     9      -1.19       -86                           IMP:N=1 IMP:P=1   $ window1\n"
			<< "88     9      -1.19       -88                           IMP:N=1 IMP:P=1   $ window2\n"
			<< "95     6      -0.58       -95                           IMP:N=1 IMP:P=1   $ door\n"
			<< "98     0                  (1 2 3 4 5 106 107 108 207 120 124 128 &\n"
			<< "                          502 503 504 505 506 507 508 509 510 &\n"
			<< "                          #211 #212 #213 #214 &\n"
			<< "                          #221 #222 #223 #224 &\n"
			<< "                          #231 #232 #233 #234 &\n"
			<< "                          #241 #242 #243 #244 &\n"
			<< "                          141 142 143 144 24 25 26 27 28 29 &\n"
			<< "                          30 40 50 60 70 31 41 51 61 71 &\n"
			<< "                          32 42 52 62 72 33 43 53 63 73 &\n"
			<< "                          34 44 54 64 74 35 45 55 65 75 &\n"
			<< "                          80 81 83 84 -99 401 402 404 405 406&\n"
			<< "                          #(-404 403)):(81 -82 83): &\n"
			<< "                          (-85 86):(-87 88):(-94 95) &\n"
			<< "                                                        IMP:N=1 IMP:P=1   $ void\n"
			<< "99     0                  99                            IMP:N=0 IMP:P=0   $ graveyard\n"
			<< "c end of cell cards\n\n"
			//*/
			
			<< "c beginning of surface cards\n"
			
			<< "211      RCC      " << detectorX << " " << detectorY-(detectorCaseL-scintL-pmtL)/2 << " " << detectorZ << " 0 " << -scintL << " 0 " << detectorR << "      $ scintillator\n"
			<< "212      RCC      " << detectorX << " " << detectorY-(detectorCaseL-scintL-pmtL)/2-scintL-0.01 << " " << detectorZ << " 0 " << -pmtL << " 0 " << detectorR << "      $ PMT\n"
			<< "213      RCC      " << detectorX << " " << detectorY-detectorCaseTh << " " << detectorZ << " 0 " << -detectorCaseL+2*detectorCaseTh << " 0 " << detectorCaseR-detectorCaseTh << "      $ inner surface of detector case\n"
			<< "214      RCC      " << detectorX << " " << detectorY << " " << detectorZ << " 0 " << -detectorCaseL << " 0 " << detectorCaseR << "      $ outer surface of detector case\n"

			<< "221      RCC      " << detectorX << " " << detectorY << " " << detectorZ << " 0 " << shieldTh << " 0 " << shieldR << "      $ hevimet shield cylinder\n"
			<< "222      S        " << detectorX << " " << detectorY+shieldTh-shieldCurveR << " " << detectorZ << " " << shieldCurveR << "      $ hevimet shield spherical curve\n"
						
			<< "207      RCC      " << X+plasmaL+vacVesGapL+vacVesExtL+vacVesEndThick << " " << Y << " " << Z << " " << endWindowCaseTh << " 0 0 " << endWindowCaseR << "      $ vacuum vessel end window case\n"
			<< "208      RCC      " << X+plasmaL+vacVesGapL+vacVesExtL+vacVesEndThick+endWindowGapTh << " " << Y << " " << Z << " " << endWindowTh << " 0 0 " << endWindowR << "      $ vacuum vessel end window\n"
			<< "209      RCC      " << X+plasmaL+vacVesGapL+vacVesExtL << " " << Y << " " << Z << " " << vacVesEndThick+endWindowCaseTh << " 0 0 " << endWindowGapR << "      $ vacuum vessel end window gap\n"

			<< "101      RCC      " << X+plasmaL << " " << Y << " " << Z << " " << -plasmaL << " 0 0 " << plasmaR << "      $ plasma\n"
			<< "102      RCC      " << X-length+plasmaL << " " << Y << " " << Z << " " << length << " 0 0 " << wInner << "      $ inner surface of tungston\n"
			<< "103      RCC      " << X-length+plasmaL << " " << Y << " " << Z << " " << length << " 0 0 " << electR << "      $ outer radius of tungston and inner radius of outer electrode\n"
			<< "104      RCC      " << X-length+plasmaL << " " << Y << " " << Z << " " << length << " 0 0 " << electOuter << "      $ outer radius of outer electrode\n"
			<< "105      RCC      " << X-length+plasmaL << " " << Y << " " << Z << " " << length+vacVesGapL+vacVesExtL << " 0 0 " << vacVesR << "      $ inner radius of vacuum vessel\n"
			<< "106      RCC      " << X-length+plasmaL << " " << Y << " " << Z << " " << length+vacVesGapL+vacVesExtL << " 0 0 " << vacVesOuter << "      $ outer radius of vacuum vessel\n"
			<< "107      RCC      " << X+plasmaL+vacVesGapL+vacVesExtL-vacVesEndThick << " " << Y << " " << Z << " " << 2*vacVesEndThick << " 0 0 " << vacVesOuter+vacVesEndThick << "      $ vacuum vessel end cap\n"
			<< "108      RCC      " << X+plasmaL+vacVesGapL-halfAttachWidth << " " << Y << " " << Z << " " << 2*halfAttachWidth << " 0 0 " << vacVesOuter+attachThick << "      $ extension attachment\n"
			<< "110      K/X      " << X-coneWThick+coneShift << " " << Y << " " << Z << " " << slopeSquared << " -1      $ nose cone inner\n"
			<< "111      K/X      " << X+coneShift << " " << Y << " " << Z << " " << slopeSquared << " -1      $ nose cone outer\n"
			<< "112      PX       " << X-coneL << "      $ nose cone end\n"
			<< "113      PX       " << X << "      $ nose cone outer tip\n"
			<< "114      PX       " << X-wThick << "      $ nose cone inner tip\n"
			<< "115      RCC      " << X-coneL << " " << Y << " " << Z << " " << coneL << " 0 0 " << coneR << "      $ nose cone cylinder outer\n"
			<< "116      RCC      " << X-coneL << " " << Y << " " << Z << " " << coneL << " 0 0 " << coneR-wThick << "      $ nose cone cylinder inner\n"
			
			<< "301      RCC      " << X+plasmaL << " " << Y << " " << Z << " " << endWallTh << " 0 0 " << endWallOuterR << "      $ graphite end cap\n"
			<< "302      RCC      " << X+plasmaL << " " << Y << " " << Z << " " << endWallTh << " 0 0 " << endWallInnerR << "      $ graphite end cap hole\n"
			
			<< "311      BOX      " << X+plasmaL << " " << Y-endWallMidR*sin(spokeAngleShift-spokeThAngle/2) << " " << Z-endWallMidR*cos(spokeAngleShift-spokeThAngle/2) << "&\n"
								  "0 " << -spokeTh*cos(spokeAngleShift) << " " << spokeTh*sin(spokeAngleShift) << "&\n"
								  "0 " << 2*endWallMidR*sin(spokeAngleShift) << " " << 2*endWallMidR*cos(spokeAngleShift) << "&\n"
								  << endWallTh << " 0 0 \n"
			
			<< "312      BOX      " << X+plasmaL << " " << Y-endWallMidR*sin(1*spokeAngle+spokeAngleShift-spokeThAngle/2) << " " << Z-endWallMidR*cos(1*spokeAngle+spokeAngleShift-spokeThAngle/2) << "&\n"
								  "0 " << -spokeTh*cos(1*spokeAngle+spokeAngleShift) << " " << spokeTh*sin(1*spokeAngle+spokeAngleShift) << "&\n"
								  "0 " << 2*endWallMidR*sin(1*spokeAngle+spokeAngleShift) << " " << 2*endWallMidR*cos(1*spokeAngle+spokeAngleShift) << "&\n"
								  << endWallTh << " 0 0 \n"
								  
			<< "313      BOX      " << X+plasmaL << " " << Y-endWallMidR*sin(2*spokeAngle+spokeAngleShift-spokeThAngle/2) << " " << Z-endWallMidR*cos(2*spokeAngle+spokeAngleShift-spokeThAngle/2) << "&\n"
								  "0 " << -spokeTh*cos(2*spokeAngle+spokeAngleShift) << " " << spokeTh*sin(2*spokeAngle+spokeAngleShift) << "&\n"
								  "0 " << 2*endWallMidR*sin(2*spokeAngle+spokeAngleShift) << " " << 2*endWallMidR*cos(2*spokeAngle+spokeAngleShift) << "&\n"
								  << endWallTh << " 0 0 \n"
								  
			<< "314      BOX      " << X+plasmaL << " " << Y-endWallMidR*sin(3*spokeAngle+spokeAngleShift-spokeThAngle/2) << " " << Z-endWallMidR*cos(3*spokeAngle+spokeAngleShift-spokeThAngle/2) << "&\n"
								  "0 " << -spokeTh*cos(3*spokeAngle+spokeAngleShift) << " " << spokeTh*sin(3*spokeAngle+spokeAngleShift) << "&\n"
								  "0 " << 2*endWallMidR*sin(3*spokeAngle+spokeAngleShift) << " " << 2*endWallMidR*cos(3*spokeAngle+spokeAngleShift) << "&\n"
								  << endWallTh << " 0 0 \n"
			
			<< "119      RCC      " << X-length+plasmaL << " " << Y << " " << Z << " " << length-plasmaL-coneL << " 0 0 " << coneR << "      $ inner electrode outer tungsten surface\n"
			<< "120      RCC      " << hotPlateEdge-innerExtra << " " << Y << " " << Z << " " << innerElectL << " 0 0 " << coneR-wThick << "      $ inner electrode outer surface\n"
			<< "121      RCC      " << hotPlateEdge-innerExtra+innerElectThick << " " << Y << " " << Z << " " << innerElectL-innerElectThick << " 0 0 " << coneR-innerElectThick << "      $ inner electrode inner surface\n"
			
			<< "501      RCC      " << extWindowsX << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowR << "	$ ext window case inner\n"
			<< "502      RCC      " << extWindowsX << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowCaseR << "	$ ext window case outer\n"
			<< "503      RCC      " << extWindowsX+windowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowR << " 	$ window 2 id\n"
			<< "504      RCC      " << extWindowsX+windowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowCaseR << "	$ window 2 od\n"
			<< "505      RCC      " << extWindowsX+2*windowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowR << " 	$ window 3 id\n"
			<< "506      RCC      " << extWindowsX+2*windowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowCaseR << "	$ window 3 od\n"
			<< "507      RCC      " << extWindowsX+3*windowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowR << "	$ window 4 id\n"
			<< "508      RCC      " << extWindowsX+3*windowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << windowCaseR << "	$ window 4 od\n"
			<< "509      RCC      " << extWindowsX+3*windowSpace+bigWindowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << bigWindowR << "	$ window 5 id\n"
			<< "510      RCC      " << extWindowsX+3*windowSpace+bigWindowSpace << " " << Y-vacVesOuter-extWindowCaseTh << " " << Z << " 0 " << 2*(vacVesOuter+extWindowCaseTh) << " 0 " << bigWindowCaseR << "	$ window 5 od\n"
			<< "520      RPP      " << extWindowsX-windowR-fudge << " " << extWindowsX+4*windowSpace+bigWindowR+fudge << " " << Y-vacVesOuter-extWindowCaseTh+extWindowTh << " " << Y+vacVesOuter+extWindowCaseTh-extWindowTh << " " << Z-bigWindowR << " " << Z+bigWindowR << "      $ for voiding out window gaps\n"

			<< "109      PX       " << X-length+plasmaL+electDivideL << "      $ inner electrode steel/copper divider\n"
			<< "122      RCC      " << X-length+plasmaL+electDivideL-ssPlugThick << " " << Y << " " << Z << " " << ssPlugThick << " 0 0 " << coneR-innerElectThick << "      $ stainless steel plug\n"
			<< "123      RCC      " << X-length+plasmaL+electDivideL << " " << Y << " " << Z << " " << cuPlugThick << " 0 0 " << coneR-innerElectThick << "      $ copper plug\n"
			
			<< "124      RPP      " << X+plasmaL+windowCaseShift-windowCaseL << " " << X+plasmaL+windowCaseShift<< " " << Y-vacVesOuter-windowCaseTh << " " << Y+vacVesOuter+windowCaseTh << " " << Z-windowCaseH/2 << " " << Z+windowCaseH/2 << "      $ vacuum vessel window case\n"
			<< "125      RPP      " << X+plasmaL+windowGapShift-windowGapL << " " << X+plasmaL+windowGapShift << " " << Y-vacVesOuter-windowCaseTh << " " << Y+vacVesOuter+windowCaseTh << " " << Z-windowGapH/2 << " " << Z+windowGapH/2 << "      $ vacuum vessel window case hole\n"
			<< "126      RPP      " << X+plasmaL+windowShift-windowL << " " << X+windowShift+plasmaL << " " << Y-vacVesOuter-windowOut-windowTh << " " << Y-vacVesOuter-windowOut << " " << Z-windowH/2 << " " << Z+windowH/2 << "      $ vacuum vessel window1\n"
			<< "127      RPP      " << X+plasmaL+windowShift-windowL << " " << X+windowShift+plasmaL << " " << Y+vacVesOuter+windowOut << " " << Y+vacVesOuter+windowOut+windowTh << " " << Z-windowH/2 << " " << Z+windowH/2 << "      $ vacuum vessel window2\n"
			
			<< "128      RPP      " << X+plasmaL+windowCaseShift-windowCaseL << " " << X+plasmaL+windowCaseShift<< " " << Y-windowCaseH/2 << " " << Y+windowCaseH/2 << " " << Z-vacVesOuter-windowCaseTh << " " << Z+vacVesOuter+windowCaseTh << "      $ vacuum vessel window case\n"
			<< "129      RPP      " << X+plasmaL+windowGapShift-windowGapL << " " << X+plasmaL+windowGapShift << " " << Y-windowGapH/2 << " " << Y+windowGapH/2 << " " << Z-vacVesOuter-windowCaseTh << " " << Z+vacVesOuter+windowCaseTh << "      $ vacuum vessel window case hole\n"
			<< "130      RPP      " << X+plasmaL+windowShift-windowL << " " << X+windowShift+plasmaL << " " << Y-windowH/2 << " " << Y+windowH/2 << " " << Z-vacVesOuter-windowOut-windowTh << " " << Z-vacVesOuter-windowOut << "      $ vacuum vessel window3\n"
			<< "131      RPP      " << X+plasmaL+windowShift-windowL << " " << X+windowShift+plasmaL << " " << Y-windowH/2 << " " << Y+windowH/2 << " " << Z+vacVesOuter+windowOut << " " << Z+vacVesOuter+windowOut+windowTh << "      $ vacuum vessel window4\n"
			
			<< "132      RPP      " << X+(plasmaL-electSlotL)/2 << " " << X+(plasmaL+electSlotL)/2 << " " << Y-electSlotW/2 << " " << Y+electSlotW/2 << " " << Z-electOuter << " " << Z+electOuter << "      $ outer electrode slot\n"
			<< "133      RPP      " << X+(plasmaL-electSlotL)/2 << " " << X+(plasmaL+electSlotL)/2 << " " << Y-electOuter << " " << Y+electOuter << " " << Z-electSlotW/2 << " " << Z+electSlotW/2 << "      $ outer electrode slot\n"
			
			<< "141      RCC      " << X-length+plasmaL-coldPlateTh << " " << Y << " " << Z << " " << coldPlateTh << " 0 0 " << plateR << "      $ cold plate\n"
			<< "142      RCC      " << X-length+plasmaL-insulatorL/2 << " " << Y << " " << Z << " " << insulatorL << " 0 0 " <<coneR+insulatorTh << "  $ insulator\n"
			<< "143      RCC      " << X-length+plasmaL-coldPlateTh-plateDistance-hotPlateTh << " " << Y << " " << Z << " " << hotPlateTh << " 0 0 " << plateR << "      $ hot plate\n"
			<< "144      RCC      " << X-length+plasmaL-coldPlateTh-plateDistance << " " << Y << " " << Z << " " << brassSplitRingL << " 0 0 " << coneR+brassSplitRingTh/2 << "     $ brass split ring\n"
			
			<< "401    RPP      " << X-length+plasmaL+60.96 << " " << X-length/2+243.84 << " " << Y+vacVesOuter+50.8 << " " << Y+vacVesOuter+142.24 << " " << Z-30.48 << " " << Z-22.86 <<"      $ R Optical Table\n"
			<< "402    RPP      " << X-length+plasmaL << " " << X-length/2+182.88 << " " << Y-vacVesOuter-167.64 << " " << Y-vacVesOuter-76.2 << " " << Z-35.56 << " " << Z-27.94 << "      $ L Optical Table\n"
			
			<< "403    RCC      " << X-length+plasmaL+30.48 << " " << Y+58.6486 << " " << Z-60.96 << " 0 " << 137.16 << " 0 " << 30.48 << "      $ Surge Tank\n"
			<< "404    RCC      " << X-length+plasmaL+30.48 << " " << Y+58.6486 << " " << Z-60.96 << " 0 " << 139.7 << " 0 " << 33.02 << "      $ OD of surge Tank\n"			
			<< "405    RCC      " << X-length+plasmaL+30.48 << " " << Y+55.0418 << " " << Z-60.96 << " 0 " << 3.6068 << " 0 " << 33.02 << "      $ Conflat\n"
			<< "406    RCC      " << X-length+plasmaL+30.48 << " " << Y+50.8 << " " << Z-60.96 << " 0 " << 4.2418 << " 0 " << 33.02 << "      $ 1st Conflat\n"
			
			<< "1      RPP      " << human1X-humanThick/2 << " " << human1X+humanThick/2 << " " << human1Y-humanWidth/2 << " " << human1Y+humanWidth/2 << " 0 " << humanHeight << "      $ human1 box\n"
			<< "2      RPP      " << human2X-humanThick/2 << " " << human2X+humanThick/2 << " " << human2Y-humanWidth/2 << " " << human2Y+humanWidth/2 << " 0 " << humanHeight << "      $ human2 box\n"
			<< "3      RPP      " << human3X-humanThick/2 << " " << human3X+humanThick/2 << " " << human3Y-humanWidth/2 << " " << human3Y+humanWidth/2 << " 0 " << humanHeight << "      $ human3 box\n"
			<< "4      RPP      " << human4X-humanThick/2 << " " << human4X+humanThick/2 << " " << human4Y-humanWidth/2 << " " << human4Y+humanWidth/2 << " 0 " << humanHeight << "      $ human4 box\n"
			<< "5      RPP      " << human5X-humanHeight/2 << " " << human5X+humanHeight/2 << " " << human5Y-humanWidth/2 << " " << human5Y+humanWidth/2 << " " << ceilingH+ceilingThick+1 << " " << ceilingH+ceilingThick+humanThick+1 << "      $ human5 box\n"
			
			<< "33     RPP      " << -thick1 << " 0 0 " << 548.64+thick5 << " 0 " << ceilingH << "      $ wall 1a\n"
			<< "34     RPP      " << -thick5 << " 91.44 548.64 " << 548.64+thick1 << " 0 " << ceilingH << "       $ wall 2a\n"
			<< "35     RPP      " << 91.44-thick1 << " 91.44 548.64 762 0 " << ceilingH << "      $ wall 3a\n"
			
			<< "43     RPP      " << -thick2 << " " << -thick1 << " 0 " << 548.64+thick5 << " 0 " << ceilingH << "      $ wall 1b\n"
			<< "44     RPP      " << -thick5 << " 91.44 " << 548.64+thick1 << " " << 548.64+thick2 << " 0 " << ceilingH << "       $ wall 2b\n"
			<< "45     RPP      " << 91.44-thick2 << " " << 91.44-thick1 << " 548.64 762 0 " << ceilingH << "      $ wall 3b\n"
			
			<< "53     RPP      " << -thick3 << " " << -thick2 << " 0 " << 548.64+thick5 << " 0 " << ceilingH << "      $ wall 1c\n"
			<< "54     RPP      " << -thick5 << " 91.44 " << 548.64+thick2 << " " << 548.64+thick3 << " 0 " << ceilingH << "       $ wall 2c\n"
			<< "55     RPP      " << 91.44-thick3 << " " << 91.44-thick2 << " 548.64 762 0 " << ceilingH << "      $ wall 3c\n"
			
			<< "63     RPP      " << -thick4 << " " << -thick3 << " 0 " << 548.64+thick5 << " 0 " << ceilingH << "      $ wall 1d\n"
			<< "64     RPP      " << -thick5 << " 91.44 " << 548.64+thick3 << " " << 548.64+thick4 << " 0 " << ceilingH << "       $ wall 2d\n"
			<< "65     RPP      " << 91.44-thick4 << " " << 91.44-thick3 << " 548.64 762 0 " << ceilingH << "      $ wall 3d\n"
			
			<< "73     RPP      " << -thick5 << " " << -thick4 << " 0 " << 548.64+thick5 << " 0 " << ceilingH << "      $ wall 1e\n"
			<< "74     RPP      " << -thick5 << " 91.44 " << 548.64+thick4 << " " << 548.64+thick5 << " 0 " << ceilingH << "       $ wall 2e\n"
			<< "75     RPP      " << 91.44-thick5 << " " << 91.44-thick4 << " 548.64 762 0 " << ceilingH << "      $ wall 3e\n"
			
			<< "24     RPP      " << 91.44-thick5 << " 2133.6 762 " << 762+wallThick << " 0 " << ceilingH << "      $ wall 4\n"
			<< "25     RPP      2133.6 " << 2133.6+wallThick << " " << 274.32-wallThick << " " << 762+wallThick << " 0 " << ceilingH << "      $ wall 5\n"
			<< "26     RPP      1508.76 2133.6 " << 274.32-wallThick << " 274.32 0 " << ceilingH << "     $ wall 6\n"
			<< "27     RPP      1508.76 " << 1508.76+wallThick << " " << -wallThick << " " << 274.32-wallThick << " 0 " << ceilingH << "      $ wall 7\n"
			<< "28     RPP      " << -thick5 << " 1508.76 " << -wallThick << " 0 0 " << ceilingH << "      $ wall 8\n"
			<< "29     RPP      1188.72 1219.2 0 213.36 0 " << ceilingH << "      $ wall 9\n"
			
			<< "30     RPP      792 " << 792+layer1 << " 0 " << 531 << " 0 " << dividerH << "      $ wall 10a\n"
			<< "31     RPP      792 " << 1036+layer5 << " " << 531-layer1 << " " << 531 << " 0 " << dividerH << "      $ wall 11a\n"
			<< "32     RPP      1036 " << 1036+layer1 << " " << 531-layer5 << " 762 0 " << dividerH << "      $ wall 12a\n"
			
			<< "40     RPP      " << 792+layer1 << " " << 792+layer2 << " 0 " << 531 << " 0 " << dividerH << "      $ wall 10b\n"
			<< "41     RPP      792 " << 1036+layer5 << " " << 531-layer2 << " " << 531-layer1 << " 0 " << dividerH << "      $ wall 11b\n"
			<< "42     RPP      " << 1036+layer1 << " " << 1036+layer2 << " " << 531-layer5 << " 762 0 " << dividerH << "      $ wall 12b\n"
			
			<< "50     RPP      " << 792+layer2 << " " << 792+layer3 << " 0 " << 531 << " 0 " << dividerH << "      $ wall 10c\n"
			<< "51     RPP      792 " << 1036+layer5 << " " << 531-layer3 << " " << 531-layer2 << " 0 " << dividerH << "      $ wall 11c\n"
			<< "52     RPP      " << 1036+layer2 << " " << 1036+layer3 << " " << 531-layer5 << " 762 0 " << dividerH << "      $ wall 12c\n"
			
			<< "60     RPP      " << 792+layer3 << " " << 792+layer4 << " 0 " << 531 << " 0 " << dividerH << "      $ wall 10d\n"
			<< "61     RPP      792 " << 1036+layer5 << " " << 531-layer4 << " " << 531-layer3 << " 0 " << dividerH << "      $ wall 11d\n"
			<< "62     RPP      " << 1036+layer3 << " " << 1036+layer4 << " " << 531-layer5 << " 762 0 " << dividerH << "      $ wall 12d\n"
			
			<< "70     RPP      " << 792+layer4 << " " << 792+layer5 << " 0 " << 531 << " 0 " << dividerH << "      $ wall 10e\n"
			<< "71     RPP      792 " << 1036+layer5 << " " << 531-layer5 << " " << 531-layer4 << " 0 " << dividerH << "      $ wall 11e\n"
			<< "72     RPP      " << 1036+layer4 << " " << 1036+layer5 << " " << 531-layer5 << " 762 0 " << dividerH << "      $ wall 12e\n"
			
			<< "80     RPP      " << -wallThick << " " << 1508.76+wallThick << " " << -wallThick << " " << 762+wallThick << " " << ceilingH << " " << ceilingH+ceilingThick << "      $ ceiling\n"
			<< "81     RPP      1508.76 " << 2133.6+wallThick << " " << 274.32-wallThick << " " << 762+wallThick << " " << ceilingH2 << " " << ceilingH2+ceilingThick << "      $ ceiling2\n"
			<< "82     RPP      1508.76 " << 2133.6+wallThick << " " << 274.32-wallThick << " " << 762+wallThick << " " << ceilingH2 << " " << ceilingH << "      $ ceiling2 gap\n"
			<< "83     RPP      1508.76 " << 1508.76+wallThick << " " << 274.32-wallThick << " " << 762+wallThick << " " << ceilingH2+ceilingThick << " " << ceilingH << "      $ ceiling2 wall\n"
			<< "84     RPP      " << -wallThick << " " << 2133.6+wallThick << " " << -wallThick << " " << 762+wallThick << " " << -floorThick << " 0      $ floor\n"
			
			<< "85     RPP      792 " << 792+layer5 << " 128 278 110 196      $ window gap\n"
			<< "86     RPP      792 " << 792+wallWindowTh << " 128 278 110 196      $ window\n"
			<< "87     RPP      843 993 " << 531-layer5 << " 531 110 196      $ window gap\n"
			<< "88     RPP      843 993 " << 531-wallWindowTh << " 531 110 196      $ window\n"
			
			<< "90     P        1 1 0 " << 792+531 << "      $ diagonal plane for barrier\n"
			<< "91     P        1 1 0 " << 1036+531 << "      $ diagonal plane for barrier\n"
			<< "92     P        1 1 0 " << 0+548.64 << "      $ diagonal plane for barrier\n"
			<< "93     P        1 1 0 " << 91.44+548.64 << "      $ diagonal plane for barrier\n"
			
			<< "94     RPP      792 " << 792+layer5 << " 313 493 0 246      $ door gap\n"
			<< "95     RPP      792 " << 792+doorThick << " 313 493 0 246      $ door\n"
			
			<< "99     RPP      -500 2633.6 -500 1262 -500 " << 500+ceilingH << "      $ universe\n"

			
			<< "c end of surface cards\n\n"

			<< "c beginning of data cards\n"
			
			<< "SDEF PAR=1 ERG=2.45 CEL=101 AXS=1 0 0 POS= " << X << " " << Y << " " << Z << " RAD=d2 EXT=d5\n"
			<< "SP1 -21 1\n"
			<< "SI2 0 " << plasmaR << "\n"
			//<< "SI5 14 16\n"
			//<< "SI5 10 20\n"
			<< "SI5 0 30\n"
			//<< "SI5 0 " << plasmaL << "\n"
			<< "c\n"
			
			/*
			<< "SDEF PAR=2 ERG=d1 CEL=101 AXS=1 0 0 POS= " << X+length/2-plasmaL << " " << Y << " " << Z << " RAD=d2 EXT=d5\n"
			<< "SI1 H 0.1 1.0\n"
			<< "SP1 D 0.0 1.0\n"
			<< "SP2 -21 1\n"
			<< "SI2 0 " << plasmaR << "\n"
			<< "SI5 0 " << plasmaL << "\n"
			<< "c\n"
			*/
           
			<< "MODE N\n"
			//<< "MODE P\n"
			//<< "MODE N P\n"
			//<< "PHYS:P 100 0 0 -1 0\n"
			
			
			<< "TR1 " << detectorX1 << " " << detectorY1 << " " << detectorZ1 << " 1 0 0 0 " << cosAngle1 << " " << sinAngle1 << " 0 " << -sinAngle1 << " " << cosAngle1 << " 1\n"
			<< "TR2 " << detectorX1 << " " << detectorY1 << " " << detectorZ1 << " 1 0 0 0 " << cosAngle1 << " " << sinAngle1 << " 0 " << -sinAngle1 << " " << cosAngle1 << " 1\n"
			<< "TR3 " << detectorX1 << " " << detectorY1 << " " << detectorZ1 << " 1 0 0 0 " << cosAngle1 << " " << sinAngle1 << " 0 " << -sinAngle1 << " " << cosAngle1 << " 1\n"
			<< "TR4 " << detectorX1 << " " << detectorY1 << " " << detectorZ1 << " 1 0 0 0 " << cosAngle1 << " " << sinAngle1 << " 0 " << -sinAngle1 << " " << cosAngle1 << " 1\n"
			
			<< "TR11 " << detectorX2 << " " << detectorY2 << " " << detectorZ2 << " 1 0 0 0 " << cosAngle2 << " " << sinAngle2 << " 0 " << -sinAngle2 << " " << cosAngle2 << " 1\n"
			<< "TR12 " << detectorX2 << " " << detectorY2 << " " << detectorZ2 << " 1 0 0 0 " << cosAngle2 << " " << sinAngle2 << " 0 " << -sinAngle2 << " " << cosAngle2 << " 1\n"
			<< "TR13 " << detectorX2 << " " << detectorY2 << " " << detectorZ2 << " 1 0 0 0 " << cosAngle2 << " " << sinAngle2 << " 0 " << -sinAngle2 << " " << cosAngle2 << " 1\n"
			<< "TR14 " << detectorX2 << " " << detectorY2 << " " << detectorZ2 << " 1 0 0 0 " << cosAngle2 << " " << sinAngle2 << " 0 " << -sinAngle2 << " " << cosAngle2 << " 1\n"
			
			<< "TR21 " << detectorX3 << " " << detectorY3 << " " << detectorZ3 << " 1 0 0 0 " << cosAngle3 << " " << sinAngle3 << " 0 " << -sinAngle3 << " " << cosAngle3 << " 1\n"
			<< "TR22 " << detectorX3 << " " << detectorY3 << " " << detectorZ3 << " 1 0 0 0 " << cosAngle3 << " " << sinAngle3 << " 0 " << -sinAngle3 << " " << cosAngle3 << " 1\n"
			<< "TR23 " << detectorX3 << " " << detectorY3 << " " << detectorZ3 << " 1 0 0 0 " << cosAngle3 << " " << sinAngle3 << " 0 " << -sinAngle3 << " " << cosAngle3 << " 1\n"
			<< "TR24 " << detectorX3 << " " << detectorY3 << " " << detectorZ3 << " 1 0 0 0 " << cosAngle3 << " " << sinAngle3 << " 0 " << -sinAngle3 << " " << cosAngle3 << " 1\n"
			
			
			<< "fmesh104:n geom=rec origin 515 70 190\n"
			<< "           imesh= 565\n"
			<< "           iints= 50\n"
			<< "           jmesh= 120 170 220\n"
			<< "           jints= 50  50  50\n"
			<< "           kmesh= 240 290\n"
			<< "           kints= 50  50\n"
			<< "           out=ij\n"
			<< "           emesh 0.4 2.5\n"
			<< "c\n"
			
			/*
			<< "fmesh104:n geom=rec origin 435 154 39\n"
			<< "           imesh= 475 515 555 595 635 675 715\n"
			<< "           iints= 40  40  40  40  40  40  40\n"
			<< "           jmesh= 194 234 274 314 354\n"
			<< "           jints= 40  40  40  40  40\n"
			<< "           kmesh= 79  119 159 199 239\n"
			<< "           kints= 40  40  40  40  40\n"
			<< "           out=ij\n"
			<< "           emesh 0.4 2.5\n"
			<< "c\n"
			*/
			
			/*
			<< "c fmesh104:n geom=rec origin -100 -100 -100\n"
			<< "c            imesh= 300 700 1100 1500 1900 2300\n"
			<< "c            iints= 50 50 50 50 50 50\n"
			<< "c            jmesh= 60 220 380 540 700 860\n"
			<< "c            jints= 20 20 20 20 20 20\n"
			<< "c            kmesh= 16 132 248 364 480 596\n"
			<< "c            kints= 15 15 15 15 15 15\n"
			<< "c            out=ij\n"
			<< "c            emesh 2.45\n"
			<< "c DE104 0.01     0.1      0.5      1.0      2.5         $ Flux to Dose calculation energies\n"
			<< "c DF104 3.56e-06 2.17e-05 9.26e-05 1.32e-04 1.25e-04    $ Flux to Dose\n"
			<< "c FM104 " << unitConversion << "                        $ Converts output to rems/pulse\n"
			<< "c\n"
			
			<< "c fmesh204:p geom=rec origin -100 -100 -100\n"
			<< "c            imesh= 300 700 1100 1500 1900 2300\n"
			<< "c            iints= 50 50 50 50 50 50\n"
			<< "c            jmesh= 60 220 380 540 700 860\n"
			<< "c            jints= 20 20 20 20 20 20\n"
			<< "c            kmesh= 16 132 248 364 480 596\n"
			<< "c            kints= 15 15 15 15 15 15\n"
			<< "c            out=ij\n"
			<< "c DE204 0.01     0.1      0.5      1.0      2.6      7.5      15         $ Flux to Dose calculation energies\n"
			<< "c DF204 3.96e-06 2.83e-07 1.17e-06 1.98e-06 3.82e-06 7.66e-06 1.33e-05   $ Flux to Dose\n"
			<< "c FM204 " << unitConversion << "                                         $ Converts output to rems/pulse\n"
			<< "c\n"
			
			<< "F12:N 1.2                                             $ Flux through human\n"
			<< "DE12 0.01     0.1      0.5      1.0      2.5          $ Flux to Dose calculation energies\n"
			<< "DF12 3.56e-06 2.17e-05 9.26e-05 1.32e-04 1.25e-04     $ Flux to Dose\n"
			<< "FM12 " << unitConversion << "                         $ Converts output to rems/pulse\n"
			<< "FC12 Test Neutron Flux (human 1)\n"
			<< "c\n"
			
			<< "F112:P 1.2                                                             $ Flux through human\n"
			<< "DE112 0.01     0.1      0.5      1.0      2.6      7.5      15         $ Flux to Dose calculation energies\n"
			<< "DF112 3.96e-06 2.83e-07 1.17e-06 1.98e-06 3.82e-06 7.66e-06 1.33e-05   $ Flux to Dose\n"
			<< "FM112 " << unitConversion << "                                         $ Converts output to rems/pulse\n"
			<< "FC112 Test Photon Flux (human 1)\n"
			<< "c\n"
			
			<< "F22:N 2.2                                             $ Flux through human\n"
			<< "DE22 0.01     0.1      0.5      1.0      2.5          $ Flux to Dose calculation energies\n"
			<< "DF22 3.56e-06 2.17e-05 9.26e-05 1.32e-04 1.25e-04     $ Flux to Dose\n"
			<< "FM22 " << unitConversion << "                         $ Converts output to rems/pulse\n"
			<< "FC22 Test Neutron Flux (human 2)\n"
			<< "c\n"
			
			<< "F122:P 2.2                                                             $ Flux through human\n"
			<< "DE122 0.01     0.1      0.5      1.0      2.6      7.5      15         $ Flux to Dose calculation energies\n"
			<< "DF122 3.96e-06 2.83e-07 1.17e-06 1.98e-06 3.82e-06 7.66e-06 1.33e-05   $ Flux to Dose\n"
			<< "FM122 " << unitConversion << "                                         $ Converts output to rems/pulse\n"
			<< "FC122 Test Photon Flux (human 2)\n"
			<< "c\n"
			
			<< "F32:N 3.2                                             $ Flux through human\n"
			<< "DE32 0.01     0.1      0.5      1.0      2.5          $ Flux to Dose calculation energies\n"
			<< "DF32 3.56e-06 2.17e-05 9.26e-05 1.32e-04 1.25e-04     $ Flux to Dose\n"
			<< "FM32 " << unitConversion << "                         $ Converts output to rems/pulse\n"
			<< "FC32 Test Neutron Flux (human 3)\n"
			<< "c\n"
			
			<< "F132:P 3.2                                                             $ Flux through human\n"
			<< "DE132 0.01     0.1      0.5      1.0      2.6      7.5      15         $ Flux to Dose calculation energies\n"
			<< "DF132 3.96e-06 2.83e-07 1.17e-06 1.98e-06 3.82e-06 7.66e-06 1.33e-05   $ Flux to Dose\n"
			<< "FM132 " << unitConversion << "                                         $ Converts output to rems/pulse\n"
			<< "FC132 Test Photon Flux (human 3)\n"
			<< "c\n"
			
			<< "F42:N 4.1                                             $ Flux through human\n"
			<< "DE42 0.01     0.1      0.5      1.0      2.5          $ Flux to Dose calculation energies\n"
			<< "DF42 3.56e-06 2.17e-05 9.26e-05 1.32e-04 1.25e-04     $ Flux to Dose\n"
			<< "FM42 " << unitConversion << "                         $ Converts output to rems/pulse\n"
			<< "FC42 Test Neutron Flux (human 4)\n"
			<< "c\n"
			
			<< "F142:P 4.1                                                             $ Flux through human\n"
			<< "DE142 0.01     0.1      0.5      1.0      2.6      7.5      15         $ Flux to Dose calculation energies\n"
			<< "DF142 3.96e-06 2.83e-07 1.17e-06 1.98e-06 3.82e-06 7.66e-06 1.33e-05   $ Flux to Dose\n"
			<< "FM142 " << unitConversion << "                                         $ Converts output to rems/pulse\n"
			<< "FC142 Test Photon Flux (human 4)\n"
			<< "c\n"
			
			<< "F52:N 5.6                                             $ Flux through human\n"
			<< "DE52 0.01     0.1      0.5      1.0      2.5          $ Flux to Dose calculation energies\n"
			<< "DF52 3.56e-06 2.17e-05 9.26e-05 1.32e-04 1.25e-04     $ Flux to Dose\n"
			<< "FM52 " << unitConversion << "                         $ Converts output to rems/pulse\n"
			<< "FC52 Test Neutron Flux (human 5)\n"
			<< "c\n"
			
			<< "F152:P 5.6                                                             $ Flux through human\n"
			<< "DE152 0.01     0.1      0.5      1.0      2.6      7.5      15         $ Flux to Dose calculation energies\n"
			<< "DF152 3.96e-06 2.83e-07 1.17e-06 1.98e-06 3.82e-06 7.66e-06 1.33e-05   $ Flux to Dose\n"
			<< "FM152 " << unitConversion << "                                         $ Converts output to rems/pulse\n"
			<< "FC152 Test Photon Flux (human 5)\n"
			<< "c\n"
			*/
			
			/*
			<< "c Total damage calculation for alumina insulator\n"
			<< "F64:N 132\n"
			<< "FM64 " << 10000*dpaConversion << " 12 444\n"
			<< "FC64 Calculated total DPA in alumina insulator\n"
			*/
			
			<< "F4:N 221                                              $ Fluence per neutron through detector 1\n"
			<< "FC4 Detector 1 Neutron Fluence\n"
			<< "c\n"
			
			<< "F14:N 231                                             $ Fluence per neutron through detector 2\n"
			<< "FC14 Detector 2 Neutron Fluence\n"
			<< "c\n"
			
			<< "F24:N 241                                             $ Fluence per neutron through detector 3\n"
			<< "FC24 Detector 3 Neutron Fluence\n"
			<< "c\n"
			
			<< "e0 0.025 98i 2.5\n"                                                
			<< "prdmp j j 1\n"
						
			<< "c Copper (8.96 g/cm^3)\n"
			<< "m1	029063.72c 0.6917                                 $ Cu63\n"
	  		<< "	029065.72c 0.3083                                 $ Cu65\n"
			
			<< "c Tungsten (19.25 g/cm^3)\n"
			<< "m2	074182.72c 0.2650                                 $ W182\n"
			<< "	074183.72c 0.1431                                 $ W183\n"
			<< "	074184.72c 0.3064                                 $ W184\n"
			<< "	074186.72c 0.2855                                 $ W186\n"
			
			<< "c SS 304 (8.03 g/cm3)\n"
			<< "m3	006000.72c 0.001830                               $ Carbon\n"
			<< "	014028.72c 0.00902102                             $ Silicon\n"
			<< "	014029.72c 0.00045804                             $ Silicon\n"
			<< "	014030.72c 0.00030194                             $ Silicon\n"
			<< "	015031.72c 0.000408                               $ Phosphorus\n"
			<< "	016032.72c 0.00000193                             $ Sulfur\n"
			<< "	016033.72c 0.00045804                             $ Sulfur\n"
			<< "	016034.72c 0.00001082                             $ Sulfur\n"
			<< "	024050.72c 0.00872311                             $ Chromium\n"
			<< "	024052.72c 0.16821647                             $ Chromium\n"
			<< "	024053.72c 0.01907440                             $ Chromium\n"
			<< "	024054.72c 0.00474802                             $ Chromium\n"
			<< "	025055.72c 0.010001                               $ Manganese\n"
			<< "	026054.72c 0.04035242                             $ Iron\n"
			<< "	026056.72c 0.63344668                             $ Iron\n"
			<< "	026057.72c 0.01462905                             $ Iron\n"
			<< "	026058.72c 0.00194686                             $ Iron\n"
			<< "	028058.72c 0.05894583                             $ Nickel\n"
			<< "	028060.72c 0.02270571                             $ Nickel\n"
			<< "	028061.72c 0.00098709                             $ Nickel\n"
			<< "	028062.72c 0.00314657                             $ Nickel\n"
			<< "	028064.72c 0.00080180                             $ Nickel\n"
			
			<< "mx3:p 6012 14028 14029 14030 0 16032 16033 16034 24050 24052 24053\n"
			<< "	24054 25055 26054 26056 26057 26058 28058 28060 28061 28062 28064\n"
			
			<< "m4	001002.73c 1                                      $ Deuterium\n"
			
			<< "c Concrete (2.25 g/cm^3)\n"
			<< "m5	001001.72c 0.084739                               $ Hydrogen\n"
			<< "	008016.72c 0.604079                               $ Oxygen\n"
			<< "	011023.72c 0.012523                               $ Sodium\n"
			<< "	013027.72c 0.024842                               $ Aluminum\n"
			<< "	014028.72c 0.223124                               $ Silicon\n"
			<< "	014029.72c 0.011329                               $ Silicon\n"
			<< "	014030.72c 0.007468                               $ Silicon\n"
			<< "	020040.72c 0.027244                               $ Calcium\n"
			<< "	026054.72c 0.000272                               $ Iron\n"
			<< "	026056.72c 0.004268                               $ Iron\n"
			<< "	026057.72c 0.000099                               $ Iron\n"
			<< "	026058.72c 0.000013                               $ Iron\n"
			
			<< "mx5:p 1002 8016 11023 13027 14028 14029 14030 20040 26054 26056 26057 26058\n"
			
			<< "c Plywood (0.58 g/cm^3)\n"
			<< "m6	001001.72c 0.462423                               $ Hydrogen\n"
			<< "	006000.72c 0.323389                               $ Carbon\n"
			<< "	007014.72c 0.002773                               $ Nitrogen\n"
			<< "	008016.72c 0.208779                               $ Oxygen\n"
			<< "	012024.72c 0.000505                               $ Magnesium\n"
			<< "	012025.72c 0.000064                               $ Magnesium\n"
			<< "	012026.72c 0.000070                               $ Magnesium\n"
			<< "	016032.72c 0.001211                               $ Sulfur\n"
			<< "	019039.80c 0.000370                               $ Potassium\n"
			<< "	019041.80c 0.000017                               $ Potassium\n"
			<< "	020040.72c 0.000388                               $ Calcium\n"
			
			<< "mx6:p 1002 6012 7014 8016 12024 12025 12026 16032 0 0 20040\n"
			
			<< "c Kevlar (1.44 g/cm^3)\n"
			<< "m7	001001.72c 0.4117647                              $ Hydrogen\n"
			<< "	006000.72c 0.4117647                              $ Carbon\n"
			<< "	007014.72c 0.0588235                              $ Nitrogen\n"
			<< "	008016.72c 0.1176471                              $ Oxygen\n"
			
			<< "mx7:p 1002 6012 7014 8016\n"
			
			<< "c Plaster (2.32 g/cm^3)\n"
			<< "m8	001001.72c 0.333321                               $ Hydrogen\n"
			<< "	008016.72c 0.500014                               $ Oxygen\n"
			<< "	016032.72c 0.083324                               $ Sulfur\n"
			<< "	020040.72c 0.083341                               $ Calcium\n"
			
			<< "mx8:p 1002 8016 16032 20040\n"
			
			<< "c Lucite (1.19 g/cm^3)\n"
			<< "m9	001001.72c 0.533320                               $ Hydrogen\n"
			<< "	006000.72c 0.333345                               $ Carbon\n"
			<< "	008016.72c 0.133335                               $ Oxygen\n"
			
			<< "mx9:p 1002 6012 8016\n"
			
			<< "c Silica (2.32 g/cm^3)\n"
			<< "m10	008016.72c 0.532565                               $ Oxygen\n"
			<< "	014028.72c 0.431115                               $ Silicon\n"
			<< "	014029.72c 0.021890                               $ Silicon\n"
			<< "	014030.72c 0.014430                               $ Silicon\n"
			
			<< "c Aluminum (2.70 g/cm^3)\n"
			<< "m11	013027.72c 1                                      $ Aluminum\n"
			
			<< " c Aluminum Oxide (3.97 g/cm^3)\n"
			<< "m12 13027.72c 0.6                                     $ Aluminum\n"
			<< "	08016.72c 0.4                                     $ Oxygen\n"
			
			<< "c Brass (8.07 g/cm^3)\n"
			<< "m13 026054.72c 0.000058617                            $ Iron54\n"
			<< "	026056.72c 0.00092186                             $ Iron56\n"
			<< "	026057.72c 0.000021242                            $ Iron57\n"
			<< "	026058.72c 0.000000281                            $ Iron58\n"
			<< "	029063.72c 0.466705797                            $ Cu63\n"
			<< "	029065.72c 0.208212203                            $ Cu65\n"
			<< "	030064.80c 0.157910352                            $ Zinc64\n"
			<< "	030066.80c 0.088904812                            $ Zinc66\n"
			<< "	030067.80c 0.01283824                             $ Zinc67\n"
			<< "	030068.80c 0.05937686                             $ Zinc68\n"
			<< "	030070.80c 0.001925736                            $ Zinc70\n"
			<< "	050112.80c 0.000014075                            $ Tin112\n"
			<< "	050114.80c 0.000009577                            $ Tin114\n"
			<< "	050115.80c 0.000004933                            $ Tin115\n"
			<< "	050116.80c 0.000210975                            $ Tin116\n"
			<< "	050117.80c 0.000111437                            $ Tin117\n"
			<< "	050118.80c 0.000351432                            $ Tin118\n"
			<< "	050119.80c 0.000124641                            $ Tin119\n"
			<< "	050120.80c 0.000472736                            $ Tin120\n"
			<< "	050122.80c 0.000067181                            $ Tin122\n"
			<< "	050124.80c 0.000084013                            $ Tin124\n"
			<< "	082204.72c 0.000023422                            $ Lead204\n"
			<< "	082206.72c 0.000403193                            $ Lead206\n"
			<< "	082207.72c 0.000369733                            $ Lead207\n"
			<< "	082208.72c 0.000876652                            $ Lead208\n"

			<< "mx13:p 26054 26056 26057 26058 29063 29065 30064 30066 30067\n"
			<< "	30068 30070 50120 50120 50120 50120 50120 50120 50120\n"
			<< "	50120 50120 50120 82206 82206 82207 82208\n"
			
			<< "c Polyvinyl Toluene (PVT) (1.032 g/cm^3)\n"
			<< "m14 001001.72c 0.525382                               $ Hydrogen\n"
			<< "	006000.72c 0.474618                               $ Carbon\n"
			
			<< "mx14:p 1002 6012\n"
			
			<< "c Hevimet (18.22 g/cm^3)\n"
			<< "m15 074182.72c 0.2385                                 $ W182\n"
			<< "	074183.72c 0.12879                                $ W183\n"
			<< "	074184.72c 0.27576                                $ W184\n"
			<< "	074186.72c 0.25695                                $ W186\n"
			<< "	029063.72c 0.027668                               $ Cu63\n"
			<< "	029065.72c 0.012332                               $ Cu65\n"
			<< "	028058.72c 0.0408462                              $ Ni58\n"
			<< "	028060.72c 0.0157338                              $ Ni60\n"
			<< "	028061.72c 0.000684                               $ Ni61\n"
			<< "	028062.72c 0.0021804                              $ Ni62\n"
			<< "	028064.72c 0.0005556                              $ Ni64\n"
			
			<< "c Polyethylene (0.93 g/cm^3)\n"
			<< "m16	001001.72c 0.666662                               $ H1\n"
			<< "	006000.72c 0.333338                               $ C12\n"

			<< "mx16:p 1002 6012\n"

			<< "c Borated Polyethylene (1.00 g/cm^3)\n"
			<< "m17	001001.72c 0.627759                               $ H\n"
			<< "	005010.72c 0.009338                               $ B10\n"
			<< "	005011.72c 0.037352                               $ B11\n"
			<< "	006000.72c 0.325551                               $ C\n"
			
			<< "mx17:p 1002 0 0 6012\n"
			
			<< "c Graphite (1.70 g/cm^3)\n"
			<< "m18	006000.72c 1                                      $ C\n"
			
			<< "mx18:p 6012\n"
			
			<< "c\n"
			<< "NPS 50000000                                            \n"
			<< "Print 30                                                \n"
			<< "c End of data cards"

			<< std::endl;
				
	std::cout << "\n**********************************************************\n"
	<< "Generated new input file\n\n"
	<< "Detector A:\n"
	<< "radial distance = " << dRadialDist1 << " cm, axial distance = " << dAxialDist1 << " cm, height = " << dHeight1 << " cm\n"
	<< "\n"
	<< "Detector B:\n"
	<< "radial distance = " << dRadialDist2 << " cm, axial distance = " << dAxialDist2 << " cm, height = " << dHeight2 << " cm\n"
	<< "\n"
	<< "Detector C:\n"
	<< "radial distance = " << dRadialDist3 << " cm, axial distance = " << dAxialDist3 << " cm, height = " << dHeight3 << " cm\n"
	<< "**********************************************************\n"
	<< std::endl;

	fuzeOut.close();
}
