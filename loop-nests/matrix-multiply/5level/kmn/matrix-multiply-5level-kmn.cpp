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

#include "whoop.hpp"

//An output-stationary inner product 
int main(int argc, char** argv)
{

  using namespace whoop;
  
  int kTileWidth = 8;
  int kTileHeight = 2;
  int kTileDepth = 4;
  int kMiddleTileNumAcross = 2;
  int kMiddleTileNumDown = 8;
  int kMiddleTileNumBack = 4;
  int kOuterTileNumAcross = 2;
  int kOuterTileNumDown = 2;

  whoop::AddOption(&kTileWidth, "tile_width,w", "Width of the L0 tile.");
  whoop::AddOption(&kTileHeight, "tile_height,h", "Height of the L0 tile.");
  whoop::AddOption(&kTileDepth, "tile_depth,d", "Depth of the L0 tile.");
  whoop::AddOption(&kMiddleTileNumAcross, "middle_num_across,x", "Number of L0 tiles across an L1 tile.");
  whoop::AddOption(&kMiddleTileNumDown, "middle_num_down,y", "Number of L0 tiles down an L1 tile.");
  whoop::AddOption(&kMiddleTileNumBack, "middle_num_back,z", "Number of L0 tiles behind an L1 tile.");
  whoop::AddOption(&kOuterTileNumAcross, "outer_num_across,xx", "Number of L1 tiles across an L2 tile.");
  whoop::AddOption(&kOuterTileNumDown, "outer_num_down,yy", "Number of L1 tiles down an L2 tile.");

  TensorIn  input_a("input_a");
  TensorIn  input_b("input_b");
  TensorOut outputs("outputs");

  whoop::Init(argc, argv);

  int kInputAWidth = input_a.Size(0);  // K
  int kInputBWidth = input_b.Size(0);  // N
  int kInputAHeight = input_a.Size(1); // M
  int kInputBHeight = input_b.Size(1); // K
  
  const int kOutputWidth = kInputBWidth;
  const int kOutputHeight = kInputAHeight;
  outputs.Resize({kOutputHeight, kOutputWidth});

  whoop::T(0) << "Input A Width: "  << kInputAWidth << whoop::EndT;
  whoop::T(0) << "Input A Height: " << kInputAHeight << whoop::EndT;
  whoop::T(0) << "Input B Width: "  << kInputBWidth << whoop::EndT;
  whoop::T(0) << "Input B Height: " << kInputBHeight << whoop::EndT;
  whoop::T(0) << "Output Width: "   << kOutputWidth << whoop::EndT;
  whoop::T(0) << "Output Height: "  << kOutputHeight << whoop::EndT;
  whoop::T(0) << "Inner Tile Width: "   << kTileWidth << whoop::EndT;
  whoop::T(0) << "Inner Tile Height: "  << kTileHeight << whoop::EndT;
  whoop::T(0) << "Inner Tile Depth: "  << kTileDepth << whoop::EndT;
  whoop::T(0) << "Num Inner Tiles Across: "   << kInputBWidth / kTileWidth << whoop::EndT;
  whoop::T(0) << "Num Inner Tiles Down: "  << kInputAHeight / kTileHeight << whoop::EndT;
  whoop::T(0) << "Num Inner Tiles Back: "  << kInputBHeight / kTileDepth << whoop::EndT;
  whoop::T(0) << "Middle Tile Num L0 Across: " << kMiddleTileNumAcross << whoop::EndT;
  whoop::T(0) << "Middle Tile Num L0 Down: "   << kMiddleTileNumDown << whoop::EndT;
  whoop::T(0) << "Middle Tile Num L0 Back: "   << kMiddleTileNumBack << whoop::EndT;
  whoop::T(0) << "Middle Tile Total Width: "   << kMiddleTileNumAcross * kTileWidth << whoop::EndT;
  whoop::T(0) << "Middle Tile Total Height: "  << kMiddleTileNumDown * kTileHeight << whoop::EndT;
  whoop::T(0) << "Middle Tile Total Depth: "   << kMiddleTileNumBack * kTileDepth << whoop::EndT;
  whoop::T(0) << "Num Middle Tiles Across: "   << kInputBWidth / (kMiddleTileNumAcross * kTileWidth) << whoop::EndT;
  whoop::T(0) << "Num Middle Tiles Down: "     << kInputAHeight / (kMiddleTileNumDown * kTileHeight) << whoop::EndT;
  whoop::T(0) << "Num Middle Tiles Back: "     << kInputAWidth / (kMiddleTileNumBack * kTileDepth) << whoop::EndT;
  whoop::T(0) << whoop::EndT;

  whoop::ASSERT(kInputAWidth == kInputBHeight) << "The width of Matrix A must equal the height of Matrix B. A width: " << kInputAWidth << ", B height: " << kInputBHeight << whoop::EndT;
  whoop::ASSERT(kInputBWidth % (kOuterTileNumAcross * kMiddleTileNumAcross * kTileWidth) == 0) << "The width of the tile must equally divide the width of matrix B. B width: " << kInputBWidth << ", tile width: " << kTileWidth << whoop::EndT;
  whoop::ASSERT(kInputAHeight % (kOuterTileNumDown * kMiddleTileNumDown * kTileHeight) == 0) << "The height of the tile must equally divide the height of matrix A. A height: " << kInputAHeight << ", tile height: " << kTileHeight << whoop::EndT;
  whoop::ASSERT(kInputBHeight % (kMiddleTileNumBack * kTileDepth) == 0) << "The depth of the tile must equally divide the height of matrix B (and width of A). B height: " << kInputBHeight << ", tile depth: " << kTileDepth << whoop::EndT;

  // Short-form variable names
  const int M = kInputAHeight;
  const int K = kInputAWidth;
  const int N = kInputBWidth;

  const int M2 = kOuterTileNumDown;
  const int N2 = kOuterTileNumAcross;

  const int M1 = kMiddleTileNumDown;
  const int N1 = kMiddleTileNumAcross;
  const int K1 = kMiddleTileNumBack;
  
  const int M0 = kTileHeight;
  const int K0 = kTileDepth;
  const int N0 = kTileWidth;
  
  const int M3 = M / (M2 * M1 * M0);
  const int K2 = K / (K1 * K0);
  const int N3 = N / (M2 * N1 * N0);

  whoop::T(0) << "====== Buffer Entries ======" << whoop::EndT;
  whoop::T(0) << "L0:" << whoop::EndT;
  whoop::T(0) << "  Input A: 1" << whoop::EndT;
  whoop::T(0) << "  Input B: " << N0 << whoop::EndT;
  whoop::T(0) << "  Output: " << M0*N0 << whoop::EndT;
  whoop::T(0) << "  Total: " << M0*N0+N0+1 << whoop::EndT;
  whoop::T(0) << "L1:" << whoop::EndT;
  whoop::T(0) << "  Input A: " << K0*N0 << whoop::EndT;
  whoop::T(0) << "  Input B: " << N0*K0*K0*M0 << whoop::EndT;
  whoop::T(0) << "  Output: " << M1*M0*N1*N0 << whoop::EndT;
  whoop::T(0) << "  Total: " << K0*N0+N0*K0*K0*M0+M1*M0*N1*N0 << whoop::EndT;
  whoop::T(0) << "L2:" << whoop::EndT;
  whoop::T(0) << "  Input A: " << 512 << whoop::EndT;
  whoop::T(0) << "  Input B: " << 1024 << whoop::EndT;
  //whoop::T(0) << "  Outputs: << XXX << whoop::EndT;
  //whoop::T(0) << "  Total: " << 512+1024+XXX << whoop::EndT;

  Var m("m");
  Var k("k");
  Var n("n");

  Var m3("m3");
  Var n3("n3");
  Var k2("k2");
  Var m2("m2");
  Var n2("n2");
  Var m1("m1");
  Var k1("k1");
  Var n1("n1");
  Var m0("m0");
  Var k0("k0");
  Var n0("n0");

 
  t_for(m3, 0, M3);
  {
    outputs.AddTileLevel(4096);
    t_for(n3, 0, N3);
    {
      t_for(k2, 0, K2);
      {
        input_a.AddTileLevel(512);
        input_b.AddTileLevel(1024);
        // Outputs purposely skipped.
        t_for(m2, 0, M2);
        {
          t_for(n2, 0, N2);
          {
            t_for(k1, 0, K1);
            {
              // Output-stationary, row-major
              input_a.AddTileLevel(K0*N0);
              input_b.AddTileLevel(N0*K0*K0*M0);
              outputs.AddTileLevel(M1*M0*N1*N0);
              // purposely skip outputs.
              t_for(m1, 0, M1);
              {

                t_for(n1, 0, N1);
                {
                  t_for(k0, 0, K0);
                  {
                    // A-stationary, col-major L0.
                    input_a.AddTileLevel(1);
                    input_b.AddTileLevel(N0);
                    outputs.AddTileLevel(M0*N0);

                    k = k2 * K1 * K0 + k1 * K0 + k0;

                    t_for(m0, 0, M0);
                    {
                      m = m3 * M2 * M1 * M0 + m2 * M1 * M0 + m1 * M0 + m0;
                      t_for(n0, 0, N0);
                      {
                        n = n3 * N2 * N1 *N0 + n2 * N1 * N0 + n1 * N0 + n0;
                        outputs[m][n] += input_a[m][k] * input_b[k][n];
                      }
                      end();
                    }
                    end();
                  }
                  end();
                }
                end();
              }
              end();
            }
            end();
          }
          end();
        }
        end();
      }
      end();
    }
    end();
  }
  end();
  
  whoop::T(0) << "RUNNING..." << whoop::EndT;
  whoop::Run();
  whoop::Done();
}
