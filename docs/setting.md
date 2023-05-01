# Raspberry Piの設定

## 日本語入力の設定

`sudo apt install fcitx-mozc`
を利用して行った。https://cloud-work.net/linux/fcitx-mozc/

## rootユーザのパスワード入力

`sudo passwd root`でパスワード登録

## 各種モジュールのインストール

### aptのアップデート
`sudo apt update`で情報を取ってきて、`sudo apt upgrade`でモジュールを最新版にする(これはこまめに行うこと。)

### zsh

`sudo apt install zsh`、`chsh`, `/bin/zsh`
`zsh`でzshを起動し、最初の起動時には4択を聞かれるので、2を選択する

### g++

`sudo apt install g++`

### boost

`sudo apt install libboost-all-dev`

### cmake

`sudo apt install cmake`

### SWIG

`sudo apt install swig`

### git

`sudo apt install git`

### Ruby

`sudo apt-get install autoconf bison patch build-essential rustc libssl-dev libyaml-dev libreadline6-dev zlib1g-dev libgmp-dev libncurses5-dev libffi-dev libgdbm6 libgdbm-dev libdb-dev uuid-dev`

rubyのHP(<http://ruby-lang.org/en/downloads/>)に行き、2.7系の最新版をダウンロードする

~/softwareに移ったあと、以下のコマンドを打つ

`curl https://cache.ruby-lang.org/pub/ruby/3.2/ruby-3.2.2.tar.gz --output ruby3.2.2.tar.gz`

その後これを解凍する。

`tar -xf ruby3.2.2.tar.gz`

~/software/Ruby-3.2.2に移動し、`configure`、`make　-j3`、`make check -j3`、`make install -j3`

### ANLNext の導入

`git clone http://github.com/odakahirokazu/ANLNext`

その後はREADME.mdの指示に従う。

### pigpio

`sudo apt install pigpio`

~/softwareに移動して、`git clone https://github.com/joan2937/pigpio`

## git

### gitの設定

`git config --global user.email`、`git config --global user.name`で設定

### git　clone

`git clone https://STA205233/GRAMS-Balloon-Test`でテスト用レポジトリをクローンしてくる。

`git clone https://tsubasatamba/GRAMSBalloon`で本番用レポジトリをクローンしてくる。

## bluetoothの解除

`dtoverlay=disable-bt`を/boot/config.txtに記入。

## UARTの有効化

`enable_uart=1`によりUARTを有効化する。

## COMの設定

`dtoverlay=uart5`を指定することでUART5のピンを有効化。これによりCPUファンが使える。

## fanの設定

`dtoverlay=gpio-fan,gpiopin=12,temp=60000`でファンの設定ができる。

## シャットダウンスイッチの設定

`dtoverlay=gpio-shutdown,debounce=1000,gpio_pin=18`でシャットダウンスイッチが使える。

## I2C・SPIの設定

`dtparam=i2c_arm=off`
`dtparam=spi=on`として、I2C, SPIを起動。

## pigpio の設定

`sudo systemctl enable pigpiod`を実行し、pigpioデーモンを自動起動。

## Analog Discovery 2のAPI導入
AdeptとWaveformsというものをインストールしないといけない。
GRAMS-Balloon-test/sourceに入っている二つの.debで終わるファイルを使う。

GRAMS-Balloon-test/sourceに移った後、

`sudo apt install digilent.adept.runtime_2.27.9-armhf.deb`、`sudo apt install digilent.waveforms_3.19.5_armhf.deb`を行う。
