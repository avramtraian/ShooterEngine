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

Texture2D u_textures[32] : register(t0);
sampler u_texture_sampler : register(s0);

FSOutput fragment_main(FSInput input)
{
    FSOutput output;
    float4 texel = float4(1, 1, 1, 1);
    
    switch (input.texture_id)
    {
        case 0: texel = u_textures[0].Sample(u_texture_sampler, input.texture_coordinates); break;
        case 1: texel = u_textures[1].Sample(u_texture_sampler, input.texture_coordinates); break;
        case 2: texel = u_textures[2].Sample(u_texture_sampler, input.texture_coordinates); break;
        case 3: texel = u_textures[3].Sample(u_texture_sampler, input.texture_coordinates); break;
        case 4: texel = u_textures[4].Sample(u_texture_sampler, input.texture_coordinates); break;
        case 5: texel = u_textures[5].Sample(u_texture_sampler, input.texture_coordinates); break;
        case 6: texel = u_textures[6].Sample(u_texture_sampler, input.texture_coordinates); break;
        case 7: texel = u_textures[7].Sample(u_texture_sampler, input.texture_coordinates); break;
    }
    
    output.color = input.color * texel;
    return output;
}
