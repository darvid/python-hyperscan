"""CPython extension for the Hyperscan regular expression engine."""
from __future__ import absolute_import

import io
import subprocess

import setuptools
import setuptools.extension


__all__ = ('setup',)


def readme():
    try:
        with io.open('README.rst') as fp:
            return fp.read()
    except IOError:
        return ''


# http://code.activestate.com/recipes/502261-python-distutils-pkg-config/
def pkgconfig(libs, optional=''):
    flag_map = {
        'include_dirs': ('--cflags-only-I', 2),
        'library_dirs': ('--libs-only-L', 2),
        'libraries': ('--libs-only-l', 2),
        'extra_compile_args': ('--cflags-only-other', 0),
        'extra_link_args': ('--libs-only-other', 0),
    }
    ext_kwargs = {}
    for lib in libs:
        for distutils_kwarg, (pkg_option, trim_offset) in flag_map.items():
            try:
                options = subprocess.check_output(
                    ['pkg-config', optional, pkg_option, lib]).decode().split()
            except subprocess.CalledProcessError:
                continue
            ext_kwargs.setdefault(distutils_kwarg, []).extend(
                [opt[trim_offset:] for opt in options])
    return ext_kwargs


def setup():
    ext = setuptools.extension.Extension(
        'hyperscan',
        ['src/hyperscanmodule.c'],
        **pkgconfig(['libhs'])
    )
    setup_requirements = ['six', 'setuptools>=17.1', 'setuptools_scm']
    setuptools.setup(
        author='David Gidwani',
        author_email='david.gidwani@gmail.com',
        classifiers=[
            'Development Status :: 2 - Pre-Alpha',
            'Topic :: Software Development :: Libraries',
            'Topic :: Software Development :: Libraries :: Python Modules',
            'Topic :: Utilities',
            'Programming Language :: Python',
            'Programming Language :: Python :: 2',
            'Programming Language :: Python :: 2.7',
            'Programming Language :: Python :: 3',
            'Programming Language :: Python :: 3.4',
            'Programming Language :: Python :: 3.5',
            'Programming Language :: Python :: 3.6',
            'Programming Language :: Python :: Implementation :: CPython',
            'Environment :: Console',
            'Intended Audience :: Developers',
            'License :: OSI Approved :: MIT License',
            'Operating System :: POSIX :: Linux',
            'Operating System :: Unix',
            'Operating System :: MacOS',
            'Operating System :: Microsoft :: Windows',
        ],
        description=__doc__,
        ext_modules=[ext],
        license='MIT',
        long_description=readme(),
        name='hyperscan',
        setup_requires=setup_requirements,
        url='https://github.com/darvid/python-hyperscan',
        use_scm_version={
            'version_scheme': 'python-simplified-semver',
        },
        zip_safe=False,
    )


if __name__ == '__main__':
    setup()
