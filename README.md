# UFOgame-Asteroids
Made an OOP retro Asteroids game that adds funny powerup icons using the C++ SFML library.

Follow the instructions below to compile and run the the Asteroid + UFO game
1. Start CLion or IDE
2. Load CMakeLists.txt
3. Select the Asteroids | Debug in the dialog box next to the green triangle if necessary.
4. Compile the Racing program by clicking on the green triangle.  You should get the following error.
Process finished with exit code -1073741515 (0xC0000135)5.	
5. Once the file is built, the current CMakeLists.txt does not copy the needed .dll files to the build directory.
You must add two directories to the PATH in the project debug environment in order to make it work.
Click on the down arrow next to the project debug button, then choose “Edit Configurations”.
In the resulting dialog box click on the folder icon next to Environment variables. In the resulting dialog box
click on the plus sign and add a variable named PATH with a value of
"%PATH%;C:\Program Files\SFML-2.5.1\bin;C:\Program Files\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\mingw64\bin"
without the quotes. This will allow the compiled Asteroids.exe to find the .dll files when you click on run. Click OK.
