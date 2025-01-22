{
  pkgs ? import <nixpkgs> {},
}:

pkgs.stdenv.mkDerivation {
  name = "rmc";
  version = "1.0";
  src = ./.;
  installPhase = ''
    make install PREFIX=$out
  '';
}
