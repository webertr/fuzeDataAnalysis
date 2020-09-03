#!/usr/bin/env python3.4

import sympy
import math

# Simple truss problem with distrbuted 4th load

AY, FAB, FAC, FBC, FBD, FCD, FCH, FDH, FDE, FDF, FEF, FEH, FEI, FFG, FFI, FGI, GY = sympy.symbols('AY, FAB, FAC, FBC, FBD, FCD, FCH, FDH, FDE, FDF, FEF, FEH, FEI, FFG, FFI, FGI, GY')

theta = 22.5 * 2 * math.pi/360.0
sin = math.sin
cos = math.cos
load = 200

# 17 equations, 17 unknowns
trussSolve = sympy.linsolve([-AY*306 + load*(51+102+1.5*153+102),                 # Moment about G
                             GY*306 - load*(51+1.5*153+204+255),                  # Moment about A
                             FBD - FAB*cos(theta) + FBC*cos(theta),               # Node B in the x direcdtion
                             FAB*sin(theta) + FBC*sin(theta),                     # Node B in the y direction
                             FCH - FAC - FBC*cos(theta) + FCD*cos(theta),         # Node C in the x-direction
                             FBC*sin(theta) + FCD*sin(theta) - 0.5*load,          # Node C in the  y-direction
                             FDF - FBD + FDE*cos(theta) - FCD*cos(theta),         # Node D in the x-direction
                             FCD*sin(theta) + FDE*sin(theta),                     # Node D in the y-direction
                             FEI - FEH - FDE*cos(theta) + FEF*cos(theta),         # Node E in the x-direction
                             FDE*sin(theta) + FEF*sin(theta) - load,              # Node E in the y-direction
                             FFG*cos(theta) - FDF - FEF*cos(theta),               # Node F in the x-direction
                             FFG*sin(theta) + FEF*sin(theta),                     # Node F in the y-direction
                             FDH - load*1.5,                                      # Node H in the y-direction
                             FEH - FCH,                                           # Node H in the x-direction
                             FGI - FEI,                                           # Node I in the x-direction
                             FFI - load,                                          # Node I in the y-direction
                             FFG*cos(theta) + FGI],                               # Node G in the x-direction
                            AY, FAB, FAC, FBC, FBD, FCD, FCH, FDH, FDE, FDF, FEF, FEH, FEI, FFG, FFI, FGI, GY)

convert = trussSolve.args[0]

print(convert)

converted = []
for num in convert:
    converted.append(int(round(num)))

symbolsDistributedSupport = ("AY", "FAB", "FAC", "FBC", "FBD", "FCD", "FCH", "FDH", "FDE", "FDF", "FEF", "FEH", "FEI", "FFG", "FFI", "FGI", "GY")
distributedSupport = converted

print("Truss analysis with distributed load:")
for sym,num in zip(symbolsDistributedSupport,distributedSupport):
    print(sym, " : ", num)

AY = convert[0]
GY = convert[16]
print("AY: ", AY, " (Should be 87)")
print("GY: ", GY, " (Should be 133)")
torqueA  = -AY*306 + load*(51+102+1.5*153+102)
print("Torque about A: ", torqueA)

torqueG = GY*306.0 - load*(51+1.5*153+204+255)
print("Torque about G: ", torqueG)

netForceY = AY + GY - load*4.0
print("Net force in the Y-direction: ", netForceY)
