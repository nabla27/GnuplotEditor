# GnuplotEdior

2次元または3次元のグラフを作成するためのアプリケーションソフトウェアであるgnuplotのためのエディター。

# Main Features

- テキストエディター
  - 入力補完機能
  - コマンドのtoolTip表示
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
- gnuplot
  - 標準出力・標準エラーのコンソール出力
  - 前処理コマンドの登録
- スクリプトのテンプレート作成・挿入

# DEMO



# Requirement

- gnuplot<br>
  特にgnuplot.exe(wgnuplot.exeではない)

# Installation

- gnuplot
  - gnuplotを持っていない場合、gnuplotを公式サイト[(http://www.gnuplot.info/)](http://www.gnuplot.info/)からダウンロードする。

# Usage

1. gnuplotを持っていない場合、gnuplotをダウンロードする。
1. このリポジトリをClone `git clone https://github.com/nabla27/GnuplotEditor.git`
   またはzipファイルでダウンロード[(https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip)](https://github.com/nabla27/GnuplotEditor/archive/refs/heads/master.zip)する。
3. フォルダー内の bin/release/GnuplotEditor.exe を実行して起動する。
4. 起動後、メニューバーの Widget->Gnuplot Setting のPathに gnuplot.exe 実行ファイルのパスを指定する。

# Note


# License

GnuplotEditor is under [GPLv3 license](https://www.gnu.org/licenses/gpl-3.0.en.html).
