#include <command2hex.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>


int main(int argc, char *argv[]){
    //コマンドの名前
    std::string command = argv[1];

    //コマンドの引数
    std::vector<int> arg_list;
    for(int i=2;i<argc;i++){
        arg_list.push_back(boost::lexical_cast<int>(argv[i]));
    }

    // Error 判定
    error(command, argc);

    //標準出力
    std::string out = hex_output(command, arg_list);
    std::cout << out << "\n";

    return 0;
}

