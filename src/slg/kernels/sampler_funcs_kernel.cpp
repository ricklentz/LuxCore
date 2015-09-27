#include <string>
namespace slg { namespace ocl {
std::string KernelSource_sampler_funcs = 
"#line 2 \"sampler_funcs.cl\"\n"
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
"//------------------------------------------------------------------------------\n"
"// Random Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 0)\n"
"\n"
"#define Sampler_GetSamplePath(index) (Rnd_FloatValue(seed))\n"
"#define Sampler_GetSamplePathVertex(depth, index) (Rnd_FloatValue(seed))\n"
"\n"
"__global float *Sampler_GetSampleData(__global Sample *sample, __global float *samplesData) {\n"
"	const size_t gid = get_global_id(0);\n"
"	return &samplesData[gid * TOTAL_U_SIZE];\n"
"}\n"
"\n"
"__global float *Sampler_GetSampleDataPathBase(__global Sample *sample, __global float *sampleData) {\n"
"	return sampleData;\n"
"}\n"
"\n"
"__global float *Sampler_GetSampleDataPathVertex(__global Sample *sample,\n"
"		__global float *sampleDataPathBase, const uint depth) {\n"
"	return &sampleDataPathBase[IDX_BSDF_OFFSET + depth * VERTEX_SAMPLE_SIZE];\n"
"}\n"
"\n"
"void Sampler_SplatSample(\n"
"		Seed *seed,\n"
"		__global Sample *sample,\n"
"		__global float *sampleData\n"
"		FILM_PARAM_DECL\n"
"		) {\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"	Film_AddSample(sample->result.pixelX, sample->result.pixelY,\n"
"			&sample->result, 1.f\n"
"			FILM_PARAM);\n"
"#else\n"
"	Film_SplatSample(&sample->result, 1.f\n"
"			FILM_PARAM);\n"
"#endif\n"
"}\n"
"\n"
"void Sampler_NextSample(\n"
"		Seed *seed,\n"
"		__global Sample *sample,\n"
"		__global float *sampleData,\n"
"		const uint filmWidth, const uint filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"		, __global float *pixelFilterDistribution\n"
"#endif\n"
"		) {\n"
"	// Move to the next sample\n"
"	const float u0 = Rnd_FloatValue(seed);\n"
"	const float u1 = Rnd_FloatValue(seed);\n"
"\n"
"	// TODO: remove sampleData[]\n"
"	sampleData[IDX_SCREEN_X] = u0;\n"
"	sampleData[IDX_SCREEN_Y] = u1;\n"
"\n"
"	SampleResult_Init(&sample->result);\n"
"\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"	const float ux = u0 * filmWidth;\n"
"	const float uy = u1 * filmHeight;\n"
"\n"
"	const uint pixelX = min((uint)ux, (uint)(filmWidth - 1));\n"
"	const uint pixelY = min((uint)uy, (uint)(filmHeight - 1));\n"
"	sample->result.pixelX = pixelX;\n"
"	sample->result.pixelY = pixelY;\n"
"\n"
"	float uSubPixelX = ux - pixelX;\n"
"	float uSubPixelY = uy - pixelY;\n"
"\n"
"	// Sample according the pixel filter distribution\n"
"	float distX, distY;\n"
"	FilterDistribution_SampleContinuous(pixelFilterDistribution, uSubPixelX, uSubPixelY, &distX, &distY);\n"
"\n"
"	sample->result.filmX = pixelX + .5f + distX;\n"
"	sample->result.filmY = pixelY + .5f + distY;\n"
"#else\n"
"	sample->result.filmX = min(u0 * filmWidth, (float)(filmWidth - 1));\n"
"	sample->result.filmY = min(u1 * filmHeight, (float)(filmHeight - 1));\n"
"#endif\n"
"}\n"
"\n"
"void Sampler_Init(Seed *seed, __global Sample *sample, __global float *sampleData,\n"
"		const uint filmWidth, const uint filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"		, __global float *pixelFilterDistribution\n"
"#endif\n"
"		) {\n"
"	Sampler_NextSample(seed, sample, sampleData, filmWidth, filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"			, pixelFilterDistribution\n"
"#endif\n"
"			);\n"
"}\n"
"\n"
"#endif\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Metropolis Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 1)\n"
"\n"
"#define Sampler_GetSamplePath(index) (sampleDataPathBase[index])\n"
"#define Sampler_GetSamplePathVertex(depth, index) (sampleDataPathVertexBase[index])\n"
"\n"
"__global float *Sampler_GetSampleData(__global Sample *sample, __global float *samplesData) {\n"
"	const size_t gid = get_global_id(0);\n"
"	return &samplesData[gid * (2 * TOTAL_U_SIZE)];\n"
"}\n"
"\n"
"__global float *Sampler_GetSampleDataPathBase(__global Sample *sample, __global float *sampleData) {\n"
"	return &sampleData[sample->proposed * TOTAL_U_SIZE];\n"
"}\n"
"\n"
"__global float *Sampler_GetSampleDataPathVertex(__global Sample *sample,\n"
"		__global float *sampleDataPathBase, const uint depth) {\n"
"	return &sampleDataPathBase[IDX_BSDF_OFFSET + depth * VERTEX_SAMPLE_SIZE];\n"
"}\n"
"\n"
"void LargeStep(Seed *seed, __global float *proposedU) {\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"		proposedU[i] = Rnd_FloatValue(seed);\n"
"}\n"
"\n"
"float Mutate(Seed *seed, const float x) {\n"
"	const float s1 = 1.f / 512.f;\n"
"	const float s2 = 1.f / 16.f;\n"
"\n"
"	const float randomValue = Rnd_FloatValue(seed);\n"
"\n"
"	const float dx = s1 / (s1 / s2 + fabs(2.f * randomValue - 1.f)) -\n"
"		s1 / (s1 / s2 + 1.f);\n"
"\n"
"	float mutatedX = x;\n"
"	if (randomValue < 0.5f) {\n"
"		mutatedX += dx;\n"
"		mutatedX = (mutatedX < 1.f) ? mutatedX : (mutatedX - 1.f);\n"
"	} else {\n"
"		mutatedX -= dx;\n"
"		mutatedX = (mutatedX < 0.f) ? (mutatedX + 1.f) : mutatedX;\n"
"	}\n"
"\n"
"	return mutatedX;\n"
"}\n"
"\n"
"float MutateScaled(Seed *seed, const float x, const float range) {\n"
"	const float s1 = 32.f;\n"
"\n"
"	const float randomValue = Rnd_FloatValue(seed);\n"
"\n"
"	const float dx = range / (s1 / (1.f + s1) + (s1 * s1) / (1.f + s1) *\n"
"		fabs(2.f * randomValue - 1.f)) - range / s1;\n"
"\n"
"	float mutatedX = x;\n"
"	if (randomValue < 0.5f) {\n"
"		mutatedX += dx;\n"
"		mutatedX = (mutatedX < 1.f) ? mutatedX : (mutatedX - 1.f);\n"
"	} else {\n"
"		mutatedX -= dx;\n"
"		mutatedX = (mutatedX < 0.f) ? (mutatedX + 1.f) : mutatedX;\n"
"	}\n"
"\n"
"	return mutatedX;\n"
"}\n"
"\n"
"void SmallStep(Seed *seed, __global float *currentU, __global float *proposedU) {\n"
"	proposedU[IDX_SCREEN_X] = MutateScaled(seed, currentU[IDX_SCREEN_X],\n"
"			PARAM_SAMPLER_METROPOLIS_IMAGE_MUTATION_RANGE);\n"
"	proposedU[IDX_SCREEN_Y] = MutateScaled(seed, currentU[IDX_SCREEN_Y],\n"
"			PARAM_SAMPLER_METROPOLIS_IMAGE_MUTATION_RANGE);\n"
"\n"
"	for (int i = IDX_SCREEN_Y + 1; i < TOTAL_U_SIZE; ++i)\n"
"		proposedU[i] = Mutate(seed, currentU[i]);\n"
"}\n"
"\n"
"void Sampler_Init(Seed *seed, __global Sample *sample, __global float *sampleData,\n"
"		const uint filmWidth, const uint filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"		, __global float *pixelFilterDistribution\n"
"#endif\n"
"		) {\n"
"	sample->totalI = 0.f;\n"
"	sample->largeMutationCount = 1.f;\n"
"\n"
"	sample->current = NULL_INDEX;\n"
"	sample->proposed = 1;\n"
"\n"
"	sample->smallMutationCount = 0;\n"
"	sample->consecutiveRejects = 0;\n"
"\n"
"	sample->weight = 0.f;\n"
"\n"
"	__global float *sampleDataPathBase = Sampler_GetSampleDataPathBase(sample, sampleData);\n"
"	LargeStep(seed, sampleDataPathBase);\n"
"\n"
"	SampleResult_Init(&sample->result);\n"
"\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"	const float ux = sampleDataPathBase[IDX_SCREEN_X] * filmWidth;\n"
"	const float uy = sampleDataPathBase[IDX_SCREEN_Y] * filmHeight;\n"
"\n"
"	const uint pixelX = min((uint)ux, (uint)(filmWidth - 1));\n"
"	const uint pixelY = min((uint)uy, (uint)(filmHeight - 1));\n"
"	sample->result.pixelX = pixelX;\n"
"	sample->result.pixelY = pixelY;\n"
"\n"
"	float uSubPixelX = ux - pixelX;\n"
"	float uSubPixelY = uy - pixelY;\n"
"\n"
"	// Sample according the pixel filter distribution\n"
"	float distX, distY;\n"
"	FilterDistribution_SampleContinuous(pixelFilterDistribution, uSubPixelX, uSubPixelY, &distX, &distY);\n"
"\n"
"	sample->result.filmX = pixelX + .5f + distX;\n"
"	sample->result.filmY = pixelY + .5f + distY;\n"
"#else\n"
"	sample->result.filmX = min(sampleDataPathBase[IDX_SCREEN_X] * filmWidth, (float)(filmWidth - 1));\n"
"	sample->result.filmY = min(sampleDataPathBase[IDX_SCREEN_Y] * filmHeight, (float)(filmHeight - 1));\n"
"#endif\n"
"}\n"
"\n"
"void Sampler_SplatSample(\n"
"		Seed *seed,\n"
"		__global Sample *sample,\n"
"		__global float *sampleData\n"
"		FILM_PARAM_DECL\n"
"		) {\n"
"	//--------------------------------------------------------------------------\n"
"	// Accept/Reject the sample\n"
"	//--------------------------------------------------------------------------\n"
"\n"
"	uint current = sample->current;\n"
"	uint proposed = sample->proposed;\n"
"\n"
"	if (current == NULL_INDEX) {\n"
"		// It is the very first sample, I have still to initialize the current\n"
"		// sample\n"
"\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"		Film_AddSample(sample->result.pixelX, sample->result.pixelY,\n"
"				&sample->result, 1.f\n"
"				FILM_PARAM);\n"
"#else\n"
"		Film_SplatSample(&sample->result, 1.f\n"
"					FILM_PARAM);\n"
"#endif\n"
"\n"
"		sample->currentResult = sample->result;\n"
"		sample->totalI = SampleResult_Radiance_Y(&sample->result);\n"
"\n"
"		current = proposed;\n"
"		proposed ^= 1;\n"
"	} else {\n"
"		const float currentI = SampleResult_Radiance_Y(&sample->currentResult);\n"
"\n"
"		float proposedI = SampleResult_Radiance_Y(&sample->result);\n"
"		proposedI = (isnan(proposedI) || isinf(proposedI)) ? 0.f : proposedI;\n"
"\n"
"		float totalI = sample->totalI;\n"
"		uint largeMutationCount = sample->largeMutationCount;\n"
"		uint smallMutationCount = sample->smallMutationCount;\n"
"		if (smallMutationCount == 0) {\n"
"			// It is a large mutation\n"
"			totalI += proposedI;\n"
"			largeMutationCount += 1;\n"
"\n"
"			sample->totalI = totalI;\n"
"			sample->largeMutationCount = largeMutationCount;\n"
"		}\n"
"\n"
"		const float meanI = (totalI > 0.f) ? (totalI / largeMutationCount) : 1.f;\n"
"\n"
"		// Calculate accept probability from old and new image sample\n"
"		uint consecutiveRejects = sample->consecutiveRejects;\n"
"\n"
"		float accProb;\n"
"		if ((currentI > 0.f) && (consecutiveRejects < PARAM_SAMPLER_METROPOLIS_MAX_CONSECUTIVE_REJECT))\n"
"			accProb = min(1.f, proposedI / currentI);\n"
"		else\n"
"			accProb = 1.f;\n"
"\n"
"		const float newWeight = accProb + ((smallMutationCount == 0) ? 1.f : 0.f);\n"
"		float weight = sample->weight;\n"
"		weight += 1.f - accProb;\n"
"\n"
"		const float rndVal = Rnd_FloatValue(seed);\n"
"\n"
"		/*if (get_global_id(0) == 0)\n"
"			printf(\\\"[%d] Current: (%f, %f, %f) [%f] Proposed: (%f, %f, %f) [%f] accProb: %f <%f>\\\\n\\\",\n"
"					consecutiveRejects,\n"
"					currentL.r, currentL.g, currentL.b, weight,\n"
"					proposedL.r, proposedL.g, proposedL.b, newWeight,\n"
"					accProb, rndVal);*/\n"
"\n"
"		__global SampleResult *contrib;\n"
"		float norm;\n"
"\n"
"		if ((accProb == 1.f) || (rndVal < accProb)) {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\\\"\\\\t\\\\tACCEPTED !\\\\n\\\");*/\n"
"\n"
"			// Add accumulated contribution of previous reference sample\n"
"			norm = weight / (currentI / meanI + PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE);\n"
"			contrib = &sample->currentResult;\n"
"\n"
"			current ^= 1;\n"
"			proposed ^= 1;\n"
"			consecutiveRejects = 0;\n"
"\n"
"			weight = newWeight;\n"
"		} else {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\\\"\\\\t\\\\tREJECTED !\\\\n\\\");*/\n"
"\n"
"			// Add contribution of new sample before rejecting it\n"
"			norm = newWeight / (proposedI / meanI + PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE);\n"
"			contrib = &sample->result;\n"
"\n"
"			++consecutiveRejects;\n"
"		}\n"
"\n"
"		if (norm > 0.f) {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\\\"\\\\t\\\\tContrib: (%f, %f, %f) [%f] consecutiveRejects: %d\\\\n\\\",\n"
"						contrib.r, contrib.g, contrib.b, norm, consecutiveRejects);*/\n"
"\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"			Film_AddSample(contrib->pixelX, contrib->pixelY,\n"
"					contrib, norm\n"
"					FILM_PARAM);\n"
"#else\n"
"			Film_SplatSample(contrib, norm\n"
"					FILM_PARAM);\n"
"#endif\n"
"		}\n"
"\n"
"		// Check if it is an accepted mutation\n"
"		if (consecutiveRejects == 0) {\n"
"			// I can now (after Film_SplatSample()) overwrite sample->currentResult and sample->result\n"
"			sample->currentResult = sample->result;\n"
"		}\n"
"\n"
"		sample->weight = weight;\n"
"		sample->consecutiveRejects = consecutiveRejects;\n"
"	}\n"
"\n"
"	sample->current = current;\n"
"	sample->proposed = proposed;\n"
"}\n"
"\n"
"void Sampler_NextSample(\n"
"		Seed *seed,\n"
"		__global Sample *sample,\n"
"		__global float *sampleData,\n"
"		const uint filmWidth, const uint filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"		, __global float *pixelFilterDistribution\n"
"#endif\n"
"		) {\n"
"	//--------------------------------------------------------------------------\n"
"	// Mutate the sample\n"
"	//--------------------------------------------------------------------------\n"
"\n"
"	__global float *proposedU = &sampleData[sample->proposed * TOTAL_U_SIZE];\n"
"	if (Rnd_FloatValue(seed) < PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE) {\n"
"		LargeStep(seed, proposedU);\n"
"		sample->smallMutationCount = 0;\n"
"	} else {\n"
"		__global float *currentU = &sampleData[sample->current * TOTAL_U_SIZE];\n"
"\n"
"		SmallStep(seed, currentU, proposedU);\n"
"		sample->smallMutationCount += 1;\n"
"	}\n"
"\n"
"	SampleResult_Init(&sample->result);\n"
"\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"	const float ux = proposedU[IDX_SCREEN_X] * filmWidth;\n"
"	const float uy = proposedU[IDX_SCREEN_Y] * filmHeight;\n"
"\n"
"	const uint pixelX = min((uint)ux, (uint)(filmWidth - 1));\n"
"	const uint pixelY = min((uint)uy, (uint)(filmHeight - 1));\n"
"	sample->result.pixelX = pixelX;\n"
"	sample->result.pixelY = pixelY;\n"
"\n"
"	float uSubPixelX = ux - pixelX;\n"
"	float uSubPixelY = uy - pixelY;\n"
"\n"
"	// Sample according the pixel filter distribution\n"
"	float distX, distY;\n"
"	FilterDistribution_SampleContinuous(pixelFilterDistribution, uSubPixelX, uSubPixelY, &distX, &distY);\n"
"\n"
"	sample->result.filmX = pixelX + .5f + distX;\n"
"	sample->result.filmY = pixelY + .5f + distY;\n"
"#else\n"
"	sample->result.filmX = min(proposedU[IDX_SCREEN_X] * filmWidth, (float)(filmWidth - 1));\n"
"	sample->result.filmY = min(proposedU[IDX_SCREEN_Y] * filmHeight, (float)(filmHeight - 1));\n"
"#endif\n"
"}\n"
"\n"
"#endif\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Sobol Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 2)\n"
"\n"
"uint SobolSampler_SobolDimension(const uint index, const uint dimension) {\n"
"	const uint offset = dimension * SOBOL_BITS;\n"
"	uint result = 0;\n"
"	uint i = index;\n"
"\n"
"	for (uint j = 0; i; i >>= 1, j++) {\n"
"		if (i & 1)\n"
"			result ^= SOBOL_DIRECTIONS[offset + j];\n"
"	}\n"
"\n"
"	return result;\n"
"}\n"
"\n"
"float SobolSampler_GetSample(__global Sample *sample, const uint index) {\n"
"	const uint pass = sample->pass;\n"
"\n"
"	const uint iResult = SobolSampler_SobolDimension(pass, index);\n"
"	const float fResult = iResult * (1.f / 0xffffffffu);\n"
"\n"
"	// Cranley-Patterson rotation to reduce visible regular patterns\n"
"	const float shift = (index & 1) ? PARAM_SAMPLER_SOBOL_RNG0 : PARAM_SAMPLER_SOBOL_RNG1;\n"
"	const float val = fResult + shift;\n"
"\n"
"	return val - floor(val);\n"
"}\n"
"\n"
"#define Sampler_GetSamplePath(index) (SobolSampler_GetSample(sample, index))\n"
"#define Sampler_GetSamplePathVertex(depth, index) ((depth > PARAM_SAMPLER_SOBOL_MAXDEPTH) ? \\\n"
"	Rnd_FloatValue(seed) : \\\n"
"	SobolSampler_GetSample(sample, IDX_BSDF_OFFSET + (depth - 1) * VERTEX_SAMPLE_SIZE + index))\n"
"\n"
"__global float *Sampler_GetSampleData(__global Sample *sample, __global float *samplesData) {\n"
"	const size_t gid = get_global_id(0);\n"
"	return &samplesData[gid * TOTAL_U_SIZE];\n"
"}\n"
"\n"
"__global float *Sampler_GetSampleDataPathBase(__global Sample *sample, __global float *sampleData) {\n"
"	return sampleData;\n"
"}\n"
"\n"
"__global float *Sampler_GetSampleDataPathVertex(__global Sample *sample,\n"
"		__global float *sampleDataPathBase, const uint depth) {\n"
"	return &sampleDataPathBase[IDX_BSDF_OFFSET + depth * VERTEX_SAMPLE_SIZE];\n"
"}\n"
"\n"
"void Sampler_SplatSample(\n"
"		Seed *seed,\n"
"		__global Sample *sample,\n"
"		__global float *sampleData\n"
"		FILM_PARAM_DECL\n"
"		) {\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"	Film_AddSample(sample->result.pixelX, sample->result.pixelY,\n"
"			&sample->result, 1.f\n"
"			FILM_PARAM);\n"
"#else\n"
"	Film_SplatSample(&sample->result, 1.f\n"
"			FILM_PARAM);\n"
"#endif\n"
"}\n"
"\n"
"void Sampler_NextSample(\n"
"		Seed *seed,\n"
"		__global Sample *sample,\n"
"		__global float *sampleData,\n"
"		const uint filmWidth, const uint filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"		, __global float *pixelFilterDistribution\n"
"#endif\n"
"		) {\n"
"	// Move to the next sample\n"
"	sample->pass += get_global_size(0);\n"
"\n"
"	const float u0 = Sampler_GetSamplePath(IDX_SCREEN_X);\n"
"	const float u1 = Sampler_GetSamplePath(IDX_SCREEN_Y);\n"
"\n"
"	sampleData[IDX_SCREEN_X] = u0;\n"
"	sampleData[IDX_SCREEN_Y] = u1;\n"
"\n"
"	SampleResult_Init(&sample->result);\n"
"\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"	const float ux = u0 * filmWidth;\n"
"	const float uy = u1 * filmHeight;\n"
"\n"
"	const uint pixelX = min((uint)ux, (uint)(filmWidth - 1));\n"
"	const uint pixelY = min((uint)uy, (uint)(filmHeight - 1));\n"
"	sample->result.pixelX = pixelX;\n"
"	sample->result.pixelY = pixelY;\n"
"\n"
"	float uSubPixelX = ux - pixelX;\n"
"	float uSubPixelY = uy - pixelY;\n"
"\n"
"	// Sample according the pixel filter distribution\n"
"	float distX, distY;\n"
"	FilterDistribution_SampleContinuous(pixelFilterDistribution, uSubPixelX, uSubPixelY, &distX, &distY);\n"
"\n"
"	sample->result.filmX = pixelX + .5f + distX;\n"
"	sample->result.filmY = pixelY + .5f + distY;\n"
"#else\n"
"	sample->result.filmX = min(u0 * filmWidth, (float)(filmWidth - 1));\n"
"	sample->result.filmY = min(u1 * filmHeight, (float)(filmHeight - 1));\n"
"#endif\n"
"}\n"
"\n"
"void Sampler_Init(Seed *seed, __global Sample *sample, __global float *sampleData,\n"
"		const uint filmWidth, const uint filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"		, __global float *pixelFilterDistribution\n"
"#endif\n"
"		) {\n"
"	sample->pass = PARAM_SAMPLER_SOBOL_STARTOFFSET + get_global_id(0);\n"
"\n"
"	Sampler_NextSample(seed, sample, sampleData, filmWidth, filmHeight\n"
"#if defined(PARAM_USE_FAST_PIXEL_FILTER)\n"
"			, pixelFilterDistribution\n"
"#endif\n"
"			);\n"
"}\n"
"\n"
"#endif\n"
"\n"
; } }
