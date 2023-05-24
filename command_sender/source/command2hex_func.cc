#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>


//=================================== 以下、関数の定義 =================================================

//---------------------- map と vector の定義 -------------------------

std::map<std::string, int> mapdef(int flag){
std::map<std::string, int> codemap;
if (flag == 0){
    codemap["Get_Status"] = 100; codemap["Reset_Error"] = 101; codemap["Exec_Shutdown"] = 102; codemap["Exec_Reboot"] = 103; 
    codemap["Prepare_Shutdown"] = 104; codemap["Prepare_Reboot"] = 105; codemap["Start_Detector_Readout"] = 201;
    codemap["Stop_Detector_Readout"] = 202; codemap["Set_Trigger_Mode"] = 203; codemap["Set_Trigger_Channel"] = 204; 
    codemap["Set_ADC_Offset"] = 205; codemap["Exec_PMT_HV_Output"] = 206; codemap["Set_PMT_HV "] = 207; 
    codemap["Exec_TPC_HV_Output"] = 208; codemap["Set_TPC_HV"] = 209; codemap["Get_Waveform"] = 210; codemap["Dummy_1"] = 900; 
    codemap["Dummy_2"] = 901; codemap["Dummy_3"] = 902;
    }
else if (flag == 1){
    codemap["Get_Status"] = 0; codemap["Reset_Error"] = 0; codemap["Exec_Shutdown"] = 0; codemap["Exec_Reboot"] = 0; 
    codemap["Prepare_Shutdown"] = 0; codemap["Prepare_Reboot"] = 0; codemap["Start_Detector_Readout"] = 0; 
    codemap["Stop_Detector_Readout"] = 0; codemap["Set_Trigger_Mode"] = 1; codemap["Set_Trigger_Channel"] = 4; 
    codemap["Set_ADC_Offset"] = 3; codemap["Exec_PMT_HV_Output"] = 0; codemap["Set_PMT_HV "] = 1; codemap["Exec_TPC_HV_Output"] = 0; 
    codemap["Set_TPC_HV"] = 1; codemap["Get_Waveform"] = 0; codemap["Dummy_1"] = 0; codemap["Dummy_2"] = 1; codemap["Dummy_3"] = 0;
    }

return codemap;

}

std::vector<std::string> comnames(){
    std::vector<std::string> comvector{"Get_Status","Reset_Error","Exec_Shutdown","Exec_Reboot","Prepare_Shutdown",
    "Prepare_Reboot","Start_Detector_Readout","Stop_Detector_Readout","Set_Trigger_Mode","Set_Trigger_Channel",
    "Set_ADC_Offset","Exec_PMT_HV_Output","Set_PMT_HV ","Exec_TPC_HV_Output","Set_TPC_HV","Get_Waveform","Dummy_1",
    "Dummy_2","Dummy_3"};

    return comvector;
}

//---------------------- Error 判定 -------------------------
int error(std::string com_input, int argc){
    
    int nameflg = 0;
    int lenflg = 0;
    
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

//---------------------- 16進数コードを出力 -------------------------
std::string hex_output(std::string com_input, std::vector<int> arg_list){
    std::vector<int> command;
    // 開始コード
    command.push_back(235);
    command.push_back(144);
    // コマンド ID
    int command_ID = get_comid(com_input);
    int comid_1, comid_0;
    std::tie(comid_1, comid_0) = dec2byte_2byte(command_ID);
    command.push_back(comid_1);
    command.push_back(comid_0);
    //引数の個数
    int argument_len = get_argc(com_input);
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

    //16進数出力
    std::ostringstream output;
    for (auto it = command.begin(); it != command.end(); ++it) {
        output << std::setfill('0') << std::right << std::setw(2) << std::hex << *it;
    }
    return output.str();
}