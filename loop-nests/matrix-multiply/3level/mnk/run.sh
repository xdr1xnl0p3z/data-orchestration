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

executable=./matrix-multiply-3level-mnk.bin

if [ $# -eq 2 ]; then
    afile=$1
    bfile=$2
    tileh=2
    tiled=4
    tilew=8
    outerz=4
    outerx=2
elif [ $# -eq 7 ]; then
    afile=$1
    bfile=$2
    tileh=$3
    tiled=$4
    tilew=$5
    outerz=$6
    outerx=$7
else
    afile=B
    bfile=B
    tileh=2
    tiled=4
    tilew=8
    outerz=4
    outerx=2
fi

# Run the program with some interesting (and legal) default settings
${executable} \
  --tensor_input_a_file=../../data-sets/input_a_${afile}.in.txt \
  --tensor_input_b_file=../../data-sets/input_b_${bfile}.in.txt \
  --tile_height=$tileh \
  --tile_depth=$tiled \
  --tile_width=$tilew \
  --outer_num_back=$outerz \
  --outer_num_across=$outerx \
  --ref_outputs_file=../../data-sets/outputs_${afile}_${bfile}.ref.txt \
  --tensor_outputs_file=outputs_${afile}_${bfile}.out.txt
