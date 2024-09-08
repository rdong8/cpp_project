import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, cmake_layout


class ConanApplication(ConanFile):
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def configure(self):
        self.options["spdlog"].use_std_fmt = True

    def layout(self):
        if (build_dir := os.getenv("BUILD_DIR")) is None or len(build_dir) == 0:
            raise KeyError(f"Environment variable BUILD_DIR not set in {__file__}")
        cmake_layout(self, build_folder=build_dir)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()

    def requirements(self):
        requirements = self.conan_data.get('requirements', [])
        for requirement in requirements:
            self.requires(requirement)
