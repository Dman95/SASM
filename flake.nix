{
  description = "SASM (SimpleASM) - simple Open Source crossplatform IDE for NASM, MASM, GAS, FASM assembly languages.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        # Wrapper to silence "skipping incompatible" linker warnings 
        # when building 32-bit assembly on a 64-bit system.
        clean-gcc = pkgs.writeShellScriptBin "gcc" ''
          export PATH=${pkgs.gcc_multi}/bin:$PATH
          exec ${pkgs.gcc_multi}/bin/gcc "$@" 2> >(${pkgs.gnugrep}/bin/grep -v "skipping incompatible" >&2)
        '';

        sasm = pkgs.libsForQt5.mkDerivation rec {
          pname = "sasm";

          # If this is built from a git commit, use the short hash. 
          # If not (e.g. a tarball), use "dirty".
          version = if (self ? shortRev) then self.shortRev else "dirty";

          src = pkgs.lib.cleanSource ./.;

          nativeBuildInputs = with pkgs; [
            libsForQt5.qmake
            libsForQt5.wrapQtAppsHook
            copyDesktopItems
          ];

          buildInputs = [
            pkgs.libsForQt5.qtbase
            pkgs.libsForQt5.qtscript
          ];

          desktopItems = [
            (pkgs.makeDesktopItem {
              name = "sasm";
              desktopName = "SASM";
              exec = "sasm";
              icon = "sasm";
              comment = "Simple crossplatform IDE for NASM, MASM, GAS and FASM assembly languages.";
              categories = [ "Development" "IDE" "Qt"];
            })
          ];

          # Wraps GCC to silence linker warnings when searching mixed 32-bit and 64-bit libraries.
          postInstall = ''
            wrapProgram $out/bin/sasm \
              --prefix PATH : ${pkgs.lib.makeBinPath [ 
                  pkgs.nasm
                  clean-gcc
                  pkgs.gdb
              ]} \
              --prefix LIBRARY_PATH : "/run/opengl-driver/lib:${pkgs.lib.makeLibraryPath [
                  pkgs.glibc
                  pkgs.glibc.static
                  pkgs.pkgsi686Linux.glibc
              ]}" \
              --set QT_LOGGING_RULES "qt.qpa.wayland=false" # Silences the harmless "Wayland does not support QWindow::requestActivate()" warning.
            mkdir -p $out/share/pixmaps
            cp $src/Linux/share/sasm/sasm.png $out/share/pixmaps/sasm.png
          '';

          meta = with pkgs.lib; {
            description = "Simple Open Source crossplatform IDE for NASM, MASM, GAS, FASM assembly languages.";
            homepage = "https://dman95.github.io/SASM/english.html";
            license = licenses.gpl3;
            platforms = platforms.linux;
            mainProgram = "sasm";
          };
        };
      in
      {
        packages.default = sasm;
        
        apps.default = flake-utils.lib.mkApp {
          drv = sasm;
        };
      }
    );
}