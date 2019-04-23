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
scons -u -Q

executable=./conv6d-tiled-2dk1pk0r.bin

if [ $# -eq 1 ]; then
    infile=$1
	tlevel=0
	ktileSz=2
elif [ $# -eq 2 ]; then
    infile=$1
	tlevel=$2
	ktileSz=2
elif [ $# -eq 3 ]; then
    infile=$1
	tlevel=$2
	ktileSz=$3
else
    infile=A
	tlevel=0
	ktileSz=2
fi


# Run the program with some interesting (and legal) default settings
WHOOP_CHECK_REFERENCE=1 ${executable} \
  --vec_inputs_file=../../data-sets/inputs_2D${infile}.in.txt \
  --vec_weights_file=../../data-sets/weights_2D${infile}.in.txt \
  --vec_dimensions_file=../../data-sets/dimensions_2D${infile}.in.txt \
  --ref_outputs_file=../../data-sets/outputs_2D${infile}.ref.txt \
  --vec_outputs_file=./outputs_2D${infile}.out.txt \
  --ktile_size=${ktileSz} \
  --trace_level=${tlevel}

