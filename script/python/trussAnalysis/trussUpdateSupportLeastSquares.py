#!/usr/bin/env python3.4

import sympy
import math
import numpy

# Simple truss problem the added members and support at J.
# 3 moment equations about A, E and G
# Exclude 0 force member at B
# Ax = Ex = Gx = 0 because 0 external net forces
# I count 19 struts, each of which has an unknown force
# Also, Ay, Ey, and Gy are unknowns.
# So that is 22 unknowns
# 11 nodes with 2 xyy force balance equations give 22 equations.
# If you eliminate A x,y force balance, and G y force balance
# you get a total of 22 equations

# Because zero net force in the x-direction, and 0 external x forces on the truss
#AX = 0
#EX = 0
#GX = 0

# 2lbs / foot * 30 feet * 24 cables * 1.2 = 1800 lbs
# The load is now distributed over just 4 points, so
# Total load needs to add up to 900 lbs
FLL = 450
FLI = 0
FLE = 0
FLJ = 450

theta = 22.5 * 2 * math.pi/360.0
sin = math.sin
cos = math.cos


'''

B = A * X
 
The vector X is defined as:
X = [AY, 
     FAB, 
     FAC, 
     FBC, 
     FBD, 
     FCK, 
     FCL, 
     FDF, 
     FDK, 
     FDI, 
     FDE, 
     FEF, 
     FEI, 
     FEJ, 
     FFG, 
     FFJ, 
     FGJ, 
     FIL, 
     FIK, 
     FKL, 
     GY, 
     JY]

A = numpy.array([ [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Moment about A
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Moment about G
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Moment about J
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node B in the x direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node B in the y direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node C in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node C in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node D in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node D in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node E in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node E in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node F in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node F in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node G in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node I in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node I in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node J in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node J in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node K in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node K in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],   # Node L in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY] ]) # Node L in the y-direction

'''

# 22 equations, 22 unknowns
# B = A * X

B = numpy.array([5*51*FLJ + 4*51*FLE + 3*51*FLI + 2.5*51*FLL,  # Moment about A
                 -3.5*51*FLL - 3*51*FLI - 2*51*FLE - 51*FLJ,   # Moment about G
                 -2.5*51*FLL - 2*51*FLI - 51*FLE,              # Moment about J
                 0,                                            # Node B in the x direction
                 0,                                            # Node B in the y direction
                 0,                                            # Node C in the x direction
                 0,                                            # Node C in the y direction
                 0,                                            # Node D in the x direction
                 0,                                            # Node D in the y direction
                 0,                                            # Node E in the x direction
                 FLE,                                          # Node E in the y direction
                 0,                                            # Node F in the x direction
                 0,                                            # Node F in the y direction
                 0,                                            # Node G in the x direction
                 0,                                            # Node I in the x direction
                 FLI,                                          # Node I in the y direction
                 0,                                            # Node J in the x direction
                 FLJ,                                          # Node J in the y direction
                 0,                                            # Node K in the x direction
                 0,                                            # Node K in the y direction
                 0,                                            # Node L in the x direction
                 FLL])                                         # Node L in the y direction

# So they get zeroed in the array
AY = 0 
FAB = 0 
FAC = 0 
FBC = 0 
FBD = 0 
FCK = 0 
FCL = 0 
FDF = 0 
FDK = 0 
FDI = 0 
FDE = 0 
FEF = 0 
FEI = 0 
FEJ = 0 
FFG = 0 
FFJ = 0 
FGJ = 0 
FIL = 0 
FIK = 0 
FKL = 0 
GY = 0 
JY = 0

