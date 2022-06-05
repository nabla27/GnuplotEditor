#GnuplotEdior

Editor for gnuplot, application software for creating 2D or 3D graphs.

#Main Features

--Text editor
  --Input completion function
  --Display toolTip of command
  --Line number, error line display, highlight
  --Help display of target command by F1 key
  --Style customization
--File tree
  --Basic file operations (create, copy, delete, save)
  --Display of generated image
  --Classification of scripts and data files
--Cell
  --Data display
  --Automatic graph drawing due to cell changes
  --Supports copy & paste of cells such as Excel
--gnuplot
  --Standard output / Standard error console output
  --Registration of preprocessing command
--Create / insert script template

#DEMO

! [GnuplotEditor1] (https://user-images.githubusercontent.com/63175080/172048494-466826a6-d56a-4c73-860b-6269ea05ceb9.png)
<br>
! [GnuplotEditor2] (https://user-images.githubusercontent.com/63175080/172048495-61a6cd21-09c6-4cac-abf7-2b234fd085c1.png)
<br>
! [GnuplotEditor3] (https://user-images.githubusercontent.com/63175080/172048498-7a85b203-1b99-47bb-a29a-ba82612deea5.png)

#Requirement

--gnuplot <br>
  Especially gnuplot.exe (not wgnuplot.exe)

#Installation

--gnuplot <br>
  If you do not have gnuplot, download gnuplot from the official website (http://www.gnuplot.info/).

#Usage

1. If you don't have gnuplot, download gnuplot.
1. Clone this repository `git clone https://github.com/nabla27/GnuplotEditor.git`
   Or download it as a zip file (https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip).
3. Execute bin / release / GnuplotEditor.exe in the folder to start it.
4. After launching, specify the path of the gnuplot.exe executable file in the Path of her Widget-> Gnuplot Setting on the menu bar.

# Note

--Developed using Qt (Cute), a cross-platform application framework. <br>
  Repository (https://code.qt.io/cgit/qt/qtbase.git/) <br>
  Official site (https://www.qt.io/jp)

#License

GnuplotEditor is under [GPLv3 license] (https://www.gnu.org/licenses/gpl-3.0.en.html).
