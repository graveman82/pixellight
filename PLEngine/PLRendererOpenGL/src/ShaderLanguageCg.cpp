/*********************************************************\
 *  File: ShaderLanguageCg.cpp                           *
 *
 *  Copyright (C) 2002-2010 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PLRendererOpenGL/ProgramCg.h"
#include "PLRendererOpenGL/VertexShaderCg.h"
#include "PLRendererOpenGL/GeometryShaderCg.h"
#include "PLRendererOpenGL/FragmentShaderCg.h"
#include "PLRendererOpenGL/ShaderLanguageCg.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
namespace PLRendererOpenGL {


//[-------------------------------------------------------]
//[ Public static data                                    ]
//[-------------------------------------------------------]
const String ShaderLanguageCg::Cg = "Cg";


//[-------------------------------------------------------]
//[ Public virtual PLRenderer::ShaderLanguage functions   ]
//[-------------------------------------------------------]
String ShaderLanguageCg::GetShaderLanguage() const
{
	return Cg;
}

PLRenderer::VertexShader *ShaderLanguageCg::CreateVertexShader()
{
	return new VertexShaderCg(*m_pRenderer);
}

PLRenderer::GeometryShader *ShaderLanguageCg::CreateGeometryShader()
{
	return new GeometryShaderCg(*m_pRenderer);
}

PLRenderer::FragmentShader *ShaderLanguageCg::CreateFragmentShader()
{
	return new FragmentShaderCg(*m_pRenderer);
}

PLRenderer::Program *ShaderLanguageCg::CreateProgram()
{
	return new ProgramCg(*m_pRenderer);
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
ShaderLanguageCg::ShaderLanguageCg(PLRenderer::Renderer &cRenderer) :
	m_pRenderer(&cRenderer)
{
}

/**
*  @brief
*    Destructor
*/
ShaderLanguageCg::~ShaderLanguageCg()
{
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRendererOpenGL
