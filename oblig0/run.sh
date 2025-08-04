#!/usr/bin/env bash

cd "$(dirname "$0")"

if [ -f bin/debug/wordfreq ]; then
	echo "Running wordfreq debug with args: data/oxford_dict.txt min_wc=1 min_wl=1 lim_n_results=20"
	bin/debug/wordfreq data/oxford_dict.txt 1 1 20
elif [ -f bin/release/wordfreq ]; then
	echo "Running wordfreq release with args: data/oxford_dict.txt min_wc=1 min_wl=1 lim_n_results=20"
	bin/release/wordfreq data/oxford_dict.txt 1 1 20
else 
	echo "No executable found in bin/release or bin/debug\nRun make to build the executables\n"
fi

