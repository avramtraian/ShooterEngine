/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

struct FSInput
{
    float4 color : COLOR;
    float2 texture_coordinates : TEXTURE_COORDINATES;
    uint texture_id : TEXTURE_ID;
};

struct FSOutput
{
    float4 color : SV_Target0;
};

Texture2D   u_Textures[32]      : register(t0);
sampler     u_TextureSampler    : register(s0);

FSOutput fragment_main(FSInput input)
{
    FSOutput output;
    float4 texel;
    
    switch (input.texture_id)
    {
        case 0: texel = u_Textures[0].Sample(u_TextureSampler, input.texture_coordinates); break;
        case 1: texel = u_Textures[1].Sample(u_TextureSampler, input.texture_coordinates); break;
        case 2: texel = u_Textures[2].Sample(u_TextureSampler, input.texture_coordinates); break;
        case 3: texel = u_Textures[3].Sample(u_TextureSampler, input.texture_coordinates); break;
        case 4: texel = u_Textures[4].Sample(u_TextureSampler, input.texture_coordinates); break;
        case 5: texel = u_Textures[5].Sample(u_TextureSampler, input.texture_coordinates); break;
        case 6: texel = u_Textures[6].Sample(u_TextureSampler, input.texture_coordinates); break;
        case 7: texel = u_Textures[7].Sample(u_TextureSampler, input.texture_coordinates); break;
    }
    
    output.color = input.color * texel;
    return output;
}
