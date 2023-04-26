#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <TCanvas.h>
#include <TGraph.h>

template<typename T>
T getValue(int index, const std::vector<uint8_t>& vec)
{
  const int n = vec.size();
  const int byte = sizeof(T);
  if (index+byte>n) {
    std::cerr << "getValue error: out of range" << std::endl;
    std::cerr << "vec.size() = " << n << ", index = " << index << ", byte = " << byte << std::endl;
    return static_cast<T>(0);
  }
  if (byte > 4) {
    std::cerr << "TelemetryDefinition::getValue error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 4: byte = " << byte << std::endl;
    return static_cast<T>(0);
  }

  uint32_t v = 0;
  for (int i=0; i<byte; i++) {
    const int j = index + i;
    const int shift = 8 * (byte-1-i);
    v |= vec[j] << (shift);
  }
  return static_cast<T>(v);
}

template<typename T>
void getVector(int index, int num, const std::vector<uint8_t>& vec, std::vector<T>& res)
{
  const int n = vec.size();
  const int byte = sizeof(T);
  if (index+byte*num > n) {
    std::cerr << "TelemetryDefinition::getVector error: out of range" << std::endl;
    std::cerr << "vec.size() = " << n << ", index = " << index << ", byte = " << byte
    << ", num = " << num << std::endl;
    return;
  }
  if (byte > 4) {
    std::cerr << "TelemetryDefinition::getVector error: typename error" << std::endl;
    std::cerr << "byte should be equal to or less than 4: byte = " << byte << std::endl;
    return;
  }
  res.clear();
  for (int i=0; i<num; i++) {
    T v = getValue<T>(index, vec);
    res.push_back(v);
    index += byte;
  }
}

void plotWaveform(const std::vector<std::vector<double> >& wf, double dt, const std::string& filename)
{
  TCanvas* canv = new TCanvas();
  canv -> Divide(2, 2);
  std::vector<TGraph*> graph(4);

  for (int i=0; i<4; i++) {
    if (i>=static_cast<int>(wf.size())) break;
    canv->cd(i+1);
    const int n = wf[i].size();
    graph[i] = new TGraph(n);
    for (int j=0; j<n; j++) {
      const double t = dt * j;
      graph[i]->SetPoint(j, t, wf[i][j]);
    }
    graph[i]->Draw();
  }
  canv->SaveAs(filename.c_str());
  delete(canv);
}

