// Copyright (c) 2012 Roman Neuhauser
// Distributed under the MIT license (see LICENSE file)
// vim: sw=2 sts=2 ts=2 fdm=marker cms=\ //\ %s

// shared_ptr
#include <memory>

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

#include "boost/ref.hpp"
#include "boost/foreach.hpp"
#include "boost/format.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/regex.hpp"

#include "iniphile/input.hpp"
#include "iniphile/ast.hpp"
#include "iniphile/output.hpp"

#define foreach BOOST_FOREACH

using std::shared_ptr;

using std::cin;
using std::cerr;
using std::endl;
using std::ios;
using std::getline;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

using boost::cref;
using boost::format;
using namespace boost::gregorian;
using boost::regex;
using boost::smatch;

namespace
{

struct expand_sink // {{{
{
  expand_sink(string const &prefix, date const &today)
  : prefix(prefix)
  , today(today)
  {}
private:
  string const &prefix;
  date const &today;
public:
  string
  operator()(smatch const &expando) const
  {
    auto sexpando = expando.str();
    if (sexpando == "%D") return to_iso_extended_string(today);
    if (sexpando == "%P") return prefix;
    return "LOGDEMUX-BUG";
  }
}; // }}}

class rule
// {{{
{
public:
  rule(string const &prefix, string const &match, string const &sink, bool final, ostream &diag) // {{{
  : sink(sink)
  , prefix(prefix)
  , final(final)
  , pat(match, regex::perl)
  , opened_on()
  , os()
  , diag(diag)
  {
  } // }}}
  bool
  handle(date const &today, string const &line) // {{{
  {
    if (!regex_search(line, pat))
      return false;
    if (opened_on != today)
      reopen(today);
    os << line << endl;
    return final;
  } // }}}
private:
  string const sink;
  string const prefix;
  bool final;
  regex pat;
  date opened_on;
  ofstream os;
  ostream &diag;

  void
  reopen(date const &today) // {{{
  {
    if (os.is_open()) os.close();
    os.clear();
    auto fname = expand(sink, today);
    os.open(fname.c_str(), ios::app | ios::binary);
    if (os.fail())
      diag << "failed to open " << fname << endl;
    opened_on = today;
  } // }}}
  string
  expand(string const &fmt, date const &d) const // {{{
  {
    return regex_replace(
      fmt
    , regex("%[DP]\\>")
    , cref(expand_sink(prefix, d))
    );
  } // }}}
}; // }}}

class ruleset
// {{{
{
public:
  ruleset(iniphile::ast::node const &ini, string const &prefix, ostream &diag) // {{{
  {
    foreach (auto const &rname, iniphile::get(ini, "rules.order", vector<string>()))
      rules.push_back(shared_ptr<rule>(new rule(
        prefix
      , iniphile::get(ini, rname + ".match", string("")) 
      , iniphile::get(ini, rname + ".sink", string(""))  
      , iniphile::get(ini, rname + ".final", false)      
      , diag
      )));
  } // }}}
  void
  handle(date const &now, string const &line) // {{{
  {
    foreach (auto &r, rules)
      if (r->handle(now, line))
        break;
  } // }}}
private:
  vector<shared_ptr<rule>> rules;
}; // }}}

date
today() // {{{
{
  return day_clock::local_day();
} // }}}

template<class Fmt>
int
complain(int exitcode, Fmt msg) // {{{
{
  cerr << msg << endl;
  return exitcode;
} // }}}

string
basename(string const &path) // {{{
{
  return regex_replace(
    path
  , regex("^(.*/)?([^/]+)(.exe)?$", regex::perl)
  , "$2"
  );
} // }}}

}

int
main(int argc, char **argv)
{
  string self = argc ? basename(argv[0]) : "logdemux";

  if (argc < 3)
    return complain(
      EX_USAGE
    , format("usage: %1% rules prefix") % self
    );

  string inipath = argv[1];
  string prefix = argv[2];
  ifstream input;
  input.open(inipath);
  if (input.fail())
    return complain(
      EX_NOINPUT
    , format("%1%: rules file '%2%' missing") % self % inipath
    );

  auto ini = iniphile::parse(inipath, input, cerr);
  input.close();
  if (!ini)
    return complain(
      EX_DATAERR
    , format("%1%: rules file '%2%' broken") % self % inipath
    );

  auto cfg(iniphile::normalize(*ini));

  if (iniphile::get(cfg, "rules.order", string("")) == "")
    return complain(
      EX_DATAERR
    , format("%1%: no rules.order in '%2%'") % self % inipath
    );

  ruleset rules(cfg, prefix, cerr);

  string line;
  while (cin.good()) {
    while (getline(cin, line)) {
      rules.handle(today(), line);
    }
  }

  return EX_OK;
}
