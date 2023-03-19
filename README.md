[English translation](./README-eg.md)

# GnuplotEdior

2次元または3次元のグラフを作成するためのアプリケーションソフトウェアであるgnuplotのためのエディター。<br>
スクリプトやデータの編集，生成した画像やPDFの表示が行える。また，補完機能やToolTip表示によりコーディングを助ける。

# Main Features

- テキストエディター
  - 入力補完機能
  - コマンドのtoolTip表示
  - 行番号、エラー行の表示、ハイライト
  - F1キーによる対象コマンドのhelp表示
  - スタイルのカスタマイズ
- ファイルツリー
  - 基本的なファイル操作(作成,コピー,削除,保存)
  - 生成した画像の表示
  - スクリプトとデータファイルの分類
- セル
  - データの表示
  - セルの変更に伴う、グラフ自動描画
  - Excelなどのセルのcopy&paste対応
  - tableをlatexコードへの変換
- gnuplot
  - 標準出力・標準エラーのコンソール出力
  - 前処理コマンドの登録
  - 自動実行
- スクリプトのテンプレート作成・挿入
- ターミナル
  - WSLやcmdなどのターミナルの起動(テスト)

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
<br>

# Requirement

- gnuplot<br>
  特にgnuplot.exe(wgnuplot.exeではない)

# Installation

- gnuplot<br>
  gnuplotを持っていない場合、gnuplotを公式サイト[(http://www.gnuplot.info/)](http://www.gnuplot.info/)からダウンロードする。

# Usage

1. gnuplotを持っていない場合、gnuplotをダウンロードする。
2. このリポジトリをClone `git clone https://github.com/nabla27/GnuplotEditor.git`
3. フォルダー内の bin/release/GnuplotEditor.exe を実行して起動する。
4. 起動後、メニューバーの Widget->Gnuplot Setting のPathに gnuplot.exe 実行ファイルのパスを指定する。

詳細は[ダウンロードからプロットまでの流れ](./docs/ja/setup.md)を参照。

# Note

- クロスプラットフォームアプリケーションフレームワークであるQt(キュート)を用いて開発した。<br>
  リポジトリ(https://code.qt.io/cgit/qt/qtbase.git/ )<br>
  公式サイト(https://www.qt.io/jp )

# License

GnuplotEditor is under [GPLv3 license](https://www.gnu.org/licenses/gpl-3.0.en.html).
