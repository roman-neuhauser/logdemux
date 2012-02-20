# Copyright (c) 2012 Roman Neuhauser
# Distributed under the MIT license (see LICENSE file)
# vim: sw=2 sts=2 ts=2 et

curdir="$PWD"
testdir="$1"
rm -f "$testdir"/*.actual
cd "$testdir" || exit 1
$SHELL cmd "$LOGDEMUX" >out.actual 2>err.actual
ex=$?
cd "$curdir"
echo $ex > "$testdir"/exit.actual
ex=0
for exp in $(find "$testdir" -name \*.expected | sort); do
  act="${exp%.expected}.actual"
  diff="${exp%.expected}.diff"
  diff -Nu --strip-trailing-cr "$exp" "$act" > "$diff"
  dex=$?
  if test 0 -ne $dex; then
    ex=$dex
  else
    rm -f "$diff"
  fi
done
exit $ex
