//
//  Shader.fsh
//  MapClient
//
//  Created by wayfinder on 6/15/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
