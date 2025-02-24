//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: defines constants to use for the materialsystem and shaderapi
// SetxxxRenderingParameter functions
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef RENDERPARM_H
#define RENDERPARM_H

#ifndef _WIN32
#pragma once
#endif

#define MAX_VECTOR_RENDER_PARMS 20

enum RenderParamFloat_t
{
	MAX_FLOAT_RENDER_PARMS = 20
};

enum RenderParamInt_t
{
	INT_RENDERPARM_ENABLE_FIXED_LIGHTING = 0,
	INT_RENDERPARM_MORPH_ACCUMULATOR_X_OFFSET,
	INT_RENDERPARM_MORPH_ACCUMULATOR_Y_OFFSET,
	INT_RENDERPARM_MORPH_ACCUMULATOR_SUBRECT_WIDTH,
	INT_RENDERPARM_MORPH_ACCUMULATOR_SUBRECT_HEIGHT,
	INT_RENDERPARM_MORPH_ACCUMULATOR_4TUPLE_COUNT,

	INT_RENDERPARM_MORPH_WEIGHT_X_OFFSET,
	INT_RENDERPARM_MORPH_WEIGHT_Y_OFFSET,
	INT_RENDERPARM_MORPH_WEIGHT_SUBRECT_WIDTH,
	INT_RENDERPARM_MORPH_WEIGHT_SUBRECT_HEIGHT,

	INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA,

	MAX_INT_RENDER_PARMS = 20
};

#endif // RENDERPARM_H
