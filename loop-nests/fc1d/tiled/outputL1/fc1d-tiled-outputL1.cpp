/* Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "assert.h"

#include "whoop.hpp"

int main(int argc, char** argv)
{

  using namespace whoop;

  VecIn  inputs("inputs");
  VecIn  weights("weights");
  VecOut outputs("outputs");

  int kWeightWidthL0 = 4;
 
  whoop::AddOption(&kWeightWidthL0, "weight_width_L0", "Length of L0 weight tile."); 

  whoop::Init(argc, argv);

  int kInputWidth = inputs.Size();
  int kWeightWidth = weights.Size();

  assert(kInputWidth == kWeightWidth);
  
  const int kOutputWidth = 1;
  outputs.Resize(kOutputWidth);

  if(kWeightWidth % kWeightWidthL0 != 0)
  {
    std::cout << "[fc1d-tiled-output-stationary] Warning: not appropriate tile size. Removing tile effect" << std::endl;
    kWeightWidthL0 = kWeightWidth;
  }
  assert(kWeightWidth % kWeightWidthL0 == 0);


  const int kWeightWidthL1 = kWeightWidth / kWeightWidthL0;
  

  Var w1("w1");
  Var w0("w0");

  t_for(w1, 0, kWeightWidthL1);
  {
    inputs.AddTileLevel(kWeightWidthL0, 1);
    weights.AddTileLevel(kWeightWidthL0);
    outputs.AddTileLevel(1);
  
    t_for(w0, 0, kWeightWidthL0);
    {
      outputs[0] += inputs[w1*kWeightWidthL0 +w0] * weights[w1*kWeightWidthL0 +w0];
    }
    end();
  }
  end();

  std::cout << "RUNNING..." << std::endl;
  whoop::Run();
  std::cout << "DONE." << std::endl;
  for (int x = 0; x < kInputWidth; x++)
  {
    whoop::T(2) << "I " << x << " = " << inputs.At(x) << whoop::EndT;
  }
  for (int x = 0; x < kWeightWidth; x++)
  {
    whoop::T(2) << "W " << x << " = " << weights.At(x) << whoop::EndT;
  }
  for (int x = 0; x < kOutputWidth; x++)
  {
    whoop::T(2) << "O " << x << " = " << outputs.At(x) << whoop::EndT;
  }

  whoop::Done();  
}
