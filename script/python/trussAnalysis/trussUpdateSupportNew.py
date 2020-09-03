#!/usr/bin/env python3.4

import sympy
import math

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
AX = 0
EX = 0
GX = 0

# 2lbs / foot * 30 feet * 24 cables * 1.2 = 1800 lbs
# The load is now distributed over just 4 points, so
# Total load needs to add up to 900 lbs
FLL = 450
FLI = 450
FLE = 450
FLJ = 450

theta = 22.5 * 2 * math.pi/360.0
sin = math.sin
cos = math.cos

AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY = \
sympy.symbols('AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY')


# 22 equations, 22 unknowns
trussSolve = sympy.linsolve([GY*51*6 - 5*51*(FLJ-JY) - 4*51*FLE - 3*51*FLI - 2.5*51*FLL,  # Moment about A [X]
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
                            AY, FAB, FAC, FBC, FBD, FCK, FCL, FDF, FDK, FDI, FDE, FEF, FEI, FEJ, FFG, FFJ, FGJ, FIL, FIK, FKL, GY, JY)

print(trussSolve)
convert = trussSolve.args[0]

print(convert)

converted = []
for num in convert:
    converted.append(int(round(num)))

symbolsDistributedSupport = ("AY", "FAB", "FAC", "FBC", "FBD", "FCK", "FCL", "FDF", "FDK", "FDI", "FDE", "FEF", "FEI", "FEJ", "FFG", "FFJ", "FGJ", "FIL", "FIK", "FKL", "GY", "JY")
distributedSupport = converted

print("Truss analysis with distributed load:")
for sym,num in zip(symbolsDistributedSupport,distributedSupport):
    print(sym, " : ", num)

AY = convert[0]
FAB = convert[2]
GY = convert[19]
JY = convert[20]


torqueAtA = GY*51*6 - 5*51*(FLJ-JY) - 4*51*FLE - 3*51*FLI - 2.5*51*FLL
pritn("Moment about point A: ", torqueAtA)

torqueAtG = -AY*51*6 + 3.5*51*FLL + 3*51*FLI + 2*51*FLE + 51*(FLJ-JY)
pritn("Moment about point G: ", torqueAtG)

torqueAtJ = -AY*5*51 + 2.5*51*FLL + 2*51*FLI + 51*FLE + 51*GY
pritn("Moment about point J: ", torqueAtJ)

netForceXAtA = FAB*cos(theta) + FAC
print("Net force in the X-direction at A: ", netForceXAtA)

netForceXAtG =  - FFG*cos(theta) - FGJ
print("Net force in the X-direction at G: ", netForceXAtG)

netForceXAtJ =  FGJ - FEJ
print("Net force in the X-direction at J: ", netForceXAtJ)

print("Total load: ", FLL + FLI + FLE + FLJ)

