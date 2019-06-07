## Spectrocopy Monitor (spectroscopyMonitor.cpp)

# Introduction

This source code provides a function that will monitor PV's from the FuZE Control System, and attach Callback functions to each PV. The main callback function will search for a .spe file from the Light Field software, and upload the data to an MDSplus database. This is still a work in process.

# Issue: Accessing file that does not exist

There are many current outstanding issues with this soure code. I would like to start documenting them here.

I did see one issues.  Basically, the program tried to read a light field file that didn't exist. But it did not identify the file as not existing, and tried to load a LightField object using the file path. At that time, the LightField class was not checking to see if the file actually existed, so it tried to create the Light Field object, and threw an error. The output in the bash terminal looked like this:

Shot number set to: 190604001
ICCD center wavelength set to: 420
Spectrometer z set to: 0.15
Telescope Angle set to: 3 
ICCD Grating set to: 3
ICCD active slit set to: 1
Light Field Upload to MDSPlus: FuZE:ControlPLC:LightFieldMDSplusUp = 1
Error reading SPE file for frame width
fuze@ops:~/Spectroscopy/Data/190604$ ls
190604002-raw.spe  190604002.spe  190604003-raw.spe  190604003.spe  
190604004-raw.spe  190604004.spe

This section of code should have caught it:

if ( !checkFileExists(shotNumberFileName)) {
  std::cout << "No light field file available for shot number "
     << shotNumber << "\n";
  return;
}

but it did not. The "checkFileExists" function was tested and it worked. I had been seeing issues with the network attached storage drive on which the data resides. Basically, the computer would not recongize that the nas was attached. After running an "ls" command, you would get an error, then the NAS would be visible and accessible again. Not sure if that had something to do with it.

# To Do

- [x] First
- [ ] Second
- [ ] Third