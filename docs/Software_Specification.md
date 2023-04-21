
# GRAMS Balloon Software Specification

<link rel="stylesheet", href="./style.css">


## 目的


## 全体像


## ANL Modules


### AnalogDiscoveryManager

#### 機能

- Analog discovery への接続を確立する。Analog discovery への接続を試みる他のモジュールは必ず本モジュールを参照する必要がある。

#### 入力パラメータ
- なし。

#### 仕様
- <b>mod_pre_initialize</b><br>
  <tt>ADIO->initialize()</tt> で、Analog Discovery の初期化を行う。接続されているデバイスを検出し、ハンドラーIDを設定する。Analog Discovery を扱う他のモジュールは、本モジュールにアクセスすることで、ハンドラーのリストを取得する。
- <b>mod_finalize</b><br>
  Analog Discovery の終了操作を行う。

#### Core class
- AnalogDiscoveryIO.cc


### ControlHighVoltage

#### 機能

- Analog Discovery のDAC側にアクセスし、任意の電圧を出力する。

#### 入力パラメータ

- <modpar>device_id</modpar> (default: 0)<br>
- <modpar>channel</modpar> (default: 0)<br>
- <modpar>sleep</modpar> (default: 0)<br>

#### 仕様

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
  

#### Core class
- BME680IO.cc


### GetRaspiStatus

#### 機能
- Raspberyypiのステータスを取得・保持する。具体的には、CPU温度とSDカードの全体容量および残量を取得する。

#### 入力パラメータ

- <modpar>temperature_filename</modpar> (default: "/sys/class/thermal/thermal_zone0/temp")<br>
  CPU温度が書いてあるテキストファイル。基本的に変更しない。
- <modpar>path</modpar> (default: "/")<br>
  データ容量・残量を確認する対象のパス。基本的に変更しない。

#### 仕様
- <b>mod_analyze</b><br>
  まず、CPU温度を<tt>temperature_filename</tt>に読みにいく。この際、読み込まれる数字はADC値で、実際の温度の1000倍に相当する数である(°C単位)。次に、<tt>getCapacity()</tt>でディスクの全体容量と空き容量を取得する。

#### Core class
- なし。

### InterpretTelemetry


### MeasureAcceleration

#### 機能

- 加速度センサICM20948を用いて、加速度・角速度・地磁気を測定する。

#### 入力パラメータ

- <modpar>device_path</modpar> (default: "/dev/i2c-1")<br>
  ????
- <modpar>calibrate_gyro</modpar> (default: false)<br>
  角速度測定時にキャリブレーションを行うかどうかをbooleanで入力する。

#### 仕様
- <b>mod_initialize</b><br>
  ICM20948の初期化作業、および角速度キャリブレーションを行う。
- <b>mod_analyze</b><br>
  ICM20948センサにアクセスして加速度・角速度・地磁気のデータを取得する。

#### Core class
- ICM20948IO.cc
  


### MeasureTemperatureWithRTDSensor

#### 機能
- 測温抵抗体MAX31865を用いて、真空断熱容器・電磁弁・外気の温度を測定する。

#### 入力パラメータ

- <modpar>chip_select</modpar> (default: 8)<br>
  対応するpinのchip selectを入力する。
- <modpar>SPI_manager_name</modpar> (default: "SPIManager")<br>
  対応するSPIManager module の名前を入力する。

#### 仕様

- <b>mod_pre_initialize</b><br>
  <tt>chip_select</tt>の値をSPIManagerに渡す。
- <b>mod_initialize</b><br>
  GPIO および SPI のハンドラーをSPIManagerからもらう。<tt>max31865io -> setConfigureSingle()</tt>で温度計の初期設定を行う。
- <b>mod_analyze</b><br>
  MAX31865 にアクセスし、温度を測定。この際、実際の温度とADC値の両方を取得することできる。Telemetryに用いるのは、ADC値の方である。
  

#### Core class
- MAX31865IO.cc

### PushToMongoDB



### ReadWaveform

#### 機能

- Analog Discvoveryを操作することで、波形データを読み込み、保存する。
- 地上からのコマンドで波形データを要求された場合、直近で取得した波形を<tt>SendTelemetry</tt> moduleに書き込みに行く (参照渡しではなく、値をコピーする)。
  
#### 入力パラメータ

- <modpar>ad_manager_name</modpar> (default: "AnalogDiscoveryManager")<br>
  AnalogDiscoveryManager module の名前を入力する。ひとつのモジュールですべてのAnalog Discoveryを管理することが想定されるので、基本的に変更しなくてよい。
