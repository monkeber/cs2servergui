from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CS2ServerGUI(ConanFile):
    name = "cs2servergui"
    description = "A simple manager for your CS2 server"
    version = "3.0.0"

    generators = "CMakeDeps", "CMakeToolchain"
    settings = "os", "compiler", "build_type", "arch"
    build_policy = "missing"

    def requirements(self):
        self.requires("cpr/1.14.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires("rapidcsv/8.84")
        self.requires("libcurl/8.17.0")
        self.requires("m4/1.4.20", override=True)
        self.requires("libb2/20190723")
        self.requires("harfbuzz/12.3.0")
        self.requires("qt/6.10.1")

    def layout(self):
        cmake_layout(self)

    def configure(self):
        if self.settings.os == "Windows": 
            self.options["libcurl"].with_ssl = "schannel"

        self.options["harfbuzz"].with_glib = False

        self.options["qt"].shared = True

        self.options["qt"].openssl = True
        self.options["qt"].with_glib = False
        self.options["qt"].with_doubleconversion = True
        self.options["qt"].with_freetype = True
        self.options["qt"].with_fontconfig = True
        self.options["qt"].with_icu = True
        self.options["qt"].with_harfbuzz = True
        self.options["qt"].with_libjpeg = False
        self.options["qt"].with_libpng = True
        self.options["qt"].with_sqlite3 = False
        self.options["qt"].with_mysql = False
        self.options["qt"].with_pq = False
        self.options["qt"].with_odbc = False
        self.options["qt"].with_zstd = False
        self.options["qt"].with_brotli = False
        self.options["qt"].with_dbus = True
        self.options["qt"].with_libalsa = False
        self.options["qt"].with_openal = False
        self.options["qt"].with_gstreamer = False
        self.options["qt"].with_pulseaudio = False
        self.options["qt"].with_gssapi = False
        self.options["qt"].with_md4c = False
        self.options["qt"].with_x11 = True
        self.options["qt"].with_egl = True

        self.options["qt"].qtquick3d = True
        self.options["qt"].qtshadertools = True
        self.options["qt"].qtwayland = True
