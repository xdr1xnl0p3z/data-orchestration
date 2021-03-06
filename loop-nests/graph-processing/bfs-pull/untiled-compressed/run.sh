#!/bin/sh
# Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Stop on errors
set -e
# build the executable
if [ -z "$NOCOMPILE" ]; then
  echo "Compiling Binary"
  scons -u -Q
fi

#defaults
graph=A
tlevel=0
statfile="stats.txt"

if [ $# -eq 1 ]; then
    graph=$1
fi

if [ $# -eq 2 ]; then
    graph=$1
    statfile=$2
fi

if [ $# -eq 3 ]; then
    graph=$1
    statfile=$2
    tlevel=$3
fi

input_dir=../../data-sets/input/
out_dir=../../data-sets/output/

# Run the program with some interesting (and legal) default settings
WHOOP_CHECK_REFERENCE=0 ./bfs-pull-untiled-sparserow.bin \
  --vec_in-offsets_file=$input_dir/$graph.in-offsets \
  --vec_sources_file=$input_dir/$graph.sources \
  --vec_parent_file=$out_dir/bfs-pull.output.`basename $graph`.untiled.sparserow.file \
  --stats=$statfile \
  --trace_level=${tlevel}
