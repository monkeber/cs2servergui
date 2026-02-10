from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CS2ServerGUI(ConanFile):
    name = "cs2servergui"
    description = "A simple manager for your CS2 server"
    version = "3.0.0"

    # We use a cmake-driven approach (so that cmake is calling the conan and not vice versa,
    # so we only need CMakeDeps, see discussion: https://github.com/conan-io/conan/issues/17361)
    generators = "CMakeDeps"
    settings = "os", "compiler", "build_type", "arch"
    build_policy = "missing"

    def requirements(self):
        self.requires("cpr/1.14.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires("rapidcsv/8.84")
        self.requires("libcurl/8.17.0")
        self.requires("sqlitecpp/3.3.3")
        self.requires("icu/78.2")

    def layout(self):
        cmake_layout(self)

    def configure(self):
        if self.settings.os == "Windows": 
            self.options["libcurl"].with_ssl = "schannel"
