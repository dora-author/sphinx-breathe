# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import sphinx_rtd_theme
import sys
import os
import shlex

sys.path.insert(0, os.path.abspath('..'))
#from my_package import __version__ as VERSION

from sphinx.builders.html import StandaloneHTMLBuilder
import subprocess, os

# -- Project information -----------------------------------------------------

project = 'my_package'
copyright = '2023, ejkim'
author = 'ejkim'

# Version info for the project, acts as replacement for |version| and |release|
# The short X.Y version
#version = VERSION
# The full version, including alpha/beta/rc tags
release = '1.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx_rtd_theme',
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.autosectionlabel',
    #'sphinxcontrib.httpdomain',
    'sphinx_tabs.tabs',
    'sphinx-prompt',
    'sphinx.ext.todo',
    'sphinx.ext.coverage',
    'sphinx.ext.mathjax',
    'sphinx.ext.ifconfig',
    'sphinx.ext.viewcode',
    #'sphinx_sitemap',
    'sphinx.ext.inheritance_diagram',
    'breathe',
    'myst_parser'
]

# Doxygen
subprocess.call('doxygen Doxyfile.in', shell=True)

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

source_suffix = {
  '.rst': 'restructuredtext',
  '.txt': 'markdown',
  '.md': 'markdown',
}
  

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "sphinx_rtd_theme"
html_theme_options = {
'canonical_url': '',
'analytics_id': '',
'display_version': True,
'prev_next_buttons_location': 'bottom',
'style_external_links': False,

'logo_only': False,

# Toc options
'collapse_navigation': True,
'sticky_navigation': True,
'navigation_depth': 4,
'includehidden': True,
'titles_only': False
}
# html_logo = ''
# github_url = ''
# html_baseurl = ''


# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']




breathe_projects = {

"My Package": "_build/xml/"

}

breathe_default_project = "My Package"

breathe_default_members = ('members', 'undoc-members', 'show-inheritance')
