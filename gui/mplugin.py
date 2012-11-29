# mplugin.py --- 
# 
# Filename: mplugin.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue Oct  2 17:25:41 2012 (+0530)
# Version: 
# Last-Updated: Thu Nov 29 16:09:51 2012 (+0530)
#           By: subha
#     Update #: 106
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# This is to be the base class for all MOOSE GUI plugins.
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:

from PyQt4 import QtGui,QtCore,Qt

class MoosePluginBase(object):
    """Moose GUI plugin base class.

    A GUI plugin for MOOSE should extend MoosePlugin class. It has to
    implement the methods described here.

    """
    def __init__(self, mainwindow):
        self._views = []
        self._menus = []
        self.mwindow = mainwindow

    def getPreviousPlugin(self):
	"""Returns the plugin object that the gui is supposed to
	switch to when going to a smaller scale."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getNextPlugin(self):
	"""Returns the plugin object that the gui is supposed to
	switch to when going to a larger scale."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getAdjacentPlugins(self):
	"""Return a list of plugins that are valid transitions from this plugin"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getCentralWidget(self):
	"""Returns the central widget for the current level"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getLibraryWidget(self):
	"""Returns the library widget for current level of detail"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getOperationsWidget(self):
	"""Return the widget with operations that can be applied on
	objects in current level"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getSelectionWidget(self):
	"""Return the widget with selection options that can be
	applied on objects in current level"""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getViews(self):
	"""Return the view widgets available from this plugin."""        
	raise NotImplementedError('method must be reimplemented in subclass')

    def getCurrentView(self):
	"""Return the current view of this plugin."""
	raise NotImplementedError('method must be reimplemented in subclass')    

    def setCurrentView(self, view):
        """Set current view (e.g., edit/plot/run)"""
	self.currentView = view

    def getMenus(self):
        return self._menus

    # def close(self):
    #     for view in self._views:
    #         view.close()

    def getEditorView(self):
        raise NotImplementedError('method must be implemented in derived class')

    def getPlotView(self):
        raise NotImplementedError('method must be implemented in derived class')

    def getRunView(self):
        raise NotImplementedError('method must be implemented in derived class')


class ViewBase(object):
    def __init__(self, *args):
        self._menus = []
        self._toolPanes = []
        self._centralWidgets = []

    def getToolPanes(self):
        """Return a list of widgets to be displayed as dock widgets."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getToolBar(self):
        """Return the toolbar for this view."""
	raise NotImplementedError('method must be reimplemented in subclass')

    def getMenus(self):
        """Return the menus for this view."""
        return self._menus

    def getPreferences(self):
        """Return a widget for setting preferences"""
        raise NotImplementedError('method must be reimplemented in subclass')

    def getCentralWidgets(self):        
        return self._centralWidgets

class EditorBase(ViewBase):
    def __init__(self, *args):
        ViewBase.__init__(self, *args)        

    def getToolPanes(self):
        if not self._toolPanes:
            self._toolPanes = [
                self.getLibraryPane(),
                self.getSelectionPane(),
                self.getOperationsPane()]

    def getLibraryPane(self):
        """TODO: display everything under library as a tree"""
        pass

    def getSelectionPane(self):
        """TODO: provide a widget to apply selection rules"""
        pass

    def getOperationsPane(self):
        """TODO: provide a widget to apply operations on selected
        elements."""
        pass

    def createCentralWidget(self, path):
        """Create a central widget with model rooted at path"""
	raise NotImplementedError('method must be reimplemented in subclass')


class PlotBase(ViewBase):
    def __init__(self, *args):
        ViewBase.__init__(self, *args)


class RunBase(ViewBase):
    def __init__(self, *args):
        ViewBase.__init__(self, *args)


class EditorWidgetBase(QtGui.QWidget):
    """This is the base class for central widget displayed in
    editorviews."""
    def __init__(self, *args):
        QtGui.QWidget.__init__(self, *args)
        self.modelRoot = '/'

    def setModelRoot(self, path):
        self.modelRoot = path
        self.update()

    def update(self):
        """Update view."""
        raise NotImplementedError('must be implemented in derived class.')
    
    
# 
# mplugin.py ends here