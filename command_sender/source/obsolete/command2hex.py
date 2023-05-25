#!python3

import sys


def CalcCRC16_without_table(pbuffer):
    crc = 0
    size = len(pbuffer)
    for i in range(size):
        crc ^= pbuffer[i]
        for j in range(8):
            if (crc & 1):
                crc = (crc >> 1) ^ 0x8408
            else:
                crc >>= 1
    return crc


def com2hex(code, args):
    argc = len(args)
    command = []
    command.append(0xeb)
    command.append(0x90)
    for i in range(2):
        command.append((code >> 8 * (1 - i)) & 0xff)  # 上位 -> 下位
    for i in range(2):
        command.append((argc >> 8 * (1 - i)) & 0xff)  # 上位 -> 下位
    for i in range(argc):
        for j in range(4):
            command.append((int(args[i]) >> 8 * (3 - j)) & 0xff)
    crc = CalcCRC16_without_table(command)
    for i in range(2):
        command.append((crc >> 8 * (1 - i)) & 0xff)  # 上位 -> 下位
    for i in range(2):
        command.append(0xc5)
    ret = ""
    for i in range(len(command)):
        ret = ret + '{:02x}'.format(command[i])
    return ret


def hexout(CODE, *args):
    comname = ["Get_Status", "Reset_Error", "Exec_Shutdown", "Exec_Reboot", "Prepare_Shutdown", "Prepare_Reboot", "Start_Detector_Readout", "Stop_Detector_Readout", "Set_Trigger_Mode", "Set_Trigger_Channel", "Set_ADC_Offset", "Exec_TPC_HV_Output", "Set_TPC_HV", "Exec_PMT_HV_Output", "Set_PMT_HV", "Get_Waveform", "Set_Trigger_Level", "Set_Trigger_Position", "Dummy_1", "Dummy_2", "Dummy_3"]
    codeid = [100, 101, 102, 103, 104, 105, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 900, 901, 902]
    argment = [0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0]
    args = list(args)
    if not CODE in comname:
        raise ValueError(f"undefined command '{CODE}'")
    else:
        for i in range(len(comname)):
            if CODE == comname[i]:
                if len(args) != argment[i]:
                    raise TypeError(f"{CODE}() takes {argment[i]} positional arguments but {len(args)} were given")
                else:
                    with open("./commands/" + comname[i].replace(" ", "_"), "w") as fp:
                        rslt = com2hex(codeid[i], args)
                        print(rslt)
                        print(rslt, file=fp)
                        print(f"Saved to {fp.name}")


if __name__ == "__main__":
    if len(sys.argv) == 0:
        raise ValueError("Argument is needed.")
    hexout(*sys.argv[1:])
