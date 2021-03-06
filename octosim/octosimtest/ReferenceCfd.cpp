/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ReferenceCfd.h"

const unsigned int nb_points = 101;

static double percentile[nb_points] = { 0.00, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10,
0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20, 0.21, 0.22, 0.23, 0.24, 0.25, 
0.26, 0.27, 0.28, 0.29, 0.30, 0.31, 0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.40, 
0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.50, 0.51, 0.52, 0.53, 0.54, 0.55, 
0.56, 0.57, 0.58, 0.59, 0.60, 0.61, 0.62, 0.63, 0.64, 0.65, 0.66, 0.67, 0.68, 0.69, 0.70, 
0.71, 0.72, 0.73, 0.74, 0.75, 0.76, 0.77, 0.78, 0.79, 0.80, 0.81, 0.82, 0.83, 0.84, 0.85,
0.86, 0.87, 0.88, 0.89, 0.90, 0.91, 0.92, 0.93, 0.94, 0.95, 0.96, 0.97, 0.98, 0.99, 1.00 };

static unsigned long long arrival[nb_points] = { 1, 1, 1, 1, 28, 66, 143, 216, 435, 506, 602, 685, 912,
996, 1128, 1274, 1300, 1314, 1331, 1355, 1366, 1371, 1391, 1434, 1516, 1576, 1606, 1625, 1673, 
1692, 1778, 1833, 1870, 1934, 1979, 2088, 2161, 2240, 2350, 2524, 2728, 2809, 3059, 3444, 3935, 
4169, 4861, 5496, 5880, 6885, 7782, 8199, 9067, 9953, 11018, 12837, 14551, 15917, 17114, 18981, 
20098, 21172, 22670, 25035, 26413, 27544, 28325, 30150, 31454, 32357, 33725, 35087, 35629, 
37107, 38655, 40348, 41298, 42584, 43659, 46946, 48734, 51875, 54422, 60161, 69503, 80779, 
92872, 109066, 127307, 163727, 218107, 287281, 359721, 590182, 769649, 938783, 1476172, 
2329637, 6463161, 13305491, 59971052 };

static unsigned long long authoritative[nb_points] = { 198, 681, 1467, 1733, 1829, 2243, 2280, 2700,
2990, 3120, 3248, 3461, 3724, 3958, 4137, 4286, 4410, 4541, 4682, 4939, 5196, 5374, 5590, 
5848, 6056, 6211, 6456, 6662, 7022, 7243, 7489, 7739, 8163, 8439, 8760, 8796, 9399, 9636, 
9826, 10006, 10417, 10780, 11728, 12345, 12822, 13577, 13723, 14409, 14810, 15257, 15374, 
15713, 15906, 16132, 16648, 17059, 17876, 18050, 18355, 19438, 20171, 20537, 21845, 22524, 
23654, 25151, 26441, 27053, 27950, 28816, 29672, 31772, 33306, 34677, 35625, 36693, 37772, 
38890, 41933, 42454, 43524, 44281, 45313, 46578, 47522, 48435, 51186, 52446, 55885, 57837, 
59010, 61859, 67093, 71425, 76107, 80952, 91697, 100938, 140218, 167164, 234898 };

static unsigned long long authoritative_cached[nb_points] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 762, 1661, 1830, 2272,
2732, 3084, 3369, 3706, 4027, 4243, 4468, 4630, 5106, 5355, 5634, 6035, 6327, 6652, 7077, 7494,
7976, 8451, 8790, 9424, 9774, 10029, 10680, 11797, 12745, 13579, 14311, 15179, 15343, 15854,
16113, 16816, 17827, 18232, 19407, 20380, 21816, 23210, 25159, 26584, 28028, 29466, 32008,
33771, 35881, 37418, 39171, 42268, 43540, 45199, 46695, 48272, 51667, 55547, 58449, 61747,
69043, 75332, 82685, 100794, 159947, 234898 };

static unsigned int queryLength[nb_points] = { 66, 67, 67, 69, 70, 70, 70, 71, 71, 71, 71, 72, 72, 72,
73, 73, 73, 73, 73, 74, 74, 74, 74, 74, 74, 75, 75, 75, 75, 75, 75, 76, 76, 76, 76, 76, 76, 
76, 77, 77, 77, 77, 77, 77, 77, 77, 77, 78, 78, 78, 78, 78, 78, 78, 78, 79, 79, 79, 79, 79, 
80, 80, 80, 80, 80, 81, 81, 81, 81, 81, 82, 82, 82, 82, 82, 83, 83, 83, 83, 83, 84, 84, 85, 
85, 85, 85, 85, 86, 86, 86, 87, 87, 88, 89, 89, 90, 93, 93, 111, 115, 124, };

unsigned int responseLength[nb_points] = { 74, 79, 83, 86, 87, 87, 88, 90, 90, 91, 92, 92, 92, 93, 93,
94, 95, 95, 96, 97, 97, 98, 98, 99, 99, 99, 100, 101, 101, 102, 103, 104, 105, 105, 106, 106, 
108, 109, 109, 109, 110, 110, 110, 111, 112, 115, 116, 118, 119, 121, 122, 127, 128, 130, 131, 
132, 133, 134, 134, 135, 135, 137, 138, 138, 139, 140, 141, 141, 142, 143, 143, 143, 144, 145, 
146, 150, 152, 155, 157, 162, 163, 164, 167, 170, 171, 173, 176, 178, 184, 189, 192, 197, 201, 
204, 209, 211, 213, 217, 236, 267, 365, };

ReferenceCfd::ReferenceCfd()
{
}


ReferenceCfd::~ReferenceCfd()
{
}

const unsigned int ReferenceCfd::NbPoints()
{
    return nb_points;
}

const double * ReferenceCfd::Proba()
{
    return percentile;
}

const unsigned long long * ReferenceCfd::Arrival()
{
    return arrival;
}

const unsigned long long * ReferenceCfd::Authoritative()
{
    return authoritative_cached; /* authoritative; */
}

const unsigned int * ReferenceCfd::QueryLength()
{
    return queryLength;
}

const unsigned int * ReferenceCfd::ResponseLength()
{
    return responseLength;
}
