/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

struct VSInput
{
    float2 position : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 color : COLOR;
    float4 position : SV_Position;
};

VSOutput vertex_main(VSInput input)
{
    VSOutput output;
    output.color = input.color;
    output.position = float4(input.position, 0.5F, 1);
    return output;
}