A = numpy.array([ [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, 51*6, 5*51],                     # Moment about A
                  [-51*6, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, -51],                     # Moment about G
                  [-51*5, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, 51, JY],                      # Moment about J
                  [AY, -cos(theta), FAC, cos(theta), 1, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],            # Node B in the x direction
                  [AY, sin(theta), FAC, sin(theta), FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],           # Node B in the y direction
                  [AY, FAB, -1, -cos(theta), FBD, cos(theta), 1, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],             # Node C in the x-direction
                  [AY, FAB, FAC, sin(theta), FBD, sin(theta), FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],           # Node C in the y-direction
                  [AY, FAB, FAC, FBC, -1, FCK, FCL, 1, -cos(theta), FDI, cos(theta), FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],             # Node D in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, sin(theta), FDI, sin(theta), FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],           # Node D in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, -cos(theta), cos(theta), -1, 1, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],             # Node E in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, sin(theta), sin(theta), FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY],           # Node E in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, -1, FDK, FDI, FDE, -cos(theta), FEI, FEJ, cos(theta), FFJ, FGJ, FIL, FIK, FKL, GY, JY],           # Node F in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, sin(theta), FEI, FEJ, sin(theta), 1, FGJ, FIL, FIK, FKL, GY, JY],             # Node F in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, cos(theta), FFJ, 1, FIL, FIK, FKL, GY, JY],                    # Node G in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, 1, FEJ, FFG, FFJ, FGJ, -1, -sin(theta), FKL, GY, JY],                    # Node I in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, 1, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, cos(theta), FKL, GY, JY],                    # Node I in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, -1, FFG, FFJ, 1, FIL, FIK, FKL, GY, JY],                            # Node J in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, 1, FGJ, FIL, FIK, FKL, GY, 1],                            # Node J in the y-direction
                  [AY, FAB, FAC, FBC, FBD, -cos(theta), FCL, FDF, cos(theta), FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, sin(theta), FKL, GY, JY],   # Node K in the x-direction
                  [AY, FAB, FAC, FBC, FBD, -sin(theta), FCL, FDF, sin(theta), FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, -cos(theta), -1, GY, JY],    # Node K in the y-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, -1, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, 1, FIK, FKL, GY, JY],                            # Node L in the x-direction
                  [AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, 1, GY, JY] ])                         # Node L in the y-direction

X = numpy.linalg.lstsq(A, B)

namesX = ["AY", "FAB", "FAC", "FBC", "FBD", "FCK", "FCL", "FDF", "FDK", "FDI", "FDE", "FEF", "FEI", "FEJ", "FFG", "FFJ", "FGJ", "FIL", "FIK", "FKL", "GY", "JY"]


vals = X[0]

for ii in range(0, vals.size):
    print(namesX[ii], ": ", vals[ii])
    exec("%s = %f" % (namesX[ii],vals[ii]))


torqueAtA = GY*51*6 - 5*51*(FLJ-JY) - 4*51*FLE - 3*51*FLI - 2.5*51*FLL
print("Moment about point A: ", torqueAtA)

torqueAtG = -AY*51*6 + 3.5*51*FLL + 3*51*FLI + 2*51*FLE + 51*(FLJ-JY)
print("Moment about point G: ", torqueAtG)

torqueAtJ = -AY*5*51 + 2.5*51*FLL + 2*51*FLI + 51*FLE + 51*GY
print("Moment about point J: ", torqueAtJ)

netForceXAtA = FAB*cos(theta) + FAC
print("Net force in the X-direction at A: ", netForceXAtA)

netForceXAtG =  - FFG*cos(theta) - FGJ
print("Net force in the X-direction at G: ", netForceXAtG)

netForceXAtJ =  FGJ - FEJ
print("Net force in the X-direction at J: ", netForceXAtJ)

netForceY =  AY + GY + JY - FLL - FLI - FLE - FLJ
print("Net force in the Y-direction: ", netForceY)

netForceXAtD =  FDK*sin(theta) + FDE*sin(theta)
print("Net force in the X-direction at D: ", netForceXAtD)

netForceYAtD =  FDF - FBD + FDE*cos(theta) - FDK*cos(theta)
print("Net force in the Y-direction at D: ", netForceYAtD)

print("Total load: ", FLL + FLI + FLE + FLJ)

