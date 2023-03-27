
# GRAMS Balloon Software Specification

<link rel="stylesheet", href="./style.css">


## 目的





## ANL Modules

### AnalogDiscoveryManager



### GetEnvironmentalData

#### 機能

- 環境センサBME680を用いて、耐圧容器内の環境データ (温度、湿度、気圧)を取得する。

#### 入力パラメータ

- <modpar>chip_select</modpar> (default: 8)<br>
  対応するpinのchip selectを入力する。
- <modpar>SPI_manager_name</modpar> (default: "SPIManager")<br>
  対応するSPIManager module の名前を入力する。

#### 仕様

- <b>mod_pre_initialize</b><br>
  <tt>chip_select</tt>の値をSPIManagerに渡す。
- <b>mod_initialize</b><br>
  GPIO および SPI のハンドラーをSPIManagerからもらう。
- <b>mod_analyze</b><br>
  BME680 にアクセスし、温度・湿度・気圧を測定。
  <span style="color: red">測定の周期やデータの保存方法は未定。</span>

#### Core class
- BME680IO.cc


### MeasureTemperatureWithRTDSensor

#### 機能
- 側温抵抗体MAX31865を用いて、真空断熱容器・電磁弁・外気の温度を測定する。

#### 入力パラメータ

- <modpar>chip_select</modpar> (default: 8)<br>
  対応するpinのchip selectを入力する。
- <modpar>SPI_manager_name</modpar> (default: "SPIManager")<br>
  対応するSPIManager module の名前を入力する。

#### 仕様


### ReadDAQ

### ReceiveCommand



### RegulateHighVoltage

### SendTelemetry

### SPIManager

#### 機能
SPI通信を確立する。他のSPI通信を用いるmoduleは必ず本moduleを参照する。

#### 入力パラメータ
- <modpar>channel</modpar> (default: 0)
  <br>SPI通信の内部的なチャンネル。0か1が指定できるが、運用上は意味がないので、基本的には設定しなくてもよい。
- <modpar>baudrate</modpar> (default: 100000)
  <br>SPI通信のbaudrate。基本的にデフォルト値のままでよい。
- <modpar>spi_flags</modpar> (default: 0b1110000)
  <br>SPI通信のオプションを設定するフラグ。22 bitの整数で表される。特に下位bitはSPIモードを表し、対象機器のタイミング (rise/fall)・極性によって決定される。たとえば、環境データの取得では、フラグを0b1110000に設定する一方、RTDセンサによる温度測定では、0b1110001に設定しなけらばならない。そのような状況では、SPIManager moduleを2つ別々に設定する必要がある。
  詳細は、https://abyz.me.uk/rpi/pigpio/pdif2.html#spi_open を参照。

#### 仕様
- <b>mod_initialize</b><br>
  <tt>pigpio_start</tt>でGPIOにアクセスした後、<tt>spi_open</tt>でSPI通信を確立。それぞれハンドラーを取得する。<tt>GPIOHandler</tt>と<tt>SPIHandler</tt>は本モジュールが保有し、他のモジュールはこれを参照する。chip_selectの値は、便宜的に8を保有しているが、各モジュールによって異なるため、改めて設定する。登録された<tt>chip_select</tt>の全てに対して、<tt>set_mode</tt>, <tt>set_pull_up_down</tt>で初期化を行う。
- <b>mod_finalize</b><br>
  <tt>spi_close</tt>と<tt>pigpio_stop</tt>により、SPI通信の終了作業を行う。

### <span style="color: blue;">加速度センサ (仮)</span>


<modpar>aaaspi_flags</modpar>bcd

