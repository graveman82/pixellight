/*********************************************************\
 *  File: DockWidgetSceneRenderer.cpp                    *
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
#include <QtGui/qtreeview.h>
#include <QtGui/qdockwidget.h>
#include <QtGui/qmainwindow.h>
#include <PLCore/Base/Class.h>
#include <PLScene/Compositing/SceneRenderer.h>
#include <PLEngine/Application/EngineApplication.h>
#include "PLFrontendQt/QtStringAdapter.h"
#include "PLFrontendQt/DataModels/SceneRendererDataModel/SceneRendererDataModel.h"
#include "PLFrontendQt/DockWidget/DockWidgetSceneRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
namespace PLFrontendQt {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(DockWidgetSceneRenderer)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
DockWidgetSceneRenderer::DockWidgetSceneRenderer(QMainWindow *pQMainWindow) : DockWidgetScene(pQMainWindow)
{
	// Get encapsulated Qt dock widget
	QDockWidget *pQDockWidget = GetQDockWidget();
	if (pQDockWidget) {
		// Create tree view and set scene graph model
		QTreeView *pQTreeView = new QTreeView();
		pQDockWidget->setWidget(pQTreeView);
		DataModels::SceneRendererDataModel::SceneRendererDataModel *pSceneRendererDataModel = new DataModels::SceneRendererDataModel::SceneRendererDataModel(pQDockWidget);
		pQTreeView->setModel(pSceneRendererDataModel);
		pQTreeView->expandToDepth(0);

		// Set a default scene renderer to have a decent standard behaviour
		PLScene::SceneRenderer *pSceneRenderer = nullptr;
		{
			CoreApplication *pApplication = CoreApplication::GetApplication();
			if (pApplication && pApplication->IsInstanceOf("PLEngine::EngineApplication"))
				pSceneRenderer = static_cast<PLEngine::EngineApplication*>(pApplication)->GetSceneRendererTool().GetSceneRenderer();
			pSceneRendererDataModel->SetSceneRenderer(pSceneRenderer);
		}

		// Set window title
		QString sQStringWindowTitle = pQDockWidget->tr(GetClass()->GetProperties().Get("Title"));
		if (pSceneRenderer) {
			sQStringWindowTitle += ": ";
			sQStringWindowTitle += QtStringAdapter::PLToQt('\"' + pSceneRenderer->GetName() + '\"');	// Put it into quotes to make it possible to see e.g. trailing spaces
		}
		pQDockWidget->setWindowTitle(sQStringWindowTitle);

		// Add the created Qt dock widget to the given Qt main window
		pQMainWindow->addDockWidget(Qt::LeftDockWidgetArea, pQDockWidget);
	}
}

/**
*  @brief
*    Destructor
*/
DockWidgetSceneRenderer::~DockWidgetSceneRenderer()
{
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLFrontendQt
