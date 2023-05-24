#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>


struct CommandException
{
};

struct CommandProperty
{
  uint16_t code = 0;
  int argnum = 0;
};

CommandProperty get_command_code(const std::string& command_name)
{
  static std::map<std::string, CommandProperty> code_map;
  code_map["Get_Status"]             = CommandProperty{100, 0};
  code_map["Reset_Error"]            = CommandProperty{101, 0};
  code_map["Exec_Shutdown"]          = CommandProperty{102, 0};
  code_map["Exec_Reboot"]            = CommandProperty{103, 0};
  code_map["Prepare_Shutdown"]       = CommandProperty{104, 0};
  code_map["Prepare_Reboot"]         = CommandProperty{105, 0};
  code_map["Start_Detector_Readout"] = CommandProperty{201, 0};
  code_map["Stop_Detector_Readout"]  = CommandProperty{202, 0};
  code_map["Set_Trigger_Mode"]       = CommandProperty{203, 1};
  code_map["Set_Trigger_Channel"]    = CommandProperty{204, 2}; 
  code_map["Set_ADC_Offset"]         = CommandProperty{205, 3};
  code_map["Exec_TPC_HV_Output"]     = CommandProperty{206, 0};
  code_map["Set_TPC_HV "]            = CommandProperty{207, 1}; 
  code_map["Exec_PMT_HV_Output"]     = CommandProperty{208, 0};
  code_map["Set_PMT_HV"]             = CommandProperty{209, 1};
  code_map["Get_Waveform"]           = CommandProperty{210, 0};
  code_map["Set_Trigger_Level"]      = CommandProperty{211, 1};
  code_map["Set_Trigger_Position"]   = CommandProperty{212, 1};
  code_map["Dummy_1"]                = CommandProperty{900, 0}; 
  code_map["Dummy_2"]                = CommandProperty{901, 1};
  code_map["Dummy_3"]                = CommandProperty{902, 0};

  auto command = code_map.find(command_name);
  if (command == code_map.end()) {
    throw CommandException;
  }
  
  return *command;
}

//---------------------- Error 判定 -------------------------
int error(std::string com_input, int argc){
  if (*find(comnames().begin(), comnames().end(), com_input) == com_input){
    nameflg = 1;
  }

    if (argc-2 == mapdef(1)[com_input]){
        lenflg = 1;
    }

    if (!nameflg){
        std::cout << "\033[31m Error \033[m" 
        << ": invalid command name '" << com_input << "' \n";
        exit(EXIT_FAILURE);
    }

    if (!lenflg){
        std::cout << "\033[31m Error \033[m" 
        << ": '" << com_input <<"' command takes " << mapdef(1)[com_input] << " argument(s) but " << argc-2 << " were given.\n";
         exit(EXIT_FAILURE);
    }
    return 0;
}

//---------------------- コマンドIDの抽出 -------------------------
int get_comid(std::string com){
    int cid = mapdef(0)[com];
    return cid;
}

//---------------------- コマンド引数の個数抽出 -------------------------
int get_argc(std::string com){
    int argc = mapdef(1)[com];
    return argc;
}

//---------------------- 十進数 -> 2byte 256進数への変換 -------------------------
std::tuple<int, int> dec2byte_2byte(int num2){
    int num2_1 = num2/pow(256,1);
    int num2_0 = num2-256*num2_1;
    return std::make_tuple(num2_1, num2_0);
}

//---------------------- 十進数 -> 4byte 256進数への変換 -------------------------
std::tuple<int, int, int, int> dec2byte_4byte(int num4){
    int num4_3 = num4/pow(256,3);
    int num4_2 = (num4-num4_3*pow(256,3))/pow(256,2);
    int num4_1 = (num4-num4_3*pow(256,3)-num4_2*pow(256,2))/256;
    int num4_0 = (num4-num4_3*pow(256,3)-num4_2*pow(256,2)-num4_1*256);
    return std::make_tuple(num4_3, num4_2, num4_1, num4_0);
}

//---------------------- コマンド引数を取得し、4byte データを出力 -------------------------
std::vector<int> add256(std::vector<int> num){
    int numlen = sizeof(num) / sizeof(int);
    std::vector<int> arg;
    for (auto it = num.begin(); it != num.end(); ++it) {
        int arg_3, arg_2, arg_1, arg_0;
        std::tie(arg_3, arg_2, arg_1, arg_0) = dec2byte_4byte(*it);
        arg.push_back(arg_3);
        arg.push_back(arg_2);
        arg.push_back(arg_1);
        arg.push_back(arg_0);
    }
    return arg;
}

//---------------------- CRC の計算 -------------------------
std::tuple<int, int> crc_calc(std::vector<int> coma){
int crc = 0;
    for (auto i = coma.begin(); i != coma.end(); ++i) {
        crc = crc ^ *i;
        for  (int j = 0; j < 8; j++) {
            if (crc & 1){
                crc = (crc>>1)^0x8408;
            }else{
                crc >>= 1;
            }
        }
    }
    int crc_1, crc_0;
    std::tie(crc_1, crc_0) = dec2byte_2byte(crc);
    return std::make_tuple(crc_1, crc_0);
}

std::string make_byte_array(std::string command_name, const std::vector<int32_t>& arg_list)
{
  std::vector<uint8_t> command;
  // 開始コード
  command.push_back(0xEB);
  command.push_back(0x90);

  const CommandProperty property = get_command_code(command_name);
  // コマンド ID
  const int code = property.code;
  command.push_back((code & 0xFF00u) >> 8);
  command.push_back((code & 0x00FFu) >> 0);

  //引数の個数
  const int argnum = property.argnum;

  // to be continued


  int arglen_1, arglen_0;
  std::tie(arglen_1, arglen_0) = dec2byte_2byte(argument_len);
  command.push_back(arglen_1);
  command.push_back(arglen_0);
  //引数
  std::vector<int> argd;
  argd = add256(arg_list);
  for (auto it = argd.begin(); it != argd.end(); ++it) {
    command.push_back(*it);
  }
  //CRC
  int crc_1, crc_0;
  std::tie(crc_1, crc_0) = crc_calc(command);
  command.push_back(crc_1);
  command.push_back(crc_0);
  //終端文字
  for  (int j = 0; j < 2; j++) {
    command.push_back(197);
  }

  return command;
}
