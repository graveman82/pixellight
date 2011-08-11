#-------------------------------------------------
#
# Project created by QtCreator 2011-08-10T11:34:06
#
#-------------------------------------------------

QT       += core gui

TARGET = PLEditor
TEMPLATE = app
DESTDIR = $$quote(../../../Bin/Tools/PLEditor)

SOURCES += main.cpp\
		MainWindow.cpp \
    AboutDialog.cpp \
    OptionsDialog.cpp \
    AboutPluginsDialog.cpp \
    PluginManager.cpp \
    Application.cpp

HEADERS  += MainWindow.h\
	Interfaces/IPlugin.h\
	Interfaces/IPluginDockWidget.h\
    Interfaces/IPluginFactory.h \
    AboutDialog.h \
    OptionsDialog.h \
    AboutPluginsDialog.h \
    PluginManager.h \
    Application.h

FORMS    += MainWindow.ui \
    AboutDialog.ui \
    OptionsDialog.ui \
    AboutPluginsDialog.ui