- <modpar>trig_device</modpar> (default: 0)<br>
  トリガーソースとなるデバイス。2台のAnalog Discoveryを用いる場合、0か1である。
- <modpar>trig_channel</modpar> (default: 0)<br>
  トリガーソースとなるチャンネル。Analog Discoveryには2つチャンネルがあるので、0か1を指定する。前項と合わせて、トリガーソースとなる波形が決定できる。
- <modpar>trig_mode</modpar> (default: 0x02)<br>
  トリガーモードを入力する。8 bit の整数で構成され、前半4 bitはtrigger slopeを表し、後半4 bitはtrigger sourceを表す。0-255までの値を入力できるが、invalidな値も多数ある。Trigger slope は、0 (RISE), 1 (FALL), 2 (EITHER) である。Trigger source は、0 (random trigger), 1 (periodic trigger), 2 (self trigger), 11 (external trigger) である。デフォルト値は、RISE + self triggerとなっている。
  たとえば、FALL + external trigger を設定したい場合は、0x1B=27を入力すれば良い。
- <modpar>trig_level</modpar> (default: 0.0)<br>
  トリガーレベルを入力する。単位は${\rm V}$。
- <modpar>trig_position</modpar> (default: 0.0)<br>
  トリガーポジションを入力する。単位は${\rm \mu s}$。
- <modpar>time_window</modpar> (default: 10.0)<br>
  タイムウィンドウ (取得する波形の時間的長さ)を入力する。単位は${\rm \mu s}$。
- <modpar>sample_frequency</modpar> (default: 2.0)<br>
  サンプル周波数を入力する。単位はMHz。<tt>time_window</tt> $\times$ <tt>sample_frequency</tt> でトータルのサンプル数 (=波形データのサイズ)となる。
- <modpar>adc_range_list</modpar> (default: [1.0, 1.0, 1.0, 1.0])<br>
  ADC のレンジを4チャンネルそれぞれ設定する。単位は${\rm V}$。Analog Discoveryの仕様上、$5\;{\rm V}$ と$50\;{\rm V}$しか許されず、デフォルトでは1.0を入れているが実際は$5\;{\rm V}$ (実際は$5.5\;{\rm V}$くらい) となっている。基本的に変更しない。
- <modpar>adc_offset_list</modpar> (default: [0.0, 0.0, 0.0, 0.0])<br>
  ADC のオフセットを入力する。単位は${\rm V}$。基本的に変更しない。<br>
  ADCの変換公式は次のとおりである。<br>
  ${\rm Voltage}={\rm ADC}*{\rm range}/65536+{\rm offset}$<br>
  詳細は、https://www.mouser.com/pdfDocs/waveforms_sdk_rm.pdf を参照。
- <modpar>output_filename_base</modpar> (default: "output")<br>
  波形データを保存するバイナリーファイルの名前のベース。バイナリーファイルは"output_000000.dat", "output_000001.dat"... と順次生成されていく。
- <modpar>num_events_per_file</modpar> (default: 100)<br>
  1つのバイナリーファイルに格納するイベントの数を入力する。

#### 仕様

- <b>mod_initialize</b><br>
  AnalogDiscoveryManger module およびsendTelemetry moduleへのアクセスを確立する。その後、Analog Discovery のAnalogInの設定を行う。<tt>adio->setupAnalogIn()</tt> でサンプル周波数、レンジ、オフセットを設定し、その後<tt>daqio->initialize()</tt> で初期化作業を行う。さらに、<tt>daqio->setupTrigger()</tt> でトリガーのセットアップを行う。
  
- <b>mod_analyze</b><br>
  1ループごとに<tt>daqio->getData()</tt> を走らせることでデータ取得を行う。<tt>numEventsPerFile</tt>ループごとにバイナリーファイルを保存する。トリガーのセットアップを変える指令がコマンド側から来た場合にはそれを反映する。波形データをテレメトリーで送る必要がある場合には、<tt>SendTelemetry</tt> moduleにデータを書き込みに行く。

- <b>mod_finalize</b><br>
  保存していないデータを保存する。
  
#### Core class

- DAQIO.cc



### ReceiveCommand

### ReceiveTelemetry



### SendTelemetry

### ShutdownSystem

### SimpleLoop

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




<modpar>aaaspi_flags</modpar>bcd

