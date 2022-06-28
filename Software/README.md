# ソフトウェアに関する諸注意

## 前提
- libjpegのインストール


### libjpeg

[ここ](http://www.ijg.org/)からダウンロード

展開しそのフォルダ内で`./configure`を実行する

`make`でライブラリ生成、`sudo make install`でインストールを行う

実行ファイルを生成時は`-ljpeg`をつけてligjpegにリンクする