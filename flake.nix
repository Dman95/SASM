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
        lib  = pkgs.lib;

        # Wrapper to silence "skipping incompatible" linker warnings 
        # when building 32-bit assembly on a 64-bit system.
        clean-gcc = pkgs.writeShellScriptBin "gcc" ''
          #!${pkgs.bash}/bin/bash
          export PATH=${pkgs.gcc_multi}/bin:$PATH
          exec ${pkgs.gcc_multi}/bin/gcc "$@" 2>&1 | ${pkgs.gnugrep}/bin/grep -v "skipping incompatible" >&2
        '';

        -- guard optional 32-bit glibc
        i686Glibc = if lib.hasAttr "pkgsi686Linux" pkgs then pkgs.pkgsi686Linux.glibc else null;
        libList = lib.filter (x: x != null) [ pkgs.glibc pkgs.glibc.static i686Glibc ];

        sasm = pkgs.libsForQt5.mkDerivation rec {
          pname = "sasm";
          version = if (self ? shortRev) then self.shortRev else "dirty";
          src = pkgs.lib.cleanSource ./.;

          nativeBuildInputs = with pkgs; [
            libsForQt5.qmake
            libsForQt5.wrapQtAppsHook
            copyDesktopItems
          ];

          buildInputs = with pkgs; [
            libsForQt5.qtbase
            libsForQt5.qtscript
            nasm         # runtime tool — ensure it's in closure
            gdb          # runtime tool — ensure it's in closure
            clean-gcc    # the wrapper we created
          ];

          # If your build installs into $out/bin/sasm, wrap it; otherwise adjust.
          postInstall = ''
            if [ -x $out/bin/sasm ]; then
              wrapProgram $out/bin/sasm \
                --prefix PATH : ${pkgs.lib.makeBinPath [ pkgs.nasm clean-gcc pkgs.gdb ]} \
                --prefix LIBRARY_PATH : "/run/opengl-driver/lib:${pkgs.lib.makeLibraryPath libList}" \
                --set QT_LOGGING_RULES "qt.qpa.wayland=false"
            fi

            mkdir -p $out/share/pixmaps
            if [ -f $src/Linux/share/sasm/sasm.png ]; then
              cp $src/Linux/share/sasm/sasm.png $out/share/pixmaps/sasm.png
            fi
          '';

          meta = with pkgs.lib; {
            description = "Simple Open Source crossplatform IDE for NASM, MASM, GAS, FASM assembly languages.";
            homepage = "https://dman95.github.io/SASM/english.html";
            license = licenses.gpl3;
            platforms = platforms.linux;
            maintainers = [ maintainers.yourGitHubNick ];
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
