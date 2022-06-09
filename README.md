[English translation](./README-eg.md)

# GnuplotEdior

2次元または3次元のグラフを作成するためのアプリケーションソフトウェアであるgnuplotのためのエディター。<br>
このソフトのみでスクリプトやデータの編集，生成した画像の表示が行える。また，補完機能やToolTip表示によりコーディングを助ける。

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
  - latexコードへの変換
- gnuplot
  - 標準出力・標準エラーのコンソール出力
  - 前処理コマンドの登録
- スクリプトのテンプレート作成・挿入

# DEMO

![GnuplotEditor1](https://user-images.githubusercontent.com/63175080/172048494-466826a6-d56a-4c73-860b-6269ea05ceb9.png)
<br>
![GnuplotEditor2](https://user-images.githubusercontent.com/63175080/172048495-61a6cd21-09c6-4cac-abf7-2b234fd085c1.png)
<br>
![GnuplotEditor3](https://user-images.githubusercontent.com/63175080/172048498-7a85b203-1b99-47bb-a29a-ba82612deea5.png)

# Requirement

- gnuplot<br>
  特にgnuplot.exe(wgnuplot.exeではない)

# Installation

- gnuplot<br>
  gnuplotを持っていない場合、gnuplotを公式サイト[(http://www.gnuplot.info/)](http://www.gnuplot.info/)からダウンロードする。

# Usage

1. gnuplotを持っていない場合、gnuplotをダウンロードする。
1. このリポジトリをClone `git clone https://github.com/nabla27/GnuplotEditor.git`
   またはzipファイルでダウンロード[(https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip)](https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip)する。
3. フォルダー内の bin/release/GnuplotEditor.exe を実行して起動する。
4. 起動後、メニューバーの Widget->Gnuplot Setting のPathに gnuplot.exe 実行ファイルのパスを指定する。

詳細は[ダウンロードからプロットまでの流れ](./docs/ja/setup.md)を参照。

# Note

- クロスプラットフォームアプリケーションフレームワークであるQt(キュート)を用いて開発した。<br>
  リポジトリ(https://code.qt.io/cgit/qt/qtbase.git/ )<br>
  公式サイト(https://www.qt.io/jp )

# License

GnuplotEditor is under [GPLv3 license](https://www.gnu.org/licenses/gpl-3.0.en.html).
