# default.py --- 
# 
# Filename: default.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue Nov 13 15:58:31 2012 (+0530)
# Version: 
# Last-Updated: Thu Nov 29 16:10:07 2012 (+0530)
#           By: subha
#     Update #: 46
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# The default placeholder plugin for MOOSE
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

from mplugin import *

class MoosePlugin(MoosePluginBase):
    """Default plugin for MOOSE GUI"""
    def __init__(self, mainwindow):
        MoosePluginBase.__init__(self, mainwindow)

    def getPreviousPlugin(self):
        return None

    def getNextPlugin(self):
        return None

    def getAdjacentPlugins(self):
        return []

    def getViews(self):
        return self._views

    def getCurrentView(self):
        return self.currentView

    def switchView(self, view):
        self.currentView = view

    def getEditorView(self):
        if not hasattr(self, 'editorView'):
            self.editorView = MooseEditorView()
            self.currentView = self.editorView
        return self.editorView


class MooseEditorView(EditorBase):
    """Default editor.

    TODO: Implementation - display moose element tree as a tree or as
    boxes inside boxes

    """
    def __init__(self, *args):
        EditorBase.__init__(self, *args)

    def getToolPanes(self):
        return super(MooseEditorView, self).getToolPanes()

    def getLibraryPane(self):
        return super(MooseEditorView, self).getLibraryPane()

    def getOperationsWidget(self):
        return super(MooseEditorView, self).getOperationsPane()

    def getCentralWidgets(self):
        if len(self._centralWidgets) == 0:
            self._centralWidgets.append(EditorWidgetBase())
        return self._centralWidgets

# 
# default.py ends here