print("Node B in the x-direction", FBD - FAB*cos(theta) + FBC*cos(theta))

print("Node B in the y-direction", FAB*sin(theta) + FBC*sin(theta))

print("Node C in the x-direction", FCL - FAC - FBC*cos(theta) + FCK*cos(theta))

print("Node C in the y-direction", FBC*sin(theta) + FCK*sin(theta))

print("Node D in the x-direction", FDF - FBD + FDE*cos(theta) - FDK*cos(theta))

print("Node D in the y-direction", FDK*sin(theta) + FDE*sin(theta))

print("Node E in the x-direction", FEJ - FEI - FDE*cos(theta) + FEF*cos(theta))

print("Node E in the y-direction", FDE*sin(theta) + FEF*sin(theta) - FLE)

print("Node F in the x-direction", FFG*cos(theta) - FDF - FEF*cos(theta))

print("Node F in the y-direction", FFG*sin(theta) + FEF*sin(theta) + FFJ)

print("Node G in the x-direction", FFG*cos(theta) + FGJ)

print("Node I in the x-direction", FEI - FIL - FIK*sin(theta))

print("Node I in the y-direction", FDI + FIK*cos(theta) - FLI)

print("Node J in the x-direction", FGJ - FEJ)

print("Node J in the y-direction", FFJ - FLJ + JY)

print("Node K in the x-direction", FDK*cos(theta)-FCK*cos(theta)+FIK*sin(theta))

print("Node K in the y-direction", FDK*sin(theta)-FCK*sin(theta)-FKL-FIK*cos(theta))

print("Node L in the x-direction", FIL - FCL)

print("Node L in the y-direction", FKL - FLL)

'''
GY*51*6 - 5*51*(FLJ-JY) - 4*51*FLE - 3*51*FLI - 2.5*51*FLL,  # Moment about A [X]
-AY*51*6 + 3.5*51*FLL + 3*51*FLI + 2*51*FLE + 51*(FLJ-JY),   # Moment about G [X]
-AY*5*51 + 2.5*51*FLL + 2*51*FLI + 51*FLE + 51*GY,           # Moment about J [X]
FBD - FAB*cos(theta) + FBC*cos(theta),                       # Node B in the x direction [X]
FAB*sin(theta) + FBC*sin(theta),                             # Node B in the y direction [X]
FCL - FAC - FBC*cos(theta) + FCK*cos(theta),                 # Node C in the x-direction [X]
FBC*sin(theta) + FCK*sin(theta),                             # Node C in the y-direction [X]
FDF - FBD + FDE*cos(theta) - FDK*cos(theta),                 # Node D in the x-direction [X]
FDK*sin(theta) + FDE*sin(theta),                             # Node D in the y-direction [X]
FEJ - FEI - FDE*cos(theta) + FEF*cos(theta),                 # Node E in the x-direction [X]
FDE*sin(theta) + FEF*sin(theta) - FLE,                       # Node E in the y-direction [X]
FFG*cos(theta) - FDF - FEF*cos(theta),                       # Node F in the x-direction [X]
FFG*sin(theta) + FEF*sin(theta) + FFJ,                       # Node F in the y-direction [X]
FFG*cos(theta) + FGJ,                                        # Node G in the x-direction [X]
FEI - FIL - FIK*sin(theta),                                  # Node I in the x-direction [X]
FDI + FIK*cos(theta) - FLI,                                  # Node I in the y-direction [X]
FGJ - FEJ,                                                   # Node J in the x-direction [X]
FFJ - FLJ + JY,                                              # Node J in the y-direction [X]
FDK*cos(theta)-FCK*cos(theta)+FIK*sin(theta),                # Node K in the x-direction [X]
FDK*sin(theta)-FCK*sin(theta)-FKL-FIK*cos(theta),            # Node K in the y-direction [X]
FIL - FCL,                                                   # Node L in the x-direction []
FKL - FLL],                                                  # Node L in the y-direction []
'''
