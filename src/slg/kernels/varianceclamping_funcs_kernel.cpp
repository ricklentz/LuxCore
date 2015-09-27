#include <string>
namespace slg { namespace ocl {
std::string KernelSource_varianceclamping_funcs = 
"#line 2 \"varianceclamping_funcs.cl\"\n"
"\n"
"/***************************************************************************\n"
" * Copyright 1998-2015 by authors (see AUTHORS.txt)                        *\n"
" *                                                                         *\n"
" *   This file is part of LuxRender.                                       *\n"
" *                                                                         *\n"
" * Licensed under the Apache License, Version 2.0 (the \"License\");         *\n"
" * you may not use this file except in compliance with the License.        *\n"
" * You may obtain a copy of the License at                                 *\n"
" *                                                                         *\n"
" *     http://www.apache.org/licenses/LICENSE-2.0                          *\n"
" *                                                                         *\n"
" * Unless required by applicable law or agreed to in writing, software     *\n"
" * distributed under the License is distributed on an \"AS IS\" BASIS,       *\n"
" * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*\n"
" * See the License for the specific language governing permissions and     *\n"
" * limitations under the License.                                          *\n"
" ***************************************************************************/\n"
"\n"
"float3 VarianceClamping_GetWeightedFloat4(__global float *src) {\n"
"	const float4 val = VLOAD4F_Align(src);\n"
"\n"
"	if (val.w > 0.f) {\n"
"		const float k = 1.f / val.w;\n"
"		\n"
"		return ((float3)(val.x, val.y, val.z)) * k;\n"
"	} else\n"
"		return 0.f;\n"
"}\n"
"\n"
"void VarianceClamping_Clamp(__global SampleResult *sampleResult, const float sqrtVarianceClampMaxValue\n"
"	FILM_PARAM_DECL) {\n"
"	// Recover the current pixel value\n"
"	const int x = Floor2Int(sampleResult->filmX);\n"
"	const int y = Floor2Int(sampleResult->filmY);\n"
"	const uint index1 = x + y * filmWidth;\n"
"	const uint index4 = index1 * 4;\n"
"\n"
"	float3 expectedValue = 0.f;\n"
"\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_0)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[0])[index4]));\n"
"#endif\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_1)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[1])[index4]));\n"
"#endif\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_2)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[2])[index4]));\n"
"#endif\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_3)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[3])[index4]));\n"
"#endif\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_4)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[4])[index4]));\n"
"#endif\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_5)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[5])[index4]));\n"
"#endif\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_6)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[6]])[index4]));\n"
"#endif\n"
"#if defined(PARAM_FILM_RADIANCE_GROUP_7)\n"
"	expectedValue += VarianceClamping_GetWeightedFloat4(&((filmRadianceGroup[7])[index4]));\n"
"#endif\n"
"\n"
"	// Use the current pixel value as expected value\n"
"	const float minExpectedValue = fmin(expectedValue.x, fmin(expectedValue.y, expectedValue.z));\n"
"	const float maxExpectedValue = fmax(expectedValue.x, fmax(expectedValue.y, expectedValue.z));\n"
"	SampleResult_ClampRadiance(sampleResult,\n"
"			fmax(minExpectedValue - sqrtVarianceClampMaxValue, 0.f),\n"
"			maxExpectedValue + sqrtVarianceClampMaxValue);\n"
"}\n"
; } }
