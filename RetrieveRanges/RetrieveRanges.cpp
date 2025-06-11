/*
**                 Retrieve Ranges v1.1 - (January 17, 2006)
**
**   This tool parses a v1 mkv timecodes file and retrieves the frame ranges
**   in the original (undecimated) video stream that correspond to each
**   fps range in the timecode file.  NOTE:  this works by assuming all framerate
**   differences to the original are due only to decimation (removal of frames),
**   if this is not the case then this programs output will be useless.
**
**   Copyright (C) 2005-2006 Kevin Stone
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with this program; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
** USAGE INFORMATION -
**
** BIG NOTE:  this program works by assuming framerate differences from the original
**            stream are due only to frame removal/decimation!  So if they are
**            due to something else this will not work correctly.
**
** SYNTAX:  retrieveranges tc.txt original_fps end_frame [out_fps] >outfile.txt
**
** PARAMETERS:  tc.txt       = Timecode file to parse (must be v1 format).
**              original_fps = Rate of original video (e.g. 29.970, 59.940, etc...).
**              end_frame    = Frame to stop range output on. (from # from original file)
**              out_fps      = This is optional, if given, only ranges that match
**                                this fps will be output instead of all ranges. For
**                                example if you only want 29.970 ranges or 23.976 ranges.
*/
#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <string.h>
#else
#include <cstring> 
#define _strnicmp strncasecmp
#endif

bool is_safe_path(const char* path) {
    //Path traversal prevention check
    return strstr(path, "..") == NULL && path[0] != '/' && strchr(path, '\\') == NULL;
}

int main(int argc, char* argv[])
{
    if (argc < 4 || argc > 5)
    {
        printf("Syntax => retrieveranges timecodefile.txt orig_fps end_frame [out_fps] >outfile.txt\n");
        return 1;
    }

    //If path traversal is detected, don't go ahead
    if (!is_safe_path(argv[1])) {
        printf("Error: unsafe file path detected (%s)!\n", argv[1]);
        printf("Quitting...\n");
        return 2;
    }

    FILE* inFile = fopen(argv[1], "r");
    if (inFile == NULL)
    {
        printf("Error: unable to open timecodes file (%s)!\n", argv[1]);
        printf("Quitting...\n");
        return 2;
    }

    double orig_fps = atof(argv[2]);
    int end_frame = atoi(argv[3]);
    double out_fps = 0.0;
    if (argc == 5)
        out_fps = atof(argv[4]);

    std::vector<std::pair<int, int>> ranges;
    int start_frame = 0;
    int end_frame_range = 0;
    double fps = 0.0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), inFile))
    {
        if (sscanf(buffer, "%lf,%d,%d", &fps, &start_frame, &end_frame_range) == 3)
        {
            if (argc == 5)
            {
                if (fabs(fps - out_fps) < DBL_EPSILON)
                    ranges.push_back(std::make_pair(start_frame, end_frame_range));
            }
            else
            {
                ranges.push_back(std::make_pair(start_frame, end_frame_range));
            }
        }
    }

    fclose(inFile);

    for (const auto& range : ranges)
    {
        printf("%d,%d\n", range.first, range.second);
    }

    return 0;
}
