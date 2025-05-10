from setuptools import setup, Extension

module = Extension(
    'lidar_graph',
    sources=['src/data/graph/lidar_graph.c']
)

setup(
    name='lidar_graph',
    version='1.0',
    description='Lidar Graph Module for 2D LiDAR Data',
    ext_modules=[module]
)

