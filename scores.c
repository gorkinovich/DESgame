//****************************************************************************************************
// Copyright (c) 2015 Gorka Suárez García, Dan Cristian Rotaru
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//****************************************************************************************************

#include "game.h"
#include "at24c04.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//****************************************************************************************************
// Data
//****************************************************************************************************

extern GameData game_data;

//****************************************************************************************************
// Scores
//****************************************************************************************************

void InitializeScores() {
    INT8 i;
    for (i = 0; i < MAX_SCORES; ++i) {
        game_data.scores[i] = 0;
    }
    LoadScores();
}

//----------------------------------------------------------------------------------------------------

void AddScore(UINT32 score) {
    INT8 i, j;
    for (i = 0; i < MAX_SCORES; ++i) {
        if (score > game_data.scores[i]) {
            for (j = MAX_SCORES - 1; j > i; --j) {
                game_data.scores[j] = game_data.scores[j - 1];
            }
            game_data.scores[i] = score;
            break;
        }
    }
    SaveScores();
}

//----------------------------------------------------------------------------------------------------

void ResetScores() {
    UINT16 i;
    for (i = 0; i < AT24C04_DEPTH; ++i) {
        //at24c04_bytewrite(i, 0);
    }
}

//----------------------------------------------------------------------------------------------------

void SaveScores() {
    UINT16 i, j, k = 0;
    for (i = 0; i < MAX_SCORES; ++i) {
        UINT32 value = game_data.scores[i];
        UINT8 * bytes = (UINT8 *)&value;
        for (j = 0; j < sizeof(UINT32); ++j, ++k) {
            //at24c04_bytewrite(k, bytes[j]);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void LoadScores() {
    UINT16 i, j, k = 0;
    for (i = 0; i < MAX_SCORES; ++i) {
        UINT32 value = 0;
        UINT8 * bytes = (UINT8 *)&value;
        for (j = 0; j < sizeof(UINT32); ++j, ++k) {
            //at24c04_byteread(k, &bytes[j]);
        }
        game_data.scores[i] = value;
    }
}
