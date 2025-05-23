{
  pkgs ? import <nixpkgs> {},
}:

pkgs.stdenv.mkDerivation {
  name = "rmc";
  version = "1.0";
  src = ./.;
  nativeBuildInputs = [ pkgs.pkg-config pkgs.scdoc ];
  buildInputs = [ pkgs.libnotify ];
  buildPhase = ''
    make WITH_LIBNOTIFY=1
  '';
  installPhase = ''
    make install PREFIX=$out
  '';
}
