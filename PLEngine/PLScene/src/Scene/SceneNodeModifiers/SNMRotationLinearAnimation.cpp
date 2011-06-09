/*********************************************************\
 *  File: SNMRotationLinearAnimation.cpp                 *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
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
#include <PLGeneral/Tools/Timing.h>
#include <PLMath/EulerAngles.h>
#include "PLScene/Scene/SceneNode.h"
#include "PLScene/Scene/SceneContext.h"
#include "PLScene/Scene/SceneNodeModifiers/SNMRotationLinearAnimation.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
namespace PLScene {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(SNMRotationLinearAnimation)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
SNMRotationLinearAnimation::SNMRotationLinearAnimation(SceneNode &cSceneNode) : SNMTransform(cSceneNode),
	Velocity(this),
	EventHandlerUpdate(&SNMRotationLinearAnimation::OnUpdate, this)
{
}

/**
*  @brief
*    Destructor
*/
SNMRotationLinearAnimation::~SNMRotationLinearAnimation()
{
}


//[-------------------------------------------------------]
//[ Protected virtual SceneNodeModifier functions         ]
//[-------------------------------------------------------]
void SNMRotationLinearAnimation::OnActivate(bool bActivate)
{
	// Connect/disconnect event handler
	SceneContext *pSceneContext = GetSceneContext();
	if (pSceneContext) {
		if (bActivate)
			pSceneContext->EventUpdate.Connect(&EventHandlerUpdate);
		else
			pSceneContext->EventUpdate.Disconnect(&EventHandlerUpdate);
	}
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Called when the scene node modifier needs to be updated
*/
void SNMRotationLinearAnimation::OnUpdate()
{
	// Is the velocity not null?
	if (!Velocity.Get().IsNull()) {
		// Get rotation increase in Euler angles (degree)
		Vector3 vRot = Velocity.Get();
		vRot *= Timing::GetInstance()->GetTimeDifference();

		// Get a quaternion representation of the rotation increase
		Quaternion qRot;
		EulerAngles::ToQuaternion(static_cast<float>(vRot.x*Math::DegToRad), static_cast<float>(vRot.y*Math::DegToRad), static_cast<float>(vRot.z*Math::DegToRad), qRot);

		// Apply the rotation increase
		GetSceneNode().GetTransform().SetRotation(GetSceneNode().GetTransform().GetRotation()*qRot);
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLScene
