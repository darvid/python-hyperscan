# -*- coding: utf-8 -*-
import pkg_resources


extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.napoleon',
    'sphinx.ext.todo',
]
templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
project = u'python-hyperscan'
copyright = u'2017, David Gidwani'
author = u'David Gidwani'
version = pkg_resources.get_distribution('hyperscan').version
language = None
exclude_patterns = []
pygments_style = 'sphinx'
todo_include_todos = True
html_theme = 'material_design'
html_theme_options = {
    'pygments_theme': 'lovelace',
}
html_static_path = []
htmlhelp_basename = 'python-hyperscandoc'
latex_documents = [
    (master_doc, 'python-hyperscan.tex', u'python-hyperscan Documentation',
     u'David Gidwani', 'manual'),
]
man_pages = [
    (master_doc, 'python-hyperscan', u'python-hyperscan Documentation',
     [author], 1)
]
texinfo_documents = [
    (master_doc, 'python-hyperscan', u'python-hyperscan Documentation',
     author, 'python-hyperscan', 'One line description of project.',
     'Miscellaneous'),
]
intersphinx_mapping = {'https://docs.python.org/': None}
