////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Simple shader with added color transform
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* pcShader_PositionTextureAlpha_frag_fs = STRINGIFY(

\n#ifdef GL_ES\n
precision mediump float;
\n#endif\n

varying vec2 v_texCoord;
varying vec4 v_fragmentColor;

uniform vec4 colorTransformMult;
uniform vec4 colorTransformOffsets;

void main()
{
    vec4 texColor = texture2D(CC_Texture0, v_texCoord);
    
    const float kMinimalAlphaAllowed = 1.0e-8;
    texColor.a = clamp(texColor.a, kMinimalAlphaAllowed, 1.0);
    
    texColor = vec4(texColor.rgb / texColor.a, texColor.a);

    vec4 ctxColor = texColor * colorTransformMult + colorTransformOffsets;
    ctxColor *= v_fragmentColor;

    texColor = vec4(ctxColor.rgb * ctxColor.a, ctxColor.a);
    
    gl_FragColor = texColor;
}
);
