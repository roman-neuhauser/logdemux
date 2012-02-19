// Copyright (c) 2012 Roman Neuhauser
// Distributed under the MIT license (see LICENSE file)
// vim: sw=2 sts=2 ts=2 fdm=marker cms=\ //\ %s

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>

#define EX_OK           0       /* successful termination */
#define EX_USAGE        64      /* command line usage error */
#define EX_DATAERR      65      /* data format error */
#define EX_NOINPUT      66      /* cannot open input */

#include "boost/foreach.hpp"
#include "boost/format.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/regex.hpp"
#include "boost/shared_ptr.hpp"

#include "iniphile/input.hpp"
#include "iniphile/ast.hpp"
#include "iniphile/output.hpp"

#define foreach BOOST_FOREACH

using std::cin;
using std::cerr;
using std::endl;
using std::ios;
using std::getline;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

using boost::format;

using namespace boost::gregorian;

using boost::regex;
using boost::shared_ptr;

namespace
{

class rule
{
public:
  rule(string const &prefix, date const &now, string const &match, string const &sink, bool final) // {{{
  : sink(sink)
  , prefix(prefix)
  , final(final)
  , pat(regex(match, regex::perl))
  , opened_on(now)
  , os(expand(sink, now).c_str(), ios::app | ios::binary)
  {
  } // }}}
  bool
  handle(date const &now, string const &line) // {{{
  {
    if (!regex_search(line, pat))
      return false;
    if (opened_on < now)
      reopen(os, now);
    os << line << endl;
    return final;
  } // }}}
private:
  string const &sink;
  string const &prefix;
  bool final;
  regex pat;
  date const &opened_on;
  ofstream os;

  void
  reopen(ofstream &os, date const &now) // {{{
  {
    os.close();
    os.open(expand(sink, now).c_str(), ios::app | ios::binary);
  } // }}}
  string
  expand(string fmt, date const &d) // {{{
  {
    fmt = regex_replace(fmt, regex("%D"), to_iso_extended_string(d));
    fmt = regex_replace(fmt, regex("%P"), prefix);
    return fmt;
  } // }}}
};

template<class Ini = iniphile::ast::node>
class ruleset
{
public:
  ruleset(Ini const &ini, string const &prefix, date const &now)
  : ini(ini)
  , prefix(prefix)
  {
    foreach (auto &rname, iniphile::get(ini, "rules.order", vector<string>()))
      create_rule(rname, now);
  }
  void
  handle(date const &now, string const &line) // {{{
  {
    foreach (shared_ptr<rule> &r, rules)
      if (r->handle(now, line))
        break;
  } // }}}
private:
  Ini const &ini;
  string const &prefix;
  vector<shared_ptr<rule>> rules;

  void
  create_rule(string const &rname, date const &now) // {{{
  {
    auto match = iniphile::get(ini, rname + ".match", string(""));
    auto sink = iniphile::get(ini, rname + ".sink", string(""));
    auto final = iniphile::get(ini, rname + ".final", false);
    rules.push_back(shared_ptr<rule>(new rule(prefix, now, match, sink, final)));
  } // }}}
};

date
today() // {{{
{
  return day_clock::local_day();
} // }}}

template<class Fmt>
int
complain(int exitcode, Fmt msg)
{
  cerr << msg << endl;
  return exitcode;
}

}

int
main(int argc, char **argv)
{
  string self = argc > 0 ? argv[0] : "logdemux";
  string bself = self.substr(self.find_last_of("\\/") + 1);

  if (argc < 3)
    return complain(
      EX_USAGE
    , format("usage: %1% rules prefix") % bself
    );

  string ini(argv[1]);
  string prefix(argv[2]);
  ifstream sini;
  sini.open(ini);
  if (sini.fail())
    return complain(
      EX_NOINPUT
    , format("%1%: rules file '%2%' missing") % bself % ini
    );

  auto cfg = iniphile::parse(sini, cerr);
  if (!cfg)
    return complain(
      EX_DATAERR
    , format("%1%: rules file '%2%' broken") % bself % ini
    );

  ruleset<> rules(iniphile::normalize(*cfg), prefix, today());

  string line;
  while (cin.good()) {
    while (getline(cin, line)) {
      rules.handle(today(), line);
    }
  }

  return EX_OK;
}
