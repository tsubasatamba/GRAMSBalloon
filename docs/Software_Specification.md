
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

- Analog Discovery のDAC側にアクセスし、任意の電圧を出力する。出力した電圧でHVを制御する。
- 高圧電源の扱いは慎重に行う必要があるため、電圧の変化は2段階認証を必要としている。つまり、目標電圧をセットしたのち、あらためて実行許可を出す必要がある。
- TPC用とPMT用の2つのmoduleを用意することが想定されている。

#### 入力パラメータ

- <modpar>device_id</modpar> (default: 0)<br>
  対象となるAnalog Discovery の device ID。
- <modpar>channel</modpar> (default: 0)<br>
  対象となるAnalog Discovery の channel。前項と合わせて、出力するDAC端子が一意に決まる。
- <modpar>sleep</modpar> (default: 500)<br>
  電圧を出力した後、sleep する時間。単位は${\rm ms}$。

#### 仕様

- <b>mod_initialize</b><br>
  <tt>AnalogDiscoveryManager</tt> module にアクセスし、AnalogOut の初期設定を行う。
- <b>mod_analyze</b><br>
  現在の出力電圧 (<tt>currentVoltage_</tt>)と次の出力電圧 (<tt>nextVoltage_</tt>)が異なっていた場合、<tt>nextVoltage_</tt> を新たにDACで出力する。ただし、メンバー変数 <tt>exec_</tt> がfalseのときはこの操作は行われない。<tt>exec_</tt> は、操作を行うごとにfalseに戻されるため、設定が意図した通りになっているかどうかを確認した後、逐一true に書き換える必要がある。

#### Core class

- なし。

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
  I2C通信の際の仮想ファイルのパス。基本的に変更しない。
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
- <modpar>start_reading</modpar> (default: false)<br>
  はじめから波形読み出しを行うかどうかをboolean で入力する。

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

#### 機能

- 地上からのコマンドをシリアル通信を介して受け取る。
- コマンドを解釈した後、適切な指令を各モジュールに配分する。
- コマンド一覧は以下を参照<br>
  https://docs.google.com/spreadsheets/d/149plbWC4adAmXE9alBa7HCU7ituWycGj6gzw1qqyXyk/edit#gid=2044691227

#### 入力パラメータ

- <modpar>baudrate</modpar> (default: B9600)<br>
  通信のbaudrate を入力する。基本的に、Bの後の数字を整数で入力すれば良い。
- <modpar>serial_path</modpar> (default: "/dev/null")<br>
  シリアル通信に用いるデバイススペシャルファイルのパス。Raspi でシリアル通信を行う際、"/dev/ttyAMA0" などである。
- <modpar>open_mode</modpar> (default: O_RDWR=2)<br>
  シリアル通信のオプションであるopen_mode を入力する。READONLY=0, WRITEONLY=1, READWRITE=2, NONBLOCK=4 である。READWRITE and NONBLOCK としたい場合は6と指定する。Raspi でコマンドを受け取る際はREADWRITE=2 を指定するのが良い。
- <modpar>TPC_HVController_module_name</modpar> (default: "ControlHighVoltage_TPC")<br>
  ControlHighVoltage モジュールのうち、TPC をコントロールするモジュールの名前。基本的に変更しない。
- <modpar>PMT_HVController_module_name</modpar> (default: "ControlHighVoltage_PMT")<br>
  ControlHighVoltage モジュールのうち、PMT をコントロールするモジュールの名前。基本的に変更しない。

#### 仕様

- <b>mod_initialize</b><br>
  他モジュールへのアクセスを確立するとともに、シリアル通信の初期設定を行う。
- <b>mod_analyze</b><br>
  1ループごとにコマンド受け取りの試みる。シリアル通信を介して信号が何も来ない場合、一定時間 (10秒)を経てからタイムアウトとなり、次のループに移行する。信号が来た場合、その文字列を解釈する。開始コード、終了コード、CRC、コマンドコード が正常であることを確認した後、意図された指示を対象となる他モジュールへ反映する。コマンドの受信が途中で途切れた場合、次のループでその続きを受信することを試みる。また、1ループの処理でコマンドが複数個連なって受信されてしまった場合、1個目のコマンドのみ有効になる。そのため、地上からコマンドを複数送信する際、ある程度の間隔 (1秒あれば問題ない)をあけなければならない。

#### Core class

- CommandDefinition.cc<br>
  コマンドの解釈方法が記されている。
- SerialCommunication.cc<br>
  シリアル通信の操作が記されている。




### ReceiveTelemetry



### SendTelemetry

### ShutdownSystem

#### 機能

- Raspi のシャットダウン、再起動を制御する。

#### 入力パラメータ

- なし。

#### 仕様

- <b>mod_analyze</b><br>
  シャットダウンもしくは再起動の操作を行うかどうか判断し、実行する。
  これらの操作は重大な影響を及ぼすため、二段階認証を採用している。
  たとえば、シャットダウンの際には、<tt>prepareShutdown_</tt> というboolean 変数と<tt>shutDown_</tt> というboolean 変数が両方ともtrue になっていなければ実行されない。再起動の場合も同様である。これらの変数は地上からコマンドを送ることで書き換えることができる。(つまり、2種類のコマンドを送らないと実行されない)

#### Core class

- なし。

### SPIManager

#### 機能
- SPI通信を確立する。他のSPI通信を用いるmoduleは必ず本moduleを参照する。

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

