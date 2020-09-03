#!/usr/bin/env python3.4

import sympy
import math

# Simple truss problem the added members and support at J
# 3 moment equations about A, G, and J. Ignore the x,y force balance equations at A, and the y force balance equations at G and J
# Net Force in Y must be zero.
# 12 nodes => 24 x,y force balance equations, minute the 4 described above = 20 x,y force balance equations.
# 21 struts = 21 unknowns + the Ay, Gy and Jy => 24 unknowns.
# The 20 x,y force balance eautions + the 3 moment equations + Y net force = 24 equations
# Now support at node J. So we have a JY

# Because zero net force in the x-direction, and 0 external x forces on the truss
AX = 0
GX = 0
JX = 0



AY, FAB, FAH, FBH, FBC, FBD, FCH, FCL, FCK, FDE, FDI, FDK, FDF, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIK, FIL, FKL, GY, JY = \
sympy.symbols('AY, FAB, FAH, FBH, FBC, FBD, FCH, FCL, FCK, FDE, FDI, FDK, FDF, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIK, FIL, FKL, GY, JY')

theta = 22.5 * 2 * math.pi/360.0
sin = math.sin
cos = math.cos
# 2lbs / foot * 30 feet * 24 cables * 1.2 = 1800 lbs
# The load is now distributed over just 4 points, so
load1 = 450
# The load closest to point G
load2 = 450

# 17 equations, 17 unknowns
trussSolve = sympy.linsolve([-AY*51*6 + load1*51*3.5 + (load2 - JY)*51,           # Moment about G [X]
                             GY*51*6 - load1*51*2.5 + (JY - load2)*51*5,          # Moment about A [X]
                             GY*51 + load1*128 - AY*255,                          # Moment about J [X]
                             FBD - FAB*cos(theta) + FBC*cos(theta),               # Node B in the x direction [X]
                             FAB*sin(theta) + FBC*sin(theta)+FBH,                 # Node B in the y direction [X]
                             FCL - FCH - FBC*cos(theta) + FCK*cos(theta),         # Node C in the x-direction [X]
                             FBC*sin(theta) + FCK*sin(theta),                     # Node C in the y-direction [X]
                             FDF - FBD + FDE*cos(theta) - FDK*cos(theta),         # Node D in the x-direction [X]
                             FDK*sin(theta) + FDE*sin(theta),                     # Node D in the y-direction [X]
                             FEJ - FEI - FDE*cos(theta) + FEF*cos(theta),         # Node E in the x-direction [X]
                             FDE*sin(theta) + FEF*sin(theta),                     # Node E in the y-direction [X]
                             FFG*cos(theta) - FDF - FEF*cos(theta),               # Node F in the x-direction [X]
                             FFG*sin(theta) + FEF*sin(theta) + FFJ,               # Node F in the y-direction [X]
                             FFG*cos(theta) + FGJ,                                # Node G in the x-direction [X]
                             FCH - FAH,                                           # Node H in the x-direction [X]
                             FBH,                                                 # Node H in the y-direction [X]
                             FEI - FIL - FIK*sin(theta),                          # Node I in the x-direction [X]
                             FDI + FIK*cos(theta),                                # Node I in the y-direction [X]
                             FGJ - FEJ,                                           # Node J in the x-direction [X]
                             FFJ - load2 + JY,                                    # Node J in the y-direction [X]
                             FDK*cos(theta)-FCK*cos(theta)+FIK*sin(theta),        # Node K in the x-direction [X]
                             FDK*sin(theta)-FCK*sin(theta)-FKL-FIK*cos(theta),    # Node K in the y-direction [X]
                             FIL - FCL,                                           # Node L in the x-direction [X]
                             FKL - load1],                                        # Node L in the y-direction [X]
                            AY, FAB, FAH, FBH, FBC, FBD, FCH, FCL, FCK, FDE, FDI, FDK, FDF, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIK, FIL, FKL, GY, JY)

print(trussSolve)
convert = trussSolve.args[0]

print(convert)

converted = []
for num in convert:
    converted.append(int(round(num)))

symbolsDistributedSupport = ("AY", "FAB", "FAH", "FBH", "FBC", "FBD", "FCH", "FCL", "FCK", "FDE", "FDI", "FDK", "FDF", "FEF", "FEI", "FEJ", "FFG", "FFJ", "FGJ", "FIK", "FIL", "FKL", "GY", "JY")
distributedSupport = converted

print("Truss analysis with distributed load:")
for sym,num in zip(symbolsDistributedSupport,distributedSupport):
    print(sym, " : ", num)

AY = convert[0]
FAB = convert[1]
FAH = convert[2]
GY = convert[22]
FFG = convert[16]
FGJ = convert[18]
FEJ = convert[15]
JY = convert[23]

torqueA  = GY*51*6 - load1*51*2.5 + (JY - load2)*51*5
print("Torque about A: ", torqueA)

torqueG = -AY*51*6 + load1*51*3.5 + (load2 - JY)*51
print("Torque about G: ", torqueG)

netForceY = AY + GY + JY - (load1 + load2)
print("Net force in the Y-direction: ", netForceY)

netForceXAtA = FAB*cos(theta) + FAH 
print("Net force in the X-direction at A: ", netForceXAtA)

netForceXAtG =  - FFG*cos(theta) - FGJ
print("Net force in the X-direction at G: ", netForceXAtG)

netForceXAtJ =  FGJ - FEJ
print("Net force in the X-direction at J: ", netForceXAtJ)

print("Total load: ", load1+load2)

#print("JY: ", JY)
