#!/bin/sh

ECHO=echo
RM="rm -f"

if [ ! -e .have_gists ]
then
   $ECHO "PRNGs and adapters aren't downloaded."
   exit 1
fi

$ECHO 'Removing PRNGs and adapters...'

$RM arc4.hpp chacha.hpp gjrand.hpp jsf.hpp lehmer.hpp sfc.hpp splitmix.hpp xoroshiro.hpp xorshift.hpp xoshiro.hpp
$RM -r pcg-cpp-master

$RM .have_gists
