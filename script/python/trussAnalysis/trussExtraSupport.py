#!/usr/bin/env python3.4

import sympy
import math

# Simple truss problem with extra support and distrbuted 4th load

AY, EY, FAB, FAC, FBC, FBD, FCD, FCH, FDH, FDE, FDF, FEF, FEH, FEI, FFG, FFI, FGI, GY = sympy.symbols('AY, EY, FAB, FAC, FBC, FBD, FCD, FCH, FDH, FDE, FDF, FEF, FEH, FEI, FFG, FFI, FGI, GY')

theta = 22.5 * 2 * math.pi/360.0
sin = math.sin
cos = math.cos
load = 200

# 19 equations, 18 unknowns
trussSolve = sympy.linsolve([GY*306 - load*(51+1.5*153+255) + (EY-load)*204,      # Moment about A
                             -AY*204 + load*51 + 1.5*51*load - 51*load + GY*102,  # Moment about E
                             -AY*306 + load*(51+102+1.5*153) - 102*(EY-load),     # Moment about G                             
                             FAB*cos(theta)+FAC,                                  # Node A in the x direction
                             FBD - FAB*cos(theta) + FBC*cos(theta),               # Node B in the x direcdtion
                             FAB*sin(theta) + FBC*sin(theta),                     # Node B in the y direction
                             FCH - FAC - FBC*cos(theta) + FCD*cos(theta),         # Node C in the x-direction
                             FBC*sin(theta) + FCD*sin(theta) - 0.5*load,          # Node C in the y-direction
                             FDF - FBD + FDE*cos(theta) - FCD*cos(theta),         # Node D in the x-direction
                             FCD*sin(theta) + FDE*sin(theta),                     # Node D in the y-direction
                             FEI - FEH - FDE*cos(theta) + FEF*cos(theta),         # Node E in the x-direction
                             FDE*sin(theta) + FEF*sin(theta) - load + EY,         # Node E in the y-direction
                             FFG*cos(theta) - FDF - FEF*cos(theta),               # Node F in the x-direction
                             FFG*sin(theta) + FEF*sin(theta),                     # Node F in the y-direction
                             FEH - FCH,                                           # Node H in the x-direction
                             FDH - load*1.5,                                      # Node H in the y-direction
                             FGI - FEI,                                           # Node I in the x-direction
                             FFI - load,                                          # Node I in the y-direction
                             FFG*cos(theta) + FGI],                               # Node G in the x-direction
                            AY, EY, FAB, FAC, FBC, FBD, FCD, FCH, FDH, FDE, FDF, FEF, FEH, FEI, FFG, FFI, FGI, GY)

convert = trussSolve.args[0]

print(convert)

converted = []
for num in convert:
    converted.append(int(round(num)))

symbolsExtraSupport = ("AY", "EY", "FAB", "FAC", "FBC", "FBD", "FCD", "FCH", "FDH", "FDE", "FDF", "FEF", "FEH", "FEI", "FFG", "FFI", "FGI", "GY")
extraSupport = converted

print("Truss analysis with extra support and distributed load:")
for sym,num in zip(symbolsExtraSupport,extraSupport):
    print(sym, " : ", num)

AY = convert[0]
EY = convert[1]
GY = convert[17]
print("AY: ", AY, " (Should be )")
print("EY: ", EY, " (Should be )")
print("GY: ", GY, " (Should be )")
torqueA  = GY*306 - load*(51+1.5*153+255) + (EY-load)*204
print("Torque about A: ", torqueA)

torqueE  = -AY*204 + load*51 + 1.5*51*load - 51*load + GY*102
print("Torque about E: ", torqueE)

torqueG = -AY*306 + load*(51+102+1.5*153) - 102*(EY-load)
print("Torque about G: ", torqueG)

netForceY = AY + GY + EY - load*4.0
print("Net force in the Y-direction: ", netForceY)

