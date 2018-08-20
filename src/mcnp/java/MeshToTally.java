// Zack Draper
// FuZE Neutronics
// June 2018

// This program reads two MCNP meshtal files. For each file, it calculates
// the average fluence across a specified region in the mesh. One meshtal
// file should track the neutron fluence with the full geometry of the room
// in place, and the other should use the same mesh, but with the geometry
// voided out. This program will output the ratio of the fluences.
// This ratio, the h-factor, is necessary for determining the neutron source
// rate based on detector readings. The specified region should correspond
// to the scintillator inside a detector.

import java.util.*;
import java.io.*;
import java.math.*;
import java.text.*;

public class MeshToTally {
   
   // These constants are used to define a new coordinate system
   // with the nosecone at (0, 0, 0).
   public static final double NOSECONE_X = 515;
   public static final double NOSECONE_Y = 254;
   public static final double NOSECONE_Z = 139;
      
   public static void main (String[] args) throws IOException {
      
      NumberFormat formatter = new DecimalFormat("0.00E00");
      NumberFormat formatter2 = new DecimalFormat("0.000");
      
      Scanner console = new Scanner(System.in);
      
      // The fluences are extracted from the meshtal files and written to the
      // following file to be read later.
      String outputFile = "meshToTally_output";
      PrintWriter output = new PrintWriter(new FileWriter(outputFile));
      
      // Path of the meshtal file with the full geometry in place.
      String fullPath = "/Users/zack/Desktop/FuZE/meshtal_truss";
      
      // Path of the meshtal file with void geometry.
      String voidPath = "/Users/zack/Desktop/FuZE/meshtal_truss_void";
      
            
      // neutron energy range and scintillator coordinates
      
      double eLowerBound = 4.00E-01;
      double eUpperBound = 2.50E+00;
      
      double xLowerBound = 15 + NOSECONE_X;
      double xUpperBound = 20 + NOSECONE_X;
      
      double yLowerBound = -55 + NOSECONE_Y;
      double yUpperBound = -50 + NOSECONE_Y;
      
      double zLowerBound = 50 + NOSECONE_Z;
      double zUpperBound = 55 + NOSECONE_Z;
      
      
      // Parses through the full-geometry meshtal file looking for fluence data.
      output.println("full-geometry fluence:");
      boolean fullSuccess = findFluences(eLowerBound, eUpperBound,
                            xLowerBound, xUpperBound, yLowerBound, yUpperBound,
                            zLowerBound, zUpperBound, output, fullPath);
      
      // Parses through the void-geometry meshtal file looking for fluence data.
      output.println("void-geometry fluence:");
      boolean voidSuccess = findFluences(eLowerBound, eUpperBound,
                            xLowerBound, xUpperBound, yLowerBound, yUpperBound,
                            zLowerBound, zUpperBound, output, voidPath);
      
      output.close();
      
      // If fluence data was found for both meshtal files, the relavent data
      // is extracted from the output file and is printed to the console. 
      if (fullSuccess && voidSuccess) {
         
         double[] averages = getAverages(outputFile);
         double[] stdErrors = getErrors(outputFile, averages);
         double hFactor = averages[0]/averages[1];
         double hError = hFactor*Math.sqrt(Math.pow(stdErrors[0]/averages[0], 2)
                         + Math.pow(stdErrors[1]/averages[1], 2));
         
         System.out.println("full-geometry fluence: (" + formatter.format(averages[0])
                            + " +/- " + formatter.format(stdErrors[0]) + ") cm^-2");
         System.out.println("void-geometry fluence: (" + formatter.format(averages[1])
                            + " +/- " + formatter.format(stdErrors[1]) + ") cm^-2");
         System.out.println("h-factor: " + formatter2.format(hFactor)
                            + " +/- " + formatter2.format(hError));
         System.out.println("uncertainty: ~" + Math.round(100*hError/hFactor) + "%");
      } else {
         System.out.println("data not found");
      }
   }
   
   // Parses through a meshtal file and finds the fluence in a specified region.
   // Outputs the data using the PrintWriter taken as a parameter.
   // Takes 8 doubles representing the upper and lower bound energy and scintillator
   // coordinates as parameters. Also takes the path of the meshtal file as a parameter.
   // Returns true if data was output using the PrintWriter and false otherwise.
   public static boolean findFluences(double eLowerBound, double eUpperBound,
                                      double xLowerBound, double xUpperBound,
                                      double yLowerBound, double yUpperBound,
                                      double zLowerBound, double zUpperBound,
                                      PrintWriter output, String path) throws IOException {
      
      // These flags allow the code to keep track of where it is in the meshtal file.
      boolean rightEnergy = false;
      boolean rightZ = false;
      boolean dataFound = false;
      
      // This keeps track of whether data has been found and output
      boolean success = false;
      
      double[] energyBin = new double[2];
      
      // These integers are used to keep track of the location in the meshtal file
      // that corresponds to the x-coodinates in the specified region.
      int xMin = 0;
      int xSpan = 0;
      
      FileInputStream inputStream = null;
      Scanner sc = null;
      try {
         inputStream = new FileInputStream(path);
         sc = new Scanner(inputStream, "UTF-8");
         while (sc.hasNextLine()) {
            String line = sc.nextLine();
            Scanner lineScan = new Scanner(line);
            // If the desired fluence data has been found, it is parsed
            // and written to the output file.
            if (dataFound) {
               if (lineScan.hasNextDouble()) {
                  double yCoordinate = lineScan.nextDouble();
                  if (yCoordinate >= yLowerBound && yCoordinate <= yUpperBound) {
                     for (int i = 0; i < xMin; i++) {
                        lineScan.nextDouble();
                     }
                     // Maybe add this in later if multiple energy bins are to be considered
                     //output.println("Energy: " + energyBin[0] + " - " + energyBin[1] + " MeV");
                     for (int i = 0; i < xSpan; i++) {
                        output.println(lineScan.nextDouble());
                        success = true;
                     }
                  }
               // Error data is not currently being collected.
               } else if (line.equals("     Relative Errors")){
                  dataFound = false;
               }
            // If the line that specifies the energy range in the meshtal file
            // corresponds to the desired energy range, a flag is set.
            } else if (line.startsWith("Energy Bin: ")) {
               lineScan.next();
               lineScan.next();
               double eMin = lineScan.nextDouble();
               lineScan.next();
               double eMax = lineScan.nextDouble();
               rightEnergy = (eMin == eLowerBound && eMax == eUpperBound);
               energyBin[0] = eMin;
               energyBin[1] = eMax;
            // We are only considering a subset of the total energy.
            } else if (line.equals("Total Energy Bin")) {
               rightEnergy = false;
            // If this code has found the region of the meshtal file that corresponds to
            // the correct energy, and a line signals that it contains information about
            // the z-coordinate of the following data, the z-coordinate is checked to
            // see if it falls within the specified upper and lower z-coordinate bounds.
            } else if (rightEnergy && line.startsWith("  Z bin: ")) {
               lineScan.next();
               lineScan.next();
               double zMin = lineScan.nextDouble();
               lineScan.next();
               double zMax = lineScan.nextDouble();
               rightZ = (zMin >= zLowerBound && zMax <= zUpperBound);
            // If this code has found the region of the meshtal file that corresponds to
            // the correct energy and correct z-coordinate, and a line signals that it
            // contains information about the x and y coordinates of the data, the desired
            // data has been found. The location in the meshtal file that corresponds
            // to the desired x-coodinates is noted.
            } else if (rightEnergy && rightZ 
                        && line.equals("     Tally Results:  X (across) by Y (down)")) {
               dataFound = true;
               String nextLine = sc.nextLine();
               Scanner nextScan = new Scanner(nextLine);
               xMin = 0;
               xSpan = 0;
               boolean rightX = false;
               while (nextScan.hasNextDouble()) {
                  double xCoordinate = nextScan.nextDouble();
                  if (xCoordinate >= xLowerBound && xCoordinate <= xUpperBound) {
                     xSpan++;
                     rightX = true;
                  } else if (!rightX) {
                     xMin++;
                  }
               }
            } 
         }
         if (sc.ioException() != null) {
            throw sc.ioException();
         }
      } finally {
         if (inputStream != null) {
            inputStream.close();
         }
         if (sc != null) {
            sc.close();
         }
      }
      // If data has been written to the output file, true is returned.
      // Otherwise, false is returned.
      return success;
   }
   
   // Takes the name of the output file as a parameter and calculates the average
   // fluences by scanning through the data written to the output file.
   // Returns an array containing two doubles.
   // The first double corresponds to the average fluence for the full geometry.
   // The second corresponds to the average fluence for the void geometry.
   public static double[] getAverages(String outputFile) throws IOException {
      double[] averages = new double[2];
      FileInputStream inputStream = null;
      Scanner sc = null;
      try {
         inputStream = new FileInputStream(outputFile);
         sc = new Scanner(inputStream, "UTF-8");
         
         sc.nextLine();
         
         int fullCount = 0;
         double fullTotal = 0;
         while (sc.hasNextDouble()) {
            fullTotal += sc.nextDouble();
            fullCount++;
         }
         averages[0] = fullTotal/fullCount;
         
         sc.nextLine();
         if (sc.hasNextLine()) {
            sc.nextLine();
         }
         
         int voidCount = 0;
         double voidTotal = 0;
         while (sc.hasNextDouble()) {
            voidTotal += sc.nextDouble();
            voidCount++;
         }
         averages[1] = voidTotal/voidCount;
               
         if (sc.ioException() != null) {
            throw sc.ioException();
         }
      } finally {
         if (inputStream != null) {
            inputStream.close();
         }
         if (sc != null) {
            sc.close();
         }
      }
      return averages;
   }
   
   // Takes the name of the output file as a parameter and calculates the standard
   // error of the fluence data by scanning through the output file.
   // Returns an array containing two doubles.
   // The first double corresponds to the standard error for the full geometry.
   // The second corresponds to the standard error for the void geometry.
   public static double[] getErrors(String outputFile, double[] averages) throws IOException {
      double[] stdErrors = new double[2];
      FileInputStream inputStream = null;
      Scanner sc = null;
      try {
         inputStream = new FileInputStream(outputFile);
         sc = new Scanner(inputStream, "UTF-8");
         
         sc.nextLine();
         
         int fullCount = 0;
         double fullDevs = 0;
         while (sc.hasNextDouble()) {
            fullDevs += Math.pow(averages[0]-sc.nextDouble(), 2);
            fullCount++;
         }
         stdErrors[0] = Math.sqrt(fullDevs/((fullCount-1)*fullCount));
         
         sc.nextLine();
         if (sc.hasNextLine()) {
            sc.nextLine();
         }
         
         int voidCount = 0;
         double voidDevs = 0;
         while (sc.hasNextDouble()) {
            voidDevs += Math.pow(averages[1]-sc.nextDouble(), 2);
            voidCount++;
         }
         stdErrors[1] = Math.sqrt(voidDevs/((voidCount-1)*voidCount));
               
         if (sc.ioException() != null) {
            throw sc.ioException();
         }
      } finally {
         if (inputStream != null) {
            inputStream.close();
         }
         if (sc != null) {
            sc.close();
         }
      }
      return stdErrors;
   }
   
}
