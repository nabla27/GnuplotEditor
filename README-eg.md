# GnuplotEdior

Editor for gnuplot, application software for creating 2D or 3D graphs.<br>
Only this software can edit scripts and data, and display generated images. In addition, the complement function and ToolTip display help coding.

# Main Features

- Text editor
  - Input completion function
  - Display toolTip of command
  - Line number, error line display, highlight
  - Help display of target command by F1 key
  - Style customization
- File tree
  - Basic file operations (create, copy, delete, save)
  - Display of generated image
  - Classification of scripts and data files
- Cell
  - Data display
  - Automatic graph drawing due to cell changes
  - Supports copy & paste of cells such as Excel
- gnuplot
  - Standard output / Standard error console output
  - Registration of preprocessing command
- Create / insert script template

# DEMO

![1](https://user-images.githubusercontent.com/63175080/226178701-be30c2c4-480f-4074-adec-bf1681592fef.png)
<br>
![2](https://user-images.githubusercontent.com/63175080/226178737-d5094f3c-23a7-4a5c-81cd-7cd4ec6d16cd.png)
<br>
![3](https://user-images.githubusercontent.com/63175080/226178760-182fb9fc-fcbd-49e7-b52e-21b820d61117.png)
<br>
![4](https://user-images.githubusercontent.com/63175080/226178764-781ce216-273e-4672-9ed0-2c6b8332f3e9.png)
<br>
![5](https://user-images.githubusercontent.com/63175080/226178768-8ba78d6f-db83-490e-b9f8-67857db7b943.png)

# Requirement

- gnuplot <br>
  Especially gnuplot.exe (not wgnuplot.exe)

# Installation

- gnuplot <br>
  If you do not have gnuplot, download gnuplot from the official website (http://www.gnuplot.info/).

# Usage

1. If you don't have gnuplot, download gnuplot.
1. Clone this repository `git clone https://github.com/nabla27/GnuplotEditor.git`.
3. Execute bin / release / GnuplotEditor.exe in the folder to start it.
4. After launching, specify the path of the gnuplot.exe executable file in the Path of her Widget-> Gnuplot Setting on the menu bar.

See [Flow from download to plot](./docs/eg/setup.md) for details.

# Note

- Developed using Qt (Cute), a cross-platform application framework. <br>
  Repository (https://code.qt.io/cgit/qt/qtbase.git/) <br>
  Official site (https://www.qt.io/jp)

# License

GnuplotEditor is under [GPLv3 license] (https://www.gnu.org/licenses/gpl-3.0.en.html).
