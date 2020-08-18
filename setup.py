from setuptools import find_packages, setup

# usage: python setup.py bdist_wheel
setup(
    name='lidarviewer',
    version='1.0.0',
    zip_safe=False,
    packages=find_packages(),  # copy folders to package root
    package_data={
        'lidarviewer': ['lidarviewer_exec'],  # copy files ALREADY in package to install folder
    },
    author='Zhiliang Zhou',
    author_email='zhouzhiliang@gmail.com',
    description='lidarviewer',
    long_description='derived from pptk package',
)
