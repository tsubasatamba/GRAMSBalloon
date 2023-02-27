# Raspberry Piの設定

## 日本語入力の設定
`sudo apt install fcitx-mozc`
を利用して行った。

## bluetoothの解除
`dtoverlay=disable-bt`を/boot/config.txtに記入。

## UARTの有効化
`enable_uart=1`によりUARTを有効化する。

## COMの設定
`dtoverlay=uart5`を指定することでUART5のピンを有効化。これによりCPUファンが使える。

## fanの設定
`dtoverlay=gpio-fan,gpiopin=12,temp=60000`でファンの設定ができる。
## シャットダウンスイッチの設定
`dtoverlay=gpio-shutdown,debounce=1000`でシャットダウンスイッチが使える。

## I2C・SPIの設定
dtparam=i2c_arm=off
dtparam=spi=on