#include <iostream>


//============================ 関数リスト ====================================
int error(std::string com_input, int argc);
int get_comid(std::string com);
int get_argc(std::string com);
std::tuple<int, int> dec2byte_2byte(int num2);
std::tuple<int, int, int, int> dec2byte_4byte(int num4);
std::vector<int> add256(std::vector<int> num);
std::tuple<int, int> crc_calc(std::vector<int> coma);
std::string hex_output(std::string com_input, std::vector<int> arg_list);
//==========================================================================
