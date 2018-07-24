#!/usr/bin/env python

import MDSplus
import matplotlib.pyplot as plt

# Connecting to the true
#t = MDSplus.Tree("fuze", 180215012)
t = MDSplus.Tree("fuze", 180723022)

# Accessing and plotting the Line-integrated image
image = t.getNode("\\NL_DHI").getData().data()
plt.imshow(image)
plt.show()
#testCol = 60
testCol = 2

# Accessing and plotting the radial density profiles and error bars
ne = t.getNode("\\NE_DHI").getData().data()
neData = ne[testCol,:]
neError = t.getNode("\\NE_DHI:ERROR").getData().data()
neErrorData = neError[testCol,:]
neR = t.getNode("\\NE_DHI:R").getData().data()
neZ = t.getNode("\\NE_DHI:Z").getData().data()
plt.errorbar(neR,neData,yerr = neErrorData)
plt.show()

print "Z = ", neZ[testCol], " m"

# Accessing and plotting the radial azimuthal magnetic field profile with error bars
B_THETA = t.getNode("\\B_THETA_DHI").getData().data()
B_THETAData = B_THETA[testCol,:]
B_THETAError = t.getNode("\\B_THETA_DHI:ERROR").getData().data()
B_THETAErrorData = B_THETAError[testCol,:]
B_THETAR = t.getNode("\\B_THETA_DHI:R").getData().data()
plt.errorbar(B_THETAR,B_THETAData,yerr = B_THETAErrorData)
plt.show()

# Accessing and plotting the radial temperature profile with error bars
T = t.getNode("\\T_DHI").getData().data()
TData = T[testCol,:]
TError = t.getNode("\\T_DHI:ERROR").getData().data()
TErrorData = TError[testCol,:]
TR = t.getNode("\\T_DHI:R").getData().data()
plt.errorbar(TR,TData,yerr = TErrorData)
plt.show()

# Printing out the units for the data
print "LINE_INT units are: ", t.getNode("\\NL_DHI").getUnits()
print "NE units are: ", t.getNode("\\NE_DHI").getUnits()
print "NE error units are: ", t.getNode("\\NE_DHI:ERROR").getUnits()
print "B_THETA error units are: ", t.getNode("\\B_THETA_DHI").getUnits()
print "B_THETA error units are: ", t.getNode("\\B_THETA_DHI:ERROR").getUnits()
print "T units are: ", t.getNode("\\T_DHI").getUnits()
print "T error units are: ", t.getNode("\\T_DHI:ERROR").getUnits()
print "Radial distance in: ", t.getNode("\\NE_DHI:R").getUnits()
