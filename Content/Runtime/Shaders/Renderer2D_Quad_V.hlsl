/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

struct VSInput
{
    float2 position : POSITION;
    float4 color : COLOR;
    float2 texture_coordinates : TEXTURE_COORDINATES;
    uint texture_id : TEXTURE_ID;
};

struct VSOutput
{
    float4 color : COLOR;
    float2 texture_coordinates : TEXTURE_COORDINATES;
    uint texture_id : TEXTURE_ID;
    float4 position : SV_Position;
};

VSOutput vertex_main(VSInput input)
{
    VSOutput output;
    output.color = input.color;
    output.texture_coordinates = input.texture_coordinates;
    output.texture_id = input.texture_id;
    output.position = float4(input.position, 0.5F, 1);
    return output;
}
