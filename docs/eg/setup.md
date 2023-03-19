# Flow from download to plot

1. If gnuplot has not been downloaded, download it from the [Official Site](http://www.gnuplot.info/).

3. How to download. <br>
  Download this repository locally using either method. <br>
  - Clone the repository. `git clone https://github.com/nabla27/GnuplotEditor.git` <br>
  - [From here](https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip) Download as a zip file.
  
3. Launch the app. <br>
  Execute "bin/release/GnuplotEditor.exe" in the folder. <br>
  Then the main window will be displayed.
  ![gp-setup1](https://user-images.githubusercontent.com/63175080/172050026-a184275b-271e-47ef-8c96-5724faea9ba4.png) <br>
  
4. Open the gnuplot settings screen. <br>
  Select ”Widget->Gnuplot Setting” in the top menu bar.
  This will open the gnuplot settings window.
  ![gp-setup2](https://user-images.githubusercontent.com/63175080/172050173-264a4cb2-43a4-4c1f-96de-5f72fb190fcc.png) <br>
  
5. Set the execution path of gnuplot. <br>
  Enter the path of the gnuplot executable file "gnuplot.exe" in the item "Path". <br>
  If the path of gnuplot is set in the environment variable, there is no problem with just "gnuplot.exe". <br>
  You can browse local files from the file dialog by pressing the button on the right.
  
6. Create a script file. <br>
  In the file tree on the left, right-click the item with the folder icon, select "new" and enter the file name "test0.txt" to be created.
  ![gp-setup3](https://user-images.githubusercontent.com/63175080/172050913-cc638100-de97-4498-9693-072d3f4bf021.png)
  
7. Select a file. <br>
  Double-click the created file "test0.txt" added to the file tree. <br>
  Then the contents of the file will be displayed in the Gnuplot tab.
  ![gp-setup4](https://user-images.githubusercontent.com/63175080/172051040-e8a2af57-c995-49f8-89ad-19396b81e106.png)
  
8. Plot. <br>
  You can enter the appropriate command in the editor and run gnuplot with the "Run" button on the top right or the "Alt+R" keys.
  ![gp-setup5](https://user-images.githubusercontent.com/63175080/172051279-faa0c05c-717e-45e6-9626-e604e95217e8.png)
