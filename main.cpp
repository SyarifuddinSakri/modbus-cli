#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <modbus/modbus-tcp.h>
#include <modbus/modbus.h>
#include <string>
#include <thread>

bool is32 = false;
bool isSigned = false;
bool rec = false;
int reg_count = 1;
int address;
uint16_t port = 502;
std::string ip;

void start() {
  modbus_t *ctx = modbus_new_tcp(ip.c_str(), port);
  if (modbus_connect(ctx) == -1) {
    std::cerr << "Connection Failed" << std::endl;
    modbus_free(ctx);
  }

  if (isSigned) {
    std::cout << "value set to signed" << std::endl;
  }

  uint16_t tab_reg[32];
  if (is32)
    reg_count = reg_count * 2;

  do {
    int rc = modbus_read_registers(ctx, address, reg_count, tab_reg);
    if (rc == -1) {
      std::cerr << "Failed to read registers\n";
    } else {
      if (is32) {
        for (int i = 0; i < rc; i += 2) {
          uint32_t out = tab_reg[i] << 16 | tab_reg[i + 1];
          if (isSigned) {
            int32_t sout = (int32_t)out;
            std::cout << "Reg[" << i << "] = " << sout << "\n";
          } else {
            std::cout << "Reg[" << i << "] = " << out << "\n";
          }
        }
      } else {
        for (int i = 0; i < rc; ++i) {
          if (isSigned) {
            std::cout << "Reg[" << i << "] = " << (int16_t)tab_reg[i] << "\n";
          } else {
            std::cout << "Reg[" << i << "] = " << tab_reg[i] << "\n";
          }
        }
      }
    }
    if (rec)
      std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (rec);

  modbus_close(ctx);
  modbus_free(ctx);
}

int main(int argc, char **argv) {
  CLI::App app{"Modbus CLI Tools"};

  // Adding option for the CLI
  app.add_option("ip", ip, "IP address to connect to")->required();
  app.add_option("address", address, "Modbus address to read")->required();
  app.add_option("-c, --count", reg_count, "Register count to read");
	app.add_option("-p, --port", port, "Port number for the socket connection");
  app.add_flag("-d, --double", is32, "Set reading to 32 bit value")
      ->take_last();
  app.add_flag("-s, --signed", isSigned, "Set reading to signed value")
      ->take_last();
  app.add_flag("-r, --recursive", rec, "Set reading continuously")->take_last();

  CLI11_PARSE(app, argc, argv);
  start();
}
