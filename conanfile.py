import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import copy
from conan.tools.build import can_run

class CppTemplateConan(ConanFile):
    name = 'cpp_template'
    version = '0.0.1'

    license = 'MIT'
    author = 'Matthew Guidry'
    url = 'https://github.com/mguid65/CppTemplate'
    description = 'C++ Template Project'
    topics = 'template'
    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'CMakeDeps', 'CMakeToolchain'
    package_type = 'header-library'
    build_policy = 'missing'

    def export_sources(self):
        copy(self, "LICENSE.md", self.recipe_folder, self.export_sources_folder)
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "cmake/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "include/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "source/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "test/*", self.recipe_folder, self.export_sources_folder)

    def requirements(self):
        self.requires("sdl/2.30.8")
        self.requires('fmt/11.0.2')
        self.test_requires('catch2/3.5.2')

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "test",  'unit_tests')
            self.run(cmd, env="conanrun")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ['cpp_template']