int main(int argc, char **argv)
{
  std::vector<int16_t> data;
  const std::string filename = argv[1];
  std::ifstream ifs(filename, std::ios::binary);
  std::vector<uint8_t> arr;

  while (!ifs.eof()) {
    char c;
    ifs.read(&c, 1);
    arr.push_back(static_cast<uint8_t>(c));
  }
  std::cout << arr.size() << std::endl;
  std::cout << static_cast<int>(arr.back()) << std::endl;
  std::cout << static_cast<int>(arr[arr.size()-2]) << std::endl;
    
  std::vector<int16_t> file_header;
  getVector<int16_t>(0, 32, arr, file_header);

  for (int i=0; i<static_cast<int>(file_header.size()); i++) {
    std::cout << i << " " << file_header[i] << std::endl;
  }

  const int trigger_device = static_cast<int>(file_header[0]);
  const int trigger_channel = static_cast<int>(file_header[1]);
  const int trigger_src = static_cast<int>(file_header[2]);
  const int trigger_slope = static_cast<int>(file_header[3]);
  const double trigger_level = static_cast<double>(file_header[4]) / 1E3;
  const double trigger_position = static_cast<double>(file_header[5])/1E3;
  const double sample_frequency = static_cast<double>(file_header[6])/1E3;
  const double time_window = static_cast<double>(file_header[7])/1E3;
  const int num_sample = static_cast<int>(file_header[8]);
  const int num_event_per_file = static_cast<int>(file_header[9]);
  timeval header_time;
  header_time.tv_sec = ((static_cast<int>(file_header[10]))<<16) + static_cast<int>(file_header[11]);
  header_time.tv_usec = ((static_cast<int>(file_header[12]))<<16) + static_cast<int>(file_header[13]);
  std::vector<double> range(4);
  std::vector<double> offset(4);
  int index = 14;
  for (int i=0; i<4; i++) {
    int v = static_cast<int>(file_header[index])<<16;
    v |= static_cast<int>(file_header[index+1]);
    range[i] = static_cast<double>(v)/1E3;
    index += 2;
  }
  for (int i=0; i<4; i++) {
    int v = static_cast<int>(file_header[index])<<16;
    v |= static_cast<int>(file_header[index+1]);
    offset[i] = static_cast<double>(v)/1E3;
    index += 2;
  }

  std::cout << "trigger device: " << trigger_device << std::endl;
  std::cout << "trigger channel: " << trigger_channel << std::endl;
  std::cout << "trigger src: " << trigger_src << std::endl;
  std::cout << "trigger slope: " << trigger_slope << std::endl;
  std::cout << "trigger level (V): " << trigger_level << std::endl;
  std::cout << "trigger position (us): " << trigger_position << std::endl;
  std::cout << "sample frequency (MHz): " << sample_frequency << std::endl;
  std::cout << "time window (us): " << time_window << std::endl;
  std::cout << "num sample: " << num_sample << std::endl;
  std::cout << "num event per file: " << num_event_per_file << std::endl;
  std::cout << "time sec: " << header_time.tv_sec << std::endl;
  std::cout << "time usec: " << header_time.tv_usec << std::endl;
  std::cout << "range: [" << range[0]  << ", " << range[1] << ", " << range[2] << ", " << range[3] << "]" << std::endl;
  std::cout << "range: [" << offset[0]  << ", " << offset[1] << ", " << offset[2] << ", " << offset[3] << "]" << std::endl;

  index = 64;
  for (int i=0; i<num_event_per_file; i++) {
    std::vector<int16_t> event_header(5);
    getVector<int16_t>(index, 5, arr, event_header);
    std::cout << "event ID: " << event_header[0] << std::endl;
    index += sizeof(int16_t) * 5;
    std::vector<std::vector<double> > wf(4, std::vector<double>(num_sample));
    for (int j=0; j<4; j++) {
      for (int k=0; k<num_sample; k++) {
        int16_t val = getValue<int16_t>(index, arr);
        wf[j][k] = static_cast<double>(val) * range[j] / 65536;
        index += sizeof(int16_t);
      }
    }
    const double dt = 1.0 / sample_frequency;
    const std::string image_filename = "test.png";
    plotWaveform(wf, dt, image_filename);
  }
  std::cout << "index: " << index << std::endl;

  

    // int index = 0;
    // while (index<64) {
    //     short val = ((static_cast<int16_t>(temp[index]))<<8) + (static_cast<short>(temp[index+1]));
    //     std::cout << index/2 << " " << (int)temp[index] << " " << (int)temp[index+1] << " " <<  val << std::endl;
    //     index += 2;
    // }

    // const int num_events = 5;
    // const int num_channels = 4;
    // const int num_bins = 180;
    // for (int i=0; i<num_events; i++) {
    //     const int ev_header_size = 5;
    //     for (int j=0; j<ev_header_size; j++) {
    //         index += 2;
    //     }
    //     for (int j=0; j<num_channels; j++) {
    //         std::cout << "channel " << j << std::endl;
    //         std::vector<short> data;
    //         for (int k=0; k<num_bins; k++) {
    //             int16_t val = ((static_cast<short>(temp[index]))<<8) + (static_cast<short>(temp[index+1]));
    //             data.push_back(val);
    //             index += 2;
    //         }
    //         std::cout << "size: " << num_bins << std::endl;
    //         for (int k=0; k<num_bins; k++) {
    //             std::cout << data[k] << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    // }
    
    return 0;
}