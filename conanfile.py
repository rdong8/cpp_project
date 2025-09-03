import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, cmake_layout


class ConanApplication(ConanFile):
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def build_requirements(self):
        self.tool_requires("cmake/[>=4.1]")

    def configure(self):
        spdlog_options = self.options["spdlog"]
        spdlog_options.use_std_fmt = True

        # https://github.com/conan-io/conan-center-index/issues/28311
        boost_options = self.options["boost"]
        boost_options.without_locale = True
        boost_options.without_stacktrace = True

    def layout(self):
        if not (build_dir := os.getenv("BUILD_DIR")):
            raise KeyError(f"Environment variable BUILD_DIR not set in {__file__}")

        cmake_layout(self, build_folder=build_dir)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def requirements(self):
        requirements = self.conan_data.get("requirements", [])
        tuple(map(self.requires, requirements))
