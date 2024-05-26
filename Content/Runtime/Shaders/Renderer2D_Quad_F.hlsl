/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

struct FSInput
{
    float4 color : COLOR;
};

struct FSOutput
{
    float4 color : SV_Target0;
};

FSOutput fragment_main(FSInput input)
{
    FSOutput output;
    output.color = float4(input.color.rgb, 1);
    return output;
}
