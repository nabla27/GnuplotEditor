# ダウンロードからプロットまでの流れ

1. gnuplotがダウンロードされていない場合、[公式サイト](http://www.gnuplot.info/)からダウンロードする。

3. ダウンロード方法する。 <br> 
  いづれかの方法でこのリポジトリをローカルにダウンロードする。<br>
  ・リポジトリをクローンする。`git clone https://github.com/nabla27/GnuplotEditor.git` <br>
  ・[ここから](https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip)zipファイルでダウンロードする。
  
3. アプリを起動させる。 <br>
  フォルダー内の "bin/release/GnuplotEditor.exe" を実行する。 <br>
  そうするとメインウィンドウが表示される。
  ![gp-setup1](https://user-images.githubusercontent.com/63175080/172050026-a184275b-271e-47ef-8c96-5724faea9ba4.png) <br>
  
4. gnuplotの設定画面を開く。 <br>
  トップのメニュバーで Widget->Gnuplot Setting を選択する。
  そうするとgnuplotの設定ウィンドウが開かれる。
  ![gp-setup2](https://user-images.githubusercontent.com/63175080/172050173-264a4cb2-43a4-4c1f-96de-5f72fb190fcc.png) <br>
  
5. gnuplotの実行パスを設定する。 <br> 
  項目”Path”にgnuplotの実行ファイル”gnuplot.exe”のパスを入力する。 <br>
  gnuplotのパスが環境変数で設定されている場合、”gnuplot.exe”のみでも問題ない。<br>
  右のボタンを押すと、ファイルダイアログからローカルのファイルを参照できる。
  
6. スクリプトファイルを作成する。 <br>
  左のファイルツリーで、フォルダーアイコンのアイテムを右クリックし、”new”を選択して作成するファイル名”test0.txt”と入力する。
  ![gp-setup3](https://user-images.githubusercontent.com/63175080/172050913-cc638100-de97-4498-9693-072d3f4bf021.png)
  
7. ファイルを選択する。 <br> 
  ファイルツリーに追加された作成したファイル”test0.txt”をダブルクリックする。 <br>
  そうすると、Gnuplotタブにファイルの内容が表示される。
  ![gp-setup4](https://user-images.githubusercontent.com/63175080/172051040-e8a2af57-c995-49f8-89ad-19396b81e106.png)
  
8. プロットする。 <br>
  適当なコマンドをエディターに入力し、右上の”Run”ボタンまたは”Alt+R”キーでgnuplotを実行できる。
  ![gp-setup5](https://user-images.githubusercontent.com/63175080/172051279-faa0c05c-717e-45e6-9626-e604e95217e8.png)
  
