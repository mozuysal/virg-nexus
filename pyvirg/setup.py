from setuptools import setup

setup(
    name='virg',
    version='0.1',
    packages=['virg', 'virg.nexus'],
    install_requires=[
        'pyyaml>=5.4'
    ],
)
