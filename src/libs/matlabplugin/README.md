# MATLAB plugin for Tigon 

## MATLAB setup
When using MATLAB plugin for the first time it is important to set up the
MATLAB environment to work with API calls.
 
### Windows:
- Add to the PATH environment variable: ``` matlabroot\bin\\win64```
- Register MATLAB as a com server
     + Start MATLAB as a user with administrator privilidges and run:
       ```!matlab -regserver```
 
### Mac:
Add MATLAB application to the PATH environment variable with:
``` 
sudo launchctl config user path ```$PATH:matlabroot/bin```
```
where ```matlabroot``` is the path to the root application folder of MATLAB,
usually ```/Applications/MATLAB_R20XX_X.app/```.
 
### Linux:
- Add Matlab binary to PATH variable.
Optional (rpath properties should take care of this):
- ```setenv LD_LIBRARY_PATH \a matlabroot/bin/glnxa64:\a matlabroot/sys/os/glnxa64```
 
For more information see http://uk.mathworks.com/help/matlab/matlab_external/building-and-running-engine-applications-on-unix-operating-systems.html