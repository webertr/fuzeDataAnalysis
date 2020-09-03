#!/usr/bin/env python3.4

import sympy
import math

# Simple truss problem the added members
# 2 moment equations about A and G. Ignore the x,y force balance equations at A, and the y force balance equation at G
# 12 nodes => 24 x,y force balance equations, minute the 3 described above = 21 x,y force balance equations.
# 21 struts = 21 unknowns + the Ay and Gy => 23 unknowns.
# The 21 x,y force balance eautions + the 2 moment equations = 23 unknowns.

AX = 0
GX = 0

AY, FAB, FAH, FBH, FBC, FBD, FCH, FCL, FCK, FDE, FDI, FDK, FDF, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIK, FIL, FKL, GY = \
sympy.symbols('AY, FAB, FAH, FBH, FBC, FBD, FCH, FCL, FCK, FDE, FDI, FDK, FDF, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIK, FIL, FKL, GY')

theta = 22.5 * 2 * math.pi/360.0
sin = math.sin
cos = math.cos
# 2lbs / foot * 30 feet * 24 cables * 1.2 = 1800 lbs
# The load is now distributed over just 4 points, so
load = 450

# 17 equations, 17 unknowns
trussSolve = sympy.linsolve([AY*306 - load*(51+179),                              # Moment about G [X]
                             GY*306 - load*(128 + 255),                           # Moment about A [X]
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
                             FGJ - FEJ,                                           # Node J in the y-direction [X]
                             FFJ - load,                                          # Node J in the y-direction [X]
                             FDK*cos(theta)-FCK*cos(theta)+FIK*sin(theta),        # Node K in the x-direction [X]
                             FDK*sin(theta)-FCK*sin(theta)-FKL-FIK*cos(theta),    # Node K in the y-direction [X]
                             FIL - FCL,                                           # Node L in the x-direction [X]
                             FKL - load],                                         # Node L in the y-direction [X]
                            AY, FAB, FAH, FBH, FBC, FBD, FCH, FCL, FCK, FDE, FDI, FDK, FDF, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIK, FIL, FKL, GY)

convert = trussSolve.args[0]

print(convert)

converted = []
for num in convert:
    converted.append(int(round(num)))

symbolsDistributedSupport = ("AY", "FAB", "FAH", "FBH", "FBC", "FBD", "FCH", "FCL", "FCK", "FDE", "FDI", "FDK", "FDF", "FEF", "FEI", "FEJ", "FFG", "FFJ", "FGJ", "FIK", "FIL", "FKL", "GY")
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

torqueA  = GY*306 - load*(128 + 255)
print("Torque about A: ", torqueA)

torqueG = AY*306 - load*(51+179)
print("Torque about G: ", torqueG)

netForceY = AY + GY - load*2.0
print("Net force in the Y-direction: ", netForceY)

netForceXAtA = FAB*cos(theta) + FAH 
print("Net force in the X-direction at A: ", netForceXAtA)

netForceXAtG =  - FFG*cos(theta) - FGJ
print("Net force in the X-direction at G: ", netForceXAtG)

print("Total load: ", load)
