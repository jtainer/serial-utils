// 
// Serial Port Utils
//
// Copyright (c) 2024 Jonathan Tainer. Subject to the BSD 2-Clause License.
//

int serial_open(const char* modem_dev);
void serial_close();
const char* serial_getline();
