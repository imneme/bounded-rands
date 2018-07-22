#!/bin/sh

DOWNLOADER="curl -OL" 
UNZIP=unzip
RM="rm -f"
ECHO=echo
TOUCH=touch

if [ -e .have_gists ]
then
   $ECHO "PRNGs and adapters are downloaded. Use ./remove-gists.sh first."
   exit 1
fi

$ECHO 'Downloading PRNGs and adapters...'

$DOWNLOADER 'https://gist.github.com/imneme/f0fe8877e4deb3f6b9200a17c18bf155/raw//chacha.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/4f2bf4b4f3a221ef051cf108d6b64d5a/raw/arc4.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/6179748664e88ef3c34860f44309fc71/raw/splitmix.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/3eb1bcc5418c4ae83c4c6a86d9cbb1cd/raw/xoshiro.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/aeae7628565f15fb3fef54be8533e39c/raw/lehmer.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/85cff47d4bad8de6bdeb671f9c76c814/raw/jsf.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/7a783e20f71259cc13e219829bcea4ac/raw/gjrand.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/f1f7821f07cf76504a97f6537c818083/raw/sfc.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/9b769cefccac1f2bd728596da3a856dd/raw/xorshift.hpp'
$DOWNLOADER 'https://gist.github.com/imneme/f76f4bb7b7f67ff0850199ab7c077bf7/raw/xoroshiro.hpp'
$DOWNLOADER 'https://github.com/imneme/pcg-cpp/archive/master.zip'
$UNZIP master.zip
$RM master.zip

$TOUCH .have_gists
