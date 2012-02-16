// Copyright (c) 2012 Roman Neuhauser
// Distributed under the MIT license (see LICENSE file)
// vim: sw=2 sts=2 ts=2 fdm=marker cms=\ //\ %s

#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/format.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::ios;
using std::getline;
using std::ostream;
using std::ofstream;
using std::string;

using namespace boost::gregorian;
using boost::format;
using boost::io::all_error_bits;
using boost::io::too_few_args_bit;
using boost::io::too_many_args_bit;

namespace
{

void
open(ofstream &log, format &fmt, date today)
{
  auto fname = str(fmt % to_iso_extended_string(today)).c_str();
  log.open(fname, ios::app | ios::binary);
}

}

int
main(int argc, char **argv)
{
  string line;
  ofstream log_main;
  ofstream log_fcgid;
  auto fmt_main = format(argc > 1 ? argv[1] : "error");
  fmt_main.exceptions(all_error_bits ^ too_many_args_bit);
  auto fmt_fcgid = format(argc > 2 ? argv[2] : "fcgid");
  fmt_fcgid.exceptions(all_error_bits ^ too_many_args_bit);
  auto today = day_clock::local_day();

  while (1) {
    auto opened_on = today;
    open(log_main, fmt_main, today);
    open(log_fcgid, fmt_fcgid, today);
    while (getline(cin, line)) {
      today = day_clock::local_day();
      if (today > opened_on) {
        log_main.close();
        log_fcgid.close();
        break;
      }
      ostream *os;
      if (line.find(" mod_fcgid: stderr: ") != string::npos)
        os = &log_fcgid;
      else
        os = &log_main;
      *os << line << endl;
    }
  }
  return 0;
}
